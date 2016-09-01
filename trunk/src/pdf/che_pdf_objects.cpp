#include "../../include/pdf/che_pdf_objects.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_file.h"
#include <memory.h>

CHE_PDF_Object::CHE_PDF_Object( PDF_OBJ_TYPE type, CHE_Allocator * pAllocator /*= nullptr*/ )
	: CHE_Object( pAllocator ), mModified( false ), mType( type ) {}

PDF_OBJ_TYPE CHE_PDF_Object::GetType() const
{
	return mType;
}

CHE_PDF_ObjectPtr CHE_PDF_Object::Clone()
{
	CHE_PDF_ObjectPtr ptr;
	switch ( mType )
	{
	case OBJ_TYPE_NULL:
		ptr = GetNullPtr()->Clone();
		break;
	case OBJ_TYPE_BOOLEAN:
		ptr = GetBooleanPtr()->Clone();
		break;
	case OBJ_TYPE_NUMBER:
		ptr = GetNumberPtr()->Clone();
		break;
	case OBJ_TYPE_STRING:
		ptr = GetStringPtr()->Clone();
		break;
	case OBJ_TYPE_NAME:
		ptr = GetNamePtr()->Clone();
		break;
	case OBJ_TYPE_ARRAY:
		ptr = GetArrayPtr()->Clone();
		break;
	case OBJ_TYPE_DICTIONARY:
		ptr = GetDictPtr()->Clone();
		break;
	case OBJ_TYPE_REFERENCE:
		ptr = GetRefPtr()->Clone();
		break;
	case OBJ_TYPE_STREAM:
		ptr = GetStreamPtr()->Clone();
		break;
	case OBJ_TYPE_INVALID:
	default:
		break;
	}
	return ptr;
}

void CHE_PDF_Object::Release()
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

void CHE_PDF_Object::SetModified( bool bValue )
{
	mModified = bValue;
}

bool	CHE_PDF_Object::IsModified()
{
	return mModified;
}

