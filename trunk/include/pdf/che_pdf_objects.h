#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"
#include "../che_string.h"
#include "../che_datastructure.h"
#include "che_pdf_define.h"
#include "che_pdf_encrypt.h"

class CHE_PDF_Null;
class CHE_PDF_Boolean;
class CHE_PDF_Number;
class CHE_PDF_String;
class CHE_PDF_Name;
class CHE_PDF_Reference;
class CHE_PDF_Array;
class CHE_PDF_Dictionary;
class CHE_PDF_Stream;
class CHE_PDF_File;

class CHE_PDF_Object : public CHE_Object
{
public:
	PDF_OBJ_TYPE		GetType() const { return mType; }

	CHE_PDF_Object *	Clone();

	HE_VOID				Release();

	CHE_PDF_Boolean *	ToBoolean() const;
	CHE_PDF_Number *	ToNumber() const;
	CHE_PDF_String *	ToString() const;
	CHE_PDF_Name *		ToName() const;
	CHE_PDF_Array*		ToArray() const;
	CHE_PDF_Dictionary*	ToDict() const;
	CHE_PDF_Reference * ToReference() const;
	CHE_PDF_Stream *	ToStream() const;

protected:
	CHE_PDF_Object( CHE_Allocator * pAllocator = NULL ) : CHE_Object(pAllocator), mType(OBJ_TYPE_INVALID) {}

	~CHE_PDF_Object() {} //todo 是否要加上Release调用

	PDF_OBJ_TYPE		mType;

	friend class CHE_Allocator;
};

class CHE_PDF_IndirectObject : public CHE_Object
{
public:
	CHE_PDF_IndirectObject( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Object * pObj,
		CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL )
		: CHE_Object(pAllocator), mObjNum(objNum), mGenNum(genNum), mpObj(pObj), mpFile(pFile) {}

	HE_DWORD			GetObjNum() const { return mObjNum; }
	HE_DWORD			GetGenNum() const { return mGenNum; }
	CHE_PDF_Object *	GetObj() const { return mpObj; }
	CHE_PDF_File *		GetFile() const { return mpFile; }

	CHE_PDF_IndirectObject * Clone()
	{
		return GetAllocator()->New<CHE_PDF_IndirectObject>( mObjNum, mGenNum, mpObj ? mpObj->Clone() : NULL, mpFile, GetAllocator() );
	}

	HE_VOID	Release()
	{
		if ( mpObj )
		{
			mpObj->Release();
		}
		GetAllocator()->Delete( this );
	}

private:
	HE_DWORD			mObjNum;
	HE_DWORD			mGenNum;
	CHE_PDF_Object *	mpObj;
	CHE_PDF_File *		mpFile;
};

class CHE_PDF_ObjectCollector : public CHE_Object
{
public:
	CHE_PDF_ObjectCollector( CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_ObjectCollector();

	HE_VOID Push( CHE_PDF_Object * pObj );

	HE_VOID Push( CHE_PDF_IndirectObject * pInObj );

	HE_VOID PopAll();

	HE_VOID ReleaseAll();

private:
	CHE_Queue<CHE_PDF_Object*> mQueue;
	CHE_Queue<CHE_PDF_IndirectObject*> mInQueue;
};

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:
	static CHE_PDF_Null* Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Null>( pAllocator );
		}else{
			return new CHE_PDF_Null( NULL );
		}
	}

	CHE_PDF_Null * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Null>( GetAllocator() );
	}

private:
	CHE_PDF_Null( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Object(pAllocator) { mType = OBJ_TYPE_NULL; }

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:
	static CHE_PDF_Boolean*	Create( HE_BOOL value, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Boolean>( value, pAllocator );
		}else{
			return new CHE_PDF_Boolean( value, NULL );
		}
	}
	HE_BOOL	GetValue() { return mbValue; }
	HE_VOID SetValue( HE_BOOL value ) { mbValue = value; }

	CHE_PDF_Boolean * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Boolean>( mbValue, GetAllocator() );
	}

private:
	CHE_PDF_Boolean( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mbValue(FALSE) { mType = OBJ_TYPE_BOOLEAN; }
	
	CHE_PDF_Boolean( HE_BOOL value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mbValue(value) { mType = OBJ_TYPE_BOOLEAN; }

	HE_BOOL	mbValue;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:
	static CHE_PDF_Number* Create( HE_INT32 value,  CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Number>( value, pAllocator );
		}else{
			return new CHE_PDF_Number( value, NULL );
		}
	}

	static CHE_PDF_Number* Create( HE_FLOAT value, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Number>( value, pAllocator );
		}else{
			return new CHE_PDF_Number( value, NULL );
		}
	}

	HE_BOOL		IsInteger() const { return mbInteger; }
	HE_INT32	GetInteger() const { return mbInteger ? mInteger : (HE_INT32)mFloat; }
	HE_FLOAT	GetFloat() const { return mbInteger ? (HE_FLOAT)mInteger : mFloat; }
	HE_VOID		SetValue( HE_INT32 value ) { mbInteger = TRUE; mInteger = value; }
	HE_VOID		SetValue( HE_FLOAT value ) { mbInteger = FALSE; mFloat = value; }

	CHE_PDF_Number * Clone()
	{
		if ( mbInteger )
		{
			return GetAllocator()->New<CHE_PDF_Number>( mInteger, GetAllocator() );
		}else
		{
			return  GetAllocator()->New<CHE_PDF_Number>( mFloat, GetAllocator() );
		}
	}

