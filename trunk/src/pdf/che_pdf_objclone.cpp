#include "../../include/pdf/che_pdf_objclone.h"

bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum == data2.mObjNumbers.genNum )
	{
		return true;
	}
	return false;
}

bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if (	( data1.mObjNumbers.objNum > data2.mObjNumbers.objNum ) ||
			( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum > data2.mObjNumbers.genNum ) )
	{
		return true;
	}
	return false;
}

bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if (	( data1.mObjNumbers.objNum < data2.mObjNumbers.objNum ) ||
			( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum < data2.mObjNumbers.genNum ) )
	{
		return true;
	}
	return false;
}

CHE_PDF_Array* CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( pArray == NULL || pFile == NULL )
	{
		return NULL;
	}
	CHE_PDF_Array * pNewArray = CHE_PDF_Array::Create( pFile->GetAllocator() );
	CHE_PDF_Object * pTmpObj = NULL;
	for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )	{
		pTmpObj = pArray->GetElement( i );
		switch ( pTmpObj->GetType() )
		{
		case OBJ_TYPE_NULL:
			pNewArray->Append( CHE_PDF_Null::Create( pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_BOOLEAN:
			pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->ToBoolean()->GetValue(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_NUMBER:
			if ( pTmpObj->ToNumber()->IsInteger() )
			{
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger(), pFile->GetAllocator() ) );
			}else{
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat(), pFile->GetAllocator() ) );
			}
			break;
		case OBJ_TYPE_STRING:
			pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_NAME:
			pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_ARRAY:
			pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->ToReference(), pFile, pMgr );
				pNewArray->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, pTmpObj->ToReference()->GetFile(), pFile->GetAllocator() ) );
				break;
			}
		default:
			break;
		}
	}
	return pNewArray;
}

CHE_PDF_Dictionary* CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr )
{
	if ( pDict == NULL || pFile == NULL )
	{
		return NULL;
	}

	CHE_PDF_Dictionary * pNewDict = CHE_PDF_Dictionary::Create( pFile->GetAllocator() );
	CHE_PDF_Object * pTmpObj = NULL;
	CHE_ByteString key;
	for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
	{
		pTmpObj = pDict->GetElementByIndex( i );
		pDict->GetKeyByIndex( i, key );
		switch ( pTmpObj->GetType() )
		{
		case OBJ_TYPE_NULL:
			pNewDict->SetAtNull( key );
			break;
		case OBJ_TYPE_BOOLEAN:
			pNewDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
			break;
		case OBJ_TYPE_NUMBER:
			if ( pTmpObj->ToNumber()->IsInteger() )
			{
				pNewDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
			}else{
				pNewDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
			}
			break;
		case OBJ_TYPE_STRING:
			pNewDict->SetAtString( key, pTmpObj->ToString()->GetString() );
			break;
		case OBJ_TYPE_NAME:
			pNewDict->SetAtName( key, pTmpObj->ToName()->GetString() );
			break;
		case OBJ_TYPE_ARRAY:
			pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->ToReference(), pFile, pMgr );
				pNewDict->SetAtReference( key, refInfo.objNum, refInfo.genNum, pTmpObj->ToReference()->GetFile() );
				break;
			}
		default:
			break;
		}
	}
	return pNewDict;
}

