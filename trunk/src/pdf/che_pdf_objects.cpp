#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_objects.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_file.h"
#include <memory.h>

CHE_PDF_Object * CHE_PDF_Object::Clone()
{
	CHE_PDF_Object * objRet = NULL;
	switch ( mType )
	{
	case OBJ_TYPE_NULL:
		objRet = GetAllocator()->New<CHE_PDF_Null>( GetAllocator() );
		break;
	case OBJ_TYPE_BOOLEAN:
		objRet = ToBoolean()->Clone();
		break;
	case OBJ_TYPE_NUMBER:
		objRet = ToNumber()->Clone();
		break;
	case OBJ_TYPE_STRING:
		objRet = ToString()->Clone();
		break;
	case OBJ_TYPE_NAME:
		objRet = ToName()->Clone();
		break;
	case OBJ_TYPE_ARRAY:
		objRet = ToArray()->Clone();
		break;
	case OBJ_TYPE_DICTIONARY:
		objRet = ToDict()->Clone();
		break;
	case OBJ_TYPE_REFERENCE:
		objRet = ToReference()->Clone();
		break;
	case OBJ_TYPE_STREAM:
		objRet = ToStream()->Clone();
		break;
	case OBJ_TYPE_INVALID:
	default:
		objRet = GetAllocator()->New<CHE_PDF_Object>( GetAllocator() );
		break;
	}
	return objRet;
}

HE_VOID CHE_PDF_Object::Release()
{
	switch ( mType )
	{
	case OBJ_TYPE_NULL:
		GetAllocator()->Delete<CHE_PDF_Null>( (CHE_PDF_Null*)this );
		break;
	case OBJ_TYPE_BOOLEAN:
		GetAllocator()->Delete<CHE_PDF_Boolean>( (CHE_PDF_Boolean*)this );
		break;
	case OBJ_TYPE_NUMBER:
		GetAllocator()->Delete<CHE_PDF_Number>( (CHE_PDF_Number*)this );
		break;
	case OBJ_TYPE_STRING:
		GetAllocator()->Delete<CHE_PDF_String>( (CHE_PDF_String*)this );
		break;
	case OBJ_TYPE_NAME:
		GetAllocator()->Delete<CHE_PDF_Name>( (CHE_PDF_Name*)this );
		break;
	case OBJ_TYPE_ARRAY:
		GetAllocator()->Delete<CHE_PDF_Array>( (CHE_PDF_Array*)this );
		break;
	case OBJ_TYPE_DICTIONARY:
		GetAllocator()->Delete<CHE_PDF_Dictionary>( (CHE_PDF_Dictionary*)this );
		break;
	case OBJ_TYPE_REFERENCE:
		GetAllocator()->Delete<CHE_PDF_Reference>( (CHE_PDF_Reference*)this );
		break;
	case OBJ_TYPE_STREAM:
		GetAllocator()->Delete<CHE_PDF_Stream>( (CHE_PDF_Stream*)this );
		break;
	case OBJ_TYPE_INVALID:
	default:
		GetAllocator()->Delete<CHE_PDF_Object>( this );
		break;
	}
}


CHE_PDF_Boolean * CHE_PDF_Object::ToBoolean() const
{
	if ( mType == OBJ_TYPE_BOOLEAN )
	{
		return (CHE_PDF_Boolean*)this;
	}
	return NULL;
}

CHE_PDF_Number * CHE_PDF_Object::ToNumber() const
{
	if ( mType == OBJ_TYPE_NUMBER )
	{
		return (CHE_PDF_Number*)this;
	}
	return NULL;
}

CHE_PDF_String * CHE_PDF_Object::ToString() const
{
	if ( mType == OBJ_TYPE_STRING )
	{
		return (CHE_PDF_String*)this;
	}
	return NULL;
}

CHE_PDF_Name * CHE_PDF_Object::ToName() const
{
	if ( mType == OBJ_TYPE_NAME )
	{
		return (CHE_PDF_Name*)this;
	}
	return NULL;
}

CHE_PDF_Array * CHE_PDF_Object::ToArray() const
{
	if ( mType == OBJ_TYPE_ARRAY )
	{
		return (CHE_PDF_Array*)this;
	}
	return NULL;
}

