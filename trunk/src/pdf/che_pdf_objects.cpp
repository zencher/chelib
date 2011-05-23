#include "../../include/pdf/che_pdf_objects.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/che_datastructure.h"
#include <memory.h>

HE_VOID CHE_PDF_Object::Release()
{
	switch ( m_Type )
	{
	case PDFOBJ_INVALID:
		GetAllocator()->Delete<CHE_PDF_Object>( this );
		break;
	case PDFOBJ_BOOLEAN:
		GetAllocator()->Delete<CHE_PDF_Boolean>( (CHE_PDF_Boolean*)this );
		break;
	case PDFOBJ_NUMBER:
		GetAllocator()->Delete<CHE_PDF_Number>( (CHE_PDF_Number*)this );
		break;
	case PDFOBJ_STRING:
		GetAllocator()->Delete<CHE_PDF_String>( (CHE_PDF_String*)this );
		break;
	case PDFOBJ_NAME:
		GetAllocator()->Delete<CHE_PDF_Name>( (CHE_PDF_Name*)this );
		break;
	case PDFOBJ_ARRAY:
		GetAllocator()->Delete<CHE_PDF_Array>( (CHE_PDF_Array*)this );
		break;
	case PDFOBJ_DICTIONARY:
		GetAllocator()->Delete<CHE_PDF_Dictionary>( (CHE_PDF_Dictionary*)this );
		break;
	case PDFOBJ_STREAM:
		GetAllocator()->Delete<CHE_PDF_Stream>( (CHE_PDF_Stream*)this );
		break;
	case PDFOBJ_NULL:
		GetAllocator()->Delete<CHE_PDF_Null>( (CHE_PDF_Null*)this );
		break;
	case PDFOBJ_REFERENCE:
		GetAllocator()->Delete<CHE_PDF_Reference>( (CHE_PDF_Reference*)this );
		break;
	case PDFOBJ_INDIRECTOBJ:
		GetAllocator()->Delete<CHE_PDF_IndirectObject>( (CHE_PDF_IndirectObject*)this );
		break;
	default:
		GetAllocator()->Delete<CHE_PDF_Object>( (CHE_PDF_Object*)this );
		break;
	}
}

CHE_PDF_Array::~CHE_PDF_Array()
{
	CHE_PDF_Object * pTmpObj = NULL;
	for ( HE_DWORD i = 0; i < m_array.GetCount(); i++ )
	{
		pTmpObj = (CHE_PDF_Object *)m_array.GetItem( i );
		if ( pTmpObj )
		{
			pTmpObj->Release();
		}
	}
	m_array.Clear();
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

CHE_PDF_Dictionary::~CHE_PDF_Dictionary()
{
	CHE_PDF_Object * pTmpObj = NULL;
	for ( HE_DWORD i = 0; i < m_Map.GetCount(); i++ )
	{
		pTmpObj = (CHE_PDF_Object *)m_Map.GetItemByIndex( i );
		if ( pTmpObj )
		{
			pTmpObj->Release();
		}
	}
	m_Map.Clear();
}

CHE_PDF_Object*	CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key )const
{
	return (CHE_PDF_Object*)m_Map.GetItem( key );
}

