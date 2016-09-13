#include "../../include/pdf/pdf_object.h"
#include "../../include/pdf/pdf_filter.h"
#include "../../include/pdf/pdf_parser.h"
#include "../../include/pdf/pdf_file.h"
#include <memory.h>

namespace chelib {
    
using namespace std;

CPDF_Object::CPDF_Object( PDF_OBJ_TYPE type, CAllocator * pAllocator /*= nullptr*/ )
	: CObject( pAllocator ), mModified( false ), mType( type ) {}

PDF_OBJ_TYPE CPDF_Object::GetType() const
{
	return mType;
}

CPDF_ObjectPtr CPDF_Object::Clone()
{
	CPDF_ObjectPtr ptr;
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

void CPDF_Object::Release()
{
	switch ( mType )
	{
	case OBJ_TYPE_NULL:
		GetAllocator()->Delete<CPDF_Null>( (CPDF_Null*)this );
		break;
	case OBJ_TYPE_BOOLEAN:
		GetAllocator()->Delete<CPDF_Boolean>( (CPDF_Boolean*)this );
		break;
	case OBJ_TYPE_NUMBER:
		GetAllocator()->Delete<CPDF_Number>( (CPDF_Number*)this );
		break;
	case OBJ_TYPE_STRING:
		GetAllocator()->Delete<CPDF_String>( (CPDF_String*)this );
		break;
	case OBJ_TYPE_NAME:
		GetAllocator()->Delete<CPDF_Name>( (CPDF_Name*)this );
		break;
	case OBJ_TYPE_ARRAY:
		GetAllocator()->Delete<CPDF_Array>( (CPDF_Array*)this );
		break;
	case OBJ_TYPE_DICTIONARY:
		GetAllocator()->Delete<CPDF_Dictionary>( (CPDF_Dictionary*)this );
		break;
	case OBJ_TYPE_REFERENCE:
		GetAllocator()->Delete<CPDF_Reference>( (CPDF_Reference*)this );
		break;
	case OBJ_TYPE_STREAM:
		GetAllocator()->Delete<CPDF_Stream>( (CPDF_Stream*)this );
		break;
	case OBJ_TYPE_INVALID:
	default:
		GetAllocator()->Delete<CPDF_Object>( this );
		break;
	}
}

void CPDF_Object::SetModified( bool bValue )
{
	mModified = bValue;
}

bool	CPDF_Object::IsModified()
{
	return mModified;
}

CPDF_NullPtr CPDF_Object::GetNullPtr() const
{
	CPDF_NullPtr ptr;
	if ( mType == OBJ_TYPE_NULL )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_BooleanPtr CPDF_Object::GetBooleanPtr() const
{
	CPDF_BooleanPtr ptr;
	if ( mType == OBJ_TYPE_BOOLEAN )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_NumberPtr CPDF_Object::GetNumberPtr() const
{
	CPDF_NumberPtr ptr;
	if ( mType == OBJ_TYPE_NUMBER )
	{
		ptr.Reset((CPDF_Object*) this );
	}
	return ptr;
}

CPDF_StringPtr CPDF_Object::GetStringPtr() const
{
	CPDF_StringPtr ptr;
	if ( mType == OBJ_TYPE_STRING )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_NamePtr CPDF_Object::GetNamePtr() const
{
	CPDF_NamePtr ptr;
	if ( mType == OBJ_TYPE_NAME )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_ArrayPtr CPDF_Object::GetArrayPtr() const
{
	CPDF_ArrayPtr ptr;
	if ( mType == OBJ_TYPE_ARRAY )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_DictionaryPtr CPDF_Object::GetDictPtr() const
{
	CPDF_DictionaryPtr ptr;
	if ( mType == OBJ_TYPE_DICTIONARY )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_ReferencePtr CPDF_Object::GetRefPtr() const
{
	CPDF_ReferencePtr ptr;
	if ( mType == OBJ_TYPE_REFERENCE )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_StreamPtr CPDF_Object::GetStreamPtr() const
{
	CPDF_StreamPtr ptr;
	if ( mType == OBJ_TYPE_STREAM )
	{
		ptr.Reset( (CPDF_Object*)this );
	}
	return ptr;
}

CPDF_ObjectPtr::CPDF_ObjectPtr( const CPDF_ObjectPtr & ptr )
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

CPDF_ObjectPtr::~CPDF_ObjectPtr()
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

CPDF_ObjectPtr CPDF_ObjectPtr::operator = ( const CPDF_ObjectPtr & ptr )
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

void CPDF_ObjectPtr::Reset( CPDF_Object * pObj /*= nullptr*/ )
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

// CPDF_BooleanPtr CPDF_ObjectPtr::GetBooleanPtr() const
// {
// 	CPDF_BooleanPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_BOOLEAN )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_NumberPtr CPDF_ObjectPtr::GetNumberPtr() const
// {
// 	CPDF_NumberPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NUMBER )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_StringPtr CPDF_ObjectPtr::GetStringPtr() const
// {
// 	CPDF_StringPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STRING )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_NamePtr	CPDF_ObjectPtr::GetNamePtr() const
// {
// 	CPDF_NamePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NAME )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_ArrayPtr CPDF_ObjectPtr::GetArrayPtr() const
// {
// 	CPDF_ArrayPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_DictionaryPtr CPDF_ObjectPtr::GetDictPtr() const
// {
// 	CPDF_DictionaryPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_DICTIONARY )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_ReferencePtr CPDF_ObjectPtr::GetRefPtr() const
// {
// 	CPDF_ReferencePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_REFERENCE )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CPDF_StreamPtr CPDF_ObjectPtr::GetStreamPtr() const
// {
// 	CPDF_StreamPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STREAM )
// 	{
// 		ptr.Reset( mpObj );
// 	}
// 	return ptr;
// }

CPDF_NullPtr CPDF_Null::Create( CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_NullPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();	
	}
	ptr.Reset( pAllocator->New<CPDF_Null>( pAllocator ) );
	return ptr;
}

CPDF_NullPtr CPDF_Null::Clone()
{
	CPDF_NullPtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Null>( GetAllocator() ) );
	return ptr;
}

CPDF_BooleanPtr CPDF_Boolean::Create( bool value, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_BooleanPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Boolean>( value, pAllocator ) );
	return ptr;
}

CPDF_BooleanPtr CPDF_Boolean::Clone()
{
	CPDF_BooleanPtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Boolean>( mbValue, GetAllocator() ) );
	return ptr;
}

CPDF_NumberPtr CPDF_Number::Create( int32 value,  CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_NumberPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Number>( value, pAllocator ) );
	return ptr;
}

CPDF_NumberPtr CPDF_Number::Create( FLOAT value, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_NumberPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Number>( value, pAllocator ) );
	return ptr;
}

CPDF_NumberPtr CPDF_Number::Clone()
{
	CPDF_NumberPtr ptr;
	if ( mbInteger )
	{
		ptr.Reset( GetAllocator()->New<CPDF_Number>( mInteger, GetAllocator() ) );
	}else
	{
		ptr.Reset( GetAllocator()->New<CPDF_Number>( mFloat, GetAllocator() ) );
	}
	return ptr;
}

CPDF_StringPtr CPDF_String::Create( const CByteString & str, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_StringPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_String>( str, pAllocator ) );
	return ptr;
}

CByteString & CPDF_String::GetString()
{
	return mString;
}

void	CPDF_String::SetString( CByteString & name )
{
	mString = name;

	SetModified( TRUE );
}

CPDF_StringPtr CPDF_String::Clone()
{
	CPDF_StringPtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_String>( mString, GetAllocator() ) );
	return ptr;
}

CPDF_NamePtr CPDF_Name::Create( const CByteString & str, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_NamePtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Name>( str, pAllocator ) );
	return ptr;
}

