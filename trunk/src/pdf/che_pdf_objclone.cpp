#include "../../include/pdf/che_pdf_objclone.h"

namespace chelib {

CPDF_ArrayPtr CloneDirectArrayObj( const CPDF_ArrayPtr & arrayPtr, CPDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= nullptr*/ )
{
	if ( ! arrayPtr || pFile == nullptr )
	{
		return CPDF_ArrayPtr();
	}

	CPDF_ObjectPtr objPtr;
	CPDF_ArrayPtr newArrayPtr = CPDF_Array::Create( pFile->GetAllocator() );
	size_t arraySize = arrayPtr->GetCount();
	for ( size_t i = 0; i < arraySize; ++i )
	{
		objPtr = arrayPtr->GetElement( i );
		switch (objPtr->GetType())
		{
		case OBJ_TYPE_NULL:
			newArrayPtr->Append(CPDF_Null::Create(pFile->GetAllocator()));
			break;
		case OBJ_TYPE_BOOLEAN:
			newArrayPtr->Append(CPDF_Boolean::Create(objPtr->GetBooleanPtr()->GetValue(), pFile->GetAllocator()));
			break;
		case OBJ_TYPE_NUMBER:
			if (objPtr->GetNumberPtr()->IsInteger())
			{
				newArrayPtr->Append(CPDF_Number::Create(objPtr->GetNumberPtr()->GetInteger(), pFile->GetAllocator()));
			}else{
				newArrayPtr->Append(CPDF_Number::Create(objPtr->GetNumberPtr()->GetFloat(), pFile->GetAllocator()));
			}
			break;
		case OBJ_TYPE_STRING:
			newArrayPtr->Append(CPDF_String::Create(objPtr->GetStringPtr()->GetString(), pFile->GetAllocator()));
			break;
		case OBJ_TYPE_NAME:
			newArrayPtr->Append(CPDF_Name::Create(objPtr->GetNamePtr()->GetString(), pFile->GetAllocator()));
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
				newArrayPtr->Append(CPDF_Reference::Create(refInfo.objNum, refInfo.genNum, objPtr->GetRefPtr()->GetFile(), pFile->GetAllocator()));
				break;
			}
		default: break;
		}
	}
	return newArrayPtr;
}

