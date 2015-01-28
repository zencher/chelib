#include "../../include/pdf/che_pdf_objclone.h"

CHE_PDF_ArrayPtr CloneDirectArrayObj( const CHE_PDF_ArrayPtr & arrayPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( ! arrayPtr || pFile == NULL )
	{
		return CHE_PDF_ArrayPtr();
	}

	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_ArrayPtr newArrayPtr = CHE_PDF_Array::Create( pFile->GetAllocator() );
	HE_ULONG arraySize = arrayPtr->GetCount();
	for ( HE_ULONG i = 0; i < arraySize; ++i )
	{
		objPtr = arrayPtr->GetElement( i );
		switch (objPtr->GetType())
		{
		case OBJ_TYPE_NULL:
			newArrayPtr->Append(CHE_PDF_Null::Create(pFile->GetAllocator()));
			break;
		case OBJ_TYPE_BOOLEAN:
			newArrayPtr->Append(CHE_PDF_Boolean::Create(objPtr->GetBooleanPtr()->GetValue(), pFile->GetAllocator()));
			break;
		case OBJ_TYPE_NUMBER:
			if (objPtr->GetNumberPtr()->IsInteger())
			{
				newArrayPtr->Append(CHE_PDF_Number::Create(objPtr->GetNumberPtr()->GetInteger(), pFile->GetAllocator()));
			}else{
				newArrayPtr->Append(CHE_PDF_Number::Create(objPtr->GetNumberPtr()->GetFloat(), pFile->GetAllocator()));
			}
			break;
		case OBJ_TYPE_STRING:
			newArrayPtr->Append(CHE_PDF_String::Create(objPtr->GetStringPtr()->GetString(), pFile->GetAllocator()));
			break;
		case OBJ_TYPE_NAME:
			newArrayPtr->Append(CHE_PDF_Name::Create(objPtr->GetNamePtr()->GetString(), pFile->GetAllocator()));
			break;
		case OBJ_TYPE_ARRAY:
			newArrayPtr->Append(CloneDirectArrayObj(objPtr->GetArrayPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_DICTIONARY:
			newArrayPtr->Append(CloneDirectDictObj(objPtr->GetDictPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_REFERENCE:
			{
				PDF_RefInfo refInfo = CloneIndirectObj(objPtr->GetRefPtr(), pFile, pMgr);
				newArrayPtr->Append(CHE_PDF_Reference::Create(refInfo.objNum, refInfo.genNum, objPtr->GetRefPtr()->GetFile(), pFile->GetAllocator()));
				break;
			}
		default: break;
		}
	}
	return newArrayPtr;
}

CHE_PDF_DictionaryPtr CloneDirectDictObj( const CHE_PDF_DictionaryPtr & dictPtr, CHE_PDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= NULL*/ )
{
	if ( !dictPtr || pFile == NULL )
	{
		return CHE_PDF_DictionaryPtr();
	}

	CHE_ByteString key;
	CHE_PDF_ObjectPtr objPtr;
	CHE_PDF_DictionaryPtr newDictPtr = CHE_PDF_Dictionary::Create( pFile->GetAllocator() );

	dictPtr->MoveToFirst();
	while ( dictPtr->GetKeyAndElement(key, objPtr) )
	{
		switch ( objPtr->GetType() )
		{
		case OBJ_TYPE_NULL:
			newDictPtr->SetAtNull(key);
			break;
		case OBJ_TYPE_BOOLEAN:
			newDictPtr->SetAtBoolean(key, objPtr->GetBooleanPtr()->GetValue());
			break;
		case OBJ_TYPE_NUMBER:
			if ( objPtr->GetNumberPtr()->IsInteger() )
			{
				newDictPtr->SetAtInteger(key, objPtr->GetNumberPtr()->GetInteger());
			}else{
				newDictPtr->SetAtFloatNumber(key, objPtr->GetNumberPtr()->GetFloat());
			}
			break;
		case OBJ_TYPE_STRING:
			newDictPtr->SetAtString(key, objPtr->GetStringPtr()->GetString());
			break;
		case OBJ_TYPE_NAME:
			newDictPtr->SetAtName(key, objPtr->GetNamePtr()->GetString());
			break;
		case OBJ_TYPE_ARRAY:
			newDictPtr->SetAtArray(key, CloneDirectArrayObj(objPtr->GetArrayPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_DICTIONARY:
			newDictPtr->SetAtDictionary(key, CloneDirectDictObj(objPtr->GetDictPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_REFERENCE:
			{
				PDF_RefInfo refInfo = CloneIndirectObj(objPtr->GetRefPtr(), pFile, pMgr);
				newDictPtr->SetAtReference(key, refInfo.objNum, refInfo.genNum, objPtr->GetRefPtr()->GetFile());
				break;
			}
		default: break;
		}
	}

	return newDictPtr;
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
		if ( pMgr->GetMapObjNum(refInfo, refInfo) )
		{
			return refInfo;
		}
	}

	PDF_RefInfo newRefInfo;
	CHE_PDF_ObjectPtr newObjPtr;
	CHE_PDF_ObjectPtr ObjPtr = RefPtr->GetRefObj();

	//某些情况下，由于应用的某个对象确实不存在，原因可能是生成文件的问题，需要做这种容错性处理
	if ( ! ObjPtr )
	{
		CHE_PDF_NullPtr nullPtr;
		newRefInfo = pFile->CreateNullObject( nullPtr );
		if ( nullPtr && pMgr )
		{
			pMgr->SetMap( refInfo, newRefInfo );
		}
		return newRefInfo;
	}
	
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

			HE_ULONG arraySize = tmpArrayPtr->GetCount();
			for ( HE_ULONG i = 0; i < arraySize; i++ )
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
			CHE_PDF_DictionaryPtr newDictPtr;
			newRefInfo = pFile->CreateDictObject( newDictPtr );
			if ( newDictPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}

			CHE_ByteString key;
			CHE_PDF_ObjectPtr tmpObjPtr;
			CHE_PDF_DictionaryPtr tmpDictPtr = ObjPtr->GetDictPtr();
			
			tmpDictPtr->MoveToFirst();
			while (tmpDictPtr->GetKeyAndElement(key, tmpObjPtr))
			{
				switch (tmpObjPtr->GetType())
				{
				case OBJ_TYPE_NULL:
					newDictPtr->SetAtNull(key);
					break;
				case OBJ_TYPE_BOOLEAN:
					newDictPtr->SetAtBoolean(key, tmpObjPtr->GetBooleanPtr()->GetValue());
					break;
				case OBJ_TYPE_NUMBER:
					if (tmpObjPtr->GetNumberPtr()->IsInteger())
					{
						newDictPtr->SetAtInteger(key, tmpObjPtr->GetNumberPtr()->GetInteger());
					}else{
						newDictPtr->SetAtFloatNumber(key, tmpObjPtr->GetNumberPtr()->GetFloat());
					}
					break;
				case OBJ_TYPE_STRING:
					newDictPtr->SetAtString(key, tmpObjPtr->GetStringPtr()->GetString());
					break;
				case OBJ_TYPE_NAME:
					newDictPtr->SetAtName(key, tmpObjPtr->GetNamePtr()->GetString());
					break;
				case OBJ_TYPE_ARRAY:
					newDictPtr->SetAtArray(key, CloneDirectArrayObj(tmpObjPtr->GetArrayPtr(), pFile, pMgr));
					break;
				case OBJ_TYPE_DICTIONARY:
					newDictPtr->SetAtDictionary(key, CloneDirectDictObj(tmpObjPtr->GetDictPtr(), pFile, pMgr));
					break;
				case OBJ_TYPE_REFERENCE:
					{
						PDF_RefInfo refInfo = CloneIndirectObj(tmpObjPtr->GetRefPtr(), pFile, pMgr);
						newDictPtr->SetAtReference(key, refInfo.objNum, refInfo.genNum, tmpObjPtr->GetRefPtr()->GetFile());
						break;
					}
				default: break;
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