CPDF_NamePtr CPDF_Name::Clone()
{
	CPDF_NamePtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Name>( mName, GetAllocator() ) );
	return ptr;
}

CPDF_ReferencePtr CPDF_Reference::Create( size_t refNum, size_t genNum, CPDF_File * pFile, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_ReferencePtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Reference>( refNum, genNum, pFile, pAllocator ) );
	return ptr;
}


CPDF_ReferencePtr CPDF_Reference::Clone()
{
	CPDF_ReferencePtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Reference>( mRefObjNum, mRefGenNum, mpFile, GetAllocator() ) );
	return ptr;
}

CPDF_ObjectPtr CPDF_Reference::GetRefObj()
{
	if ( mpFile == nullptr )
	{
		return CPDF_ObjectPtr();
	}
	PDF_RefInfo refInfo;
	refInfo.objNum = mRefObjNum;
	refInfo.genNum = mRefGenNum;
	return mpFile->GetObject( refInfo );
}

CPDF_ObjectPtr CPDF_Reference::GetRefObj( PDF_OBJ_TYPE Type )
{
	CPDF_ObjectPtr ptr;
	if ( mpFile == nullptr )
	{
		return CPDF_ObjectPtr();
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
				return CPDF_ReferencePtr();
			}
			if ( ptr->GetType() == Type )
			{
				return ptr;
			}
			if ( ptr->GetType() == OBJ_TYPE_REFERENCE || ptr->GetType() == OBJ_TYPE_ARRAY )
			{
				continue;
			}
			return CPDF_ObjectPtr();
		}else if ( ptr->GetType() == OBJ_TYPE_ARRAY )
		{
			CPDF_ObjectPtr pElement;
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
			return CPDF_ObjectPtr();
		}else if ( ptr->GetType() == Type )
		{
			return ptr;
		}else{
			return CPDF_ObjectPtr();
		}
	}
	return CPDF_ObjectPtr();
}

