#include "../../include/pdf/che_pdf_filter.h"
#include "../../extlib/zlib/zlib.h"
#include "../../extlib/jbig2dec/jbig2.h"
#include "../../extlib/openjpeg/openjpeg.h"

HE_VOID CHE_PDF_HexFilter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0  )
	{
		return;
	}
    HE_BYTE data[2];
    while( length-- )
    {
        data[0]  = (*pData & 0xF0) >> 4;
        data[0] += (data[0] > 9 ? 'A' - 10 : '0');
        data[1]  = (*pData & 0x0F);
        data[1] += (data[1] > 9 ? 'A' - 10 : '0');
		buffer.Write( data, 2 );
        ++pData;
    }
	data[0] = '>';
	buffer.Write( data, 1 );
}

HE_VOID CHE_PDF_HexFilter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0  )
	{
		return;
	}
	HE_BOOL	bLow = FALSE;
    HE_BYTE chr = 0;
	HE_BYTE	val = 0;
    while( length-- ) 
    {
        chr  = *pData;
		if ( chr == '>' )
		{
			break;
		}
		if ( chr >= '0' && chr <= '9' )
		{
			val += (chr-'0');
		}else if ( chr >= 'a' && chr <= 'f' )
		{
			val += (chr-'a'+10);
		}else if ( chr >= 'A' && chr <= 'F' )
		{
			val += (chr-'A'+10);
		}
		if ( bLow == TRUE )
		{
			buffer.Write( &val, 1 );
			val = 0;
			bLow = FALSE;
		}else{
			val *= 16;
			bLow = TRUE;
		}
        ++pData;
    }
}

HE_VOID CHE_PDF_ASCII85Filter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	m_count = 0;
    m_tuple = 0;

	HE_ULONG  c;
    const HE_CHAR *   z = "z";
	
    while( length ) 
    {
        c = *pData & 0xff;
        switch (m_count++) {
		case 0: m_tuple |= ( c << 24); break;
		case 1: m_tuple |= ( c << 16); break;
		case 2: m_tuple |= ( c <<  8); break;
		case 3:
			m_tuple |= c;
			if( 0 == m_tuple ) 
			{
				buffer.Write( (unsigned char*)z, 1 );
			}
			else
			{
				this->EncodeTuple( m_tuple, m_count, buffer ); 
			}
			
			m_tuple = 0;
			m_count = 0;
			break;
        }
        --length;
        ++pData;
    }
	if ( m_count > 0 )
	{
		this->EncodeTuple( m_tuple, m_count, buffer );
	}
	buffer.Write( (unsigned char*)("~>"), 2 );
}

HE_VOID CHE_PDF_ASCII85Filter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	m_count = 0;
    m_tuple = 0;
    HE_BOOL foundEndMarker = FALSE;
	const HE_ULONG sPowers85[] = { 85*85*85*85, 85*85*85, 85*85, 85, 1 };
	
    while( length && !foundEndMarker ) 
    {
        switch ( *pData ) 
        {
		default:
			if ( *pData < '!' || *pData > 'u') 
			{
				//error
				return;
			}
			
			m_tuple += ( *pData - '!') * sPowers85[m_count++];
			if( m_count == 5 ) 
			{
				WidePut( m_tuple, m_count-1, buffer );
				m_count = 0;
				m_tuple = 0;
			}
			break;
		case 'z':
			if (m_count != 0 ) 
			{
				//error
				return;
			}
			this->WidePut( 0, 4, buffer );
			break;
		case '~':
			++pData; 
			--length;
			if( length && *pData != '>' ) 
			{
				//error
				return;
			}
			foundEndMarker = true;
			break;
		case '\n': case '\r': case '\t': case ' ':
		case '\0': case '\f': case '\b': case 0177:
			break;
        }
        --length;
        ++pData;
	}
	
	if( m_count > 1 ) 
	{
		WidePut( m_tuple, m_count-1, buffer );
		m_count = 0;
		m_tuple = 0;
	}
}

HE_VOID CHE_PDF_ASCII85Filter::EncodeTuple( HE_ULONG tuple, HE_ULONG count, CHE_DynBuffer & buffer )
{
	HE_ULONG i = 5;
	HE_ULONG z = 0;
	HE_CHAR buf[5];
	HE_CHAR out[5];
	HE_CHAR* start = buf;
	
	do 
	{
		*start++ = (HE_CHAR)(tuple % 85);
		tuple /= 85;
	} 
	while (--i > 0);
	
	i = count;
	do 
	{
		out[z++] = (HE_CHAR)(*--start) + '!';
	} 
	while( i-- > 0 );

	buffer.Write( (unsigned char*)out, z );
}
	
HE_VOID CHE_PDF_ASCII85Filter::WidePut( HE_ULONG tuple, HE_ULONG bytes, CHE_DynBuffer & buffer )
{
    char data[4];

	data[0] = static_cast<char>(tuple >> 24);
	data[1] = static_cast<char>(tuple >> 16);
	data[2] = static_cast<char>(tuple >>  8);
	data[3] = static_cast<char>(tuple);

	if ( bytes != 4 )
	{
		switch ( bytes )
		{
		case 3:
			if ( data[3] != 0 )
			{
				data[2]++;
			}
			break;
		case 2:
			if ( data[2] != 0 || data[3] != 0 )
			{
				data[1]++;
			}
			break;
		case 1:
			if ( data[1] != 0 || data[2] != 0 || data[3] != 0 )
			{
				data[0]++;
			}
			break;
		default:
			break;
		}
	}

	buffer.Write( (unsigned char*)data, bytes );
}

