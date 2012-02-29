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
class CHE_PDF_Parser;

class CHE_PDF_Object : public CHE_Object
{
public:
	PDF_OBJ_TYPE		GetType() const { return m_Type; }

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
	CHE_PDF_Object( CHE_Allocator * pAllocator = NULL ) : CHE_Object(pAllocator), m_Type(OBJ_TYPE_INVALID) {}

	~CHE_PDF_Object() {} //todo 是否要加上Release调用

	PDF_OBJ_TYPE		m_Type;

	friend class CHE_Allocator;
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
	CHE_PDF_Null( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Object(pAllocator) { m_Type = OBJ_TYPE_NULL; }

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
	HE_BOOL	GetValue() { return m_bValue; }
	HE_VOID SetValue( HE_BOOL value ) { m_bValue = value; }

	CHE_PDF_Boolean * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Boolean>( m_bValue, GetAllocator() );
	}

private:
	CHE_PDF_Boolean( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_bValue(FALSE) { m_Type = OBJ_TYPE_BOOLEAN; }
	
	CHE_PDF_Boolean( HE_BOOL value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_bValue(value) { m_Type = OBJ_TYPE_BOOLEAN; }

	HE_BOOL	m_bValue;

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

	HE_BOOL		IsInteger() const { return m_bInteger; }
	HE_INT32	GetInteger() const { return m_bInteger ? m_Integer : (HE_INT32)m_Float; }
	HE_FLOAT	GetFloat() const { return m_bInteger ? (HE_FLOAT)m_Integer : m_Float; }
	HE_VOID		SetValue( HE_INT32 value ) { m_bInteger = TRUE; m_Integer = value; }
	HE_VOID		SetValue( HE_FLOAT value ) { m_bInteger = FALSE; m_Float = value; }

	CHE_PDF_Number * Clone()
	{
		if ( m_bInteger )
		{
			return GetAllocator()->New<CHE_PDF_Number>( m_Integer, GetAllocator() );
		}else
		{
			return  GetAllocator()->New<CHE_PDF_Number>( m_Float, GetAllocator() );
		}
	}

private:
	CHE_PDF_Number( HE_INT32 value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_bInteger(TRUE), m_Integer(value) { m_Type = OBJ_TYPE_NUMBER; }

	CHE_PDF_Number( HE_FLOAT value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_bInteger(FALSE), m_Float(value) { m_Type = OBJ_TYPE_NUMBER; }

	HE_BOOL			m_bInteger;
	union {
		HE_INT32	m_Integer;
		HE_FLOAT	m_Float;
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

	CHE_ByteString	GetString() { return m_String; }
	HE_VOID			SetString( CHE_ByteString & name ) { m_String = name; }

	CHE_PDF_String * Clone()
	{
		return GetAllocator()->New<CHE_PDF_String>( m_String, GetAllocator() );
	}

private:
	CHE_PDF_String( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_String(pAllocator) { m_Type = OBJ_TYPE_STRING; }

	CHE_PDF_String( const CHE_ByteString& str, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_String(str) { m_Type = OBJ_TYPE_STRING; }

	CHE_ByteString		m_String;

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

	CHE_ByteString	GetString() { return m_Name; }
	HE_VOID			SetString( CHE_ByteString & name ) { m_Name = name; };

	CHE_PDF_Name * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Name>( m_Name, GetAllocator() );
	}
	
private:
	CHE_PDF_Name( const CHE_ByteString& str,  CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_Name(str) { m_Type = OBJ_TYPE_NAME; }

	CHE_ByteString	m_Name;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

struct HE_PDF_InObjectNumbers
{
	HE_DWORD objNum;
	HE_DWORD genNum;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
	static CHE_PDF_Reference* Create( HE_DWORD refNum, HE_DWORD genNum = 0, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Reference>( refNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Reference( refNum, genNum, pParser, NULL );	
		}
	}

	HE_DWORD			GetRefNum() const { return m_RefObjNum; }
	HE_VOID				SetRefNum( HE_DWORD objNum ) { m_RefObjNum = objNum; }
	HE_DWORD			GetGenNum() const { return m_RefGenNum; }
	HE_VOID				SetGenNum( HE_DWORD genNum ) { m_RefGenNum = genNum; }

	CHE_PDF_Object *	GetRefObj() const;
	CHE_PDF_Object *	GetRefObj( PDF_OBJ_TYPE Type ) const;
	CHE_PDF_Parser *	GetParser() const { return mpParser; }

	CHE_PDF_Reference * Clone()
	{
		return GetAllocator()->New<CHE_PDF_Reference>( m_RefObjNum, m_RefGenNum, mpParser, GetAllocator() );
	}

private:
	CHE_PDF_Reference( HE_DWORD refNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_RefObjNum(refNum), m_RefGenNum(genNum), mpParser(pParser) { m_Type = OBJ_TYPE_REFERENCE; }

	HE_DWORD m_RefObjNum;
	HE_DWORD m_RefGenNum;
	CHE_PDF_Parser * mpParser;

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

	HE_DWORD			GetCount() const { return m_array.GetCount(); }
	CHE_PDF_Object*		GetElement( HE_DWORD index ) const;
	CHE_PDF_Object*		GetElement( HE_DWORD index, PDF_OBJ_TYPE Type ) const;
	CHE_PDF_Object*		GetElementByType( PDF_OBJ_TYPE Type ) const;

	HE_BOOL				Append( CHE_PDF_Object * pObj );
//  HE_BOOL				Insert( HE_DWORD index, CHE_PDF_Object * pObj );
//  CHE_PDF_Object *	Replace( HE_DWORD index, CHE_PDF_Object * pObj );
//  HE_BOOL				Remove( HE_DWORD index );

	CHE_PDF_Array *		Clone();

private:
	CHE_PDF_Array( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_array(pAllocator) { m_Type = OBJ_TYPE_ARRAY; }

	~CHE_PDF_Array();

	CHE_PtrArray m_array;

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
	CHE_PDF_Object*			GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE type ) const;
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
	HE_VOID					SetAtReference( const CHE_ByteString & key, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser );
// 	CHE_PDF_Object *		Replace( CHE_ByteString & key, CHE_PDF_Object * pObj );
// 	HE_BOOL					Remove( CHE_ByteString & key );

	CHE_PDF_Dictionary *	Clone();

private:
	CHE_PDF_Dictionary( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object(pAllocator), m_Map(pAllocator) { m_Type = OBJ_TYPE_DICTIONARY; }

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

class CHE_PDF_IndirectObject : public CHE_Object
{
public:
	CHE_PDF_IndirectObject( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Object * pObj,
		CHE_PDF_Parser * pParser, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator ), m_dwObjNum(objNum), m_pObj(pObj), m_dwGenNum(genNum), m_pParser(pParser) {}

	HE_DWORD			GetObjNum() const { return m_dwObjNum; }
	HE_DWORD			GetGenNum() const { return m_dwGenNum; }
	CHE_PDF_Object *	GetObj() const { return m_pObj; }
	CHE_PDF_Parser *	GetParser() const { return m_pParser; }

	CHE_PDF_IndirectObject * Clone();

	HE_VOID				Release()
	{
		if ( m_pObj )
		{
			m_pObj->Release();
		}
		GetAllocator()->Delete( this );
	}

private:
	HE_DWORD			m_dwObjNum;
	HE_DWORD			m_dwGenNum;
	CHE_PDF_Object *	m_pObj;
	CHE_PDF_Parser *	m_pParser;
};

#endif
