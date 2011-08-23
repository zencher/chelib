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
	HE_DWORD			GetObjNum() const { return m_dwObjNum; }
	HE_DWORD			GetGenNum() const { return m_dwGenNum; }
	CHE_PDF_Parser *	GetParser() const { return m_pParser; }

	CHE_PDF_Boolean *	ToBoolean() const;
	CHE_PDF_Number *	ToNumber() const;
	CHE_PDF_String *	ToString() const;
	CHE_PDF_Name *		ToName() const;
	CHE_PDF_Reference * ToReference() const;
	CHE_PDF_Array*		ToArray() const;
	CHE_PDF_Dictionary*	ToDict() const;
	CHE_PDF_Stream *	ToStream() const;

	HE_VOID				Release();

protected:
	CHE_PDF_Object( CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_Object( pAllocator )
		{ m_Type = OBJ_TYPE_INVALID; m_dwObjNum = 0; m_dwGenNum = 0; m_pParser = pParser; }

	~CHE_PDF_Object() {}

	PDF_OBJ_TYPE		m_Type;
	HE_DWORD 			m_dwObjNum;
	HE_DWORD			m_dwGenNum;
	CHE_PDF_Parser *	m_pParser;
	
	friend class CHE_Allocator;
};

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:
	static CHE_PDF_Null* Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Null>( objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Null( objNum, genNum, pParser, NULL );
		}
	}