HE_VOID CHE_PDF_FlateFilter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE tmpBuffer[4096];
	HE_ULONG nWrittenData = 0;
	z_stream stream;
	HE_INT32 param = Z_NO_FLUSH;

	stream.zalloc = NULL;
	stream.zfree = NULL;
    stream.avail_in = (HE_INT32)length;
    stream.next_in  = pData;
	deflateInit( &stream, Z_DEFAULT_COMPRESSION );
	
    while( TRUE )
	{
        stream.avail_out = 4096;
        stream.next_out  = tmpBuffer;

		switch ( deflate( &stream, param ) )
		{
		case Z_OK:
			if ( stream.avail_out != 0 )
			{
				param = Z_FINISH;
			}
			break;
		case Z_BUF_ERROR:
		case Z_STREAM_END:
			nWrittenData = 4096 - stream.avail_out;
			buffer.Write( tmpBuffer, nWrittenData );
			deflateEnd( &stream );
			return;
			break;
		default:
			deflateEnd( &stream );
			return;
			break;
		}
        nWrittenData = 4096 - stream.avail_out;
		buffer.Write( tmpBuffer, nWrittenData );
    }
}

HE_VOID CHE_PDF_FlateFilter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE tmpBuffer[4096];
	HE_ULONG nWrittenData = 0;
	z_stream stream;
	HE_INT32 param = Z_NO_FLUSH;

	stream.zalloc = NULL;
	stream.zfree = NULL;
	stream.avail_in = (HE_INT32)length;
	stream.next_in  = pData;
	inflateInit( &stream );

	while( TRUE )
	{
		stream.avail_out = 4096;
		stream.next_out  = tmpBuffer;
   
		int i = 0;
		switch ( i = inflate( &stream, param ) )
		{
		case Z_OK:
			if ( stream.avail_out != 0 )
			{
				param = Z_FINISH;
			}
			break;
		case Z_STREAM_END:
			nWrittenData = 4096 - stream.avail_out;
			if ( m_pPredictor )
			{
				m_pPredictor->Decode( tmpBuffer, nWrittenData, buffer );
			}else{
				buffer.Write( tmpBuffer, nWrittenData );
			}
			inflateEnd( &stream );
			return;
			break;
		//case Z_BUF_ERROR:
		//	break;
		default:
			inflateEnd( &stream );
			return;
		}   
		nWrittenData = 4096 - stream.avail_out;
		if ( m_pPredictor )
		{
			m_pPredictor->Decode( tmpBuffer, nWrittenData, buffer );
		}else{
			buffer.Write( tmpBuffer, nWrittenData );
		}
	}
}

const unsigned short CHE_PDF_LZWFilter::s_masks[] = {	0x01FF,
														0x03FF,
														0x07FF,
														0x0FFF };

const unsigned short CHE_PDF_LZWFilter::s_clear  = 0x0100;      // clear table
const unsigned short CHE_PDF_LZWFilter::s_eod    = 0x0101;      // end of data

HE_VOID CHE_PDF_LZWFilter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	return;
}

HE_VOID CHE_PDF_LZWFilter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & outBuffer )
{
	m_mask       = 0;
    m_code_len   = 9;
    m_character  = 0;
	
    m_bFirst     = true;

	unsigned int       buffer_size = 0;
    const unsigned int buffer_max  = 24;
	
    HE_ULONG         old         = 0;
    HE_ULONG         code        = 0;
    HE_ULONG         buffer      = 0;
	
    TLzwItem           item;
	
    std::vector<unsigned char> data;
	
    if( m_bFirst ) 
    {
        m_character = *pData;
        m_bFirst    = false;
    }
	
    while( length ) 
    {
        // Fill the buffer
        while( buffer_size <= (buffer_max-8) && length )
        {
            buffer <<= 8;
            buffer |= static_cast<HE_ULONG>(static_cast<unsigned char>(*pData));
            buffer_size += 8;
			
            ++pData;
            length--;
        }
		
        // read from the buffer
        while( buffer_size >= m_code_len ) 
        {
            code         = (buffer >> (buffer_size - m_code_len)) & CHE_PDF_LZWFilter::s_masks[m_mask];
            buffer_size -= m_code_len;
			
            if( code == CHE_PDF_LZWFilter::s_clear ) 
            {
                m_mask     = 0;
                m_code_len = 9;
				
                InitTable();
            }
            else if( code == CHE_PDF_LZWFilter::s_eod ) 
            {
                length = 0;
                break;
            }
            else 
            {
                if( code >= m_table.size() )
                {
//                     if (old >= m_table.size())
//                     {
//                         PODOFO_RAISE_ERROR( ePdfError_ValueOutOfRange );
//                     }
                    data = m_table[old].value;
                    data.push_back( m_character );
                }
                else
                    data = m_table[code].value;
				
                // Write data to the output device
                if( m_pPredictor ) 
                    m_pPredictor->Decode( (HE_LPBYTE)(&(data[0])), data.size(), outBuffer );
                else
                    outBuffer.Write( (HE_LPBYTE)(&(data[0])), data.size() );
				
                m_character = data[0];
                if( old < m_table.size() ) // fix the first loop
                    data = m_table[old].value;
                data.push_back( m_character );
				
                item.value = data;
                m_table.push_back( item );
				
                old = code;
				
                switch( m_table.size() ) 
                {
				case 511:
				case 1023:
				case 2047:
					++m_code_len;
					++m_mask;
				default:
					break;
                }
            }
        }
    }
	return;
}