CHE_PDF_Dictionary * CHE_PDF_Object::ToDict() const
{
	if ( mType == OBJ_TYPE_DICTIONARY )
	{
		return (CHE_PDF_Dictionary*)this;
	}
	return NULL;
}

CHE_PDF_Reference * CHE_PDF_Object::ToReference() const
{
	if ( mType == OBJ_TYPE_REFERENCE )
	{
		return (CHE_PDF_Reference*)this;
	}
	return NULL;
}

CHE_PDF_Stream *	CHE_PDF_Object::ToStream() const
{
	if ( mType == OBJ_TYPE_STREAM )
	{
		return (CHE_PDF_Stream*)this;
	}
	return NULL;
}

CHE_PDF_Object * CHE_PDF_Reference::GetRefObj( CHE_PDF_ObjectCollector & objCollector )
{
	if ( mpFile == NULL )
	{
		return NULL;
	}
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = mRefObjNum;
	refInfo.genNum = mRefGenNum;
	CHE_PDF_Object * pObj = mpFile->GetObject( refInfo );
	objCollector.Push( pObj );
	return pObj;
}

CHE_PDF_Object * CHE_PDF_Reference::GetRefObj( PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector )
{
	if ( mpFile == NULL )
	{
		return NULL;
	}
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = mRefObjNum;
	refInfo.genNum = mRefGenNum;
	CHE_PDF_Object * pCurObj = mpFile->GetObject( refInfo );
	if ( pCurObj == NULL )
	{
		return NULL;
	}
	objCollector.Push( pCurObj );
	if ( pCurObj->GetType() == Type || Type == OBJ_TYPE_INVALID )
	{
		return pCurObj;
	}

	while ( TRUE )
	{
		if ( pCurObj->GetType() == OBJ_TYPE_REFERENCE )
		{
			pCurObj = pCurObj->ToReference()->GetRefObj( objCollector );
			if ( pCurObj == NULL )
			{
				return NULL;
			}
			if ( pCurObj->GetType() == Type )
			{
				return pCurObj;
			}
			if ( pCurObj->GetType() == OBJ_TYPE_REFERENCE || pCurObj->GetType() == OBJ_TYPE_ARRAY )
			{
				continue;
			}
			return NULL;
		}else if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_Object * pElement = NULL;
			HE_DWORD dwCount = pCurObj->ToArray()->GetCount();
			HE_BOOL bNeedContinue = FALSE;
			for ( HE_DWORD i = 0; i < dwCount; i++ )
			{
				pElement = pCurObj->ToArray()->GetElement( i, Type, objCollector );
				if ( pElement == NULL )
				{
					continue;
				}
				if ( pElement->GetType() == Type )
				{
					return pElement;
				}else if ( pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_ARRAY )
				{
					pCurObj = pElement;
					bNeedContinue = TRUE;
					break;
				}
			}
			if ( bNeedContinue == TRUE )
			{
				continue;
			}
			return NULL;
		}else if ( pCurObj->GetType() == Type )
		{
			return pCurObj;
		}else{
			return NULL;
		}
	}
	return NULL;
}

CHE_PDF_Array::~CHE_PDF_Array()
{
	CHE_PDF_Object * pElement = NULL;
	for ( HE_DWORD i = 0; i < mArray.GetCount(); i++ )
	{
		pElement = (CHE_PDF_Object *)mArray.GetItem( i );
		if ( pElement )
		{
			pElement->Release();
		}
	}
	mArray.Clear();
}

CHE_PDF_Object * CHE_PDF_Array::GetElement( HE_DWORD index ) const
{
	return (CHE_PDF_Object*)( mArray.GetItem( index ) );
}