private:
	CHE_PDF_Number( HE_INT32 value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mbInteger(TRUE), mInteger(value) { mType = OBJ_TYPE_NUMBER; }

	CHE_PDF_Number( HE_FLOAT value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mbInteger(FALSE), mFloat(value) { mType = OBJ_TYPE_NUMBER; }

	HE_BOOL			mbInteger;
	union {
		HE_INT32	mInteger;
		HE_FLOAT	mFloat;
	};

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_String : public CHE_PDF_Object
{
public:
	static CHE_PDF_String*	Create( const CHE_ByteString& str, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_String>( str, pAllocator );
		}else{
			return new CHE_PDF_String( str, NULL );
		}
	}

	CHE_ByteString	GetString() { return mString; }
	HE_VOID			SetString( CHE_ByteString & name ) { mString = name; }

	CHE_PDF_String * Clone()
	{
		return GetAllocator()->New<CHE_PDF_String>( mString, GetAllocator() );
	}

private:
	CHE_PDF_String( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mString(pAllocator) { mType = OBJ_TYPE_STRING; }

	CHE_PDF_String( const CHE_ByteString& str, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mString(str) { mType = OBJ_TYPE_STRING; }

	CHE_ByteString		mString;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:
	static CHE_PDF_Name* Create( const CHE_ByteString& str, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Name>( str, pAllocator );
		}else{
			return new CHE_PDF_Name( str, NULL );
		}
	}

	CHE_ByteString	GetString() { return mName; }
	HE_VOID			SetString( CHE_ByteString & name ) { mName = name; };

	CHE_PDF_Name * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Name>( mName, GetAllocator() );
	}
	
private:
	CHE_PDF_Name( const CHE_ByteString& str,  CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mName(str) { mType = OBJ_TYPE_NAME; }

	CHE_ByteString	mName;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

struct HE_PDF_RefInfo
{
	HE_DWORD objNum;
	HE_DWORD genNum;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
	static CHE_PDF_Reference* Create( HE_DWORD refNum, HE_DWORD genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Reference>( refNum, genNum, pFile, pAllocator );
		}else{
			return new CHE_PDF_Reference( refNum, genNum, pFile, NULL );	
		}
	}

	HE_DWORD			GetRefNum() const { return mRefObjNum; }
	HE_VOID				SetRefNum( HE_DWORD objNum ) { mRefObjNum = objNum; }
	HE_DWORD			GetGenNum() const { return mRefGenNum; }
	HE_VOID				SetGenNum( HE_DWORD genNum ) { mRefGenNum = genNum; }
	HE_PDF_RefInfo		GetRefInfo() { HE_PDF_RefInfo refInfo; refInfo.objNum = mRefObjNum; refInfo.genNum = mRefObjNum; return refInfo; }
	HE_VOID				SetRefInfo( HE_PDF_RefInfo refInfo )  { mRefObjNum = refInfo.objNum; mRefGenNum = refInfo.genNum; }

	CHE_PDF_Object *	GetRefObj( CHE_PDF_ObjectCollector & objCollector );
	CHE_PDF_Object *	GetRefObj( PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector );
	CHE_PDF_File *		GetFile() const { return mpFile; }

	CHE_PDF_Reference * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Reference>( mRefObjNum, mRefGenNum, mpFile, GetAllocator() );
	}

private:
	CHE_PDF_Reference( HE_DWORD refNum, HE_DWORD genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mRefObjNum(refNum), mRefGenNum(genNum), mpFile(pFile) { mType = OBJ_TYPE_REFERENCE; }

	HE_DWORD mRefObjNum;
	HE_DWORD mRefGenNum;
	CHE_PDF_File * mpFile;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:
	static CHE_PDF_Array* Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Array>( pAllocator );
		}else{
			return new CHE_PDF_Array( NULL );
		}
	}

	HE_DWORD			GetCount() const { return mArray.GetCount(); }
	CHE_PDF_Object*		GetElement( HE_DWORD index ) const;
	CHE_PDF_Object*		GetElement( HE_DWORD index, PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector );
	CHE_PDF_Object*		GetElementByType( PDF_OBJ_TYPE Type, CHE_PDF_ObjectCollector & objCollector );

	HE_BOOL				Append( CHE_PDF_Object * pObj );
//  HE_BOOL				Insert( HE_DWORD index, CHE_PDF_Object * pObj );
//  CHE_PDF_Object *	Replace( HE_DWORD index, CHE_PDF_Object * pObj );
//  HE_BOOL				Remove( HE_DWORD index );

	CHE_PDF_Array *		Clone();

private:
	CHE_PDF_Array( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), mArray(pAllocator) { mType = OBJ_TYPE_ARRAY; }

	~CHE_PDF_Array();

	CHE_PtrArray mArray;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:

	static CHE_PDF_Dictionary*	Create( CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Dictionary>( pAllocator );
		}else{
			return new CHE_PDF_Dictionary( NULL );
		}
	}

	HE_DWORD				GetCount() { return m_Map.GetCount(); }
	CHE_PDF_Object*			GetElement( const CHE_ByteString & key ) const;
	CHE_PDF_Object*			GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE type, CHE_PDF_ObjectCollector & objCollector );
	CHE_PDF_Object*			GetElementByIndex( HE_DWORD index ) const { return (CHE_PDF_Object*)( m_Map.GetItemByIndex( index ) ); }
	HE_BOOL					GetKeyByIndex( HE_DWORD index, CHE_ByteString & strRet ) const { return m_Map.GetKeyByIndex( index, strRet ); }

	HE_VOID					SetAtObj( const CHE_ByteString & key, CHE_PDF_Object * pObj );
	HE_VOID					SetAtNull( const CHE_ByteString & key );
	HE_VOID					SetAtBoolean( const CHE_ByteString & key, HE_BOOL value );
	HE_VOID					SetAtInteger( const CHE_ByteString & key, HE_INT32 value );
	HE_VOID					SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value );
	HE_VOID					SetAtString( const CHE_ByteString & key, const CHE_ByteString& string );
	HE_VOID					SetAtName( const CHE_ByteString & key, const CHE_ByteString& name );
	HE_VOID					SetAtArray( const CHE_ByteString & key, CHE_PDF_Array * pArray );
	HE_VOID					SetAtDictionary( const CHE_ByteString & key, CHE_PDF_Dictionary * pDict );
	HE_VOID					SetAtReference( const CHE_ByteString & key, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_File * pFile );