HE_VOID CHE_PDF_LZWFilter::InitTable()
{
	int      i;
	TLzwItem item;
	
	m_table.clear();
	m_table.reserve( 4096 );
	
	for( i=0;i<=255;i++ )
	{
		item.value.clear();
		item.value.push_back( static_cast<unsigned char>(i) );
		m_table.push_back( item );
	}
	
	// Add dummy entry, which is never used by decoder
	item.value.clear();
	m_table.push_back( item );
}

HE_VOID CHE_PDF_RLEFileter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	CHE_DynBuffer buf( length, length, GetAllocator() );
	HE_BYTE byte = *pData;
	pData++;
	HE_ULONG lCount = 1;
	HE_BYTE countByte = 0;
	while ( length > 1 )
	{
		if ( byte == *pData )
		{
			if ( buf.GetSize() > 1 )
			{
				countByte = (HE_BYTE)(buf.GetSize() - 1);
				buffer.Write( &countByte, 1 );
				buffer.Write( buf );
				buf.Clear();
			}
			lCount++;
			if ( lCount == 128 )
			{
				countByte = (HE_BYTE)(lCount + 127);
				buffer.Write( &countByte, 1 );
				buffer.Write( &byte, 1 );
				lCount = 1;
				byte = *(++pData);
			}
		}else if ( byte != *pData )
		{
			if ( lCount >= 2 )
			{
				countByte = (HE_BYTE)(lCount + 127);
				buffer.Write( &countByte, 1 );
				buffer.Write( &byte, 1 );
				lCount = 1;
				byte = *pData;
			}else{
				buf.Write( &byte, 1 );
				byte = *pData;
				if ( buf.GetSize() == 128 )
				{
					countByte = (HE_BYTE)(buf.GetSize() - 1);
					buffer.Write( &countByte, 1 );
					buffer.Write( buf );
					buf.Clear();
				}
			}
		}
		length--;
		pData++;
	}

	if ( buf.GetSize() > 0 )
	{
		countByte = (HE_BYTE)(buf.GetSize() - 1);
		buffer.Write( &countByte, 1 );
		buffer.Write( buf );
		buf.Clear();
	}
	if ( lCount == 1 )
	{
		countByte = (HE_BYTE)(lCount - 1);
		buffer.Write( &countByte, 1 );
		buffer.Write( &byte, 1 );
	}else if ( lCount > 1 )
	{
		countByte = (HE_BYTE)(lCount + 127);
		buffer.Write( &countByte, 1 );
		buffer.Write( &byte, 1 );
	}
	countByte = 128;
	buffer.Write( &countByte, 1 );
}

HE_VOID CHE_PDF_RLEFileter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	HE_BYTE	byteLen = 0;
	while ( length > 0 )
	{
		byteLen = *pData;
		pData++;
		length--;
		if ( byteLen == 128 )
		{
			return;
		}else if ( byteLen < 128 )
		{
			buffer.Write( pData, byteLen+1 );
			length-= byteLen+1;
			pData+= byteLen+1;
		}else if ( byteLen > 128 )
		{
			for ( HE_BYTE i = 0; i < byteLen - 127; i++ )
			{
				buffer.Write( pData, 1 );
			}
			pData++;
			length--;
		}
	}
}


CHE_PDF_FaxDecodeParams::CHE_PDF_FaxDecodeParams( CHE_PDF_DictionaryPtr dictPtr )
	: k(0), columns(1728), rows(0), eol(FALSE), eba(FALSE), eob(TRUE), bi1(FALSE)
{
	if ( dictPtr )
	{
		CHE_PDF_ObjectPtr objPtr;
		objPtr = dictPtr->GetElement( "K", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			k = objPtr->GetNumberPtr()->GetInteger();
		}
		objPtr = dictPtr->GetElement( "EndOfLine", OBJ_TYPE_BOOLEAN );
		if ( objPtr )
		{
			eol = objPtr->GetBooleanPtr()->GetValue();
		}
		objPtr = dictPtr->GetElement( "EncodedByteAlign", OBJ_TYPE_BOOLEAN );
		if ( objPtr )
		{
			eba = objPtr->GetBooleanPtr()->GetValue();
		}
		objPtr = dictPtr->GetElement( "EndOfBlock", OBJ_TYPE_BOOLEAN );
		if ( objPtr )
		{
			eob = objPtr->GetBooleanPtr()->GetValue();
		}
		objPtr = dictPtr->GetElement( "BlackIs1", OBJ_TYPE_BOOLEAN );
		if ( objPtr )
		{
			bi1 = objPtr->GetBooleanPtr()->GetValue();
		}
		objPtr = dictPtr->GetElement( "Columns", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			columns = objPtr->GetNumberPtr()->GetInteger();
		}
		objPtr = dictPtr->GetElement( "Rows", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			rows = objPtr->GetNumberPtr()->GetInteger();
		}
	}
}



typedef struct cfd_node_s cfd_node;

struct cfd_node_s
{
	short val;
	short nbits;
};


#define 	cfd_white_initial_bits			8
#define		cfd_black_initial_bits			7
#define		cfd_2d_initial_bits				7
#define		cfd_uncompressed_initial_bits	6

#define 	FAXERROR			-1
#define		ZEROS			-2
#define		UNCOMPRESSED	-3

/* semantic codes for cf_2d_decode */