private:
	CHE_PDF_Null( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_Type = OBJ_TYPE_NULL; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:
	static CHE_PDF_Boolean*	Create( HE_BOOL value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Boolean>( value, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Boolean( value, objNum, genNum,  pParser, NULL );
		}
	}
	HE_BOOL	GetValue() { return m_bValue; }
	HE_VOID SetValue( HE_BOOL value ) { m_bValue = value; }

private:
	CHE_PDF_Boolean( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_Type = OBJ_TYPE_BOOLEAN; m_bValue = FALSE; m_dwObjNum = objNum; m_dwGenNum = genNum; }
	
	CHE_PDF_Boolean( HE_BOOL value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_Type = OBJ_TYPE_BOOLEAN; m_bValue = value; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	HE_BOOL	m_bValue;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:
	static CHE_PDF_Number* Create( HE_INT32 value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Number>( value, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Number( value, objNum, genNum, pParser, NULL );
		}
	}

	static CHE_PDF_Number* Create( HE_FLOAT value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Number>( value, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Number( value, objNum, genNum, pParser, NULL );
		}
	}

	HE_BOOL		IsInteger() const { return m_bInteger; }
	HE_INT32	GetInteger() const { return m_bInteger ? m_Integer : (HE_INT32)m_Float; }
	HE_FLOAT	GetFloat() const { return m_bInteger ? (HE_FLOAT)m_Integer : m_Float; }
	HE_VOID		SetValue( HE_INT32 value ) { m_bInteger = TRUE; m_Integer = value; }
	HE_VOID		SetValue( HE_FLOAT value ) { m_bInteger = FALSE; m_Float = value; }

private:
	CHE_PDF_Number( HE_INT32 value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_bInteger = TRUE; m_Integer = value; m_Type = OBJ_TYPE_NUMBER; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	CHE_PDF_Number( HE_FLOAT value, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_bInteger = FALSE; m_Float = value; m_Type = OBJ_TYPE_NUMBER; m_dwObjNum = objNum; m_dwGenNum = genNum; }

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
	static CHE_PDF_String*	Create( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_String>( str, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_String( str, objNum, genNum, pParser, NULL );
		}
	}

	CHE_ByteString	GetString() { return m_String; }
	HE_VOID			SetString( CHE_ByteString & name ) { m_String = name; }

private:
	CHE_PDF_String( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator ), m_String( pAllocator )
		{ m_Type = OBJ_TYPE_STRING; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	CHE_PDF_String( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator ), m_String( str )
		{ m_Type = OBJ_TYPE_STRING; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	CHE_ByteString		m_String;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:
	static CHE_PDF_Name* Create( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Name>( str, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Name( str, objNum, genNum, pParser, NULL );
		}
	}

	CHE_ByteString	GetString() { return m_Name; }
	HE_VOID			SetString( CHE_ByteString & name ) { m_Name = name; };
	
private:
	CHE_PDF_Name( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator ), m_Name( str )
		{ m_Type = OBJ_TYPE_NAME; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	CHE_ByteString	m_Name;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
	static CHE_PDF_Reference* Create( HE_DWORD refNum, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Reference>( refNum, objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Reference( refNum, objNum, genNum, pParser, NULL );	
		}
	}

	HE_DWORD			GetRefNum() const { return m_RefObjNum; }
	HE_VOID				SetRefNum( HE_DWORD objNum ) { m_RefObjNum = objNum; }

	CHE_PDF_Object *	GetRefObj() const;
	CHE_PDF_Object *	GetRefObj( PDF_OBJ_TYPE Type ) const;

private:
	CHE_PDF_Reference( HE_DWORD refNum, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator )
		{ m_Type = OBJ_TYPE_REFERENCE; m_RefObjNum = refNum; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	HE_DWORD m_RefObjNum;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:
	static CHE_PDF_Array* Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Array>( objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Array( objNum, genNum, pParser, NULL );
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

private:
	CHE_PDF_Array( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator ), m_array( pAllocator )
		{ m_Type = OBJ_TYPE_ARRAY; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	~CHE_PDF_Array();

	CHE_PtrArray m_array;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:

	static CHE_PDF_Dictionary*	Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Dictionary>( objNum, genNum, pParser, pAllocator );
		}else{
			return new CHE_PDF_Dictionary( objNum, genNum, pParser, NULL );
		}
	}

	HE_DWORD				GetCount() { return m_Map.GetCount(); }
	CHE_PDF_Object*			GetElement( const CHE_ByteString & key ) const;
	CHE_PDF_Object*			GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE type ) const;
	CHE_PDF_Object*			GetElementByIndex( HE_DWORD index ) const { return (CHE_PDF_Object*)( m_Map.GetItemByIndex( index ) ); }
	HE_BOOL					GetKeyByIndex( HE_DWORD index, CHE_ByteString & strRet ) const { return m_Map.GetKeyByIndex( index, strRet ); }

	HE_VOID					SetAtNull( const CHE_ByteString & key );
	HE_VOID					SetAtBoolean( const CHE_ByteString & key, bool value );
	HE_VOID					SetAtInteger( const CHE_ByteString & key, HE_INT32 value );
	HE_VOID					SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value );
	HE_VOID					SetAtString( const CHE_ByteString & key, const CHE_ByteString& string );
	HE_VOID					SetAtName( const CHE_ByteString & key, const CHE_ByteString& name );
	HE_VOID					SetAtArray( const CHE_ByteString & key, CHE_PDF_Array * pArray );
	HE_VOID					SetAtDictionary( const CHE_ByteString & key, CHE_PDF_Dictionary * pDict );
	HE_VOID					SetAtReference( const CHE_ByteString & key, HE_DWORD objnum );
// 	CHE_PDF_Object *		Replace( CHE_ByteString & key, CHE_PDF_Object * pObj );
// 	HE_BOOL					Remove( CHE_ByteString & key );

private:
	CHE_PDF_Dictionary( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( pParser, pAllocator ), m_Map( pAllocator )
		{ m_Type = OBJ_TYPE_DICTIONARY; m_dwObjNum = objNum; m_dwGenNum = genNum; }

	~CHE_PDF_Dictionary();

	CHE_ByteStringToPtrMap m_Map;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Stream : public CHE_PDF_Object
{
public:
	static CHE_PDF_Stream* Create(	HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL,
		CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL )
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( objNum, genNum, pEncrypt, pParser, pAllocator );
		}else{
			return new CHE_PDF_Stream( objNum, genNum, pEncrypt, pParser, pAllocator );
		}
	}

	static CHE_PDF_Stream* Create(	HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary* pDict, 
		HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL,
		CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL ) 
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( pData, size, pDict, objNum, genNum, pEncrypt, pParser, pAllocator );
		}else{
			return new CHE_PDF_Stream( pData, size, pDict, objNum, genNum, pEncrypt, pParser, NULL );
		}
	}

	static CHE_PDF_Stream* Create(	IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict,
		HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL,
		CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL ) 
	{
		if ( pAllocator )
		{
			return pAllocator->New<CHE_PDF_Stream>( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pParser, pAllocator );
		}else{
			return new CHE_PDF_Stream( pFile, offset, size, pDict, objNum, genNum, pEncrypt, pParser, NULL );
		}
	}

	HE_BOOL					SetDict( CHE_PDF_Dictionary * pDict );
	CHE_PDF_Dictionary*		GetDict() const { return m_pDict; }
	HE_DWORD				GetRawSize() const { return m_dwSize; }
	HE_DWORD				GetRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const;
	HE_BOOL					SetRawData( HE_LPBYTE pData, HE_DWORD dwDataSize, HE_BYTE filter = STREAM_FILTER_NULL );

private:
	CHE_PDF_Stream(	HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict, HE_DWORD objNum, HE_DWORD genNum, 
		CHE_PDF_Encrypt * pEncrypt = NULL, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum,
		CHE_PDF_Encrypt * pEncrypt = NULL, CHE_PDF_Parser * pParser = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream(	HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_PDF_Parser * pParser = NULL, 
					CHE_Allocator * pAllocator = NULL );

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

	HE_LPCBYTE				GetData() const { return m_pData; }
	HE_DWORD				GetSize() const { return m_dwSize; }
	
private:
	HE_LPBYTE				m_pData;
	HE_DWORD				m_dwSize;

	const CHE_PDF_Stream*	m_pStream;
};

#endif