CHE_PDF_Object* CHE_PDF_Array::GetElement( HE_DWORD index, PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector )
{
	CHE_PDF_Object * pCurObj = GetElement( index );
	if ( pCurObj == NULL )
	{
		return NULL;
	}
	PDF_OBJ_TYPE curType = pCurObj->GetType();

	if ( curType == Type || Type == OBJ_TYPE_INVALID )
	{
		return pCurObj;
	}

	if ( pCurObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		pCurObj = pCurObj->ToReference()->GetRefObj( Type, objCollector );
		return pCurObj;
	}else if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
	{
		CHE_PDF_Object * pElement = NULL;
		HE_DWORD dwCount = pCurObj->ToArray()->GetCount();
		for ( HE_DWORD i = 0; i < dwCount; i++ )
		{
			pElement = GetElement( i, Type, objCollector );
			if ( pElement != NULL )
			{
				return pElement;
			}
		}
		return NULL;
	}else{
		return NULL;
	}
}

CHE_PDF_Object* CHE_PDF_Array::GetElementByType( PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector )
{
	CHE_PDF_Object * pElement = NULL;
	HE_DWORD lCount = mArray.GetCount();
	for ( HE_DWORD i = 0; i < lCount; i++ )
	{
		pElement = GetElement( i, Type, objCollector );
		if ( pElement != NULL )
		{
			return pElement;
		}
	}
	return NULL;
}

HE_BOOL CHE_PDF_Array::Append( CHE_PDF_Object * pObj )
{
	if ( pObj )
	{
		return mArray.Append( (HE_LPVOID)pObj );
	}
	return FALSE;
}

// HE_BOOL * CHE_PDF_Array::Insert( HE_DWORD index, CHE_PDF_Object * pObj )
// {
// 	return FALSE;
// }
// 
// CHE_PDF_Object * CHE_PDF_Array::Replace( HE_DWORD index, CHE_PDF_Object * pObj )
// {
// 	return FALSE;
// }
// 
// HE_BOOL CHE_PDF_Array::Remove( HE_DWORD index )
// {
// 	return NULL;
// }

CHE_PDF_Array * CHE_PDF_Array::Clone()
{
	CHE_PDF_Array * objRet = GetAllocator()->New<CHE_PDF_Array>( GetAllocator() );
	if ( objRet )
	{
		CHE_PDF_Object * tmpObj = NULL;
		for ( HE_DWORD i = 0; i < GetCount(); ++i )
		{
			tmpObj = GetElement( i );
			if ( tmpObj )
			{
				tmpObj = tmpObj->Clone();
			}
			objRet->Append( tmpObj );
		}
	}
	return objRet;
}

CHE_PDF_Dictionary::~CHE_PDF_Dictionary()
{
	CHE_PDF_Object * pElement = NULL;
	for ( HE_DWORD i = 0; i < m_Map.GetCount(); i++ )
	{
		pElement = (CHE_PDF_Object *)( m_Map.GetItemByIndex( i ) );
		if ( pElement )
		{
			pElement->Release();
		}
	}
}

CHE_PDF_Object*	CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key )const
{
	return (CHE_PDF_Object*)( m_Map.GetItem( key ) );
}

CHE_PDF_Object*	CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector )
{
	CHE_PDF_Object * pCurObj = NULL;
	pCurObj = (CHE_PDF_Object*)m_Map.GetItem( key );
	if ( pCurObj == NULL )
	{
		return NULL;
	}
	PDF_OBJ_TYPE curType = pCurObj->GetType();
	if ( curType == Type || Type == OBJ_TYPE_INVALID )
	{
		return pCurObj;
	}
	if ( pCurObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		pCurObj = pCurObj->ToReference()->GetRefObj( Type, objCollector );
		return pCurObj;
	}else if ( pCurObj->GetType() == OBJ_TYPE_ARRAY )
	{
		CHE_PDF_Object * pElement = NULL;
		CHE_PDF_Array * pTmpArray = pCurObj->ToArray();
		for ( HE_DWORD i = 0; i < pTmpArray->GetCount(); ++i )
		{
			pElement = pTmpArray->GetElement( i, Type, objCollector );
			if ( pElement )
			{
				return pElement;
			}
		}
	}
	return NULL;
}

// HE_BOOL CHE_PDF_Dictionary::Replace( CHE_ByteString & key, CHE_PDF_Object * pObj )
// {
// 	return FALSE;
// }
// 
// HE_BOOL CHE_PDF_Dictionary::Remove( CHE_ByteString & key )
// {
// 	return FALSE;
// }

