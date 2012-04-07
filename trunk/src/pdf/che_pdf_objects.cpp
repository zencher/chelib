#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_objects.h"
#include "../../include/pdf/che_pdf_filter.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_file.h"
#include <memory.h>

CHE_PDF_Object::CHE_PDF_Object( PDF_OBJ_TYPE type, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mModified( FALSE ), mType( type ) {}

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

CHE_PDF_NullPtr CHE_PDF_Object::GetNullPtr() const
{
	CHE_PDF_NullPtr ptr;
	if ( mType == OBJ_TYPE_NULL )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Object::GetBooleanPtr() const
{
	CHE_PDF_BooleanPtr ptr;
	if ( mType == OBJ_TYPE_BOOLEAN )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Object::GetNumberPtr() const
{
	CHE_PDF_NumberPtr ptr;
	if ( mType == OBJ_TYPE_NUMBER )
	{
		ptr.reset((CHE_PDF_Object*) this );
	}
	return ptr;
}

CHE_PDF_StringPtr CHE_PDF_Object::GetStringPtr() const
{
	CHE_PDF_StringPtr ptr;
	if ( mType == OBJ_TYPE_STRING )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Object::GetNamePtr() const
{
	CHE_PDF_NamePtr ptr;
	if ( mType == OBJ_TYPE_NAME )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ArrayPtr CHE_PDF_Object::GetArrayPtr() const
{
	CHE_PDF_ArrayPtr ptr;
	if ( mType == OBJ_TYPE_ARRAY )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_DictionaryPtr CHE_PDF_Object::GetDictPtr() const
{
	CHE_PDF_DictionaryPtr ptr;
	if ( mType == OBJ_TYPE_DICTIONARY )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Object::GetRefPtr() const
{
	CHE_PDF_ReferencePtr ptr;
	if ( mType == OBJ_TYPE_REFERENCE )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Object::GetStreamPtr() const
{
	CHE_PDF_StreamPtr ptr;
	if ( mType == OBJ_TYPE_STREAM )
	{
		ptr.reset( (CHE_PDF_Object*)this );
	}
	return ptr;
}

CHE_PDF_ObjectPtr::CHE_PDF_ObjectPtr( const CHE_PDF_ObjectPtr & ptr )
	: mpObj( NULL )
{
	if ( mpObj != ptr.mpObj && ptr.mpObj != NULL )
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
			mpObj = NULL;
		}
	}
	mpObj = ptr.mpObj;
	if ( mpObj )
	{
		mpObj->mRefs.AddRef();
	}
	return *this;
}

HE_VOID CHE_PDF_ObjectPtr::reset( CHE_PDF_Object * pObj /*= NULL*/ )
{
	if ( mpObj != pObj && pObj != NULL )
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
	else
	{
		mpObj = NULL;
	}
}

// CHE_PDF_BooleanPtr CHE_PDF_ObjectPtr::GetBooleanPtr() const
// {
// 	CHE_PDF_BooleanPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_BOOLEAN )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_NumberPtr CHE_PDF_ObjectPtr::GetNumberPtr() const
// {
// 	CHE_PDF_NumberPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NUMBER )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_StringPtr CHE_PDF_ObjectPtr::GetStringPtr() const
// {
// 	CHE_PDF_StringPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STRING )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_NamePtr	CHE_PDF_ObjectPtr::GetNamePtr() const
// {
// 	CHE_PDF_NamePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_NAME )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ArrayPtr CHE_PDF_ObjectPtr::GetArrayPtr() const
// {
// 	CHE_PDF_ArrayPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_ARRAY )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_DictionaryPtr CHE_PDF_ObjectPtr::GetDictPtr() const
// {
// 	CHE_PDF_DictionaryPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_DICTIONARY )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_ReferencePtr CHE_PDF_ObjectPtr::GetRefPtr() const
// {
// 	CHE_PDF_ReferencePtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_REFERENCE )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }
// 
// CHE_PDF_StreamPtr CHE_PDF_ObjectPtr::GetStreamPtr() const
// {
// 	CHE_PDF_StreamPtr ptr;
// 	if ( mpObj && mpObj->GetType() == OBJ_TYPE_STREAM )
// 	{
// 		ptr.reset( mpObj );
// 	}
// 	return ptr;
// }

CHE_PDF_NullPtr CHE_PDF_Null::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_NullPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();	
	}
	ptr.reset( pAllocator->New<CHE_PDF_Null>( pAllocator ) );
	return ptr;
}

CHE_PDF_NullPtr CHE_PDF_Null::Clone()
{
	CHE_PDF_NullPtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Null>( GetAllocator() ) );
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Boolean::Create( HE_BOOL value, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_BooleanPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Boolean>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_BooleanPtr CHE_PDF_Boolean::Clone()
{
	CHE_PDF_BooleanPtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Boolean>( mbValue, GetAllocator() ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Create( HE_INT32 value,  CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_NumberPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Number>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Create( HE_FLOAT value, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_NumberPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Number>( value, pAllocator ) );
	return ptr;
}

CHE_PDF_NumberPtr CHE_PDF_Number::Clone()
{
	CHE_PDF_NumberPtr ptr;
	if ( mbInteger )
	{
		ptr.reset( GetAllocator()->New<CHE_PDF_Number>( mInteger, GetAllocator() ) );
	}else
	{
		ptr.reset( GetAllocator()->New<CHE_PDF_Number>( mFloat, GetAllocator() ) );
	}
	return ptr;
}

CHE_PDF_StringPtr CHE_PDF_String::Create( const CHE_ByteString & str, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_StringPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_String>( str, pAllocator ) );
	return ptr;
}

CHE_ByteString & CHE_PDF_String::GetString()
{
	return mString;
}

HE_VOID	CHE_PDF_String::SetString( CHE_ByteString & name )
{
	mString = name;
}

CHE_PDF_StringPtr CHE_PDF_String::Clone()
{
	CHE_PDF_StringPtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_String>( mString, GetAllocator() ) );
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Name::Create( const CHE_ByteString & str, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_NamePtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Name>( str, pAllocator ) );
	return ptr;
}

CHE_PDF_NamePtr CHE_PDF_Name::Clone()
{
	CHE_PDF_NamePtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Name>( mName, GetAllocator() ) );
	return ptr;
}

CHE_PDF_ReferencePtr CHE_PDF_Reference::Create( HE_DWORD refNum, HE_DWORD genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_ReferencePtr ptr;
	if ( pAllocator )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Reference>( refNum, genNum, pFile, pAllocator ) );
	return ptr;
}


CHE_PDF_ReferencePtr CHE_PDF_Reference::Clone()
{
	CHE_PDF_ReferencePtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Reference>( mRefObjNum, mRefGenNum, mpFile, GetAllocator() ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Reference::GetRefObj()
{
	if ( mpFile == NULL )
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
	if ( mpFile == NULL )
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
			HE_DWORD dwCount = ptr->GetArrayPtr()->GetCount();
			HE_BOOL bNeedContinue = FALSE;
			for ( HE_DWORD i = 0; i < dwCount; i++ )
			{
				pElement = ptr->GetArrayPtr()->GetElement( i, Type );
				if ( pElement == NULL )
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

CHE_PDF_ArrayPtr CHE_PDF_Array::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_ArrayPtr ptr;
	if ( pAllocator )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Array>( pAllocator ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElement( HE_DWORD index ) const
{
	if ( index < GetCount() )
	{
		return mArray[index];
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElement( HE_DWORD index, PDF_OBJ_TYPE Type )
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
		return ptr->GetRefPtr()->GetRefObj( Type );;
	}
	return CHE_PDF_ObjectPtr();
}

CHE_PDF_ObjectPtr CHE_PDF_Array::GetElementByType( PDF_OBJ_TYPE Type )
{
	CHE_PDF_ObjectPtr ptr;

	HE_DWORD lCount = mArray.size();

	for ( HE_DWORD i = 0; i < lCount; i++ )
	{
		ptr = GetElement( i, Type );
		if ( ptr )
		{
			return ptr;
		}
	}
	return ptr;
}

HE_BOOL CHE_PDF_Array::Append( const CHE_PDF_ObjectPtr & ptr )
{
	if ( ptr )
	{
		mArray.push_back( ptr );
		return TRUE;
	}
	return FALSE;
}

CHE_PDF_ArrayPtr CHE_PDF_Array::Clone()
{
	CHE_PDF_ArrayPtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Array>( GetAllocator() ) );
	if ( ptr )
	{
		CHE_PDF_ObjectPtr tmpPtr;
		for ( HE_DWORD i = 0; i < GetCount(); ++i )
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

CHE_PDF_DictionaryPtr CHE_PDF_Dictionary::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_DictionaryPtr ptr;
	if ( pAllocator )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Dictionary>( pAllocator ) );
	return ptr;
}

CHE_PDF_ObjectPtr CHE_PDF_Dictionary::GetElement( const CHE_ByteString & key )const
{
	for ( size_t i = 0; i < mKeys.size(); ++i )
	{
		if ( mKeys[i] == key )
		{
			return mPtrs[i];
		}
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

HE_VOID CHE_PDF_Dictionary::SetAtObj( const CHE_ByteString & key, const CHE_PDF_ObjectPtr & ptr )
{
	if ( key.GetLength() > 0 )
	{
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtNull( const CHE_ByteString & key )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_NullPtr ptr = CHE_PDF_Null::Create( GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtBoolean( const CHE_ByteString & key, BOOL value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_BooleanPtr ptr = CHE_PDF_Boolean::Create( value, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtInteger( const CHE_ByteString & key, HE_INT32 value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create( value, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_NumberPtr ptr = CHE_PDF_Number::Create( value, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtString( const CHE_ByteString & key, const CHE_ByteString & string )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_StringPtr ptr = CHE_PDF_String::Create( string, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtName( const CHE_ByteString & key, const CHE_ByteString & name )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_NamePtr ptr = CHE_PDF_Name::Create( name, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtArray( const CHE_ByteString & key, const CHE_PDF_ArrayPtr & ptr )
{
	if ( key.GetLength() > 0 && ptr )
	{
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID CHE_PDF_Dictionary::SetAtDictionary( const CHE_ByteString & key, const CHE_PDF_DictionaryPtr & ptr )
{
	if ( key.GetLength() > 0 )
	{
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

HE_VOID	CHE_PDF_Dictionary::SetAtReference( const CHE_ByteString & key, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_File * pFile )
{
	if ( key.GetLength() > 0 )
	{
		CHE_PDF_ReferencePtr ptr = CHE_PDF_Reference::Create( objNum, genNum, pFile, GetAllocator() );
		for ( size_t i = 0; i < mKeys.size(); ++i )
		{
			if ( mKeys[i] == key )
			{
				mPtrs[i] = ptr;
				return;
			}
		}
		mKeys.push_back( key );
		mPtrs.push_back( ptr );
	}
}

CHE_PDF_DictionaryPtr CHE_PDF_Dictionary::Clone()
{
	CHE_PDF_DictionaryPtr ptr;
	ptr.reset( GetAllocator()->New<CHE_PDF_Dictionary>( GetAllocator() ) );
	if ( ptr )
	{
		CHE_PDF_ObjectPtr tmpPtr;
		CHE_ByteString keyStr( GetAllocator() );
		for ( HE_DWORD i = 0; i < GetCount(); ++i )
		{
			GetKeyByIndex( i, keyStr );
			tmpPtr = GetElementByIndex( i );
			if ( tmpPtr )
			{
				tmpPtr = tmpPtr->Clone();
			}
			ptr->SetAtObj( keyStr, tmpPtr );
		}
	}
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt /*= NULL*/, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Stream>( objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( HE_LPBYTE pData, HE_DWORD size, const CHE_PDF_DictionaryPtr & pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt /*= NULL*/, CHE_Allocator * pAllocator /*= NULL*/ ) 
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Stream>( pData, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_StreamPtr CHE_PDF_Stream::Create( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, const CHE_PDF_DictionaryPtr pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt /*= NULL*/, CHE_Allocator * pAllocator /*= NULL*/ ) 
{
	CHE_PDF_StreamPtr ptr;
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}
	ptr.reset( pAllocator->New<CHE_PDF_Stream>( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pAllocator ) );
	return ptr;
}

CHE_PDF_Stream::CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, const CHE_PDF_DictionaryPtr & ptr,
								HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
	if ( ptr )
	{
		mDictPtr = ptr;
	}else{
		mDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetAtInteger( "Length", m_dwSize );
}
	
CHE_PDF_Stream::CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, const CHE_PDF_DictionaryPtr & ptr,
								HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
	if ( ptr )
	{
		mDictPtr = ptr;
	}else{
		mDictPtr = CHE_PDF_Dictionary::Create( GetAllocator() );
	}
	mDictPtr->SetAtInteger( "Length", m_dwSize );
}

CHE_PDF_Stream::CHE_PDF_Stream(	HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt,
								CHE_Allocator * pAllocator ) : CHE_PDF_Object( OBJ_TYPE_STREAM, pAllocator )
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
		HE_LPBYTE pDataBuf = GetAllocator()->NewArray<HE_BYTE>( m_dwSize );
		ptr = CHE_PDF_Stream::Create( pDataBuf, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}else
	{
		ptr = CHE_PDF_Stream::Create( m_pFile, m_FileOffset, m_dwSize, pDict, m_dwObjNum, m_dwGenNum, m_pEncrypt, GetAllocator() );
	}
	return ptr;
}

HE_VOID	CHE_PDF_Stream::SetDict( const CHE_PDF_DictionaryPtr & pDict )
{
	mDictPtr = pDict;
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
	if ( ! mDictPtr )
	{
		mDictPtr = CHE_PDF_Dictionary::Create( NULL );
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
				mDictPtr->SetAtName( "Filter", "ASCIIHexDecode" );
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
				mDictPtr->SetAtName( "Filter", "ASCII85Decode" );
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
				mDictPtr->SetAtName( "Filter", "FlateDecode" );
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
				mDictPtr->SetAtName( "Filter", "LZWDecode" );
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
				mDictPtr->SetAtName( "Filter", "RunLengthDecode" );
			}
			break;
		}
	default:
		break;
	}
	mDictPtr->SetAtInteger( "Length", m_dwSize );
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

CHE_PDF_StreamAcc::CHE_PDF_StreamAcc( CHE_Allocator * pAllocator ) : CHE_Object( pAllocator ), m_dwSize(0), m_pDataBuf(NULL) {}

CHE_PDF_StreamAcc::~CHE_PDF_StreamAcc()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_pDataBuf );
	}
	m_dwSize = 0;
	mStreamPtr.reset();
}

HE_BOOL CHE_PDF_StreamAcc::Attach( const CHE_PDF_StreamPtr & pStream )
{
	if ( pStream )
	{
		m_pDataBuf = NULL;
		m_dwSize = 0;
		mStreamPtr = pStream;

		HE_BOOL retValue = TRUE;
		
		CHE_PDF_DictionaryPtr pDict = pStream->GetDictPtr();
		if ( pDict )
		{
			HE_DWORD lFilterCount = 0;
			HE_DWORD length = pStream->GetRawSize();
			CHE_PDF_ObjectPtr pFilter = pDict->GetElement( "Filter" );
			CHE_PDF_ObjectPtr pParms = pDict->GetElement( "DecodeParms" );
			if ( ! pFilter )
			{
				m_dwSize = length;
				m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( length );
				pStream->GetRawData( 0, m_pDataBuf, length );
				if ( pStream->m_pEncrypt && pStream->m_pEncrypt->IsPasswordOK() == TRUE )
				{
					pStream->m_pEncrypt->Decrypt( m_pDataBuf, length, pStream->GetObjNum(), pStream->GetGenNum() );
				}
				return TRUE;
			}else{
				if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
				{
					lFilterCount = pFilter->GetArrayPtr()->GetCount();
				}else{
					lFilterCount = 1;
				}
			}
			CHE_PDF_NamePtr * pFilterNameArr = NULL;
			CHE_PDF_DictionaryPtr * pParamDictArr = NULL;
	
			pFilterNameArr = GetAllocator()->NewArray<CHE_PDF_NamePtr>( lFilterCount );
			pParamDictArr = GetAllocator()->NewArray<CHE_PDF_DictionaryPtr>( lFilterCount );

			if ( pFilter->GetType() == OBJ_TYPE_NAME )
			{
				pFilterNameArr[0] = pFilter->GetNamePtr();
			}else if ( pFilter->GetType() == OBJ_TYPE_ARRAY )
			{
				CHE_PDF_ArrayPtr tmpArrayPtr = pFilter->GetArrayPtr();
				for ( HE_DWORD i = 0; i < lFilterCount; i++ )
				{
					pFilterNameArr[i] = tmpArrayPtr->GetElement( i )->GetNamePtr();
				}
			}else{
				return FALSE;
			}
			if ( pParms && pParms->GetType() == OBJ_TYPE_DICTIONARY )
			{
				pParamDictArr[0] = pParms->GetDictPtr();
			}else if ( pParms && pParms->GetType() == OBJ_TYPE_ARRAY ){
				HE_DWORD lParamCount = pParms->GetArrayPtr()->GetCount();
				for ( HE_DWORD i = 0; i < lParamCount; i++ )
				{
					pParamDictArr[i] = pParms->GetArrayPtr()->GetElement( i )->GetDictPtr();
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
				str = pFilterNameArr[i]->GetStringPtr();
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
						CHE_PDF_LZWFilter filter( NULL, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}else{
						HE_BYTE Predictor = 1;
						HE_BYTE Colors = 1;
						HE_BYTE BitsPerComponent = 8;
						HE_BYTE Columns = 8;
						HE_BYTE EarlyChange = 1;
						CHE_PDF_ObjectPtr pObj = pDecodeParams->GetElement( "Predictor" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Predictor = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Colors" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Colors = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "BitsPerComponent" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							BitsPerComponent = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Columns" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Columns = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "EarlyChange" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							EarlyChange = pObj->GetNumberPtr()->GetInteger();
						}
						CHE_PDF_Predictor pPredictor( Predictor, Colors, BitsPerComponent, Columns, EarlyChange );
						CHE_PDF_LZWFilter filter( &pPredictor, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}
				}else if ( str == "FlateDecode" || str == "Fl" )
				{
					CHE_PDF_DictionaryPtr pDecodeParams = pParamDictArr[i];
					if ( !pDecodeParams )
					{
						CHE_PDF_FlateFilter filter( NULL, GetAllocator() );
						filter.Decode( pTmp, lSize, buffer );
					}else{
						HE_BYTE Predictor = 1;
						HE_BYTE Colors = 1;
						HE_BYTE BitsPerComponent = 8;
						HE_BYTE Columns = 8;
						HE_BYTE EarlyChange = 1;
						CHE_PDF_ObjectPtr pObj = pDecodeParams->GetElement( "Predictor" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Predictor = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Colors" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Colors = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "BitsPerComponent" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							BitsPerComponent = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "Columns" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							Columns = pObj->GetNumberPtr()->GetInteger();
						}
						pObj = pDecodeParams->GetElement( "EarlyChange" );
						if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
						{
							EarlyChange = pObj->GetNumberPtr()->GetInteger();
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
			GetAllocator()->DeleteArray<CHE_PDF_NamePtr>( pFilterNameArr );
			GetAllocator()->DeleteArray<CHE_PDF_DictionaryPtr>( pParamDictArr );
			buffer.Read( pTmp, lSize );

			m_pDataBuf = pTmp;
			m_dwSize = lSize;

			return retValue;
		}else{
			m_dwSize = pStream->GetRawSize();
			m_pDataBuf = GetAllocator()->NewArray<HE_BYTE>( m_dwSize );
			pStream->GetRawData( 0, m_pDataBuf, m_dwSize );			
			return TRUE;
		}
	}
	return FALSE;
}

HE_VOID CHE_PDF_StreamAcc::Detach()
{
	if ( m_pDataBuf )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( m_pDataBuf );
		m_pDataBuf = NULL;
	}
	mStreamPtr.reset();
	m_dwSize = 0;
}

HE_BOOL IsPdfNull( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfBoolean( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfNumber( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfName( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfString( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfDict( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfArray( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfRef( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfStream( CHE_PDF_Object * pObj )
{
	if ( pObj && pObj->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfNullPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NULL )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfBooleanPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_BOOLEAN )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfNumberPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NUMBER )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfNamePtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_NAME )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfStringPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STRING )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfDictPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfArrayPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_ARRAY )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfRefPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_REFERENCE )
	{
		return TRUE;
	}
	return FALSE;
}

HE_BOOL IsPdfStreamPtr( const CHE_PDF_ObjectPtr & objPtr )
{
	if ( objPtr && objPtr->GetType() == OBJ_TYPE_STREAM )
	{
		return TRUE;
	}
	return FALSE;
}