CPDF_ArrayPtr CPDF_Array::Create( CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_ArrayPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Array>( pAllocator ) );
	return ptr;
}

CPDF_ObjectPtr CPDF_Array::GetElement( size_t index ) const
{
	if ( index < GetCount() )
	{
		return mArray[index];
	}
	return CPDF_ObjectPtr();
}

CPDF_ObjectPtr CPDF_Array::GetElement( size_t index, PDF_OBJ_TYPE Type ) const
{
	CPDF_ObjectPtr ptr = GetElement( index );
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
	return CPDF_ObjectPtr();
}

CPDF_ObjectPtr CPDF_Array::GetElementByType( PDF_OBJ_TYPE Type )
{
	CPDF_ObjectPtr ptr;

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

bool CPDF_Array::Append( const CPDF_ObjectPtr & ptr )
{
	if ( ptr )
	{
		mArray.push_back( ptr );
		SetModified( TRUE );
		return TRUE;
	}
	return false;
}

CPDF_NullPtr CPDF_Array::AppendNull()
{
    CPDF_NullPtr ptr = CPDF_Null::Create(GetAllocator());
    mArray.push_back( ptr );
    SetModified( TRUE );
    return ptr;
}

CPDF_BooleanPtr CPDF_Array::AppendBoolean(bool val/*= false*/)
{
    CPDF_BooleanPtr ptr = CPDF_Boolean::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_NumberPtr CPDF_Array::AppendNumber()
{
    CPDF_NumberPtr ptr = CPDF_Number::Create(0, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_NumberPtr CPDF_Array::AppendNumber(int32 val)
{
    CPDF_NumberPtr ptr = CPDF_Number::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_NumberPtr CPDF_Array::AppendNumber(FLOAT val)
{
    CPDF_NumberPtr ptr = CPDF_Number::Create(val, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_NamePtr CPDF_Array::AppendName()
{
    CPDF_NamePtr ptr = CPDF_Name::Create(CByteString(GetAllocator()), GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_NamePtr CPDF_Array::AppendName(CByteString & str)
{
    CPDF_NamePtr ptr = CPDF_Name::Create(str, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_StringPtr CPDF_Array::AppendString()
{
    CPDF_StringPtr ptr = CPDF_String::Create(CByteString(GetAllocator()), GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_StringPtr CPDF_Array::AppendString(CByteString & str)
{
    CPDF_StringPtr ptr = CPDF_String::Create(str, GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_ArrayPtr CPDF_Array::AppendArray()
{
    CPDF_ArrayPtr ptr = CPDF_Array::Create();
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_DictionaryPtr CPDF_Array::AppendDictionary()
{
    CPDF_DictionaryPtr ptr = CPDF_Dictionary::Create(GetAllocator());
    mArray.push_back(ptr);
    SetModified(TRUE);
    return ptr;
}

CPDF_ReferencePtr CPDF_Array::AppendReference(PDF_RefInfo info, CPDF_File * pFile)
{
    CPDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CPDF_Reference::Create(info.objNum, info.genNum, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CPDF_ReferencePtr CPDF_Array::AppendReference(size_t objNum, size_t genNum, CPDF_File * pFile)
{
    CPDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CPDF_Reference::Create(objNum, genNum, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CPDF_ReferencePtr CPDF_Array::AppendReference(CPDF_ReferencePtr & ptr)
{
    if (ptr)
    {
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

CPDF_ReferencePtr CPDF_Array::AppendReference(CPDF_File * pFile)
{
    CPDF_ReferencePtr ptr;
    if (pFile)
    {
        ptr = CPDF_Reference::Create(0, 0, pFile);
        mArray.push_back(ptr);
        SetModified(TRUE);
    }
    return ptr;
}

bool CPDF_Array::Replace(size_t index, const CPDF_ObjectPtr & pObj)
{
    if (index > mArray.size())
    {
        return false;
    }
    mArray[index] = pObj;
    SetModified(TRUE);
    return TRUE;
}

CPDF_NullPtr CPDF_Array::ReplaceNull( size_t index )
{
    CPDF_NullPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_NullPtr ptr = CPDF_Null::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CPDF_BooleanPtr CPDF_Array::ReplaceBoolean( size_t index )
{
    CPDF_BooleanPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_BooleanPtr ptr = CPDF_Boolean::Create(false, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CPDF_NumberPtr CPDF_Array::ReplaceNumber( size_t index )
{
    CPDF_NumberPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_NumberPtr ptr = CPDF_Number::Create(0, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CPDF_NamePtr CPDF_Array::ReplaceName( size_t index )
{
    CPDF_NamePtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_NamePtr ptr = CPDF_Name::Create(CByteString(GetAllocator()), GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CPDF_StringPtr CPDF_Array::ReplaceString( size_t index )
{
    CPDF_StringPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_StringPtr ptr = CPDF_String::Create(CByteString(GetAllocator()), GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

CPDF_ArrayPtr CPDF_Array::ReplaceArray( size_t index )
{
    CPDF_ArrayPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_ArrayPtr ptr = CPDF_Array::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}


CPDF_DictionaryPtr CPDF_Array::ReplaceDictionary( size_t index )
{
    CPDF_DictionaryPtr ptrRet;
    if (index <= mArray.size())
    {
        CPDF_DictionaryPtr ptr = CPDF_Dictionary::Create(GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}


CPDF_ReferencePtr CPDF_Array::ReplaceReference( size_t index, CPDF_File * pFile )
{
    CPDF_ReferencePtr ptrRet;
    if (index <= mArray.size() && pFile)
    {
        CPDF_ReferencePtr ptr = CPDF_Reference::Create(0, 0, pFile, GetAllocator());
        mArray[index] = ptr;
        SetModified(TRUE);
    }
    return ptrRet;
}

void CPDF_Array::Clear()
{
    mArray.clear();
    SetModified(TRUE);
}

CPDF_ArrayPtr CPDF_Array::Clone()
{
	CPDF_ArrayPtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Array>( GetAllocator() ) );
	if ( ptr )
	{
		CPDF_ObjectPtr tmpPtr;
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

bool CPDF_Array::IsModified()
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

bool CPDF_Array::GetRect( CRect & rect ) const
{
	if ( GetCount() >= 4 )
	{
		FLOAT llx = 0, lly = 0, rux = 0, ruy = 0;
		CPDF_ObjectPtr objPtr;
		CPDF_NumberPtr numberPtr;
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

bool CPDF_Array::GetMatrix( CMatrix & maxtrix ) const
{
	if ( GetCount() >= 6 )
	{
		FLOAT a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
		CPDF_ObjectPtr objPtr;
		CPDF_NumberPtr numberPtr;
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

CPDF_DictionaryPtr CPDF_Dictionary::Create( CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_DictionaryPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Dictionary>( pAllocator ) );
	return ptr;
}

CPDF_ObjectPtr CPDF_Dictionary::GetElement( const CByteString & key )const
{
    std::unordered_map<string,CPDF_ObjectPtr>::const_iterator it = mMap.find( std::string(key.GetData()) );
	if ( it != mMap.end() )
	{
		return it->second;
	}
	return CPDF_ObjectPtr();
}

CPDF_ObjectPtr CPDF_Dictionary::GetElement( const CByteString & key, PDF_OBJ_TYPE Type )
{
	CPDF_ObjectPtr ptr = GetElement( key );
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

	return CPDF_ObjectPtr();
}

bool CPDF_Dictionary::SetObject( const CByteString & key, const CPDF_ObjectPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CPDF_NullPtr	CPDF_Dictionary::SetNull( const CByteString & key )
{
    CPDF_NullPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CPDF_Null::Create( GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_BooleanPtr CPDF_Dictionary::SetBoolean( const CByteString & key, bool value )
{
    CPDF_BooleanPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CPDF_Boolean::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_NumberPtr CPDF_Dictionary::SetInteger( const CByteString & key, int32 value )
{
    CPDF_NumberPtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CPDF_Number::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_NumberPtr CPDF_Dictionary::SetFloatNumber( const CByteString & key, FLOAT value )
{
    CPDF_NumberPtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CPDF_Number::Create( value, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_StringPtr CPDF_Dictionary::SetString( const CByteString & key, const CByteString & str )
{
    CPDF_StringPtr ptr;
	if ( key.GetLength() > 0 )
	{
 		ptr = CPDF_String::Create( str, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_NamePtr	CPDF_Dictionary::SetName( const CByteString & key, const CByteString & name )
{
    CPDF_NamePtr ptr;
	if ( key.GetLength() > 0 )
	{
		ptr = CPDF_Name::Create( name, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

bool	CPDF_Dictionary::SetArray( const CByteString & key, const CPDF_ArrayPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CPDF_ArrayPtr CPDF_Dictionary::SetArray( const CByteString & key )
{
    CPDF_ArrayPtr ptr;
    if (key.GetLength() > 0)
    {
        ptr = CPDF_Array::Create(GetAllocator());
        mMap[string(key.GetData())] = ptr;
        SetModified( TRUE );
    }
    return ptr;
}

bool CPDF_Dictionary::SetDictionary( const CByteString & key, const CPDF_DictionaryPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
        return TRUE;
	}
    return false;
}

CPDF_DictionaryPtr CPDF_Dictionary::SetDictionary( const CByteString & key )
{
    CPDF_DictionaryPtr ptr;
    if (key.GetLength() > 0)
    {
        ptr = CPDF_Dictionary::Create(GetAllocator());
        mMap[string(key.GetData())] = ptr;
        SetModified( TRUE );
    }
    return ptr;
}

CPDF_ReferencePtr CPDF_Dictionary::SetReference( const CByteString & key, size_t objNum, size_t genNum, CPDF_File * pFile )
{
    CPDF_ReferencePtr ptr;
	if ( key.GetLength() > 0 )
	{
        ptr = CPDF_Reference::Create( objNum, genNum, pFile, GetAllocator() );
		mMap[string(key.GetData())] = ptr;
		SetModified( TRUE );
	}
    return ptr;
}

CPDF_DictionaryPtr CPDF_Dictionary::Clone()
{
	CPDF_DictionaryPtr ptr;
	ptr.Reset( GetAllocator()->New<CPDF_Dictionary>( GetAllocator() ) );
	if ( ptr )
	{
		unordered_map<string,CPDF_ObjectPtr>::iterator it;
		for ( it = mMap.begin(); it != mMap.end(); ++it )
		{
			ptr->SetObject( it->first.c_str(), it->second );
		}
	}
	return ptr;
}

void CPDF_Dictionary::Clear()
{
    mMap.clear();
    mIt = mMap.end();
    SetModified(TRUE);
}

bool CPDF_Dictionary::IsModified()
{
	if ( mModified )
	{
		return TRUE;
	}
	unordered_map<string,CPDF_ObjectPtr>::iterator it;
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

bool CPDF_Dictionary::CheckName( const CByteString & key, const CByteString & name, bool bRequired /*= TRUE*/ )
{
	CPDF_ObjectPtr objPtr = GetElement( key, OBJ_TYPE_NAME );
	if ( objPtr )
	{
		CPDF_NamePtr namePtr = objPtr->GetNamePtr();
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

void CPDF_Dictionary::MoveToFirst()
{
    mIt = mMap.begin();
}

bool CPDF_Dictionary::GetKeyAndElement( CByteString & key, CPDF_ObjectPtr & objPtr )
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

bool CPDF_Dictionary::Remove( const CByteString & key )
{
    unordered_map<string,CPDF_ObjectPtr>::iterator it = mMap.find( key.GetData() );
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

CPDF_StreamPtr CPDF_Stream::Create( size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt /*= nullptr*/, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Stream>( objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CPDF_StreamPtr CPDF_Stream::Create( PBYTE pData, size_t size, const CPDF_DictionaryPtr & pDict, size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt /*= nullptr*/, CAllocator * pAllocator /*= nullptr*/ ) 
{
	CPDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Stream>( pData, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CPDF_StreamPtr CPDF_Stream::Create( IRead* pFile, size_t offset, size_t size, const CPDF_DictionaryPtr pDict, size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt /*= nullptr*/, CAllocator * pAllocator /*= nullptr*/ )
{
	CPDF_StreamPtr ptr;
	if ( pAllocator == nullptr )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.Reset( pAllocator->New<CPDF_Stream>( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CPDF_Stream::CPDF_Stream( PBYTE pData, size_t size, const CPDF_DictionaryPtr & ptr,
								size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt,
								CAllocator * pAllocator ) : CPDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
		mDictPtr = CPDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetInteger( "Length", (int32)m_dwSize );
}
	
CPDF_Stream::CPDF_Stream( IRead* pFile, size_t offset, size_t size, const CPDF_DictionaryPtr & ptr,
                          size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt, CAllocator * pAllocator )
  : CPDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
		mDictPtr = CPDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetInteger( "Length", (int32)m_dwSize );
}

CPDF_Stream::CPDF_Stream( size_t objNum, size_t genNum, CPDF_Encrypt * pEncrypt, CAllocator * pAllocator )
  : CPDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
	mDictPtr = CPDF_Dictionary::Create( GetAllocator() );
}

CPDF_Stream::~CPDF_Stream()
{
 	if ( m_bMem == TRUE && m_pDataBuf )
 	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
 	}
}

CPDF_StreamPtr CPDF_Stream::Clone()
{
	CPDF_StreamPtr ptr;
	CPDF_DictionaryPtr pDict;
	if ( mDictPtr )
	{
		pDict = mDictPtr->Clone();
	}
	if ( m_bMem )
	{
		PBYTE pDataBuf = GetAllocator()->NewArray<BYTE>( m_dwSize );
		ptr = CPDF_Stream::Create( pDataBuf, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}else
	{
		ptr = CPDF_Stream::Create( m_pFile, m_FileOffset, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}
	return ptr;
}

bool	CPDF_Stream::IsModified()
{
	if ( mModified || mDictPtr->IsModified() )
	{
		mModified = TRUE;
		return TRUE;
	}
	return false;
}

void	CPDF_Stream::SetDictPtr( const CPDF_DictionaryPtr & pDict )
{
	mDictPtr = pDict;

	SetModified( TRUE );
}

bool CPDF_Stream::SetRawData( PBYTE pData, size_t dwDataSize, BYTE filter/* = STREAM_FILTER_NULL*/ )
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
		mDictPtr = CPDF_Dictionary::Create( nullptr );
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
			CDynBuffer buf;
			CPDF_HexFilter filter;
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
			CDynBuffer buf;
			CPDF_ASCII85Filter filter;
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
			CDynBuffer buf;
			CPDF_FlateFilter filter;
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
			CDynBuffer buf;
			CPDF_LZWFilter filter;
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
			CDynBuffer buf;
			CPDF_RLEFileter filter;
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

size_t CPDF_Stream::GetRawData( size_t offset, PBYTE pBuf, size_t buf_size ) const
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

CPDF_StreamAcc::CPDF_StreamAcc( CAllocator * pAllocator ) : CObject( pAllocator ), m_dwSize(0), m_pDataBuf(nullptr) {}

CPDF_StreamAcc::~CPDF_StreamAcc()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
	}
	m_dwSize = 0;
	mStreamPtr.Reset();
}

bool CPDF_StreamAcc::Attach( const CPDF_StreamPtr & stmPtr, PDF_STREAM_DECODE_MODE mode )
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
	CPDF_DictionaryPtr pDict = stmPtr->GetDictPtr();
	if ( pDict )
	{
		size_t lFilterCount = 0;
		size_t length = stmPtr->GetRawSize();
		CPDF_ObjectPtr pFilter = pDict->GetElement( "Filter" );
		CPDF_ObjectPtr pParms = pDict->GetElement( "DecodeParms" );

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

		CPDF_NamePtr * pFilterNameArr = nullptr;
		CPDF_DictionaryPtr * pParamDictArr = nullptr;
	
		pFilterNameArr = GetAllocator()->NewArray<CPDF_NamePtr>( lFilterCount );
		pParamDictArr = GetAllocator()->NewArray<CPDF_DictionaryPtr>( lFilterCount );

		if ( pFilter->GetType() == OBJ_TYPE_NAME )
		{
			pFilterNameArr[0] = pFilter->GetNamePtr();
		}else if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
		{
			CPDF_ArrayPtr tmpArrayPtr = pFilter->GetArrayPtr();
			for ( size_t i = 0; i < lFilterCount; i++ )
			{
				pFilterNameArr[i] = tmpArrayPtr->GetElement( i )->GetNamePtr();
			}
		}else{
			GetAllocator()->DeleteArray<CPDF_NamePtr>( pFilterNameArr );
			GetAllocator()->DeleteArray<CPDF_DictionaryPtr>( pParamDictArr );
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
		CByteString str( GetAllocator() );
		CDynBuffer buffer( bufSize * 2, bufSize, GetAllocator() );
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
				CPDF_HexFilter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "ASCII85Decode" || str == "A85" )
			{
				CPDF_ASCII85Filter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "LZWDecode" || str == "LZW" )
			{
				CPDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				if ( ! pDecodeParams )
				{
					CPDF_LZWFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else{
					CPDF_Predictor pPredictor( pDecodeParams, GetAllocator() );
					CPDF_LZWFilter filter( GetAllocator() );
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
				CPDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				if ( !pDecodeParams )
				{
					CPDF_FlateFilter filter( GetAllocator() );
					filter.Decode( pTmp, lSize, buffer );
				}else{
					CPDF_Predictor pPredictor( pDecodeParams, GetAllocator() );
					CPDF_FlateFilter filter( GetAllocator() );
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
				CPDF_RLEFileter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "CCITTFaxDecode" || str == "CCF" )
			{
				CPDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
				CPDF_FaxDecodeParams params( pDecodeParams );
				CPDF_FaxFilter filter( &params, GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "JBIG2Decode" )
			{
				CPDF_JBig2Filter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "DCTDecode" || str == "DCT" )
			{
				CPDF_DCTDFilter filter( GetAllocator() );
				filter.Decode( pTmp, lSize, buffer );
			}else if ( str == "JPXDecode" )
			{
                CPDF_JPXFilter fileter( GetAllocator() );
                fileter.Decode( pTmp, lSize, buffer );
			}else if ( str == "Crypt" )
			{
				retValue = false;
			}

			if ( retValue == false )
			{
				GetAllocator()->DeleteArray<BYTE>( pTmp );
				GetAllocator()->DeleteArray<CPDF_NamePtr>( pFilterNameArr );
				GetAllocator()->DeleteArray<CPDF_DictionaryPtr>( pParamDictArr );
				return false;
			}else{
				lSize = buffer.GetSize();
				GetAllocator()->DeleteArray<BYTE>( pTmp );
				pTmp = GetAllocator()->NewArray<BYTE>( lSize );
				buffer.Read( pTmp, lSize );
			}
		}

		GetAllocator()->DeleteArray<CPDF_NamePtr>( pFilterNameArr );
		GetAllocator()->DeleteArray<CPDF_DictionaryPtr>( pParamDictArr );

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

void CPDF_StreamAcc::Detach()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<BYTE>( m_pDataBuf );
		m_pDataBuf = nullptr;
	}
	mStreamPtr.Reset();
	m_dwSize = 0;
}

bool IsPdfNull( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfBoolean( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNumber( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfName( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfString( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfDict( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfArray( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfRef( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStream( CPDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNullPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfBooleanPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNumberPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfNamePtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStringPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfDictPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfArrayPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfRefPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return false;
}

bool IsPdfStreamPtr( const CPDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return false;
}

}//namespace