HE_VOID	CHE_PDF_Dictionary::SetAtNull( const CHE_ByteString & key )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Null * pNullOjb = CHE_PDF_Null::Create( this->GetObjNum(), this->GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pNullOjb );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtBoolean( const CHE_ByteString & key, bool value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtInteger( const CHE_ByteString & key, HE_INT32 value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtString( const CHE_ByteString & key, const CHE_ByteString& string )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_String * pObj = CHE_PDF_String::Create( string, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtName( const CHE_ByteString & key, const CHE_ByteString& name )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Name * pObj = CHE_PDF_Name::Create( name, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtArray( const CHE_ByteString & key, CHE_PDF_Array * pArray )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pArray );
	}
}

HE_VOID CHE_PDF_Dictionary::SetAtDictionary( const CHE_ByteString & key, CHE_PDF_Dictionary * pDict )
{
	if ( key.GetLength() > 0 )
	{
		m_Map.Append( key, (HE_LPBYTE)pDict );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtReference( const CHE_ByteString & key, HE_DWORD objnum )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Reference * pObj = CHE_PDF_Reference::Create( objnum, GetObjNum(), GetGenNum(), GetAllocator() );
		m_Map.Append( key, (HE_LPBYTE)pObj );
	}
}

CHE_PDF_Stream::CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt, CHE_Allocator * pAllocator )
:CHE_PDF_Object( pAllocator )
{
	m_pEncrypt = pEncrypt;
	m_ObjNum = objNum;
	m_GenNum = genNum;
	m_Type = PDFOBJ_STREAM;
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = NULL;
	if ( pData != NULL && size != 0 )
	{
		m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( size );
		memcpy( m_pDataBuf, pData, size );
	}
	if ( pDict )
	{
		m_pDict = pDict;
	}
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum,
								CHE_PDF_Encrypt * pEncrypt, CHE_Allocator * pAllocator ) : CHE_PDF_Object( pAllocator )
{
	m_pEncrypt = pEncrypt;
	m_ObjNum = objNum;
	m_GenNum = genNum;
	m_Type = PDFOBJ_STREAM;
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

CHE_PDF_Stream::~CHE_PDF_Stream()
{
	if ( m_pDict )
	{
		m_pDict->Release();
	}
}

HE_DWORD CHE_PDF_Stream::ReadRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const
{
	if ( pBuf == NULL || buf_size == 0 || offset >= m_dwSize )
 	{
 		return 0;
 	}
	if ( m_bMem == FALSE )
	{
		return m_pFile->ReadBlock( pBuf, offset + m_FileOffset, buf_size );
	}else{
		if ( offset + buf_size > m_dwSize )
 		{
			buf_size = m_dwSize - offset;
 		}
		memcpy( m_pDataBuf + offset, pBuf, buf_size );
 		return buf_size;
	}
}

CHE_PDF_StreamAcc::CHE_PDF_StreamAcc( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator )
{
	m_dwSize = 0;
	m_pData = NULL;
	m_pStream = NULL;
}

CHE_PDF_StreamAcc::~CHE_PDF_StreamAcc()
{
	if ( m_pData )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_pData );
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
			HE_DWORD lFilterCount = 0;
			HE_DWORD length = pStream->GetRawSize();
			CHE_PDF_Object * pFilter = pDict->GetElement( CHE_ByteString("Filter") );
			CHE_PDF_Object * pParms = pDict->GetElement( CHE_ByteString("DecodeParms") );
			if ( pFilter == NULL )
			{
				m_dwSize = length;
				m_pData = GetAllocator()->NewArray<HE_BYTE>( length );
				pStream->ReadRawData( 0, m_pData, length );
				if ( pStream->m_pEncrypt && pStream->m_pEncrypt->IsPasswordOK() == TRUE )
				{
					pStream->m_pEncrypt->Decrypt( m_pData, length, pStream->GetObjNum(), pStream->GetGenNum() );
				}
				return TRUE;
			}else{
				if ( pFilter->GetType() == PDFOBJ_ARRAY )
				{
					lFilterCount = ((CHE_PDF_Array*)pFilter)->GetCount();
				}else{
					lFilterCount = 1;
				}
			}
			CHE_PDF_Name ** pFilterNameArr = NULL;
			CHE_PDF_Dictionary ** pParamDictArr = NULL;
	
			pFilterNameArr = GetAllocator()->NewArray<CHE_PDF_Name*>( lFilterCount );
			pParamDictArr = GetAllocator()->NewArray<CHE_PDF_Dictionary*>( lFilterCount );

			for ( HE_DWORD j = 0; j < lFilterCount; j++ )
			{
				pFilterNameArr[j] = NULL;
				pParamDictArr[j] = NULL;
			}
			if ( pFilter->GetType() == PDFOBJ_NAME )
			{
				pFilterNameArr[0] = (CHE_PDF_Name*)pFilter;
			}else if ( pFilter->GetType() == PDFOBJ_ARRAY )
			{
				for ( HE_DWORD i = 0; i < lFilterCount; i++ )
				{
					pFilterNameArr[i] = (CHE_PDF_Name*)(((CHE_PDF_Array*)pFilter)->GetElement( i ));
				}
			}else{
				return FALSE;
			}
			if ( pParms == NULL )
			{
				for ( HE_DWORD i = 0; i < lFilterCount; i++ )
				{
					pParamDictArr[i] = NULL;
				}
			}else if ( pParms->GetType() == PDFOBJ_DICTIONARY )
			{
				pParamDictArr[0] = (CHE_PDF_Dictionary*)pParms;
			}else if ( pParms->GetType() == PDFOBJ_ARRAY ){
				HE_DWORD lParamCount = ((CHE_PDF_Array*)pParms)->GetCount();
				for ( HE_DWORD i = 0; i < lParamCount; i++ )
				{
					pParamDictArr[i] = (CHE_PDF_Dictionary*)((CHE_PDF_Array*)pParms)->GetElement( i );
				}
			}
			HE_DWORD bufSize = (length == 0) ? 1024 : length;
			CHE_DynBuffer buffer( bufSize, bufSize, GetAllocator() );
			HE_DWORD lSize = pStream->GetRawSize();
			HE_LPBYTE pTmp = NULL;
			pTmp = GetAllocator()->NewArray<HE_BYTE>( lSize );

			pStream->ReadRawData( 0, pTmp, lSize );
			if ( pStream->m_pEncrypt && pStream->m_pEncrypt->IsPasswordOK() == TRUE )
			{
				pStream->m_pEncrypt->Decrypt( pTmp, lSize, pStream->GetObjNum(), pStream->GetGenNum() );
			}
			CHE_ByteString str( GetAllocator() );
			for ( HE_DWORD i = 0; i < lFilterCount; i++ )
			{
				str = pFilterNameArr[i]->GetString();
				if ( str == "ASCIIHexDecode" )
				{
					CHE_PDF_HexFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else if ( str == "ASCII85Decode" )
				{
					CHE_PDF_ASCII85Filter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else if ( str == "LZWDecode" )
				{
					CHE_PDF_Dictionary * pDecodeParams = pParamDictArr[i];
					if ( pDecodeParams == NULL )
					{
						CHE_PDF_LZWFilter filter( NULL, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}else{
						HE_BYTE Predictor = 1;
						HE_BYTE Colors = 1;
						HE_BYTE BitsPerComponent = 8;
						HE_BYTE Columns = 8;
						HE_BYTE EarlyChange = 1;
						CHE_PDF_Object * pObj = pDecodeParams->GetElement( CHE_ByteString("Predictor") );
						if ( pObj != NULL )
						{
							Predictor = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("Colors") );
						if ( pObj != NULL )
						{
							Colors = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("BitsPerComponent") );
						if ( pObj != NULL )
						{
							BitsPerComponent = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("Columns") );
						if ( pObj != NULL )
						{
							Columns = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("EarlyChange") );
						if ( pObj != NULL )
						{
							EarlyChange = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						CHE_PDF_Predictor pPredictor( Predictor, Colors, BitsPerComponent, Columns, EarlyChange );
						CHE_PDF_LZWFilter filter( &pPredictor, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}
				}else if ( str == "FlateDecode" )
				{
					CHE_PDF_Dictionary * pDecodeParams = pParamDictArr[i];
					if ( pDecodeParams == NULL )
					{
						CHE_PDF_FlateFilter filter( NULL, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}else{
						HE_BYTE Predictor = 1;
						HE_BYTE Colors = 1;
						HE_BYTE BitsPerComponent = 8;
						HE_BYTE Columns = 8;
						HE_BYTE EarlyChange = 1;
						CHE_PDF_Object * pObj = pDecodeParams->GetElement( CHE_ByteString("Predictor") );
						if ( pObj != NULL )
						{
							Predictor = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("Colors") );
						if ( pObj != NULL )
						{
							Colors = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("BitsPerComponent") );
						if ( pObj != NULL )
						{
							BitsPerComponent = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("Columns") );
						if ( pObj != NULL )
						{
							Columns = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						pObj = pDecodeParams->GetElement( CHE_ByteString("EarlyChange") );
						if ( pObj != NULL )
						{
							EarlyChange = ((CHE_PDF_Number*)pObj)->GetInteger();
						}
						CHE_PDF_Predictor pPredictor( Predictor, Colors, BitsPerComponent, Columns, EarlyChange );
						CHE_PDF_FlateFilter filter( &pPredictor, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}
				}else if ( str == "RunLengthDecode" )
				{
					CHE_PDF_RLEFileter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
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
					buffer.Read( m_pData, buffer.GetByteCount() );*/
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

			lSize = buffer.GetByteCount();
			GetAllocator()->DeleteArray<HE_BYTE>( pTmp );
			pTmp = GetAllocator()->NewArray<HE_BYTE>( lSize );
			GetAllocator()->DeleteArray<CHE_PDF_Name*>( pFilterNameArr );
			GetAllocator()->DeleteArray<CHE_PDF_Dictionary*>( pParamDictArr );
			buffer.Read( pTmp, lSize );

			m_pData = pTmp;
			m_dwSize = lSize;
			return TRUE;
		}
	}
	return FALSE;
}

HE_VOID CHE_PDF_StreamAcc::Detach()
{
	if ( m_pData )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_pData );
		m_pData = NULL;
	}
	m_pStream = NULL;
	m_dwSize = 0;
}

CHE_PDF_IndirectObject::CHE_PDF_IndirectObject( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Object * pObj, CHE_Allocator * pAllocator ) : CHE_PDF_Object( pAllocator )
{
	m_ObjNum = objNum;
	m_GenNum = genNum, 
	m_pObj = pObj;
	m_Type = PDFOBJ_INDIRECTOBJ;
}

CHE_PDF_IndirectObject::~CHE_PDF_IndirectObject()
{
	if ( m_pObj )
	{
		m_pObj->Release();
	}
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

HE_BOOL CHE_PDF_IndirectObjectCollector::Add( CHE_PDF_IndirectObject * pObj )
{
	if ( pObj == NULL )
	{
		return FALSE;
	}
	return m_map.Append( pObj->GetObjNum(), (HE_LPVOID)pObj );
}

HE_VOID CHE_PDF_IndirectObjectCollector::ReleaseObj()
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

