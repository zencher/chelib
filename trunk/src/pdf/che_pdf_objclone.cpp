// #include "../../include/pdf/che_pdf_objclone.h"
// 
// bool operator == ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
// {
// 	if ( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum == data2.mObjNumbers.genNum )
// 	{
// 		return true;
// 	}
// 	return false;
// }
// 
// bool operator > ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
// {
// 	if (	( data1.mObjNumbers.objNum > data2.mObjNumbers.objNum ) ||
// 			( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum > data2.mObjNumbers.genNum ) )
// 	{
// 		return true;
// 	}
// 	return false;
// }
// 
// bool operator < ( const ObjectCloneMgrData & data1, const ObjectCloneMgrData & data2 )
// {
// 	if (	( data1.mObjNumbers.objNum < data2.mObjNumbers.objNum ) ||
// 			( data1.mObjNumbers.objNum == data2.mObjNumbers.objNum && data1.mObjNumbers.genNum < data2.mObjNumbers.genNum ) )
// 	{
// 		return true;
// 	}
// 	return false;
// }
// 
// CHE_PDF_ArrayPtr CloneDirectArrayObj( const CHE_PDF_ArrayPtr & pArray, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
// {
// 	if ( ! pArray || pFile == NULL )
// 	{
// 		return CHE_PDF_ArrayPtr();
// 	}
// 	CHE_PDF_ArrayPtr pNewArray = CHE_PDF_Array::Create( pFile->GetAllocator() );
// 	CHE_PDF_ObjectPtr pTmpObj;
// 	for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )	{
// 		pTmpObj = pArray->GetElement( i );
// 		switch ( pTmpObj->GetType() )
// 		{
// 		case OBJ_TYPE_NULL:
// 			pNewArray->Append( CHE_PDF_Null::Create( pFile->GetAllocator() ) );
// 			break;
// 		case OBJ_TYPE_BOOLEAN:
// 			pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->GetBooleanPtr()->GetValue(), pFile->GetAllocator() ) );
// 			break;
// 		case OBJ_TYPE_NUMBER:
// 			if ( pTmpObj->GetNumberPtr()->IsInteger() )
// 			{
// 				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->GetNumberPtr()->GetInteger(), pFile->GetAllocator() ) );
// 			}else{
// 				pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->GetNumberPtr()->GetFloat(), pFile->GetAllocator() ) );
// 			}
// 			break;
// 		case OBJ_TYPE_STRING:
// 			pNewArray->Append( CHE_PDF_String::Create( pTmpObj->GetStringPtr()->GetStringPtr(), pFile->GetAllocator() ) );
// 			break;
// 		case OBJ_TYPE_NAME:
// 			pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->GetNamePtr()->GetStringPtr(), pFile->GetAllocator() ) );
// 			break;
// 		case OBJ_TYPE_ARRAY:
// 			pNewArray->Append( CloneDirectArrayObj( pTmpObj->GetArrayPtr(), pFile, pMgr ) );
// 			break;
// 		case OBJ_TYPE_DICTIONARY:
// 			pNewArray->Append( CloneDirectDictObj( pTmpObj->GetDictPtr(), pFile, pMgr ) );
// 			break;
// 		case OBJ_TYPE_REFERENCE:
// 			{
// 				PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->GetRefPtr(), pFile, pMgr );
// 				pNewArray->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, pTmpObj->GetReference()->GetFile(), pFile->GetAllocator() ) );
// 				break;
// 			}
// 		default:
// 			break;
// 		}
// 	}
// 	return pNewArray;
// }
// 
// CHE_PDF_DictionaryPtr CloneDirectDictObj( const CHE_PDF_DictionaryPtr & pDict, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr )
// {
// 	if ( ! pDict || pFile == NULL )
// 	{
// 		return CHE_PDF_DictionaryPtr();
// 	}
// 
// 	CHE_PDF_DictionaryPtr pNewDict = CHE_PDF_Dictionary::Create( pFile->GetAllocator() );
// 	CHE_PDF_ObjectPtr pTmpObj;
// 	CHE_ByteString key;
// 	for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
// 	{
// 		pTmpObj = pDict->GetElementByIndex( i );
// 		pDict->GetKeyByIndex( i, key );
// 		switch ( pTmpObj->GetType() )
// 		{
// 		case OBJ_TYPE_NULL:
// 			pNewDict->SetAtNull( key );
// 			break;
// 		case OBJ_TYPE_BOOLEAN:
// 			pNewDict->SetAtBoolean( key, pTmpObj->GetBooleanPtr()->GetValue() );
// 			break;
// 		case OBJ_TYPE_NUMBER:
// 			if ( pTmpObj->GetNumberPtr()->IsInteger() )
// 			{
// 				pNewDict->SetAtInteger( key, pTmpObj->GetNumberPtr()->GetInteger() );
// 			}else{
// 				pNewDict->SetAtFloatNumber( key, pTmpObj->GetNumberPtr()->GetFloat() );
// 			}
// 			break;
// 		case OBJ_TYPE_STRING:
// 			pNewDict->SetAtString( key, pTmpObj->GetStringPtr()->GetStringPtr() );
// 			break;
// 		case OBJ_TYPE_NAME:
// 			pNewDict->SetAtName( key, pTmpObj->GetNamePtr()->GetStringPtr() );
// 			break;
// 		case OBJ_TYPE_ARRAY:
// 			pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->GetArrayPtr(), pFile, pMgr ) );
// 			break;
// 		case OBJ_TYPE_DICTIONARY:
// 			pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->GetDictPtr(), pFile, pMgr ) );
// 			break;
// 		case OBJ_TYPE_REFERENCE:
// 			{
// 				PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->GetRefPtr(), pFile, pMgr );
// 				pNewDict->SetAtReference( key, refInfo.objNum, refInfo.genNum, pTmpObj->GetReference()->GetFile() );
// 				break;
// 			}
// 		default:
// 			break;
// 		}
// 	}
// 	return pNewDict;
// }
// 
// PDF_RefInfo CloneIndirectObject( const CHE_PDF_ReferencePtr & pRefObj, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr/* = NULL */ )
// {
// 	PDF_RefInfo refInfo;
// 	refInfo.objNum = 0;
// 	refInfo.genNum = 0;
// 	if ( pRefObj == NULL || pFile == NULL )
// 	{
// 		return refInfo;
// 	}
// 	refInfo.objNum = pRefObj->GetRefNum();
// 	refInfo.genNum = pRefObj->GetGenNum();
// 	if ( pMgr != NULL && pMgr->IsCloned( refInfo ) == TRUE )
// 	{
// 		return pMgr->GetMapObjNum( refInfo );
// 	}
// 	CHE_PDF_ObjectPtr pObj = pRefObj->GetRefObj();
// 	CHE_PDF_IndirectObject * pInObj = NULL;
// 	PDF_RefInfo newRefInfo;
// 	switch ( pObj->GetType() )
// 	{
// 	case OBJ_TYPE_NULL:
// 		{
// 			pInObj = pFile->CreateInObj_Null();
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_BOOLEAN:
// 		{
// 			pInObj = pFile->CreateInObj_Boolean( pObj->GetBooleanPtr()->GetValue() );
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_NUMBER:
// 		{
// 			if ( pObj->GetNumberPtr()->IsInteger() )
// 			{
// 				pInObj = pFile->CreateInObj_Number( pObj->GetNumberPtr()->GetInteger() );
// 			}else{
// 				pInObj = pFile->CreateInObj_Number( pObj->GetNumberPtr()->GetFloat() );
// 			}
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_STRING:
// 		{
// 			pInObj = pFile->CreateInObj_String( pObj->GetStringPtr()->GetStringPtr() );
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_NAME:
// 		{
// 			pInObj = pFile->CreateInObj_Name( pObj->GetNamePtr()->GetStringPtr() );
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_ARRAY:
// 		{
// 			pInObj = pFile->CreateInObj_Array();
// 			CHE_PDF_Array * pNewArray = pInObj->GetObj()->GetArrayPtr();
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			CHE_PDF_ArrayPtr pArray = pObj->GetArrayPtr();
// 			CHE_PDF_ObjectPtr pTmpObj;
// 			for ( HE_DWORD i = 0; i < pArray->GetCount(); i++ )
// 			{
// 				pTmpObj = pArray->GetElement( i );
// 				switch ( pTmpObj->GetType() )
// 				{
// 				case OBJ_TYPE_NULL:
// 					pNewArray->Append( CHE_PDF_Null::Create() );
// 					break;
// 				case OBJ_TYPE_BOOLEAN:
// 					pNewArray->Append( CHE_PDF_Boolean::Create( pTmpObj->GetBooleanPtr()->GetValue() ) );
// 					break;
// 				case OBJ_TYPE_NUMBER:
// 					if ( pTmpObj->GetNumberPtr()->IsInteger() )
// 					{
// 						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->GetNumberPtr()->GetInteger(), pFile->GetAllocator() ) );
// 					}else{
// 						pNewArray->Append( CHE_PDF_Number::Create( pTmpObj->GetNumberPtr()->GetFloat(), pFile->GetAllocator() ) );
// 					}
// 					break;
// 				case OBJ_TYPE_STRING:
// 					pNewArray->Append( CHE_PDF_String::Create( pTmpObj->GetStringPtr()->GetStringPtr(), pFile->GetAllocator() ) );
// 					break;
// 				case OBJ_TYPE_NAME:
// 					pNewArray->Append( CHE_PDF_Name::Create( pTmpObj->GetNamePtr()->GetStringPtr(), pFile->GetAllocator() ) );
// 					break;
// 				case OBJ_TYPE_ARRAY:
// 					pNewArray->Append( CloneDirectArrayObj( pTmpObj->GetArrayPtr(), pFile, pMgr ) );
// 					break;
// 				case OBJ_TYPE_DICTIONARY:
// 					pNewArray->Append( CloneDirectDictObj( pTmpObj->GetDictPtr(), pFile, pMgr ) );
// 					break;
// 				case OBJ_TYPE_REFERENCE:
// 					{
// 						PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->GetRefPtr(), pFile, pMgr );
// 						pNewArray->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, pTmpObj->GetReference()->GetFile(), pFile->GetAllocator() ) );
// 						break;
// 					}
// 				default:
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_DICTIONARY:
// 		{
// 			pInObj = pFile->CreateInObj_Dict();
// 			CHE_PDF_Dictionary * pNewDict = pInObj->GetObj()->GetDictPtr();
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			CHE_PDF_DictionaryPtr pDict = pObj->GetDictPtr();
// 			CHE_PDF_ObjectPtr pTmpObj;
// 			CHE_ByteString key;
// 			for ( HE_DWORD i = 0; i < pDict->GetCount(); i++ )
// 			{
// 				pTmpObj = pDict->GetElementByIndex( i );
// 				pDict->GetKeyByIndex( i, key );
// 				switch ( pTmpObj->GetType() )
// 				{
// 				case OBJ_TYPE_NULL:
// 					pNewDict->SetAtNull( key );
// 					break;
// 				case OBJ_TYPE_BOOLEAN:
// 					pNewDict->SetAtBoolean( key, pTmpObj->GetBooleanPtr()->GetValue() );
// 					break;
// 				case OBJ_TYPE_NUMBER:
// 					if ( pTmpObj->GetNumberPtr()->IsInteger() )
// 					{
// 						pNewDict->SetAtInteger( key, pTmpObj->GetNumberPtr()->GetInteger() );
// 					}else{
// 						pNewDict->SetAtFloatNumber( key, pTmpObj->GetNumberPtr()->GetFloat() );
// 					}
// 					break;
// 				case OBJ_TYPE_STRING:
// 					pNewDict->SetAtString( key, pTmpObj->GetStringPtr()->GetStringPtr() );
// 					break;
// 				case OBJ_TYPE_NAME:
// 					pNewDict->SetAtName( key, pTmpObj->GetNamePtr()->GetStringPtr() );
// 					break;
// 				case OBJ_TYPE_ARRAY:
// 					pNewDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->GetArrayPtr(), pFile, pMgr ) );
// 					break;
// 				case OBJ_TYPE_DICTIONARY:
// 					pNewDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->GetDictPtr(), pFile, pMgr ) );
// 					break;
// 				case OBJ_TYPE_REFERENCE:
// 					{
// 						PDF_RefInfo refInfo = CloneIndirectObject( pTmpObj->GetRefPtr(), pFile, pMgr );
// 						pNewDict->SetAtReference( key, refInfo.objNum, refInfo.genNum, pTmpObj->GetReference()->GetFile() );
// 						break;
// 					}
// 				default:
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	case OBJ_TYPE_STREAM:
// 		{
// 			pInObj = pFile->CreateInObj_Stream();
// 			CHE_PDF_Stream * pNewStream = pInObj->GetObj()->GetStream();
// 			newRefInfo.objNum = pInObj->GetObjNum();
// 			newRefInfo.genNum = pInObj->GetGenNum();
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			CHE_PDF_DictionaryPtr pNewDict = CloneDirectDictObj( pObj->GetStreamPtr()->GetDictPtr(), pFile, pMgr );
// 			pNewStream->SetDict( pNewDict );
// 			HE_LPBYTE pbuffer = new unsigned char[pObj->GetStream()->GetRawSize()];
// 			pObj->GetStream()->GetRawData( 0, pbuffer, pObj->GetStream()->GetRawSize() );
// 			pNewStream->SetRawData( pbuffer, pObj->GetStream()->GetRawSize() );
// 			delete [] pbuffer;
// 			pbuffer = NULL;
// 			break;
// 		}
// 	case OBJ_TYPE_REFERENCE:
// 		{
// 			newRefInfo = CloneIndirectObject( pObj->GetRefPtr(), pFile, pMgr );
// 			if ( pMgr )
// 			{
// 				pMgr->SetMap( refInfo, newRefInfo );
// 			}
// 			break;
// 		}
// 	default:
// 		break;
// 	}
// 	return newRefInfo;
// }