#define	P -4
#define	H -5
#define	VR3 0
#define	VR2 1
#define	VR1 2
#define	V0 3
#define	VL1 4
#define	VL2 5
#define	VL3 6

/* White decoding table. */
static const cfd_node cf_white_decode[] = {
	{256,12},{272,12},{29,8},{30,8},{45,8},{46,8},{22,7},{22,7},
	{23,7},{23,7},{47,8},{48,8},{13,6},{13,6},{13,6},{13,6},{20,7},
	{20,7},{33,8},{34,8},{35,8},{36,8},{37,8},{38,8},{19,7},{19,7},
	{31,8},{32,8},{1,6},{1,6},{1,6},{1,6},{12,6},{12,6},{12,6},{12,6},
	{53,8},{54,8},{26,7},{26,7},{39,8},{40,8},{41,8},{42,8},{43,8},
	{44,8},{21,7},{21,7},{28,7},{28,7},{61,8},{62,8},{63,8},{0,8},
	{320,8},{384,8},{10,5},{10,5},{10,5},{10,5},{10,5},{10,5},{10,5},
	{10,5},{11,5},{11,5},{11,5},{11,5},{11,5},{11,5},{11,5},{11,5},
	{27,7},{27,7},{59,8},{60,8},{288,9},{290,9},{18,7},{18,7},{24,7},
	{24,7},{49,8},{50,8},{51,8},{52,8},{25,7},{25,7},{55,8},{56,8},
	{57,8},{58,8},{192,6},{192,6},{192,6},{192,6},{1664,6},{1664,6},
	{1664,6},{1664,6},{448,8},{512,8},{292,9},{640,8},{576,8},{294,9},
	{296,9},{298,9},{300,9},{302,9},{256,7},{256,7},{2,4},{2,4},{2,4},
	{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},{2,4},
	{2,4},{2,4},{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},
	{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},{3,4},{128,5},{128,5},{128,5},
	{128,5},{128,5},{128,5},{128,5},{128,5},{8,5},{8,5},{8,5},{8,5},
	{8,5},{8,5},{8,5},{8,5},{9,5},{9,5},{9,5},{9,5},{9,5},{9,5},{9,5},
	{9,5},{16,6},{16,6},{16,6},{16,6},{17,6},{17,6},{17,6},{17,6},
	{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},{4,4},
	{4,4},{4,4},{4,4},{4,4},{4,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},
	{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},
	{14,6},{14,6},{14,6},{14,6},{15,6},{15,6},{15,6},{15,6},{64,5},
	{64,5},{64,5},{64,5},{64,5},{64,5},{64,5},{64,5},{6,4},{6,4},
	{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},{6,4},
	{6,4},{6,4},{6,4},{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},
	{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},{7,4},{-2,3},{-2,3},
	{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},
	{-1,0},{-1,0},{-1,0},{-1,0},{-3,4},{1792,3},{1792,3},{1984,4},
	{2048,4},{2112,4},{2176,4},{2240,4},{2304,4},{1856,3},{1856,3},
	{1920,3},{1920,3},{2368,4},{2432,4},{2496,4},{2560,4},{1472,1},
	{1536,1},{1600,1},{1728,1},{704,1},{768,1},{832,1},{896,1},
	{960,1},{1024,1},{1088,1},{1152,1},{1216,1},{1280,1},{1344,1},
	{1408,1}
};

/* Black decoding table. */
static const cfd_node cf_black_decode[] = {
	{128,12},{160,13},{224,12},{256,12},{10,7},{11,7},{288,12},{12,7},
	{9,6},{9,6},{8,6},{8,6},{7,5},{7,5},{7,5},{7,5},{6,4},{6,4},{6,4},
	{6,4},{6,4},{6,4},{6,4},{6,4},{5,4},{5,4},{5,4},{5,4},{5,4},{5,4},
	{5,4},{5,4},{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},
	{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},{1,3},{4,3},{4,3},{4,3},{4,3},
	{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},
	{4,3},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},
	{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},
	{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},{3,2},
	{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},
	{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},
	{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},{2,2},
	{-2,4},{-2,4},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},
	{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-3,5},{1792,4},
	{1792,4},{1984,5},{2048,5},{2112,5},{2176,5},{2240,5},{2304,5},
	{1856,4},{1856,4},{1920,4},{1920,4},{2368,5},{2432,5},{2496,5},
	{2560,5},{18,3},{18,3},{18,3},{18,3},{18,3},{18,3},{18,3},{18,3},
	{52,5},{52,5},{640,6},{704,6},{768,6},{832,6},{55,5},{55,5},
	{56,5},{56,5},{1280,6},{1344,6},{1408,6},{1472,6},{59,5},{59,5},
	{60,5},{60,5},{1536,6},{1600,6},{24,4},{24,4},{24,4},{24,4},
	{25,4},{25,4},{25,4},{25,4},{1664,6},{1728,6},{320,5},{320,5},
	{384,5},{384,5},{448,5},{448,5},{512,6},{576,6},{53,5},{53,5},
	{54,5},{54,5},{896,6},{960,6},{1024,6},{1088,6},{1152,6},{1216,6},
	{64,3},{64,3},{64,3},{64,3},{64,3},{64,3},{64,3},{64,3},{13,1},
	{13,1},{13,1},{13,1},{13,1},{13,1},{13,1},{13,1},{13,1},{13,1},
	{13,1},{13,1},{13,1},{13,1},{13,1},{13,1},{23,4},{23,4},{50,5},
	{51,5},{44,5},{45,5},{46,5},{47,5},{57,5},{58,5},{61,5},{256,5},
	{16,3},{16,3},{16,3},{16,3},{17,3},{17,3},{17,3},{17,3},{48,5},
	{49,5},{62,5},{63,5},{30,5},{31,5},{32,5},{33,5},{40,5},{41,5},
	{22,4},{22,4},{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},
	{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},{14,1},
	{15,2},{15,2},{15,2},{15,2},{15,2},{15,2},{15,2},{15,2},{128,5},
	{192,5},{26,5},{27,5},{28,5},{29,5},{19,4},{19,4},{20,4},{20,4},
	{34,5},{35,5},{36,5},{37,5},{38,5},{39,5},{21,4},{21,4},{42,5},
	{43,5},{0,3},{0,3},{0,3},{0,3}
};

