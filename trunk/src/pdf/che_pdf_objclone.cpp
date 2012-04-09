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

CHE_PDF_ArrayPtr CloneDirectArrayObj( const CHE_PDF_ArrayPtr & ArrayPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( ! ArrayPtr || pFile == NULL )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ObjectPtr ObjPtr;

	CHE_PDF_ArrayPtr NewArrayPtr = CHE_PDF_Array::Create( pFile->GetAllocator() );
	
	HE_DWORD arraySize = ArrayPtr->GetCount();

	for ( HE_DWORD i = 0; i < arraySize; ++i )
	{
		ObjPtr = ArrayPtr->GetElement( i );

		switch ( ObjPtr->GetType() )
		{
		case OBJ_TYPE_NULL:
			NewArrayPtr->Append( CHE_PDF_Null::Create( pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_BOOLEAN:
			NewArrayPtr->Append( CHE_PDF_Boolean::Create( ObjPtr->GetBooleanPtr()->GetValue(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_NUMBER:
			if ( ObjPtr->GetNumberPtr()->IsInteger() )
			{
				NewArrayPtr->Append( CHE_PDF_Number::Create( ObjPtr->GetNumberPtr()->GetInteger(), pFile->GetAllocator() ) );
			}else{
				NewArrayPtr->Append( CHE_PDF_Number::Create( ObjPtr->GetNumberPtr()->GetFloat(), pFile->GetAllocator() ) );
			}
			break;
		case OBJ_TYPE_STRING:
			NewArrayPtr->Append( CHE_PDF_String::Create( ObjPtr->GetStringPtr()->GetString(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_NAME:
			NewArrayPtr->Append( CHE_PDF_Name::Create( ObjPtr->GetNamePtr()->GetString(), pFile->GetAllocator() ) );
			break;
		case OBJ_TYPE_ARRAY:
			NewArrayPtr->Append( CloneDirectArrayObj( ObjPtr->GetArrayPtr(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			NewArrayPtr->Append( CloneDirectDictObj( ObjPtr->GetDictPtr(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				PDF_RefInfo refInfo = CloneIndirectObj( ObjPtr->GetRefPtr(), pFile, pMgr );
				NewArrayPtr->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, ObjPtr->GetRefPtr()->GetFile(), pFile->GetAllocator() ) );
				break;
			}
		default:
			break;
		}
	}
	return NewArrayPtr;
}

CHE_PDF_DictionaryPtr CloneDirectDictObj( const CHE_PDF_DictionaryPtr & DictPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( ! DictPtr || pFile == NULL )
	{
		return CHE_PDF_DictionaryPtr();
	}

	CHE_ByteString key;
	CHE_PDF_ObjectPtr ObjPtr;
	CHE_PDF_DictionaryPtr NewDictPtr = CHE_PDF_Dictionary::Create( pFile->GetAllocator() );
	
	HE_DWORD dictSize = DictPtr->GetCount();

	for ( HE_DWORD i = 0; i < dictSize; ++i )
	{
		DictPtr->GetKeyByIndex( i, key );

		ObjPtr = DictPtr->GetElementByIndex( i );

		switch ( ObjPtr->GetType() )
		{
		case OBJ_TYPE_NULL:
			NewDictPtr->SetAtNull( key );
			break;
		case OBJ_TYPE_BOOLEAN:
			NewDictPtr->SetAtBoolean( key, ObjPtr->GetBooleanPtr()->GetValue() );
			break;
		case OBJ_TYPE_NUMBER:
			if ( ObjPtr->GetNumberPtr()->IsInteger() )
			{
				NewDictPtr->SetAtInteger( key, ObjPtr->GetNumberPtr()->GetInteger() );
			}else{
				NewDictPtr->SetAtFloatNumber( key, ObjPtr->GetNumberPtr()->GetFloat() );
			}
			break;
		case OBJ_TYPE_STRING:
			NewDictPtr->SetAtString( key, ObjPtr->GetStringPtr()->GetString() );
			break;
		case OBJ_TYPE_NAME:
			NewDictPtr->SetAtName( key, ObjPtr->GetNamePtr()->GetString() );
			break;
		case OBJ_TYPE_ARRAY:
			NewDictPtr->SetAtArray( key, CloneDirectArrayObj( ObjPtr->GetArrayPtr(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_DICTIONARY:
			NewDictPtr->SetAtDictionary( key, CloneDirectDictObj( ObjPtr->GetDictPtr(), pFile, pMgr ) );
			break;
		case OBJ_TYPE_REFERENCE:
			{
				PDF_RefInfo refInfo = CloneIndirectObj( ObjPtr->GetRefPtr(), pFile, pMgr );
				NewDictPtr->SetAtReference( key, refInfo.objNum, refInfo.genNum, ObjPtr->GetRefPtr()->GetFile() );
				break;
			}
		default:
			break;
		}
	}
	return NewDictPtr;
}

PDF_RefInfo CloneIndirectObj( const CHE_PDF_ReferencePtr & RefPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /* = NULL */ )
{
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	if ( ! RefPtr || pFile == NULL )
	{
		return refInfo;
	}

	refInfo.objNum = RefPtr->GetRefNum();
	refInfo.genNum = RefPtr->GetGenNum();

	if ( pMgr && ( pMgr->IsCloned( refInfo ) == TRUE ) )
	{
		return pMgr->GetMapObjNum( refInfo );
	}

	PDF_RefInfo newRefInfo;
	CHE_PDF_ObjectPtr newObjPtr;
	CHE_PDF_ObjectPtr ObjPtr = RefPtr->GetRefObj();
	
	switch ( ObjPtr->GetType() )
	{
	case OBJ_TYPE_NULL:
		{
			CHE_PDF_NullPtr nullPtr;
			newRefInfo = pFile->CreateNullObject( nullPtr );
			if ( nullPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			CHE_PDF_BooleanPtr boolPtr;
			newRefInfo = pFile->CreateBooleanObject( boolPtr );
			if ( boolPtr && pMgr )
			{
				boolPtr->SetValue( ObjPtr->GetBooleanPtr()->GetValue() );
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_NUMBER:
		{
			CHE_PDF_NumberPtr numberPtr;
			newRefInfo = pFile->CreateNumberObject( numberPtr );
			if ( ObjPtr->GetNumberPtr()->IsInteger() )
			{	
				numberPtr->SetValue( ObjPtr->GetNumberPtr()->GetInteger() );
			}else{
				numberPtr->SetValue( ObjPtr->GetNumberPtr()->GetFloat() );
			}
			if ( numberPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_STRING:
		{
			CHE_PDF_StringPtr stringPtr;
			newRefInfo = pFile->CreateStringObject( stringPtr );
			if ( stringPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			CHE_PDF_NamePtr namePtr;
			newRefInfo = pFile->CreateNameObject( namePtr );
			if ( namePtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CHE_PDF_ArrayPtr NewArrayPtr;
			newRefInfo = pFile->CreateArrayObject( NewArrayPtr );
			if ( NewArrayPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}

			CHE_PDF_ObjectPtr tmpObjPtr;
			CHE_PDF_ArrayPtr tmpArrayPtr = ObjPtr->GetArrayPtr();

			HE_DWORD arraySize = tmpArrayPtr->GetCount();
			for ( HE_DWORD i = 0; i < arraySize; i++ )
			{
				tmpObjPtr = tmpArrayPtr->GetElement( i );
				switch ( tmpObjPtr->GetType() )
				{
				case OBJ_TYPE_NULL:
					NewArrayPtr->Append( CHE_PDF_Null::Create( pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_BOOLEAN:
					NewArrayPtr->Append( CHE_PDF_Boolean::Create( tmpObjPtr->GetBooleanPtr()->GetValue(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_NUMBER:
					if ( tmpObjPtr->GetNumberPtr()->IsInteger() )
					{
						NewArrayPtr->Append( CHE_PDF_Number::Create( tmpObjPtr->GetNumberPtr()->GetInteger(), pFile->GetAllocator() ) );
					}else{
						NewArrayPtr->Append( CHE_PDF_Number::Create( tmpObjPtr->GetNumberPtr()->GetFloat(), pFile->GetAllocator() ) );
					}
					break;
				case OBJ_TYPE_STRING:
					NewArrayPtr->Append( CHE_PDF_String::Create( tmpObjPtr->GetStringPtr()->GetString(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_NAME:
					NewArrayPtr->Append( CHE_PDF_Name::Create( tmpObjPtr->GetNamePtr()->GetString(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_ARRAY:
					NewArrayPtr->Append( CloneDirectArrayObj( tmpObjPtr->GetArrayPtr(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					NewArrayPtr->Append( CloneDirectDictObj( tmpObjPtr->GetDictPtr(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						PDF_RefInfo refInfo = CloneIndirectObj( tmpObjPtr->GetRefPtr(), pFile, pMgr );
						NewArrayPtr->Append( CHE_PDF_Reference::Create( refInfo.objNum, refInfo.genNum, tmpObjPtr->GetRefPtr()->GetFile(), pFile->GetAllocator() ) );
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
			CHE_PDF_DictionaryPtr NewDictPtr;
			newRefInfo = pFile->CreateDictObject( NewDictPtr );
			if ( NewDictPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}

			CHE_ByteString key;
			CHE_PDF_ObjectPtr tmpObjPtr;
			CHE_PDF_DictionaryPtr tmpDictPtr = ObjPtr->GetDictPtr();
			
			HE_DWORD dictSize = tmpDictPtr->GetCount();
			for ( HE_DWORD i = 0; i < dictSize; i++ )
			{
				tmpDictPtr->GetKeyByIndex( i, key );

				tmpObjPtr = tmpDictPtr->GetElementByIndex( i );
				
				switch ( tmpObjPtr->GetType() )
				{
				case OBJ_TYPE_NULL:
					NewDictPtr->SetAtNull( key );
					break;
				case OBJ_TYPE_BOOLEAN:
					NewDictPtr->SetAtBoolean( key, tmpObjPtr->GetBooleanPtr()->GetValue() );
					break;
				case OBJ_TYPE_NUMBER:
					if ( tmpObjPtr->GetNumberPtr()->IsInteger() )
					{
						NewDictPtr->SetAtInteger( key, tmpObjPtr->GetNumberPtr()->GetInteger() );
					}else{
						NewDictPtr->SetAtFloatNumber( key, tmpObjPtr->GetNumberPtr()->GetFloat() );
					}
					break;
				case OBJ_TYPE_STRING:
					NewDictPtr->SetAtString( key, tmpObjPtr->GetStringPtr()->GetString() );
					break;
				case OBJ_TYPE_NAME:
					NewDictPtr->SetAtName( key, tmpObjPtr->GetNamePtr()->GetString() );
					break;
				case OBJ_TYPE_ARRAY:
					NewDictPtr->SetAtArray( key, CloneDirectArrayObj( tmpObjPtr->GetArrayPtr(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					NewDictPtr->SetAtDictionary( key, CloneDirectDictObj( tmpObjPtr->GetDictPtr(), pFile, pMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						PDF_RefInfo refInfo = CloneIndirectObj( tmpObjPtr->GetRefPtr(), pFile, pMgr );
						NewDictPtr->SetAtReference( key, refInfo.objNum, refInfo.genNum, tmpObjPtr->GetRefPtr()->GetFile() );
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
			CHE_PDF_StreamPtr NewStmPtr;
			newRefInfo = pFile->CreateStreamObject( NewStmPtr );
			if ( NewStmPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			CHE_PDF_DictionaryPtr NewDictPtr = CloneDirectDictObj( ObjPtr->GetStreamPtr()->GetDictPtr(), pFile, pMgr );
			NewStmPtr->SetDict( NewDictPtr );

			HE_LPBYTE pbuffer = GetDefaultAllocator()->NewArray<HE_BYTE>( ObjPtr->GetStreamPtr()->GetRawSize() );
			ObjPtr->GetStreamPtr()->GetRawData( 0, pbuffer, ObjPtr->GetStreamPtr()->GetRawSize() );

			NewStmPtr->SetRawData( pbuffer, ObjPtr->GetStreamPtr()->GetRawSize() );
			GetDefaultAllocator()->DeleteArray( pbuffer );

			pbuffer = NULL;
			break;
		}
	case OBJ_TYPE_REFERENCE:
		{
			newRefInfo = CloneIndirectObj( ObjPtr->GetRefPtr(), pFile, pMgr );
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