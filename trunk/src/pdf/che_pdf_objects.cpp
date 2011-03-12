#include "../../include/pdf/che_pdf_objects.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/che_dynBuffer.h"
#include <memory.h>

HE_VOID CHE_PDF_Object::Release()
{
	switch ( m_Type )
	{
	case PDFOBJ_INVALID:
		delete this;
		break;
	case PDFOBJ_BOOLEAN:
		delete ((CHE_PDF_Boolean*)this);
		break;
	case PDFOBJ_NUMBER:
		delete ((CHE_PDF_Number*)this);
		break;
	case PDFOBJ_STRING:
		delete ((CHE_PDF_String*)this);
		break;
	case PDFOBJ_NAME:
		delete ((CHE_PDF_Name*)this);
		break;
	case PDFOBJ_ARRAY:
		delete ((CHE_PDF_Array*)this);
		break;
	case PDFOBJ_DICTIONARY:
		delete ((CHE_PDF_Dictionary*)this);
		break;
	case PDFOBJ_STREAM:
		delete ((CHE_PDF_Stream*)this);
		break;
	case PDFOBJ_NULL:
		delete ((CHE_PDF_Null*)this);
		break;
	case PDFOBJ_REFERENCE:
		delete ((CHE_PDF_Array*)this);
		break;
	case PDFOBJ_INDIRECTOBJ:
		delete ((CHE_PDF_IndirectObject*)this);
		break;
	default:
		delete this;
		break;
	}
}

HE_VOID CHE_PDF_Array::Append( CHE_PDF_Object * pObj )
{
	if ( pObj )
	{
		m_array.Append( (HE_LPVOID)pObj );
	}
}

CHE_PDF_Object * CHE_PDF_Array::GetElement( HE_DWORD index ) const
{
	return (CHE_PDF_Object*)m_array.GetItem( index );
}

CHE_PDF_Object*	CHE_PDF_Dictionary::GetElement( CHE_ByteString & key )const
{
	return (CHE_PDF_Object*)m_Map.GetItem( key );
}

HE_VOID	CHE_PDF_Dictionary::SetAtBoolean( CHE_ByteString & key, bool value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtInteger( CHE_ByteString & key, HE_INT32 value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtFloatNumber( CHE_ByteString & key, HE_FLOAT value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtString( CHE_ByteString & key, const CHE_ByteString& string )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_String * pObj = CHE_PDF_String::Create( string );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtName( CHE_ByteString & key, const CHE_ByteString& name )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Name * pObj = CHE_PDF_Name::Create( name );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtArray( CHE_ByteString & key, CHE_PDF_Array * pArray )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pArray );
	}
}

HE_VOID CHE_PDF_Dictionary::SetAtDictionary( CHE_ByteString & key, CHE_PDF_Dictionary * pDict )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pDict );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtReference( CHE_ByteString & key, HE_DWORD objnum )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Reference * pObj = CHE_PDF_Reference::Create( objnum );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

CHE_PDF_Stream::CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict )
{
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = NULL;
	if ( pData != NULL && size != 0 )
	{
		m_pDataBuf = new HE_BYTE[size];
		memcpy( m_pDataBuf, pData, size );
	}
	if ( pDict )
	{
		m_pDict = pDict;
	}
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_FileRead* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict )
{
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = FALSE;
	m_FileOffset = 0;
	m_pFile = NULL;
	if ( pFile != NULL )
	{
		m_pFile = pFile;
		m_FileOffset = offset;
		m_dwSize = size;
	}
	if ( pDict )
	{
		m_pDict = pDict;
	}
}

HE_DWORD CHE_PDF_Stream::ReadRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const
{
	if ( pBuf == NULL || buf_size == 0 || offset >= m_dwSize )
	{
		return 0;
	}
	if ( m_bMem == TRUE )
	{
		if ( offset + buf_size > m_dwSize )
		{
			buf_size = m_dwSize - offset;
		}
		memcpy( m_pDataBuf + offset, pBuf, buf_size );
		return buf_size;
	}else{
		return m_pFile->ReadBlock( pBuf, offset, buf_size );
	}
}

CHE_PDF_StreamAcc::CHE_PDF_StreamAcc()
{
	m_dwSize = 0;
	m_pData = NULL;
	m_pStream = NULL;
}

CHE_PDF_StreamAcc::~CHE_PDF_StreamAcc()
{
	if ( m_pData )
	{
		delete [] m_pData;
	}
	m_dwSize = 0;
}

