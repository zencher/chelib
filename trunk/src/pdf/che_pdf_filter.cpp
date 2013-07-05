#include "../../include/pdf/che_pdf_filter.h"
#include "../../extlib/zlib/zlib.h"
#include "../../extlib/jbig2dec/jbig2.h"

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