CPDF_DictionaryPtr CloneDirectDictObj( const CPDF_DictionaryPtr & dictPtr, CPDF_File * pFile, IHE_ObjectCloneMgr * pMgr /*= nullptr*/ )
{
	if ( !dictPtr || pFile == nullptr )
	{
		return CPDF_DictionaryPtr();
	}

	CByteString key;
	CPDF_ObjectPtr objPtr;
	CPDF_DictionaryPtr newDictPtr = CPDF_Dictionary::Create( pFile->GetAllocator() );

	dictPtr->MoveToFirst();
	while ( dictPtr->GetKeyAndElement(key, objPtr) )
	{
		switch ( objPtr->GetType() )
		{
		case OBJ_TYPE_NULL:
			newDictPtr->SetNull(key);
			break;
		case OBJ_TYPE_BOOLEAN:
			newDictPtr->SetBoolean(key, objPtr->GetBooleanPtr()->GetValue());
			break;
		case OBJ_TYPE_NUMBER:
			if ( objPtr->GetNumberPtr()->IsInteger() )
			{
				newDictPtr->SetInteger(key, objPtr->GetNumberPtr()->GetInteger());
			}else{
				newDictPtr->SetFloatNumber(key, objPtr->GetNumberPtr()->GetFloat());
			}
			break;
		case OBJ_TYPE_STRING:
			newDictPtr->SetString(key, objPtr->GetStringPtr()->GetString());
			break;
		case OBJ_TYPE_NAME:
			newDictPtr->SetName(key, objPtr->GetNamePtr()->GetString());
			break;
		case OBJ_TYPE_ARRAY:
			newDictPtr->SetArray(key, CloneDirectArrayObj(objPtr->GetArrayPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_DICTIONARY:
			newDictPtr->SetDictionary(key, CloneDirectDictObj(objPtr->GetDictPtr(), pFile, pMgr));
			break;
		case OBJ_TYPE_REFERENCE:
			{
				PDF_RefInfo refInfo = CloneIndirectObj(objPtr->GetRefPtr(), pFile, pMgr);
				newDictPtr->SetReference(key, refInfo.objNum, refInfo.genNum, objPtr->GetRefPtr()->GetFile());
				break;
			}
		default: break;
		}
	}

	return newDictPtr;
}

PDF_RefInfo CloneIndirectObj( const CPDF_ReferencePtr & RefPtr, CPDF_File * pFile, IHE_ObjectCloneMgr * pMgr /* = nullptr */ )
{
	PDF_RefInfo refInfo;
	refInfo.objNum = 0;
	refInfo.genNum = 0;

	if ( ! RefPtr || pFile == nullptr )
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
	CPDF_ObjectPtr newObjPtr;
	CPDF_ObjectPtr ObjPtr = RefPtr->GetRefObj();

	//某些情况下，由于应用的某个对象确实不存在，原因可能是生成文件的问题，需要做这种容错性处理
	if ( ! ObjPtr )
	{
		CPDF_NullPtr nullPtr;
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
			CPDF_NullPtr nullPtr;
			newRefInfo = pFile->CreateNullObject( nullPtr );
			if ( nullPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_BOOLEAN:
		{
			CPDF_BooleanPtr boolPtr;
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
			CPDF_NumberPtr numberPtr;
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
			CPDF_StringPtr stringPtr;
			newRefInfo = pFile->CreateStringObject( stringPtr );
			if ( stringPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_NAME:
		{
			CPDF_NamePtr namePtr;
			newRefInfo = pFile->CreateNameObject( namePtr );
			if ( namePtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			break;
		}
	case OBJ_TYPE_ARRAY:
		{
			CPDF_ArrayPtr NewArrayPtr;
			newRefInfo = pFile->CreateArrayObject( NewArrayPtr );
			if ( NewArrayPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}

			CPDF_ObjectPtr tmpObjPtr;
			CPDF_ArrayPtr tmpArrayPtr = ObjPtr->GetArrayPtr();

			size_t arraySize = tmpArrayPtr->GetCount();
			for ( size_t i = 0; i < arraySize; i++ )
			{
				tmpObjPtr = tmpArrayPtr->GetElement( i );
				switch ( tmpObjPtr->GetType() )
				{
				case OBJ_TYPE_NULL:
					NewArrayPtr->Append( CPDF_Null::Create( pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_BOOLEAN:
					NewArrayPtr->Append( CPDF_Boolean::Create( tmpObjPtr->GetBooleanPtr()->GetValue(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_NUMBER:
					if ( tmpObjPtr->GetNumberPtr()->IsInteger() )
					{
						NewArrayPtr->Append( CPDF_Number::Create( tmpObjPtr->GetNumberPtr()->GetInteger(), pFile->GetAllocator() ) );
					}else{
						NewArrayPtr->Append( CPDF_Number::Create( tmpObjPtr->GetNumberPtr()->GetFloat(), pFile->GetAllocator() ) );
					}
					break;
				case OBJ_TYPE_STRING:
					NewArrayPtr->Append( CPDF_String::Create( tmpObjPtr->GetStringPtr()->GetString(), pFile->GetAllocator() ) );
					break;
				case OBJ_TYPE_NAME:
					NewArrayPtr->Append( CPDF_Name::Create( tmpObjPtr->GetNamePtr()->GetString(), pFile->GetAllocator() ) );
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
						NewArrayPtr->Append( CPDF_Reference::Create( refInfo.objNum, refInfo.genNum, tmpObjPtr->GetRefPtr()->GetFile(), pFile->GetAllocator() ) );
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
			CPDF_DictionaryPtr newDictPtr;
			newRefInfo = pFile->CreateDictObject( newDictPtr );
			if ( newDictPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}

			CByteString key;
			CPDF_ObjectPtr tmpObjPtr;
			CPDF_DictionaryPtr tmpDictPtr = ObjPtr->GetDictPtr();
			
			tmpDictPtr->MoveToFirst();
			while (tmpDictPtr->GetKeyAndElement(key, tmpObjPtr))
			{
				switch (tmpObjPtr->GetType())
				{
				case OBJ_TYPE_NULL:
					newDictPtr->SetNull(key);
					break;
				case OBJ_TYPE_BOOLEAN:
					newDictPtr->SetBoolean(key, tmpObjPtr->GetBooleanPtr()->GetValue());
					break;
				case OBJ_TYPE_NUMBER:
					if (tmpObjPtr->GetNumberPtr()->IsInteger())
					{
						newDictPtr->SetInteger(key, tmpObjPtr->GetNumberPtr()->GetInteger());
					}else{
						newDictPtr->SetFloatNumber(key, tmpObjPtr->GetNumberPtr()->GetFloat());
					}
					break;
				case OBJ_TYPE_STRING:
					newDictPtr->SetString(key, tmpObjPtr->GetStringPtr()->GetString());
					break;
				case OBJ_TYPE_NAME:
					newDictPtr->SetName(key, tmpObjPtr->GetNamePtr()->GetString());
					break;
				case OBJ_TYPE_ARRAY:
					newDictPtr->SetArray(key, CloneDirectArrayObj(tmpObjPtr->GetArrayPtr(), pFile, pMgr));
					break;
				case OBJ_TYPE_DICTIONARY:
					newDictPtr->SetDictionary(key, CloneDirectDictObj(tmpObjPtr->GetDictPtr(), pFile, pMgr));
					break;
				case OBJ_TYPE_REFERENCE:
					{
						PDF_RefInfo refInfo = CloneIndirectObj(tmpObjPtr->GetRefPtr(), pFile, pMgr);
						newDictPtr->SetReference(key, refInfo.objNum, refInfo.genNum, tmpObjPtr->GetRefPtr()->GetFile());
						break;
					}
				default: break;
				}
			}
			
			break;
		}
	case OBJ_TYPE_STREAM:
		{
			CPDF_StreamPtr NewStmPtr;
			newRefInfo = pFile->CreateStreamObject( NewStmPtr );
			if ( NewStmPtr && pMgr )
			{
				pMgr->SetMap( refInfo, newRefInfo );
			}
			CPDF_DictionaryPtr NewDictPtr = CloneDirectDictObj( ObjPtr->GetStreamPtr()->GetDictPtr(), pFile, pMgr );
			NewStmPtr->SetDictPtr( NewDictPtr );

			PBYTE pbuffer = GetDefaultAllocator()->NewArray<BYTE>( ObjPtr->GetStreamPtr()->GetRawSize() );
			ObjPtr->GetStreamPtr()->GetRawData( 0, pbuffer, ObjPtr->GetStreamPtr()->GetRawSize() );

			NewStmPtr->SetRawData( pbuffer, ObjPtr->GetStreamPtr()->GetRawSize() );
			GetDefaultAllocator()->DeleteArray( pbuffer );

			pbuffer = nullptr;
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

}//namespace