HE_VOID CHE_PDF_Dictionary::SetAtObj( const CHE_ByteString & key, CHE_PDF_Object * pObj )
{
	if ( key.GetLength() > 0 )
	{
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release(); 
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtNull( const CHE_ByteString & key )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Null * pObj = CHE_PDF_Null::Create( GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtBoolean( const CHE_ByteString & key, BOOL value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Boolean * pObj = CHE_PDF_Boolean::Create( value, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtInteger( const CHE_ByteString & key, HE_INT32 value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Number * pObj = CHE_PDF_Number::Create( value, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtString( const CHE_ByteString & key, const CHE_ByteString & string )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_String * pObj = CHE_PDF_String::Create( string, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtName( const CHE_ByteString & key, const CHE_ByteString & name )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Name * pObj = CHE_PDF_Name::Create( name, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPBYTE)pObj );
		}	
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtArray( const CHE_ByteString & key, CHE_PDF_Array * pArray )
{
	if ( key.GetLength() > 0 )
	{
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pArray ) && pArray )
			{
				pArray->Release();
			}		
		}else{
			m_Map.Append( key, (HE_LPVOID)pArray );
		}
	}
}

HE_VOID CHE_PDF_Dictionary::SetAtDictionary( const CHE_ByteString & key, CHE_PDF_Dictionary * pDict )
{
	if ( key.GetLength() > 0 )
	{
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pDict ) && pDict )
			{
				pDict->Release();
			}
		}else{
			m_Map.Append( key, (HE_LPVOID)pDict );
		}	
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtReference( const CHE_ByteString & key, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_File * pFile )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_Reference * pObj = CHE_PDF_Reference::Create( objNum, genNum, pFile, GetAllocator() );
		if ( m_Map.GetItem( key ) != NULL )
		{
			if ( m_Map.UpdateItem( key, (HE_LPVOID*)&pObj ) && pObj )
			{
				pObj->Release();
			}		
		}else{
			m_Map.Append( key, (HE_LPVOID)pObj );
		}
	}
}

CHE_PDF_Dictionary * CHE_PDF_Dictionary::Clone()
{
	CHE_PDF_Dictionary * objRet = GetAllocator()->New<CHE_PDF_Dictionary>( GetAllocator() );
	if ( objRet )
	{
		CHE_PDF_Object * tmpObj = NULL;
		CHE_ByteString keyStr( GetAllocator() );
		for ( HE_DWORD i = 0; i < GetCount(); ++i )
		{
			GetKeyByIndex( i, keyStr );
			tmpObj = GetElementByIndex( i );
			if ( tmpObj )
			{
				tmpObj = tmpObj->Clone();
			}
			objRet->SetAtObj( keyStr, tmpObj );
		}
	}
	return objRet;
}

CHE_PDF_Stream::CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict,
								HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( pAllocator )
{
	m_pEncrypt = pEncrypt;
	mType = OBJ_TYPE_STREAM;
	m_pDataBuf = NULL;
	m_dwSize = size;
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
	}else{
		m_pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	m_pDict->SetAtInteger( "Length", m_dwSize );
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict,
								HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object(  pAllocator )
{
	m_pEncrypt = pEncrypt;
 	m_dwObjNum = objNum;
 	m_dwGenNum = genNum;
	mType = OBJ_TYPE_STREAM;
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
	}else{
		m_pDict = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	m_pDict->SetAtInteger( "Length", m_dwSize );
}

CHE_PDF_Stream::CHE_PDF_Stream(	HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object(pAllocator)
{
	m_pEncrypt = pEncrypt;
 	m_dwObjNum = objNum;
 	m_dwGenNum = genNum;
	mType = OBJ_TYPE_STREAM;
	m_pDataBuf = NULL;
	m_dwSize = 0;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = NULL;
	m_pDict = NULL;
	m_FileOffset = 0;
	m_dwSize = 0;
	m_pDataBuf = NULL;
}

CHE_PDF_Stream::~CHE_PDF_Stream()
{
 	if ( m_bMem == TRUE && m_pDataBuf )
 	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_pDataBuf );
 	}
	if ( m_pDict )
	{
		m_pDict->Release();
	}
}