HE_PDF_RefInfo CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr/* = NULL */ )
{
	HE_PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;
	if ( pRefObj == NULL || pFile == NULL )
	{
		return refInfo;
	}
	refInfo.objNum = pRefObj->GetRefNum();
	refInfo.genNum = pRefObj->GetGenNum();
	if ( pMgr != NULL && pMgr->IsCloned( refInfo ) == TRUE )
	{
		return pMgr->GetMapObjNum( refInfo );
	}
	CHE_PDF_ObjectCollector objCollector;
	CHE_PDF_Object * pObj = pRefObj->GetRefObj( objCollector );
	CHE_PDF_IndirectObject * pInObj = NULL;
	HE_PDF_RefInfo newRefInfo;
	switch ( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			pInObj = pFile->CreateInObj_Null();
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			pInObj = pFile->CreateInObj_Boolean( pObj->ToBoolean()->GetValue() );
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			if ( pObj->ToNumber()->IsInteger() )
			{
				pInObj = pFile->CreateInObj_Number( pObj->ToNumber()->GetInteger() );
			}else{
				pInObj = pFile->CreateInObj_Number( pObj->ToNumber()->GetFloat() );
			}
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			pInObj = pFile->CreateInObj_String( pObj->ToString()->GetString() );
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			pInObj = pFile->CreateInObj_Name( pObj->ToName()->GetString() );
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			pInObj = pFile->CreateInObj_Array();
			CHE_PDF_Array * pNewArray = pInObj->GetObj()->ToArray();
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			CHE_PDF_Array * pArray = pObj->ToArray();
			CHE_PDF_Object * pTmpObj = NULL;
			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
			{
				pTmpObj = pArray->GetElement( i );
				switch ( pTmpObj->GetType() )
				{
				case OBJ_TYPE_NULL:
					pNewArray->Append( CHE_PDF_Null::Create() );
					break;
				case OBJ_TYPE_BOOLEAN:
					pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->ToBoolean()->GetValue() ) );
					break;
				case OBJ_TYPE_NUMBER:
					if ( pTmpObj->ToNumber()->IsInteger() )
					{
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger(), pFile->GetAllocator() ) );
					}else{
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat(), pFile->GetAllocator() ) );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_NAME:
					pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_ARRAY:
					pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->ToReference(), pFile, pMgr );
						pNewArray->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, pTmpObj->ToReference()->GetFile(), pFile->GetAllocator() ) );
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	case OBJ_TYPE_DICTIONARY:
		{
			pInObj = pFile->CreateInObj_Dict();
			CHE_PDF_Dictionary * pNewDict = pInObj->GetObj()->ToDict();
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			CHE_PDF_Dictionary * pDict = pObj->ToDict();
			CHE_PDF_Object * pTmpObj = NULL;
			CHE_ByteString key;
			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
			{
				pTmpObj = pDict->GetElementByIndex( i );
				pDict->GetKeyByIndex( i, key );
				switch ( pTmpObj->GetType() )
				{
				case OBJ_TYPE_NULL:
					pNewDict->SetAtNull( key );
					break;
				case OBJ_TYPE_BOOLEAN:
					pNewDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
					break;
				case OBJ_TYPE_NUMBER:
					if ( pTmpObj->ToNumber()->IsInteger() )
					{
						pNewDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
					}else{
						pNewDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewDict->SetAtString( key, pTmpObj->ToString()->GetString() );
					break;
				case OBJ_TYPE_NAME:
					pNewDict->SetAtName( key, pTmpObj->ToName()->GetString() );
					break;
				case OBJ_TYPE_ARRAY:
					pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->ToReference(), pFile, pMgr );
						pNewDict->SetAtReference( key, refInfo.objNum, refInfo.genNum, pTmpObj->ToReference()->GetFile() );
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			pInObj = pFile->CreateInObj_Stream();
			CHE_PDF_Stream * pNewStream = pInObj->GetObj()->ToStream();
			newRefInfo.objNum = pInObj->GetObjNum();
			newRefInfo.genNum = pInObj->GetGenNum();
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			CHE_PDF_Dictionary * pNewDict = CloneDirectDictObj( pObj->ToStream()->GetDict(), pFile, pMgr );
			pNewStream->SetDict( pNewDict );
			HE_LPBYTE pbuffer = new unsigned char[pObj->ToStream()->GetRawSize()];
			pObj->ToStream()->GetRawData( 0, pbuffer, pObj->ToStream()->GetRawSize() );
			pNewStream->SetRawData( pbuffer, pObj->ToStream()->GetRawSize() );
			delete [] pbuffer;
			pbuffer = NULL;
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			newRefInfo = CloneIndirectObject( pObj->ToReference(), pFile, pMgr );
			if ( pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	default:
		break;
	}
	return newRefInfo;
}