// 	CHE_PDF_Object *		Replace( CHE_ByteString & key, CHE_PDF_Object * pObj );
// 	HE_BOOL					Remove( CHE_ByteString & key );

	CHE_PDF_Dictionary *	Clone();

private:
	CHE_PDF_Dictionary( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_Map(pAllocator) { mType = OBJ_TYPE_DICTIONARY; }

	~CHE_PDF_Dictionary();

	CHE_ByteStringToPtrMap m_Map;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Stream : public CHE_PDF_Object
{
public:
	static CHE_PDF_Stream* Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( objNum, genNum, pEncrypt, pAllocator );
		}else{
			return new CHE_PDF_Stream( objNum, genNum, pEncrypt, pAllocator );
		}
	}

	static CHE_PDF_Stream* Create(	HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary* pDict, 
		HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL ) 
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( pData, size, pDict, objNum, genNum, pEncrypt, pAllocator );
		}else{
			return new CHE_PDF_Stream( pData, size, pDict, objNum, genNum, pEncrypt, NULL );
		}
	}

	static CHE_PDF_Stream* Create(	IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict,
		HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL ) 
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pAllocator );
		}else{
			return new CHE_PDF_Stream( pFile, offset, size, pDict, objNum, genNum, pEncrypt, NULL );
		}
	}

	HE_DWORD				GetObjNum() const { return m_dwObjNum; }
	HE_DWORD				GetGenNum() const { return m_dwGenNum; }
	HE_BOOL					SetDict( CHE_PDF_Dictionary * pDict );
	CHE_PDF_Dictionary*		GetDict() const { return m_pDict; }
	HE_DWORD				GetRawSize() const { return m_dwSize; }
	HE_DWORD				GetRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const;
	HE_BOOL					SetRawData( HE_LPBYTE pData, HE_DWORD dwDataSize, HE_BYTE filter = STREAM_FILTER_NULL );

	CHE_PDF_Stream *		Clone();

private:
	CHE_PDF_Stream(	HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict, HE_DWORD objNum, HE_DWORD genNum, 
					CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum,
					CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream(	HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_Stream();

	CHE_PDF_Encrypt *		m_pEncrypt;

	CHE_PDF_Dictionary*		m_pDict;
	HE_DWORD				m_dwSize;
	HE_BOOL					m_bMem;
	union {
		HE_LPBYTE			m_pDataBuf;
		IHE_Read*			m_pFile;	
	};
	HE_DWORD				m_FileOffset;

	HE_DWORD				m_dwObjNum;
	HE_DWORD				m_dwGenNum;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
	friend class CHE_PDF_StreamAcc;
};

class CHE_PDF_StreamAcc : public CHE_Object
{
public:
	CHE_PDF_StreamAcc( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_StreamAcc();
	
	HE_BOOL					Attach( const CHE_PDF_Stream * stream );
	HE_VOID					Detach();

	const CHE_PDF_Stream *	GetStream() const { return m_pStream; }

	HE_LPBYTE				GetData() const { return m_pData; }
	HE_DWORD				GetSize() const { return m_dwSize; }
	
private:
	HE_LPBYTE				m_pData;
	HE_DWORD				m_dwSize;

	const CHE_PDF_Stream*	m_pStream;
};

#endif