CHE_PDF_Stream * CHE_PDF_Stream::Clone()
{
	CHE_PDF_Stream * objRet = NULL;
	CHE_PDF_Dictionary * pDict = NULL;
	if ( m_pDict )
	{
		pDict = m_pDict->Clone();
	}
	if ( m_bMem )
	{
		HE_LPBYTE pDataBuf = GetAllocator()->NewArray<HE_BYTE>( m_dwSize );
		objRet = GetAllocator()->New<CHE_PDF_Stream>( pDataBuf, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}else
	{
		objRet = GetAllocator()->New<CHE_PDF_Stream>( m_pFile, m_FileOffset, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}
	return objRet;
}

HE_BOOL	CHE_PDF_Stream::SetDict( CHE_PDF_Dictionary * pDict )
{
	if ( pDict == NULL )
	{
		return FALSE;
	}
	if ( m_pDict != NULL )
	{
		m_pDict->Release();
	}
	m_pDict = pDict;
	return TRUE;
}

HE_BOOL CHE_PDF_Stream::SetRawData( HE_LPBYTE pData, HE_DWORD dwDataSize, HE_BYTE filter/* = STREAM_FILTER_NULL*/ )
{
	if ( pData == NULL || dwDataSize == 0 )
	{
		return FALSE;
	}
	if ( m_bMem == FALSE )
	{
		m_bMem = TRUE;
		m_pFile = NULL;
	}else{
		if ( m_pDataBuf != NULL )
		{
			GetAllocator()->DeleteArray<HE_BYTE>( m_pDataBuf );
			m_pDataBuf = NULL;
			m_dwSize = 0;
		}
	}
	if ( m_pDict == NULL )
	{
		m_pDict = CHE_PDF_Dictionary::Create( NULL );
	}
	switch( filter )
	{
	case STREAM_FILTER_NULL:
		{
			m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( dwDataSize );
			memcpy( m_pDataBuf, pData, dwDataSize );
			m_dwSize = dwDataSize;
			break;
		}
	case STREAM_FILTER_HEX:
		{
			CHE_DynBuffer buf;
			CHE_PDF_HexFilter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				m_pDict->SetAtName( "Filter", "ASCIIHexDecode" );
			}
			break;
		}
	case STREAM_FILTER_ASCII85:
		{
			CHE_DynBuffer buf;
			CHE_PDF_ASCII85Filter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				m_pDict->SetAtName( "Filter", "ASCII85Decode" );
			}
			break;
		}
	case STREAM_FILTER_FLATE:
		{
			CHE_DynBuffer buf;
			CHE_PDF_FlateFilter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				m_pDict->SetAtName( "Filter", "FlateDecode" );
			}
			break;
		}
	case STREAM_FILTER_LZW:
		{
			CHE_DynBuffer buf;
			CHE_PDF_LZWFilter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				m_pDict->SetAtName( "Filter", "LZWDecode" );
			}
			break;
		}
	case STREAM_FILTER_RLE:
		{
			CHE_DynBuffer buf;
			CHE_PDF_RLEFileter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				m_pDict->SetAtName( "Filter", "RunLengthDecode" );
			}
			break;
		}
	default:
		break;
	}
	m_pDict->SetAtInteger( "Length", m_dwSize );
	return TRUE;
}