/* 2-D decoding table. */
static const cfd_node cf_2d_decode[] = {
	{128,11},{144,10},{6,7},{0,7},{5,6},{5,6},{1,6},{1,6},{-4,4},
	{-4,4},{-4,4},{-4,4},{-4,4},{-4,4},{-4,4},{-4,4},{-5,3},{-5,3},
	{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},
	{-5,3},{-5,3},{-5,3},{-5,3},{-5,3},{4,3},{4,3},{4,3},{4,3},{4,3},
	{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},{4,3},
	{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},
	{2,3},{2,3},{2,3},{2,3},{2,3},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},{3,1},
	{3,1},{3,1},{3,1},{-2,4},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},
	{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},
	{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-1,0},{-3,3}
};

/* Uncompressed decoding table. */
static const cfd_node cf_uncompressed_decode[] = {
	{64,12},{5,6},{4,5},{4,5},{3,4},{3,4},{3,4},{3,4},{2,3},{2,3},
	{2,3},{2,3},{2,3},{2,3},{2,3},{2,3},{1,2},{1,2},{1,2},{1,2},{1,2},
	{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},
	{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
	{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
	{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},
	{-1,0},{-1,0},{8,6},{9,6},{6,5},{6,5},{7,5},{7,5},{4,4},{4,4},
	{4,4},{4,4},{5,4},{5,4},{5,4},{5,4},{2,3},{2,3},{2,3},{2,3},{2,3},
	{2,3},{2,3},{2,3},{3,3},{3,3},{3,3},{3,3},{3,3},{3,3},{3,3},{3,3},
	{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},{0,2},
	{0,2},{0,2},{0,2},{0,2},{0,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},
	{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2},{1,2}
};

/* bit magic */

static inline int getbit(const unsigned char *buf, int x)
{
	return ( buf[x >> 3] >> ( 7 - (x & 7) ) ) & 1;
}

static int
	find_changing(const unsigned char *line, int x, int w)
{
	int a, b;

	if (!line)
		return w;

	if (x == -1)
	{
		a = 0;
		x = 0;
	}
	else
	{
		a = getbit(line, x);
		x++;
	}

	while (x < w)
	{
		b = getbit(line, x);
		if (a != b)
			break;
		x++;
	}

	return x;
}

static int
	find_changing_color(const unsigned char *line, int x, int w, int color)
{
	if (!line)
		return w;

	x = find_changing(line, (x > 0 || !color) ? x : -1, w);

	if (x < w && getbit(line, x) != color)
		x = find_changing(line, x, w);

	return x;
}

static const unsigned char lm[8] = {
	0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01
};

static const unsigned char rm[8] = {
	0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE
};

static inline void setbits(unsigned char *line, int x0, int x1)
{
	int a0, a1, b0, b1, a;

	a0 = x0 >> 3;
	a1 = x1 >> 3;

	b0 = x0 & 7;
	b1 = x1 & 7;

	if (a0 == a1)
	{
		if (b1)
			line[a0] |= lm[b0] & rm[b1];
	}
	else
	{
		line[a0] |= lm[b0];
		for (a = a0 + 1; a < a1; a++)
			line[a] = 0xFF;
		if (b1)
			line[a1] |= rm[b1];
	}
}

typedef struct fz_faxd_s fz_faxd;

enum
{
	STATE_NORMAL,	/* neutral state, waiting for any code */
	STATE_MAKEUP,	/* got a 1d makeup code, waiting for terminating code */
	STATE_EOL,		/* at eol, needs output buffer space */
	STATE_H1, STATE_H2,	/* in H part 1 and 2 (both makeup and terminating codes) */
	STATE_DONE		/* all done */
};

struct fz_faxd_s
{
// 	fz_context *ctx;
// 	fz_stream *chain;

	HE_LPBYTE data;
	HE_ULONG index;
	HE_ULONG size;

	int k;
	int end_of_line;
	int encoded_byte_align;
	int columns;
	int rows;
	int end_of_block;
	int black_is_1;

	int stride;
	int ridx;

	int bidx;
	unsigned int word;

	int stage;

	int a, c, dim, eolc;
	unsigned char *ref;
	unsigned char *dst;
	unsigned char *rp, *wp;
};

static inline void eat_bits(fz_faxd *fax, int nbits)
{
	fax->word <<= nbits;
	fax->bidx += nbits;
}

static int fill_bits(fz_faxd *fax)
{
	while (fax->bidx >= 8)
	{
		int c = *(fax->data + fax->index); //;fz_read_byte(fax->chain);
		fax->index++;
		if (c == EOF)
			return EOF;
		fax->bidx -= 8;
		fax->word |= c << fax->bidx;
	}
	return 0;
}

static int get_code(fz_faxd *fax, const cfd_node *table, int initialbits)
{
	unsigned int word = fax->word;
	int tidx = word >> (32 - initialbits);
	int val = table[tidx].val;
	int nbits = table[tidx].nbits;

	if (nbits > initialbits)
	{
		int mask = (1 << (32 - initialbits)) - 1;
		tidx = val + ((word & mask) >> (32 - nbits));
		val = table[tidx].val;
		nbits = initialbits + table[tidx].nbits;
	}

	eat_bits(fax, nbits);

	return val;
}

/* decode one 1d code */
static void dec1d(/*fz_context *ctx,*/ fz_faxd *fax)
{
	int code;

	if (fax->a == -1)
		fax->a = 0;

	if (fax->c)
		code = get_code(fax, cf_black_decode, cfd_black_initial_bits);
	else
		code = get_code(fax, cf_white_decode, cfd_white_initial_bits);

// 	if (code == UNCOMPRESSED)
// 		fz_throw(ctx, "uncompressed data in faxd");
// 
// 	if (code < 0)
// 		fz_throw(ctx, "negative code in 1d faxd");
// 
// 	if (fax->a + code > fax->columns)
// 		fz_throw(ctx, "overflow in 1d faxd");

	if (fax->c)
		setbits(fax->dst, fax->a, fax->a + code);

	fax->a += code;

	if (code < 64)
	{
		fax->c = !fax->c;
		fax->stage = STATE_NORMAL;
	}
	else
		fax->stage = STATE_MAKEUP;
}

/* decode one 2d code */
static void dec2d(/*fz_context *ctx, */fz_faxd *fax)
{
	int code, b1, b2;

	if (fax->stage == STATE_H1 || fax->stage == STATE_H2)
	{
		if (fax->a == -1)
			fax->a = 0;

		if (fax->c)
			code = get_code(fax, cf_black_decode, cfd_black_initial_bits);
		else
			code = get_code(fax, cf_white_decode, cfd_white_initial_bits);

// 		if (code == UNCOMPRESSED)
// 			fz_throw(ctx, "uncompressed data in faxd");
// 
// 		if (code < 0)
// 			fz_throw(ctx, "negative code in 2d faxd");
// 
// 		if (fax->a + code > fax->columns)
// 			fz_throw(ctx, "overflow in 2d faxd");

		if (fax->c)
			setbits(fax->dst, fax->a, fax->a + code);

		fax->a += code;

		if (code < 64)
		{
			fax->c = !fax->c;
			if (fax->stage == STATE_H1)
				fax->stage = STATE_H2;
			else if (fax->stage == STATE_H2)
				fax->stage = STATE_NORMAL;
		}

		return;
	}

	code = get_code(fax, cf_2d_decode, cfd_2d_initial_bits);

	switch (code)
	{
	case H:
		fax->stage = STATE_H1;
		break;

	case P:
		b1 = find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 >= fax->columns)
			b2 = fax->columns;
		else
			b2 = find_changing(fax->ref, b1, fax->columns);
		if (fax->c) setbits(fax->dst, fax->a, b2);
		fax->a = b2;
		break;

	case V0:
		b1 = find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VR1:
		b1 = 1 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 >= fax->columns) b1 = fax->columns;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VR2:
		b1 = 2 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 >= fax->columns) b1 = fax->columns;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VR3:
		b1 = 3 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 >= fax->columns) b1 = fax->columns;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VL1:
		b1 = -1 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 < 0) b1 = 0;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VL2:
		b1 = -2 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 < 0) b1 = 0;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

	case VL3:
		b1 = -3 + find_changing_color(fax->ref, fax->a, fax->columns, !fax->c);
		if (b1 < 0) b1 = 0;
		if (fax->c) setbits(fax->dst, fax->a, b1);
		fax->a = b1;
		fax->c = !fax->c;
		break;