HE_BOOL CHE_PDF_StreamAcc::Attach( const CHE_PDF_Stream * pStream )
{
	if ( pStream )
	{
		m_pData = NULL;
		m_dwSize = 0;
		m_pStream = pStream;
		
		CHE_PDF_Dictionary * pDict = pStream->GetDict();
		if ( pDict )
		{
			HE_DWORD length = 0;
			CHE_PDF_Number * pNumObj = (CHE_PDF_Number *)pDict->GetElement( CHE_ByteString("Length") );
			if ( pNumObj )
			{
				length = pNumObj->GetInteger();
			}
			CHE_PDF_Object * pObj = pDict->GetElement( CHE_ByteString("Filter") );
			if ( pObj == NULL )
			{
				HE_DWORD bufSize = (length == 0) ? 1024 : length;
				CHE_DynBuffer buffer( bufSize, bufSize );
				if ( pObj->GetType() == PDFOBJ_NAME )
				{
					HE_DWORD lSize = pStream->GetRawSize();
					HE_LPBYTE pTmp = new HE_BYTE[lSize];
					pStream->ReadRawData( 0, pTmp, lSize );
					CHE_ByteString str = ((CHE_PDF_Name*)pObj)->GetString();
					if ( str == "ASCIIHexDecode" )
					{
						CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						m_dwSize = buffer.GetByteCount();
						buffer.Read( m_pData, buffer.GetByteCount() );
						return TRUE;
					}else if ( str == "ASCII85Decode" )
					{
						CHE_PDF_ASCII85Filter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						m_dwSize = buffer.GetByteCount();
						buffer.Read( m_pData, buffer.GetByteCount() );
						return TRUE;
					}else if ( str == "LZWDecode" )
					{
						CHE_PDF_LZWFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						m_dwSize = buffer.GetByteCount();
						buffer.Read( m_pData, buffer.GetByteCount() );
						return TRUE;
					}else if ( str == "FlateDecode" )
					{
						CHE_PDF_FlateFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						m_dwSize = buffer.GetByteCount();
						buffer.Read( m_pData, buffer.GetByteCount() );
						return TRUE;
					}else if ( str == "RunLengthDecode" )
					{
						CHE_PDF_RLEFileter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						m_dwSize = buffer.GetByteCount();
						buffer.Read( m_pData, buffer.GetByteCount() );
						return TRUE;
					}else if ( str == "CCITTFaxDecode" )
					{
						/*CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						buffer.Read( m_pData, buffer.GetByteCount() );*/
						return FALSE;
					}else if ( str == "JBIG2Decode" )
					{
						/*CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						buffer.Read( m_pData, buffer.GetByteCount() );*/
						return FALSE;
					}else if ( str == "DCTDecode" )
					{
						/*CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						buffer.Read( m_pData, buffer.GetByteCount() );*/
						return FALSE;
					}else if ( str == "JPXDecode" )
					{
						/*CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						buffer.Read( m_pData, buffer.GetByteCount() );*/\
						return FALSE;
					}else if ( str == "Crypt" )
					{
						/*CHE_PDF_HexFilter filter;
						filter.Decode( pTmp, lSize, buffer );
						m_pData = new HE_BYTE[buffer.GetByteCount()];
						buffer.Read( m_pData, buffer.GetByteCount() );*/
						return FALSE;
					}
				}
				return FALSE;
			}else{
				m_pData = new HE_BYTE[pStream->GetRawSize()];
				pStream->ReadRawData( 0, m_pData, pStream->GetRawSize() );
				return TRUE;
			}
		}
	}
	return FALSE;
}

HE_VOID CHE_PDF_StreamAcc::Detach()
{
	if ( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	m_pStream = NULL;
	m_dwSize = 0;
}

CHE_PDF_IndirectObject::CHE_PDF_IndirectObject( HE_DWORD objNum, CHE_PDF_Object * pObj )
{
	m_ObjNum = objNum;
	m_pObj = pObj;
}

CHE_PDF_Dictionary* CHE_PDF_IndirectObject::GetDict() const
{
	if ( m_pObj )
	{
		if ( m_pObj->GetType() == PDFOBJ_DICTIONARY  )
		{
			return (CHE_PDF_Dictionary*)m_pObj;
		}else if ( m_pObj->GetType() == PDFOBJ_STREAM )
		{
			return ((CHE_PDF_Stream*)m_pObj)->GetDict();
		}	 
	}
	return NULL;
}

CHE_PDF_Stream*	CHE_PDF_IndirectObject::GetStream() const
{
	if ( m_pObj )
	{
		 if ( m_pObj->GetType() == PDFOBJ_STREAM )
		{
			return (CHE_PDF_Stream*)m_pObj;
		}	 
	}
	return NULL;
}

CHE_PDF_Object* CHE_PDF_IndirectObject::GetObject() const
{
	return m_pObj;
}

HE_BOOL CHE_PDF_IndirectObjectCollector::Add( CHE_PDF_IndirectObject * pObj )
{
	if ( pObj == NULL )
	{
		return FALSE;
	}
	return m_map.Append( pObj->GetObjNum(), (HE_LPVOID)pObj );
}

HE_VOID CHE_PDF_IndirectObjectCollector::Release()
{
	HE_DWORD count = m_map.GetCount();
	CHE_PDF_IndirectObject * pObj = NULL;
	for ( HE_DWORD i = 0; i < count; i++ )
	{
		pObj = (CHE_PDF_IndirectObject*)m_map.GetItemByIndex( i );
		if ( pObj )
		{
			pObj->Release();
		}
	}
}