HE_DWORD CHE_PDF_Stream::GetRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const
{
	if ( pBuf == NULL || buf_size == 0 || offset >= m_dwSize )
 	{
 		return 0;
 	}
	if ( m_bMem == FALSE )
	{
		buf_size = m_pFile->ReadBlock( pBuf, offset + m_FileOffset, buf_size );
		if ( m_pEncrypt && m_pEncrypt->IsPasswordOK() )
		{
			m_pEncrypt->Decrypt( pBuf, buf_size, GetObjNum(), GetGenNum() );
		}
		return buf_size;
	}else{
		if ( offset + buf_size > m_dwSize )
 		{
			buf_size = m_dwSize - offset;
 		}
		memcpy( pBuf, m_pDataBuf + offset, buf_size );
		if ( m_pEncrypt && m_pEncrypt->IsPasswordOK() )
		{
			m_pEncrypt->Decrypt( pBuf, buf_size, GetObjNum(), GetGenNum() );
		}
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

		HE_BOOL retValue = TRUE;
		
		CHE_PDF_Dictionary * pDict = pStream->GetDict();
		if ( pDict )
		{
			HE_DWORD lFilterCount = 0;
			HE_DWORD length = pStream->GetRawSize();
			CHE_PDF_Object * pFilter = pDict->GetElement( "Filter" );
			CHE_PDF_Object * pParms = pDict->GetElement( "DecodeParms" );
			if ( pFilter == NULL )
			{
				m_dwSize = length;
				m_pData = GetAllocator()->NewArray<HE_BYTE>( length );
				pStream->GetRawData( 0, m_pData, length );
				if ( pStream->m_pEncrypt && pStream->m_pEncrypt->IsPasswordOK() == TRUE )
				{
					pStream->m_pEncrypt->Decrypt( m_pData, length, pStream->GetObjNum(), pStream->GetGenNum() );
				}
				return TRUE;
			}else{
				if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
				{
					lFilterCount = pFilter->ToArray()->GetCount();
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
			if ( pFilter->GetType() == OBJ_TYPE_NAME )
			{
				pFilterNameArr[0] = (CHE_PDF_Name*)pFilter;
			}else if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
			{
				for ( HE_DWORD i = 0; i < lFilterCount; i++ )
				{
					pFilterNameArr[i] = pFilter->ToArray()->GetElement( i )->ToName();
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
			}else if ( pParms->GetType() == OBJ_TYPE_DICTIONARY )
			{
				pParamDictArr[0] = pParms->ToDict();
			}else if ( pParms->GetType() == OBJ_TYPE_ARRAY ){
				HE_DWORD lParamCount = pParms->ToArray()->GetCount();
				for ( HE_DWORD i = 0; i < lParamCount; i++ )
				{
					pParamDictArr[i] = pParms->ToArray()->GetElement( i )->ToDict();
				}
			}
			HE_DWORD bufSize = (length == 0) ? 1024 : length;
			CHE_DynBuffer buffer( bufSize, bufSize, GetAllocator() );
			HE_DWORD lSize = pStream->GetRawSize();
			HE_LPBYTE pTmp = NULL;
			pTmp = GetAllocator()->NewArray<HE_BYTE>( lSize );

			pStream->GetRawData( 0, pTmp, lSize );
			CHE_ByteString str( GetAllocator() );
			for ( HE_DWORD i = 0; i < lFilterCount; i++ )
			{
				str = pFilterNameArr[i]->GetString();
				if ( str == "ASCIIHexDecode" || str == "AHx" )
				{
					CHE_PDF_HexFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else if ( str == "ASCII85Decode" || str == "A85" )
				{
					CHE_PDF_ASCII85Filter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else if ( str == "LZWDecode" || str == "LZW" )
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
						CHE_PDF_Object * pObj = pDecodeParams->GetElement( "Predictor" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Predictor = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Colors" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Colors = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "BitsPerComponent" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							BitsPerComponent = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Columns" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Columns = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "EarlyChange" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							EarlyChange = pObj->ToNumber()->GetInteger();
						}
						CHE_PDF_Predictor pPredictor( Predictor, Colors, BitsPerComponent, Columns, EarlyChange );
						CHE_PDF_LZWFilter filter( &pPredictor, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}
				}else if ( str == "FlateDecode" || str == "Fl" )
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
						CHE_PDF_Object * pObj = pDecodeParams->GetElement( "Predictor" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Predictor = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Colors" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Colors = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "BitsPerComponent" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							BitsPerComponent = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Columns" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Columns = pObj->ToNumber()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "EarlyChange" );
						if ( pObj != NULL && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							EarlyChange = pObj->ToNumber()->GetInteger();
						}
						CHE_PDF_Predictor pPredictor( Predictor, Colors, BitsPerComponent, Columns, EarlyChange );
						CHE_PDF_FlateFilter filter( &pPredictor, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}
				}else if ( str == "RunLengthDecode" || str == "RL" )
				{
					CHE_PDF_RLEFileter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else if ( str == "CCITTFaxDecode" || str == "CCF" )
				{
					/*CHE_PDF_HexFilter filter;
					filter.Decode( pTmp, lSize, buffer );
					m_pData = new HE_BYTE[buffer.GetSize()];
					buffer.Read( m_pData, buffer.GetSize() );*/
					retValue = FALSE;
				}else if ( str == "JBIG2Decode" )
				{
					/*CHE_PDF_HexFilter filter;
					filter.Decode( pTmp, lSize, buffer );
					m_pData = new HE_BYTE[buffer.GetSize()];
					buffer.Read( m_pData, buffer.GetSize() );*/
					retValue = FALSE;
				}else if ( str == "DCTDecode" || str == "DCT" )
				{
					/*CHE_PDF_HexFilter filter;
					filter.Decode( pTmp, lSize, buffer );
					m_pData = new HE_BYTE[buffer.GetSize()];
					buffer.Read( m_pData, buffer.GetSize() );*/
					retValue = FALSE;
				}else if ( str == "JPXDecode" )
				{
					/*CHE_PDF_HexFilter filter;
					filter.Decode( pTmp, lSize, buffer );
					m_pData = new HE_BYTE[buffer.GetSize()];
					buffer.Read( m_pData, buffer.GetSize() );*/
					retValue = FALSE;
				}else if ( str == "Crypt" )
				{
					/*CHE_PDF_HexFilter filter;
					filter.Decode( pTmp, lSize, buffer );
					m_pData = new HE_BYTE[buffer.GetSize()];
					buffer.Read( m_pData, buffer.GetSize() );*/
					retValue = FALSE;
				}
			}

			lSize = buffer.GetSize();
			GetAllocator()->DeleteArray<HE_BYTE>( pTmp );
			pTmp = GetAllocator()->NewArray<HE_BYTE>( lSize );
			GetAllocator()->DeleteArray<CHE_PDF_Name*>( pFilterNameArr );
			GetAllocator()->DeleteArray<CHE_PDF_Dictionary*>( pParamDictArr );
			buffer.Read( pTmp, lSize );

			m_pData = pTmp;
			m_dwSize = lSize;

			return retValue;
		}else{
			m_dwSize = pStream->GetRawSize();
			m_pData = GetAllocator()->NewArray<HE_BYTE>( m_dwSize );
			pStream->GetRawData( 0, m_pData, m_dwSize );			
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


CHE_PDF_ObjectCollector::CHE_PDF_ObjectCollector( CHE_Allocator * pAllocator )
	: CHE_Object ( pAllocator ), mQueue( pAllocator ), mInQueue( pAllocator )
{
}

CHE_PDF_ObjectCollector::~CHE_PDF_ObjectCollector()
{
	if ( ! mQueue.IsEmpty() || !mInQueue.IsEmpty() )
	{
		ReleaseAll();
	}
}

HE_VOID CHE_PDF_ObjectCollector::Push( CHE_PDF_Object * pObj )
{
	if ( pObj )
	{
		mQueue.Push( pObj );
	}
}

HE_VOID CHE_PDF_ObjectCollector::Push( CHE_PDF_IndirectObject * pInObj )
{
	if ( pInObj )
	{
		mInQueue.Push( pInObj );
	}
}

HE_VOID CHE_PDF_ObjectCollector::PopAll()
{
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_IndirectObject * pInObj = NULL;
	while ( mQueue.Pop( pObj ) ){}
	while ( mInQueue.Pop( pInObj ) ){}
}

HE_VOID CHE_PDF_ObjectCollector::ReleaseAll()
{
	CHE_PDF_Object * pObj = NULL;
	CHE_PDF_IndirectObject * pInObj = NULL;
	while ( mQueue.Pop( pObj ) )
	{
		if ( pObj )
		{
			pObj->Release();
			pObj = NULL;
		}
	}
	while ( mInQueue.Pop( pInObj ) )
	{
		if ( pInObj )
		{
			pInObj->Release();
			pInObj = NULL;
		}
	}
}