// 	case UNCOMPRESSED:
// 		fz_throw(ctx, "uncompressed data in faxd");
// 
// 	case ERROR:
// 		fz_throw(ctx, "invalid code in 2d faxd");
// 
// 	default:
// 		fz_throw(ctx, "invalid code in 2d faxd (%d)", code);
	}
}


CHE_PDF_FaxFilter::CHE_PDF_FaxFilter( CHE_PDF_FaxDecodeParams * pParams, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Filter( pAllocator ), mpParams(pParams)
{
}

CHE_PDF_FaxFilter::~CHE_PDF_FaxFilter()
{
}

HE_VOID CHE_PDF_FaxFilter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{

}

HE_VOID CHE_PDF_FaxFilter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	fz_faxd faxs;
	if ( mpParams )
	{
		faxs.ref = NULL;
		faxs.dst = NULL;

		faxs.data = pData;
		faxs.index = 0;
		faxs.size = length;

		faxs.k = mpParams->k;
		faxs.end_of_line = mpParams->eol;
		faxs.encoded_byte_align = mpParams->eba;
		faxs.columns = mpParams->columns;
		faxs.rows = mpParams->rows;
		faxs.end_of_block = mpParams->eob;
		faxs.black_is_1 = mpParams->bi1;

		faxs.stride = ((faxs.columns - 1) >> 3) + 1;
		faxs.ridx = 0;
		faxs.bidx = 32;
		faxs.word = 0;

		faxs.stage = STATE_NORMAL;
		faxs.a = -1;
		faxs.c = 0;
		faxs.dim = faxs.k < 0 ? 2 : 1;
		faxs.eolc = 0;

		faxs.ref = GetAllocator()->NewArray<HE_BYTE>( faxs.stride );
		faxs.dst = GetAllocator()->NewArray<HE_BYTE>( faxs.stride );
		faxs.rp = faxs.dst;
		faxs.wp = faxs.dst + faxs.stride;

		memset(faxs.ref, 0, faxs.stride);
		memset(faxs.dst, 0, faxs.stride);
	}
	
	fz_faxd * fax = &faxs;


// 	unsigned char *p = buf;
// 	unsigned char *ep = buf + len;
	unsigned char * tmp = NULL;
	unsigned char tmpByte = 0;

	if (fax->stage == STATE_DONE)
		return/* 0*/;

	if (fax->stage == STATE_EOL)
		goto eol;

loop:

	if (fill_bits(fax))
	{
		if (fax->bidx > 31)
		{
			if (fax->a > 0)
				goto eol;
			goto rtc;
		}
	}

	if ((fax->word >> (32 - 12)) == 0)
	{
		eat_bits(fax, 1);
		goto loop;
	}

	if ((fax->word >> (32 - 12)) == 1)
	{
		eat_bits(fax, 12);
		fax->eolc ++;

		if (fax->k > 0)
		{
			if (fax->a == -1)
				fax->a = 0;
			if ((fax->word >> (32 - 1)) == 1)
				fax->dim = 1;
			else
				fax->dim = 2;
			eat_bits(fax, 1);
		}
	}
	else if (fax->k > 0 && fax->a == -1)
	{
		fax->a = 0;
		if ((fax->word >> (32 - 1)) == 1)
			fax->dim = 1;
		else
			fax->dim = 2;
		eat_bits(fax, 1);
	}
	else if (fax->dim == 1)
	{
		fax->eolc = 0;
		dec1d(/*stm->ctx, */fax);
	}
	else if (fax->dim == 2)
	{
		fax->eolc = 0;
		dec2d(/*stm->ctx, */fax);
	}

	/* no eol check after makeup codes nor in the middle of an H code */
	if (fax->stage == STATE_MAKEUP || fax->stage == STATE_H1 || fax->stage == STATE_H2)
		goto loop;

	/* check for eol conditions */
	if (fax->eolc || fax->a >= fax->columns)
	{
		if (fax->a > 0)
			goto eol;
		if (fax->eolc == (fax->k < 0 ? 2 : 6))
			goto rtc;
	}

	goto loop;

eol:
	fax->stage = STATE_EOL;

	if (fax->black_is_1)
	{
		buffer.Write( fax->rp, fax->wp - fax->rp );
// 		while (fax->rp < fax->wp /*&& p < ep*/)
// 		{
// 			tmp = *fax->rp++;
// 		}

	}
	else
	{
		buffer.Write( fax->rp, fax->wp - fax->rp );
// 
// 		while (fax->rp < fax->wp /*&& p < ep*/)
// 		{
// 			tmpByte = *fax->rp++/* ^ 0xff*/;
// 			buffer.Write( &tmpByte, 1 );
// 		}
	}

// 	if (fax->rp < fax->wp)
// 		return p - buf;

	tmp = fax->ref;
	fax->ref = fax->dst;
	fax->dst = tmp;
	memset(fax->dst, 0, fax->stride);

	fax->rp = fax->dst;
	fax->wp = fax->dst + fax->stride;

	fax->stage = STATE_NORMAL;
	fax->c = 0;
	fax->a = -1;
	fax->ridx ++;

	if (!fax->end_of_block && fax->rows)
	{
		if (fax->ridx >= fax->rows)
			goto rtc;
	}

	/* we have not read dim from eol, make a guess */
	if (fax->k > 0 && !fax->eolc && fax->a == -1)
	{
		if (fax->ridx % fax->k == 0)
			fax->dim = 1;
		else
			fax->dim = 2;
	}

	/* if end_of_line & encoded_byte_align, EOLs are *not* optional */
	if (fax->encoded_byte_align)
	{
		if (fax->end_of_line)
			eat_bits(fax, (12 - fax->bidx) & 7);
		else
			eat_bits(fax, (8 - fax->bidx) & 7);
	}

	/* no more space in output, don't decode the next row yet */
// 	if (p == buf + len)
// 		return p - buf;

	goto loop;

rtc:
	fax->stage = STATE_DONE;
	/*return p - buf;*/
}

