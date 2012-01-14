#include "../../include/pdf/che_pdf_objclone.h"

bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum == data2.mObjNum ) return true;
	return false;
}

bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum > data2.mObjNum ) return true;
	return false;
}

bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
{
	if ( data1.mObjNum < data2.mObjNum ) return true;
	return false;
}

CHE_PDF_Array* CloneDirectArrayObj( CHE_PDF_Array * pArray, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( pArray == NULL || creator == NULL )
	{
		return NULL;
	}
	CHE_PDF_Array * pNewArray = CHE_PDF_Array::Create();
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
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger() ) );
			}else{
				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat() ) );
			}
			break;
		case OBJ_TYPE_STRING:
			pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString() ) );
			break;
		case OBJ_TYPE_NAME:
			pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString() ) );
			break;
		case OBJ_TYPE_ARRAY:
			pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), creator, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), creator, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
				pNewArray->Append( CHE_PDF_Reference::Create( refNum, pTmpObj->ToReference()->GetParser() ) );
				break;
			}
		default:
			break;
		}
	}
	return pNewArray;
}

CHE_PDF_Dictionary* CloneDirectDictObj( CHE_PDF_Dictionary * pDict, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr )
{
	if ( pDict == NULL || creator == NULL )
	{
		return NULL;
	}

	CHE_PDF_Dictionary * pNewDict = CHE_PDF_Dictionary::Create();
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
			pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), creator, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), creator, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
				pNewDict->SetAtReference( key, refNum, pTmpObj->ToReference()->GetParser() );
				break;
			}
		default:
			break;
		}
	}
	return pNewDict;
}

HE_DWORD CloneIndirectObject( CHE_PDF_Reference * pRefObj, CHE_PDF_Creator * creator, IHE_ObjectCloneMgr * pMgr/* = NULL */ )
{
	if ( pRefObj == NULL || creator == NULL )
	{
		return 0;
	}
	if ( pMgr != NULL && pMgr->IsCloned( pRefObj->GetRefNum() ) == TRUE )
	{
		return pMgr->GetMapObjNum( pRefObj->GetRefNum() );
	}
	CHE_PDF_Object * pObj = pRefObj->GetRefObj();
	HE_DWORD newObject = 0;
	switch ( pObj->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			newObject = creator->AppendIndirectObj_Null()->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			newObject = creator->AppendIndirectObj_Boolean( pObj->ToBoolean()->GetValue() )->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			if ( pObj->ToNumber()->IsInteger() )
			{
				newObject = creator->AppendIndirectObj_Number( pObj->ToNumber()->GetInteger() )->GetObjNum();
			}else{
				newObject = creator->AppendIndirectObj_Number( pObj->ToNumber()->GetFloat() )->GetObjNum();
			}
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			newObject = creator->AppendIndirectObj_String( pObj->ToString()->GetString() )->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			newObject = creator->AppendIndirectObj_Name( pObj->ToName()->GetString() )->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_IndirectObject * pInArrayObj  = creator->AppendIndirectObj_Array();
			CHE_PDF_Array * pNewArray = pInArrayObj->GetObj()->ToArray();
			newObject = pInArrayObj->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
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
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetInteger() ) );
					}else{
						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->ToNumber()->GetFloat() ) );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewArray->Append( CHE_PDF_String::Create( pTmpObj->ToString()->GetString() ) );
					break;
				case OBJ_TYPE_NAME:
					pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->ToName()->GetString() ) );
					break;
				case OBJ_TYPE_ARRAY:
					pNewArray->Append( CloneDirectArrayObj( pTmpObj->ToArray(), creator ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewArray->Append( CloneDirectDictObj( pTmpObj->ToDict(), creator )  );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
						pNewArray->Append( CHE_PDF_Reference::Create( refNum, pTmpObj->ToReference()->GetParser() ) );
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
			CHE_PDF_IndirectObject * pInDictObj  = creator->AppendIndirectObj_Dict();
			CHE_PDF_Dictionary * pNewDict = pInDictObj->GetObj()->ToDict();
			newObject = pInDictObj->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
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
					pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), creator, pMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), creator, pMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), creator, pMgr );
						pNewDict->SetAtReference( key, refNum, pTmpObj->ToReference()->GetParser() );
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
			CHE_PDF_IndirectObject * pInStreamObj = creator->AppendIndirectObj_Stream();
			CHE_PDF_Stream * pNewStream = pInStreamObj->GetObj()->ToStream();
			newObject = pInStreamObj->GetObjNum();
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			CHE_PDF_Dictionary * pNewDict = CloneDirectDictObj( pObj->ToStream()->GetDict(), creator, pMgr );
			pNewStream->SetDict( pNewDict );
			HE_LPBYTE pbuffer = new unsigned char[pObj->ToStream()->GetRawSize()];
			pObj->ToStream()->GetRawData( 0, pbuffer, pObj->ToStream()->GetRawSize() );
			pNewStream->SetRawData( pbuffer, pObj->ToStream()->GetRawSize() );
			delete [] pbuffer;
			pbuffer = NULL;
			newObject = pNewStream->GetObjNum();
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			newObject = CloneIndirectObject( pObj->ToReference(), creator, pMgr );
			if ( pMgr )
			{
				pMgr->SetMap( pRefObj->GetRefNum(), newObject );
			}
			break;
		}
	default:
		break;
	}
	return newObject;
}