CHE_PDF_NullPtr CHE_PDF_Object::GetNullPtr() const
{
	CHE_PDF_NullPtr ptr;
	if ( mType == OBJ_TYPE_NULL )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Object::GetBooleanPtr() const
{
	CHE_PDF_BooleanPtr ptr;
	if ( mType == OBJ_TYPE_BOOLEAN )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Object::GetNumberPtr() const
{
	CHE_PDF_NumberPtr ptr;
	if ( mType == OBJ_TYPE_NUMBER )
	{
		ptr.Reset((CHE_PDF_Object*) this );
	}
	return ptr;
}

CHE_PDF_StringPtr CHE_PDF_Object::GetStringPtr() const
{
	CHE_PDF_StringPtr ptr;
	if ( mType == OBJ_TYPE_STRING )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Object::GetNamePtr() const
{
	CHE_PDF_NamePtr ptr;
	if ( mType == OBJ_TYPE_NAME )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ArrayPtr CHE_PDF_Object::GetArrayPtr() const
{
	CHE_PDF_ArrayPtr ptr;
	if ( mType == OBJ_TYPE_ARRAY )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_DictionaryPtr CHE_PDF_Object::GetDictPtr() const
{
	CHE_PDF_DictionaryPtr ptr;
	if ( mType == OBJ_TYPE_DICTIONARY )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Object::GetRefPtr() const
{
	CHE_PDF_ReferencePtr ptr;
	if ( mType == OBJ_TYPE_REFERENCE )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Object::GetStreamPtr() const
{
	CHE_PDF_StreamPtr ptr;
	if ( mType == OBJ_TYPE_STREAM )
	{
		ptr.Reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ObjectPtr::CHE_PDF_ObjectPtr( const CHE_PDF_ObjectPtr & ptr )
	: mpObj( nullptr )
{
	if ( mpObj != ptr.mpObj && ptr.mpObj != nullptr )
	{
		if ( mpObj )
		{
			mpObj->mRefs.DecRef();
			if ( mpObj->mRefs == 0 )
			{
				mpObj->Release();
			}
		}
		mpObj = ptr.mpObj;
		if ( mpObj )
		{
			mpObj->mRefs.AddRef();
		}
	}
}

CHE_PDF_ObjectPtr::~CHE_PDF_ObjectPtr()
{
	if ( mpObj )
	{
		mpObj->mRefs.DecRef();
		if ( mpObj->mRefs == 0 )
		{
			mpObj->Release();
		}
	}
}

CHE_PDF_ObjectPtr CHE_PDF_ObjectPtr::operator = ( const CHE_PDF_ObjectPtr & ptr )
{
	if ( mpObj )
	{
		if ( mpObj == ptr.mpObj )
		{
			return *this;
		}
		mpObj->mRefs.DecRef();
		if ( mpObj->mRefs == 0 )
		{
			mpObj->Release();
			mpObj = nullptr;
		}
	}
	mpObj = ptr.mpObj;
	if ( mpObj )
	{
		mpObj->mRefs.AddRef();
	}
	return *this;
}

void CHE_PDF_ObjectPtr::Reset( CHE_PDF_Object * pObj /*= nullptr*/ )
{
	if ( mpObj != pObj )
	{
		if ( mpObj )
		{
			mpObj->mRefs.DecRef();
			if ( mpObj->mRefs == 0 )
			{
				mpObj->Release();
			}
		}
		mpObj = pObj;
		if ( mpObj )
		{
			mpObj->mRefs.AddRef();
		}
	}
}

// CHE_PDF_BooleanPtr CHE_PDF_ObjectPtr::GetBooleanPtr() const
// {
// 	CHE_PDF_BooleanPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_BOOLEAN )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_NumberPtr CHE_PDF_ObjectPtr::GetNumberPtr() const
// {
// 	CHE_PDF_NumberPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NUMBER )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_StringPtr CHE_PDF_ObjectPtr::GetStringPtr() const
// {
// 	CHE_PDF_StringPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STRING )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_NamePtr	CHE_PDF_ObjectPtr::GetNamePtr() const
// {
// 	CHE_PDF_NamePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NAME )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ArrayPtr CHE_PDF_ObjectPtr::GetArrayPtr() const
// {
// 	CHE_PDF_ArrayPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_DictionaryPtr CHE_PDF_ObjectPtr::GetDictPtr() const
// {
// 	CHE_PDF_DictionaryPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_DICTIONARY )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ReferencePtr CHE_PDF_ObjectPtr::GetRefPtr() const
// {
// 	CHE_PDF_ReferencePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_REFERENCE )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_StreamPtr CHE_PDF_ObjectPtr::GetStreamPtr() const
// {
// 	CHE_PDF_StreamPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STREAM )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }

CHE_PDF_NullPtr CHE_PDF_Null::Create( CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_NullPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();	
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Null>( pAllocator ) );
	return ptr;
}

CHE_PDF_NullPtr CHE_PDF_Null::Clone()
{
	CHE_PDF_NullPtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Null>( GetAllocator() ) );
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Boolean::Create( bool value, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_BooleanPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Boolean>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Boolean::Clone()
{
	CHE_PDF_BooleanPtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Boolean>( mbValue, GetAllocator() ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Create( int32 value,  CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_NumberPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Number>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Create( FLOAT value, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_NumberPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Number>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Clone()
{
	CHE_PDF_NumberPtr ptr;
	if ( mbInteger )
	{
		ptr.Reset( GetAllocator()->New<CHE_PDF_Number>( mInteger, GetAllocator() ) );
	}else
	{
		ptr.Reset( GetAllocator()->New<CHE_PDF_Number>( mFloat, GetAllocator() ) );
	}
	return ptr;
}

CHE_PDF_StringPtr CHE_PDF_String::Create( const CHE_ByteString & str, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_StringPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_String>( str, pAllocator ) );
	return ptr;
}

CHE_ByteString & CHE_PDF_String::GetString()
{
	return mString;
}

void	CHE_PDF_String::SetString( CHE_ByteString & name )
{
	mString = name;

	SetModified( TRUE );
}

CHE_PDF_StringPtr CHE_PDF_String::Clone()
{
	CHE_PDF_StringPtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_String>( mString, GetAllocator() ) );
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Name::Create( const CHE_ByteString & str, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_NamePtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Name>( str, pAllocator ) );
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Name::Clone()
{
	CHE_PDF_NamePtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Name>( mName, GetAllocator() ) );
	return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Reference::Create( size_t refNum, size_t genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_ReferencePtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Reference>( refNum, genNum, pFile, pAllocator ) );
	return ptr;
}


CHE_PDF_ReferencePtr CHE_PDF_Reference::Clone()
{
	CHE_PDF_ReferencePtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Reference>( mRefObjNum, mRefGenNum, mpFile, GetAllocator() ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Reference::GetRefObj()
{
	if ( mpFile == nullptr )
	{
		return CHE_PDF_ObjectPtr();
	}
	PDF_RefInfo refInfo;
	refInfo.objNum = mRefObjNum;
	refInfo.genNum = mRefGenNum;
	return mpFile->GetObject( refInfo );
}

CHE_PDF_ObjectPtr CHE_PDF_Reference::GetRefObj( PDF_OBJ_TYPE Type )
{
	CHE_PDF_ObjectPtr ptr;
	if ( mpFile == nullptr )
	{
		return CHE_PDF_ObjectPtr();
	}
	PDF_RefInfo refInfo;
	refInfo.objNum = mRefObjNum;
	refInfo.genNum = mRefGenNum;
	ptr = mpFile->GetObject( refInfo );
	if ( !ptr )
	{
		return ptr;
	}
	if ( ptr->GetType() == Type || Type == OBJ_TYPE_INVALID )
	{
		return ptr;
	}

	while ( TRUE )
	{
		if ( ptr->GetType() == OBJ_TYPE_REFERENCE )
		{
			ptr = ptr->GetRefPtr()->GetRefObj();
			if ( !ptr )
			{
				return CHE_PDF_ReferencePtr();
			}
			if ( ptr->GetType() == Type )
			{
				return ptr;
			}
			if ( ptr->GetType() == OBJ_TYPE_REFERENCE || ptr->GetType() == OBJ_TYPE_ARRAY )
			{
				continue;
			}
			return CHE_PDF_ObjectPtr();
		}else if ( ptr->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ObjectPtr pElement;
			size_t dwCount = ptr->GetArrayPtr()->GetCount();
			bool bNeedContinue = false;
			for ( size_t i = 0; i < dwCount; i++ )
			{
				pElement = ptr->GetArrayPtr()->GetElement( i, Type );
				if ( !pElement )
				{
					continue;
				}
				if ( pElement->GetType() == Type )
				{
					return pElement;
				}else if ( pElement->GetType() == OBJ_TYPE_REFERENCE || pElement->GetType() == OBJ_TYPE_ARRAY )
				{
					ptr = pElement;
					bNeedContinue = TRUE;
					break;
				}
			}
			if ( bNeedContinue == TRUE )
			{
				continue;
			}
			return CHE_PDF_ObjectPtr();
		}else if ( ptr->GetType() == Type )
		{
			return ptr;
		}else{
			return CHE_PDF_ObjectPtr();
		}
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ArrayPtr CHE_PDF_Array::Create( CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_ArrayPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Array>( pAllocator ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElement( size_t index ) const
{
	if ( index < GetCount() )
	{
		return mArray[index];
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElement( size_t index, PDF_OBJ_TYPE Type ) const
{
	CHE_PDF_ObjectPtr ptr = GetElement( index );
	if ( ! ptr )
	{
		return ptr;
	}

	if (  ptr->GetType() == Type || Type == OBJ_TYPE_INVALID )
	{
		return ptr;
	}

	if ( ptr->GetType() == OBJ_TYPE_REFERENCE )
	{
		return ptr->GetRefPtr()->GetRefObj( Type );
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElementByType( PDF_OBJ_TYPE Type )
{
	CHE_PDF_ObjectPtr ptr;

	size_t lCount = mArray.size();

	for ( size_t i = 0; i < lCount; i++ )
	{
		ptr = GetElement( i, Type );
		if ( ptr )
		{
			return ptr;
		}
	}
	return ptr;
}

bool CHE_PDF_Array::Append( const CHE_PDF_ObjectPtr & ptr )
{
	if ( ptr )
	{
		mArray.push_back( ptr );
		SetModified( TRUE );
		return TRUE;
	}
	return false;
}

CHE_PDF_NullPtr CHE_PDF_Array::AppendNull()
{
    CHE_PDF_NullPtr ptr = CHE_PDF_Null::Create(GetAllocator());
    mArray.push_back( ptr );
    SetModified( TRUE );
    return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Array::AppendBoolean(bool val/*= false*/)
{
    CHE_PDF_BooleanPtr ptr = CHE_PDF_Boolean::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Array::AppendNumber()
{
    CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create(0, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Array::AppendNumber(int32 val)
{
    CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Array::AppendNumber(FLOAT val)
{
    CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Array::AppendName()
{
    CHE_PDF_NamePtr ptr = CHE_PDF_Name::Create(CHE_ByteString(GetAllocator()), GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Array::AppendName(CHE_ByteString & str)
{
    CHE_PDF_NamePtr ptr = CHE_PDF_Name::Create(str, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_StringPtr CHE_PDF_Array::AppendString()
{
    CHE_PDF_StringPtr ptr = CHE_PDF_String::Create(CHE_ByteString(GetAllocator()), GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_StringPtr CHE_PDF_Array::AppendString(CHE_ByteString & str)
{
    CHE_PDF_StringPtr ptr = CHE_PDF_String::Create(str, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_ArrayPtr CHE_PDF_Array::AppendArray()
{
    CHE_PDF_ArrayPtr ptr = CHE_PDF_Array::Create();
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_DictionaryPtr CHE_PDF_Array::AppendDictionary()
{
    CHE_PDF_DictionaryPtr ptr = CHE_PDF_Dictionary::Create(GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Array::AppendReference(PDF_RefInfo info, CHE_PDF_File * pFile)
{
    CHE_PDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CHE_PDF_Reference::Create(info.objNum, info.genNum, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Array::AppendReference(size_t objNum, size_t genNum, CHE_PDF_File * pFile)
{
    CHE_PDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CHE_PDF_Reference::Create(objNum, genNum, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Array::AppendReference(CHE_PDF_ReferencePtr & ptr)
{
    if (ptr)
    {
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Array::AppendReference(CHE_PDF_File * pFile)
{
    CHE_PDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CHE_PDF_Reference::Create(0, 0, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

bool CHE_PDF_Array::Replace(size_t index, const CHE_PDF_ObjectPtr & pObj)
{
    if (index > mArray.size())
    {
        return false;
    }
    mArray[index] = pObj;
    SetModified(TRUE);
    return TRUE;
}

CHE_PDF_NullPtr CHE_PDF_Array::ReplaceNull( size_t index )
{
    CHE_PDF_NullPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_NullPtr ptr = CHE_PDF_Null::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CHE_PDF_BooleanPtr CHE_PDF_Array::ReplaceBoolean( size_t index )
{
    CHE_PDF_BooleanPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_BooleanPtr ptr = CHE_PDF_Boolean::Create(false, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CHE_PDF_NumberPtr CHE_PDF_Array::ReplaceNumber( size_t index )
{
    CHE_PDF_NumberPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create(0, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CHE_PDF_NamePtr CHE_PDF_Array::ReplaceName( size_t index )
{
    CHE_PDF_NamePtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_NamePtr ptr = CHE_PDF_Name::Create(CHE_ByteString(GetAllocator()), GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CHE_PDF_StringPtr CHE_PDF_Array::ReplaceString( size_t index )
{
    CHE_PDF_StringPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_StringPtr ptr = CHE_PDF_String::Create(CHE_ByteString(GetAllocator()), GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CHE_PDF_ArrayPtr CHE_PDF_Array::ReplaceArray( size_t index )
{
    CHE_PDF_ArrayPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_ArrayPtr ptr = CHE_PDF_Array::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}


CHE_PDF_DictionaryPtr CHE_PDF_Array::ReplaceDictionary( size_t index )
{
    CHE_PDF_DictionaryPtr ptrRet;
    if (index <= mArray.size())
    {
        CHE_PDF_DictionaryPtr ptr = CHE_PDF_Dictionary::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}


CHE_PDF_ReferencePtr CHE_PDF_Array::ReplaceReference( size_t index, CHE_PDF_File * pFile )
{
    CHE_PDF_ReferencePtr ptrRet;
    if (index <= mArray.size() && pFile)
    {
        CHE_PDF_ReferencePtr ptr = CHE_PDF_Reference::Create(0, 0, pFile, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

void CHE_PDF_Array::Clear()
{
    mArray.clear();
    SetModified(TRUE);
}

CHE_PDF_ArrayPtr CHE_PDF_Array::Clone()
{
	CHE_PDF_ArrayPtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Array>( GetAllocator() ) );
	if ( ptr )
	{
		CHE_PDF_ObjectPtr tmpPtr;
		for ( size_t i = 0; i < GetCount(); ++i )
		{
			tmpPtr = GetElement( i );
			if ( tmpPtr )
			{
				tmpPtr = tmpPtr->Clone();
			}
			ptr->Append( tmpPtr );
		}
	}
	return ptr;
}

bool CHE_PDF_Array::IsModified()
{
	if ( mModified )
	{
		return TRUE;
	}
	for ( size_t i = 0; i < mArray.size(); ++i )
	{
		if ( mArray[i]->IsModified() )
		{
			mModified = TRUE;
			return TRUE;
		}
	}
	return false;
}

bool CHE_PDF_Array::GetRect( CHE_Rect & rect ) const
{
	if ( GetCount() >= 4 )
	{
		FLOAT llx = 0, lly = 0, rux = 0, ruy = 0;
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_NumberPtr numberPtr;
		objPtr = GetElement( 0, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			llx = numberPtr->GetFloat();
		}
		objPtr = GetElement( 1, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			lly = numberPtr->GetFloat();
		}
		objPtr = GetElement( 2, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			rux = numberPtr->GetFloat();
		}
		objPtr = GetElement( 3, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			ruy = numberPtr->GetFloat();
		}
		rect.left = llx;
		rect.bottom = lly;
		rect.width = rux - llx;
		rect.height = ruy - lly;

		return TRUE;
	}

	return false;
}

bool CHE_PDF_Array::GetMatrix( CHE_Matrix & maxtrix ) const
{
	if ( GetCount() >= 6 )
	{
		FLOAT a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
		CHE_PDF_ObjectPtr objPtr;
		CHE_PDF_NumberPtr numberPtr;
		objPtr = GetElement( 0, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			a = numberPtr->GetFloat();
		}
		objPtr = GetElement( 1, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			b = numberPtr->GetFloat();
		}
		objPtr = GetElement( 2, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			c = numberPtr->GetFloat();
		}
		objPtr = GetElement( 3, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			d = numberPtr->GetFloat();
		}
		objPtr = GetElement( 4, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			e = numberPtr->GetFloat();
		}
		objPtr = GetElement( 5, OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			numberPtr = objPtr->GetNumberPtr();
			f = numberPtr->GetFloat();
		}
		maxtrix.a = a;
		maxtrix.b = b;
		maxtrix.c = c;
		maxtrix.d = d;
		maxtrix.e = e;
		maxtrix.f = f;

		return TRUE;
	}

	return false;
}

CHE_PDF_DictionaryPtr CHE_PDF_Dictionary::Create( CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_DictionaryPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Dictionary>( pAllocator ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key )const
{
	unordered_map<string,CHE_PDF_ObjectPtr>::const_iterator it = mMap.find( string(key.GetData()) );
	if ( it != mMap.end() )
	{
		return it->second;
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE Type )
{
	CHE_PDF_ObjectPtr ptr = GetElement( key );
	if ( ! ptr )
	{
		return ptr;
	}
	PDF_OBJ_TYPE curType = ptr->GetType();
	if ( curType == Type || Type == OBJ_TYPE_INVALID )
	{
		return ptr;
	}
	if ( ptr->GetType() == OBJ_TYPE_REFERENCE )
	{
		ptr = ptr->GetRefPtr()->GetRefObj( Type );
		return ptr;
	}

	return CHE_PDF_ObjectPtr();
}

bool CHE_PDF_Dictionary::SetObject( const CHE_ByteString & key, const CHE_PDF_ObjectPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CHE_PDF_NullPtr	CHE_PDF_Dictionary::SetNull( const CHE_ByteString & key )
{
    CHE_PDF_NullPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CHE_PDF_Null::Create( GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Dictionary::SetBoolean( const CHE_ByteString & key, bool value )
{
    CHE_PDF_BooleanPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CHE_PDF_Boolean::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Dictionary::SetInteger( const CHE_ByteString & key, int32 value )
{
    CHE_PDF_NumberPtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CHE_PDF_Number::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Dictionary::SetFloatNumber( const CHE_ByteString & key, FLOAT value )
{
    CHE_PDF_NumberPtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CHE_PDF_Number::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_StringPtr CHE_PDF_Dictionary::SetString( const CHE_ByteString & key, const CHE_ByteString & str )
{
    CHE_PDF_StringPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CHE_PDF_String::Create( str, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_NamePtr	CHE_PDF_Dictionary::SetName( const CHE_ByteString & key, const CHE_ByteString & name )
{
    CHE_PDF_NamePtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CHE_PDF_Name::Create( name, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

bool	CHE_PDF_Dictionary::SetArray( const CHE_ByteString & key, const CHE_PDF_ArrayPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CHE_PDF_ArrayPtr CHE_PDF_Dictionary::SetArray( const CHE_ByteString & key )
{
    CHE_PDF_ArrayPtr ptr;
    if (key.GetLength() > 0)
    {
        ptr = CHE_PDF_Array::Create(GetAllocator());
        mMap[string(key.GetData())] = ptr;
        SetModified( TRUE );
    }
    return ptr;
}

bool CHE_PDF_Dictionary::SetDictionary( const CHE_ByteString & key, const CHE_PDF_DictionaryPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CHE_PDF_DictionaryPtr CHE_PDF_Dictionary::SetDictionary( const CHE_ByteString & key )
{
    CHE_PDF_DictionaryPtr ptr;
    if (key.GetLength() > 0)
    {
        ptr = CHE_PDF_Dictionary::Create(GetAllocator());
        mMap[string(key.GetData())] = ptr;
        SetModified( TRUE );
    }
    return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Dictionary::SetReference( const CHE_ByteString & key, size_t objNum, size_t genNum, CHE_PDF_File * pFile )
{
    CHE_PDF_ReferencePtr ptr;
	if ( key.GetLength() > 0 )
	{
        ptr = CHE_PDF_Reference::Create( objNum, genNum, pFile, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CHE_PDF_DictionaryPtr CHE_PDF_Dictionary::Clone()
{
	CHE_PDF_DictionaryPtr ptr;
	ptr.Reset( GetAllocator()->New<CHE_PDF_Dictionary>( GetAllocator() ) );
	if ( ptr )
	{
		unordered_map<string,CHE_PDF_ObjectPtr>::iterator it;
		for ( it = mMap.begin(); it != mMap.end(); ++it )
		{
			ptr->SetObject( it->first.c_str(), it->second );
		}
	}
	return ptr;
}

void CHE_PDF_Dictionary::Clear()
{
    mMap.clear();
    mIt = mMap.end();
    SetModified(TRUE);
}

bool CHE_PDF_Dictionary::IsModified()
{
	if ( mModified )
	{
		return TRUE;
	}
	unordered_map<string,CHE_PDF_ObjectPtr>::iterator it;
	for ( it = mMap.begin(); it != mMap.end(); ++it )
	{
		if ( it->second->IsModified() )
		{
			mModified = TRUE;
			return TRUE;
		}
	}
	return false;
}

bool CHE_PDF_Dictionary::CheckName( const CHE_ByteString & key, const CHE_ByteString & name, bool bRequired /*= TRUE*/ )
{
	CHE_PDF_ObjectPtr objPtr = GetElement( key, OBJ_TYPE_NAME );
	if ( objPtr )
	{
		CHE_PDF_NamePtr namePtr = objPtr->GetNamePtr();
		if ( namePtr->GetString() == name )
		{
			return TRUE;
		}
		return false;
	}else if ( ! bRequired )
	{
		return TRUE;
	}
	return false;
}

void CHE_PDF_Dictionary::MoveToFirst()
{
    mIt = mMap.begin();
}

bool CHE_PDF_Dictionary::GetKeyAndElement( CHE_ByteString & key, CHE_PDF_ObjectPtr & objPtr )
{
    if ( mIt != mMap.end() )
    {
        key = mIt->first.data();
        objPtr = mIt->second;
		++mIt;
        return TRUE;
    }
    return false;
}

bool CHE_PDF_Dictionary::Remove( const CHE_ByteString & key )
{
    unordered_map<string,CHE_PDF_ObjectPtr>::iterator it = mMap.find( key.GetData() );
    if ( it != mMap.end() )
    {
        if ( mIt == it ) {
            ++mIt;
        }
        mMap.erase(it);
        return TRUE;
    }
    return false;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt /*= nullptr*/, CHE_Allocator * pAllocator /*= nullptr*/ )
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Stream>( objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( PBYTE pData, size_t size, const CHE_PDF_DictionaryPtr & pDict, size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt /*= nullptr*/, CHE_Allocator * pAllocator /*= nullptr*/ ) 
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Stream>( pData, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( IHE_Read* pFile, size_t offset, size_t size, const CHE_PDF_DictionaryPtr pDict, size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt /*= nullptr*/, CHE_Allocator * pAllocator /*= nullptr*/ ) 
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CHE_PDF_Stream>( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_Stream::CHE_PDF_Stream( PBYTE pData, size_t size, const CHE_PDF_DictionaryPtr & ptr,
								size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
{
	m_pEncrypt = pEncrypt;
	mType = OBJ_TYPE_STREAM;
	m_pDataBuf = nullptr;
	m_dwSize = size;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = nullptr;
	if ( pData != nullptr && size != 0 )
	{
		m_pDataBuf = GetAllocator()->NewArray<BYTE>( size );
		memcpy( m_pDataBuf, pData, size );
	}
	if ( ptr )
	{
		mDictPtr = ptr;
	}else{
		mDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetInteger( "Length", (int32)m_dwSize );
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_Read* pFile, size_t offset, size_t size, const CHE_PDF_DictionaryPtr & ptr,
								size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
{
	m_pEncrypt = pEncrypt;
 	m_dwObjNum = objNum;
 	m_dwGenNum = genNum;
	mType = OBJ_TYPE_STREAM;
	m_pDataBuf = nullptr;
	m_dwSize = 0;
	m_bMem = false;
	m_FileOffset = 0;
	m_pFile = nullptr;
	if ( pFile != nullptr )  
	{
		m_pFile = pFile;
		m_FileOffset = offset;
		m_dwSize = size;
	}
	if ( ptr )
	{
		mDictPtr = ptr;
	}else{
		mDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetInteger( "Length", (int32)m_dwSize );
}

CHE_PDF_Stream::CHE_PDF_Stream(	size_t objNum, size_t genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
{
	m_pEncrypt = pEncrypt;
 	m_dwObjNum = objNum;
 	m_dwGenNum = genNum;
	mType = OBJ_TYPE_STREAM;
	m_pDataBuf = nullptr;
	m_dwSize = 0;
	m_bMem = TRUE;
	m_FileOffset = 0;
	m_pFile = nullptr;
	m_FileOffset = 0;
	m_dwSize = 0;
	m_pDataBuf = nullptr;
	mDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
}

CHE_PDF_Stream::~CHE_PDF_Stream()
{
 	if ( m_bMem == TRUE && m_pDataBuf )
 	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
 	}
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Clone()
{
	CHE_PDF_StreamPtr ptr;
	CHE_PDF_DictionaryPtr pDict;
	if ( mDictPtr )
	{
		pDict = mDictPtr->Clone();
	}
	if ( m_bMem )
	{
		PBYTE pDataBuf = GetAllocator()->NewArray<BYTE>( m_dwSize );
		ptr = CHE_PDF_Stream::Create( pDataBuf, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}else
	{
		ptr = CHE_PDF_Stream::Create( m_pFile, m_FileOffset, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}
	return ptr;
}

bool	CHE_PDF_Stream::IsModified()
{
	if ( mModified || mDictPtr->IsModified() )
	{
		mModified = TRUE;
		return TRUE;
	}
	return false;
}

void	CHE_PDF_Stream::SetDictPtr( const CHE_PDF_DictionaryPtr & pDict )
{
	mDictPtr = pDict;

	SetModified( TRUE );
}

bool CHE_PDF_Stream::SetRawData( PBYTE pData, size_t dwDataSize, BYTE filter/* = STREAM_FILTER_NULL*/ )
{
	if ( pData == nullptr || dwDataSize == 0 )
	{
		return false;
	}
	if ( m_bMem == false )
	{
		m_bMem = TRUE;
		m_pFile = nullptr;
	}else{
		if ( m_pDataBuf != nullptr )
		{
			GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
			m_pDataBuf = nullptr;
			m_dwSize = 0;
		}
	}
	if ( ! mDictPtr )
	{
		mDictPtr = CHE_PDF_Dictionary::Create( nullptr );
	}
	switch( filter )
	{
	case STREAM_FILTER_NULL:
		{
			m_pDataBuf = GetAllocator()->NewArray<BYTE>( dwDataSize );
			memcpy( m_pDataBuf, pData, dwDataSize );
			m_dwSize = dwDataSize;
            mDictPtr->Remove( "Filter" );
			break;
		}
	case STREAM_FILTER_HEX:
		{
			CHE_DynBuffer buf;
			CHE_PDF_HexFilter filter;
			filter.Encode( pData, dwDataSize, buf );
			if ( buf.GetSize() > 0 )
			{
				m_pDataBuf = GetAllocator()->NewArray<BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				mDictPtr->SetName( "Filter", "ASCIIHexDecode" );
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
				m_pDataBuf = GetAllocator()->NewArray<BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				mDictPtr->SetName( "Filter", "ASCII85Decode" );
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
				m_pDataBuf = GetAllocator()->NewArray<BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				mDictPtr->SetName( "Filter", "FlateDecode" );
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
				m_pDataBuf = GetAllocator()->NewArray<BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				mDictPtr->SetName( "Filter", "LZWDecode" );
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
				m_pDataBuf = GetAllocator()->NewArray<BYTE>( buf.GetSize() );
				m_dwSize = buf.GetSize();
				memcpy( m_pDataBuf, buf.GetData(), m_dwSize );
				mDictPtr->SetName( "Filter", "RunLengthDecode" );
			}
			break;
		}
	default:
		break;
	}
	
	mDictPtr->SetInteger( "Length", (int32)m_dwSize );

	if ( m_pEncrypt && m_pEncrypt->IsPasswordOK() )
	{
		m_pEncrypt->Decrypt( m_pDataBuf, m_dwSize, GetObjNum(), GetGenNum() );
	}

	SetModified( TRUE );

	return TRUE;
}

size_t CHE_PDF_Stream::GetRawData( size_t offset, PBYTE pBuf, size_t buf_size ) const
{
	if ( pBuf == nullptr || buf_size == 0 || offset >= m_dwSize )
 	{
 		return 0;
 	}
	if ( m_bMem == false )
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

CHE_PDF_StreamAcc::CHE_PDF_StreamAcc( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ), m_dwSize(0), m_pDataBuf(nullptr) {}

CHE_PDF_StreamAcc::~CHE_PDF_StreamAcc()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
	}
	m_dwSize = 0;
	mStreamPtr.Reset();
}

bool CHE_PDF_StreamAcc::Attach( const CHE_PDF_StreamPtr & stmPtr, PDF_STREAM_DECODE_MODE mode )
{
	if ( ! stmPtr )
	{
		return false;
	}

	if ( mStreamPtr )
	{
		Detach();
	}

	bool retValue = TRUE;
	CHE_PDF_DictionaryPtr pDict = stmPtr->GetDictPtr();
	if ( pDict )
	{
		size_t lFilterCount = 0;
		size_t length = stmPtr->GetRawSize();
		CHE_PDF_ObjectPtr pFilter = pDict->GetElement( "Filter" );
		CHE_PDF_ObjectPtr pParms = pDict->GetElement( "DecodeParms" );

		if ( ! pFilter )
		{
			m_dwSize = length;
			m_pDataBuf = GetAllocator()->NewArray<BYTE>( length );
			stmPtr->GetRawData( 0, m_pDataBuf, length );
			return TRUE;
		}

		if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
		{
			lFilterCount = pFilter->GetArrayPtr()->GetCount();
		}else{
			lFilterCount = 1;
		}

		CHE_PDF_NamePtr * pFilterNameArr = nullptr;
		CHE_PDF_DictionaryPtr * pParamDictArr = nullptr;
	
		pFilterNameArr = GetAllocator()->NewArray<CHE_PDF_NamePtr>( lFilterCount );
		pParamDictArr = GetAllocator()->NewArray<CHE_PDF_DictionaryPtr>( lFilterCount );

		if ( pFilter->GetType() == OBJ_TYPE_NAME )
		{
			pFilterNameArr[0] = pFilter->GetNamePtr();
		}else if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
		{
			CHE_PDF_ArrayPtr tmpArrayPtr = pFilter->GetArrayPtr();
			for ( size_t i = 0; i < lFilterCount; i++ )
			{
				pFilterNameArr[i] = tmpArrayPtr->GetElement( i )->GetNamePtr();
			}
		}else{
			GetAllocator()->DeleteArray<CHE_PDF_NamePtr>( pFilterNameArr );
			GetAllocator()->DeleteArray<CHE_PDF_DictionaryPtr>( pParamDictArr );
			return false;
		}

		if ( pParms && pParms->GetType() == OBJ_TYPE_DICTIONARY )
		{
			pParamDictArr[0] = pParms->GetDictPtr();
		}else if ( pParms && pParms->GetType() == OBJ_TYPE_ARRAY )
		{
			size_t lParamCount = pParms->GetArrayPtr()->GetCount();
			for ( size_t i = 0; i < lParamCount; i++ )
			{
				pParamDictArr[i] = pParms->GetArrayPtr()->GetElement( i )->GetDictPtr();
			}
		}

		size_t bufSize = (length == 0) ? 1024 : length;
		CHE_ByteString str( GetAllocator() );
		CHE_DynBuffer buffer( bufSize * 2, bufSize, GetAllocator() );
		size_t lSize = stmPtr->GetRawSize();
		PBYTE pTmp = nullptr;
		pTmp = GetAllocator()->NewArray<BYTE>( lSize );
		stmPtr->GetRawData( 0, pTmp, lSize );
		
		for ( size_t i = 0; i < lFilterCount; i++ )
		{
			if ( mode == STREAM_DECODE_NOTLASTFILTER )
			{
				if ( i + 1 == lFilterCount )
				{
					break;
				}
			}

			str = pFilterNameArr[i]->GetNamePtr()->GetString();
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
				CHE_PDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				if ( ! pDecodeParams )
				{
					CHE_PDF_LZWFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else{
					CHE_PDF_Predictor pPredictor( pDecodeParams, GetAllocator() );
					CHE_PDF_LZWFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
					lSize = buffer.GetSize();
					GetAllocator()->DeleteArray<BYTE>( pTmp );
					pTmp = GetAllocator()->NewArray<BYTE>( lSize );
					buffer.Read( pTmp, lSize );
					buffer.Clear();
					pPredictor.Decode( pTmp, lSize, buffer );
				}
			}else if ( str == "FlateDecode" || str == "Fl" )
			{
				CHE_PDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				if ( !pDecodeParams )
				{
					CHE_PDF_FlateFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else{
					CHE_PDF_Predictor pPredictor( pDecodeParams, GetAllocator() );
					CHE_PDF_FlateFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
					lSize = buffer.GetSize();
					GetAllocator()->DeleteArray<BYTE>( pTmp );
					pTmp = GetAllocator()->NewArray<BYTE>( lSize );
					buffer.Read( pTmp, lSize );
					buffer.Clear();
					pPredictor.Decode( pTmp, lSize, buffer );
				}
			}else if ( str == "RunLengthDecode" || str == "RL" )
			{
				CHE_PDF_RLEFileter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "CCITTFaxDecode" || str == "CCF" )
			{
				CHE_PDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				CHE_PDF_FaxDecodeParams params( pDecodeParams );
				CHE_PDF_FaxFilter filter( &params, GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "JBIG2Decode" )
			{
				CHE_PDF_JBig2Filter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "DCTDecode" || str == "DCT" )
			{
				CHE_PDF_DCTDFilter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "JPXDecode" )
			{
                CHE_PDF_JPXFilter fileter( GetAllocator() );
                fileter.Decode( pTmp, lSize, buffer );
			}else if ( str == "Crypt" )
			{
				retValue = false;
			}

			if ( retValue == false )
			{
				GetAllocator()->DeleteArray<BYTE>( pTmp );
				GetAllocator()->DeleteArray<CHE_PDF_NamePtr>( pFilterNameArr );
				GetAllocator()->DeleteArray<CHE_PDF_DictionaryPtr>( pParamDictArr );
				return false;
			}else{
				lSize = buffer.GetSize();
				GetAllocator()->DeleteArray<BYTE>( pTmp );
				pTmp = GetAllocator()->NewArray<BYTE>( lSize );
				buffer.Read( pTmp, lSize );
			}
		}

		GetAllocator()->DeleteArray<CHE_PDF_NamePtr>( pFilterNameArr );
		GetAllocator()->DeleteArray<CHE_PDF_DictionaryPtr>( pParamDictArr );

		m_pDataBuf = pTmp;
		m_dwSize = lSize;
		return retValue;
	}else{
		m_dwSize = stmPtr->GetRawSize();
		m_pDataBuf = GetAllocator()->NewArray<BYTE>( m_dwSize );
		stmPtr->GetRawData( 0, m_pDataBuf, m_dwSize );			
		return TRUE;
	}
	return false;
}

void CHE_PDF_StreamAcc::Detach()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
		m_pDataBuf = nullptr;
	}
	mStreamPtr.Reset();
	m_dwSize = 0;
}

bool IsPdfNull( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfBoolean( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNumber( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfName( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfString( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfDict( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfArray( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfRef( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStream( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNullPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfBooleanPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNumberPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNamePtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStringPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfDictPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfArrayPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfRefPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStreamPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return false;
}