static void fz_opj_error_callback(const char *msg, void *client_data)
{
	//fz_context *ctx = (fz_context *)client_data;
	//fz_warn(ctx, "openjpeg error: %s", msg);
}

static void fz_opj_warning_callback(const char *msg, void *client_data)
{
	//fz_context *ctx = (fz_context *)client_data;
	//fz_warn(ctx, "openjpeg warning: %s", msg);
}

static void fz_opj_info_callback(const char *msg, void *client_data)
{
	/* fz_warn("openjpeg info: %s", msg); */
}

CHE_PDF_JPXFilter::CHE_PDF_JPXFilter( CHE_Allocator * pAllocator /*= NULL*/ )
    : CHE_PDF_Filter(pAllocator)
{   
}

CHE_PDF_JPXFilter::~CHE_PDF_JPXFilter()
{   
}

HE_VOID	CHE_PDF_JPXFilter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
}

HE_VOID	CHE_PDF_JPXFilter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
    if ( !pData || length <= 2 )
    {
        return;
    }
    
	opj_event_mgr_t     evtmgr;
	opj_dparameters_t   params;
	opj_dinfo_t *       info = NULL;
	opj_cio_t *         cio = NULL;
	opj_image_t *       jpx = NULL;
    CODEC_FORMAT        format;

	unsigned char *p;

	int a, n, w, h, depth, sgnd;
	int x, y, k, v;
    
	/* Check for SOC marker -- if found we have a bare J2K stream */
	if ( pData[0] == 0xFF && pData[1] == 0x4F )
    {
		format = CODEC_J2K;
    }else{
		format = CODEC_JP2;
    }
    
	memset(&evtmgr, 0, sizeof(evtmgr));
	evtmgr.error_handler = fz_opj_error_callback;
	evtmgr.warning_handler = fz_opj_warning_callback;
	evtmgr.info_handler = fz_opj_info_callback;
    
	opj_set_default_decoder_parameters( &params );
	
    //if (indexed) ???
	{
        params.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
    }
    
	info = opj_create_decompress( format );
    
	opj_set_event_mgr( (opj_common_ptr)info, &evtmgr, NULL );
	opj_setup_decoder( info, &params );
    
	cio = opj_cio_open( (opj_common_ptr)info, pData, length );
    
	jpx = opj_decode( info, cio );
    
	opj_cio_close( cio );
	opj_destroy_decompress( info );
    
	if ( !jpx )
    {
        //decode error!!!
        return;
    }
    
	for (k = 1; k < jpx->numcomps; k++)
	{
		if (jpx->comps[k].w != jpx->comps[0].w)
		{
			opj_image_destroy(jpx);
			//fz_throw(ctx, "image components have different width");
		}
		if (jpx->comps[k].h != jpx->comps[0].h)
		{
			opj_image_destroy(jpx);
			//fz_throw(ctx, "image components have different height");
		}
		if (jpx->comps[k].prec != jpx->comps[0].prec)
		{
			opj_image_destroy(jpx);
			//fz_throw(ctx, "image components have different precision");
		}
	}
    
	n = jpx->numcomps;
	w = jpx->comps[0].w;
	h = jpx->comps[0].h;
	depth = jpx->comps[0].prec;
	sgnd = jpx->comps[0].sgnd;
    
	if (jpx->color_space == CLRSPC_SRGB && n == 4) { n = 3; a = 1; }
	else if (jpx->color_space == CLRSPC_SYCC && n == 4) { n = 3; a = 1; }
	else if (n == 2) { n = 1; a = 1; }
	else if (n > 4) { n = 4; a = 1; }
	else { a = 0; }
    
    buffer.Clear();
    
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			for (k = 0; k < n + a; k++)
			{
				v = jpx->comps[k].data[y * w + x];
				if (sgnd)
					v = v + (1 << (depth - 1));
				if (depth > 8)
					v = v >> (depth - 8);
				buffer.Write( (HE_LPBYTE)&v, 1 );
			}
			if (!a)
                v = 255;
                buffer.Write( (HE_LPBYTE)&v, 1 );
		}
	}
    
/*	if (a)
	{
		if (n == 4)
		{
			fz_pixmap *tmp = fz_new_pixmap(ctx, fz_device_rgb, w, h);
			fz_convert_pixmap(ctx, tmp, img);
			fz_drop_pixmap(ctx, img);
			img = tmp;
		}
		fz_premultiply_pixmap(ctx, img);
	}*/
 
	opj_image_destroy( jpx );
}

CHE_PDF_JBig2Filter::CHE_PDF_JBig2Filter( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Filter(pAllocator), mGlobalsParam(NULL), mGlobalsParamLength(0)
{
}

CHE_PDF_JBig2Filter::~CHE_PDF_JBig2Filter()
{
}

HE_VOID CHE_PDF_JBig2Filter::Encode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
}

HE_VOID CHE_PDF_JBig2Filter::Decode( HE_LPBYTE pData, HE_ULONG length, CHE_DynBuffer & buffer )
{
	if ( pData == NULL || length == 0 )
	{
		return;
	}

	Jbig2Ctx *ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, NULL, NULL, NULL );
	Jbig2GlobalCtx *gctx = NULL;
	Jbig2Image *page = NULL;

	if ( mGlobalsParam )
	{
		jbig2_data_in( ctx, mGlobalsParam, mGlobalsParamLength );
		gctx = jbig2_make_global_ctx( ctx );
		ctx = jbig2_ctx_new( NULL, JBIG2_OPTIONS_EMBEDDED, gctx, NULL, NULL );
	}
    
	if ( !page )
	{
        jbig2_data_in( ctx, pData, length );
		jbig2_complete_page( ctx );
		page = jbig2_page_out( ctx );
        //if page = NULL error happened!
	}
    
    buffer.Clear();
    buffer.Write( page->data, page->height * page->stride );

	HE_LPBYTE p = buffer.GetData();
	HE_LPBYTE ep = buffer.GetData() + length;
	HE_LPBYTE s = page->data;
	HE_INT32 w = page->height * page->stride;
	HE_INT32 x = 0;

	while (p < ep && x < w)
	{
		*p++ = s[x++] ^ 0xff;
	}

    if ( page )
    {
		jbig2_release_page( ctx, page );
    }
	if ( gctx )
	{
		jbig2_global_ctx_free( gctx );
	}
	if ( ctx )
	{
		jbig2_ctx_free( ctx );
	}
}