#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"
#include "../che_string.h"
#include "../che_datastructure.h"
#include "../che_graphics.h"
#include "che_pdf_encrypt.h"

#include <vector>


enum PDF_OBJ_TYPE
{
	OBJ_TYPE_INVALID		= 0x00,
	OBJ_TYPE_NULL			= 0x01,
	OBJ_TYPE_BOOLEAN		= 0x02,
	OBJ_TYPE_NUMBER			= 0x03,
	OBJ_TYPE_STRING			= 0x04,
	OBJ_TYPE_NAME			= 0x05,
	OBJ_TYPE_ARRAY			= 0x06,
	OBJ_TYPE_DICTIONARY		= 0x07,
	OBJ_TYPE_STREAM			= 0x08,
	OBJ_TYPE_REFERENCE		= 0x09
};


enum PDF_STREAM_FILTER
{
	STREAM_FILTER_NULL		= 0x00,
	STREAM_FILTER_HEX		= 0x01,
	STREAM_FILTER_ASCII85	= 0x02,
	STREAM_FILTER_FLATE		= 0x03,
	STREAM_FILTER_LZW		= 0x04,
	STREAM_FILTER_RLE		= 0x05
};


class CHE_PDF_Object;
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

class CHE_PDF_ObjectPtr;
class CHE_PDF_NullPtr;
class CHE_PDF_BooleanPtr;
class CHE_PDF_NumberPtr;
class CHE_PDF_StringPtr;
class CHE_PDF_NamePtr;
class CHE_PDF_ReferencePtr;
class CHE_PDF_ArrayPtr;
class CHE_PDF_DictionaryPtr;
class CHE_PDF_StreamPtr;


HE_BOOL IsPdfNull( CHE_PDF_Object * pObj );

HE_BOOL IsPdfBoolean( CHE_PDF_Object * pObj );

HE_BOOL IsPdfNumber( CHE_PDF_Object * pObj );

HE_BOOL IsPdfName( CHE_PDF_Object * pObj );

HE_BOOL IsPdfString( CHE_PDF_Object * pObj );

HE_BOOL IsPdfDict( CHE_PDF_Object * pObj );

HE_BOOL IsPdfArray( CHE_PDF_Object * pObj );

HE_BOOL IsPdfRef( CHE_PDF_Object * pObj );

HE_BOOL IsPdfStream( CHE_PDF_Object * pObj );

HE_BOOL IsPdfNullPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfBooleanPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfNumberPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfNamePtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfStringPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfDictPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfArrayPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfRefPtr( const CHE_PDF_ObjectPtr & objPtr );

HE_BOOL IsPdfStreamPtr( const CHE_PDF_ObjectPtr & objPtr );


class CHE_PDF_Object : public CHE_Object
{
public:

	PDF_OBJ_TYPE            GetType() const;

	CHE_PDF_ObjectPtr		Clone();

	HE_VOID					Release();

	HE_VOID					SetModified( HE_BOOL );

	virtual	HE_BOOL			IsModified();

    CHE_PDF_NullPtr         GetNullPtr() const;

    CHE_PDF_BooleanPtr      GetBooleanPtr() const;

    CHE_PDF_NumberPtr       GetNumberPtr() const;

    CHE_PDF_StringPtr       GetStringPtr() const;

    CHE_PDF_NamePtr			GetNamePtr() const;

    CHE_PDF_ArrayPtr        GetArrayPtr() const;

    CHE_PDF_DictionaryPtr	GetDictPtr() const;

    CHE_PDF_ReferencePtr    GetRefPtr() const;

    CHE_PDF_StreamPtr		GetStreamPtr() const;				

protected:

	CHE_PDF_Object( PDF_OBJ_TYPE type, CHE_Allocator * pAllocator = NULL );

	HE_BOOL							mModified;

	PDF_OBJ_TYPE					mType;

	CHE_RefCount					mRefs;

	friend class CHE_Allocator;

	friend class CHE_PDF_ObjectPtr;
};

class CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_ObjectPtr() : mpObj( NULL ) {}

	CHE_PDF_ObjectPtr( const CHE_PDF_ObjectPtr & ptr );

	~CHE_PDF_ObjectPtr();

	CHE_PDF_ObjectPtr operator = ( const CHE_PDF_ObjectPtr & ptr );

	bool operator!() const { return mpObj ? FALSE : TRUE ; }

	operator HE_BOOL() const { return mpObj ? TRUE : FALSE ; }

	CHE_PDF_Object * operator->() const { return mpObj; }

	HE_VOID	reset( CHE_PDF_Object * pObj = NULL );

protected:

	CHE_PDF_Object * mpObj;
};

class CHE_PDF_NullPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Null * operator->() const { return IsPdfNull( mpObj ) ? (CHE_PDF_Null*)( mpObj )  : NULL ; }
};

class CHE_PDF_BooleanPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Boolean * operator->() const { return IsPdfBoolean( mpObj ) ? (CHE_PDF_Boolean*)( mpObj ) : NULL ; }
};

class CHE_PDF_NumberPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Number * operator->() const { return IsPdfNumber( mpObj ) ? (CHE_PDF_Number*)( mpObj ) : NULL ; }
};

class CHE_PDF_StringPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_String * operator->() const { return IsPdfString( mpObj ) ? (CHE_PDF_String*)( mpObj ) : NULL ; }
};

class CHE_PDF_NamePtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Name * operator->() const { return IsPdfName( mpObj ) ? (CHE_PDF_Name*)( mpObj ) : NULL ; }
};

class CHE_PDF_ReferencePtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Reference * operator->() const { return IsPdfRef( mpObj ) ? (CHE_PDF_Reference*)( mpObj ) : NULL ; }
};

class CHE_PDF_ArrayPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Array * operator->() const { return IsPdfArray( mpObj ) ? (CHE_PDF_Array*)( mpObj ) : NULL ; }
};

class CHE_PDF_DictionaryPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Dictionary * operator->() const { return IsPdfDict( mpObj ) ? (CHE_PDF_Dictionary*)( mpObj ) : NULL ; }
};

class CHE_PDF_StreamPtr : public CHE_PDF_ObjectPtr
{
public:
	CHE_PDF_Stream * operator->() const { return IsPdfStream( mpObj ) ? (CHE_PDF_Stream*)( mpObj ) : NULL ; }
};

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:

	static CHE_PDF_NullPtr			Create( CHE_Allocator * pAllocator = NULL );

	CHE_PDF_NullPtr					Clone();

private:

	CHE_PDF_Null( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Object( OBJ_TYPE_NULL, pAllocator ) {}

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:

	static CHE_PDF_BooleanPtr		Create( HE_BOOL value, CHE_Allocator * pAllocator = NULL );

	HE_BOOL							GetValue() { return mbValue; }

	HE_VOID							SetValue( HE_BOOL value ) { mbValue = value; SetModified( TRUE ); }

	CHE_PDF_BooleanPtr				Clone();

private:

	CHE_PDF_Boolean( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( FALSE ) {}
	
	CHE_PDF_Boolean( HE_BOOL value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_BOOLEAN, pAllocator ), mbValue( value ) {}

	HE_BOOL							mbValue;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:

	static CHE_PDF_NumberPtr		Create( HE_INT32 value,  CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_NumberPtr		Create( HE_FLOAT value, CHE_Allocator * pAllocator = NULL );

	HE_BOOL							IsInteger() const { return mbInteger; }

	HE_INT32						GetInteger() const { return mbInteger ? mInteger : (HE_INT32)mFloat; }

	HE_FLOAT						GetFloat() const { return mbInteger ? (HE_FLOAT)mInteger : mFloat; }

	HE_VOID							SetValue( HE_INT32 value ) { mbInteger = TRUE; mInteger = value; SetModified( TRUE ); }

	HE_VOID							SetValue( HE_FLOAT value ) { mbInteger = FALSE; mFloat = value; SetModified( TRUE ); }

	CHE_PDF_NumberPtr				Clone();

private:

	CHE_PDF_Number( HE_INT32 value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( TRUE ), mInteger( value ) {}

	CHE_PDF_Number( HE_FLOAT value, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_NUMBER, pAllocator ), mbInteger( FALSE ), mFloat( value ) {}

	HE_BOOL							mbInteger;
	union {
		HE_INT32					mInteger;
		HE_FLOAT					mFloat;
	};

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_String : public CHE_PDF_Object
{
public:

	static CHE_PDF_StringPtr		Create( const CHE_ByteString & str, CHE_Allocator * pAllocator = NULL );

	CHE_ByteString &				GetString();

	HE_VOID							SetString( CHE_ByteString & name );

	CHE_PDF_StringPtr				Clone();

private:

	CHE_PDF_String( CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( pAllocator ) {};

	CHE_PDF_String( const CHE_ByteString & str, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_STRING, pAllocator ), mString( str ) {}

	CHE_ByteString					mString;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:

	static CHE_PDF_NamePtr			Create( const CHE_ByteString & str, CHE_Allocator * pAllocator = NULL );

	CHE_ByteString					GetString() { return mName; }

	HE_VOID							SetString( CHE_ByteString & name ) { mName = name; SetModified( TRUE ); };

	CHE_PDF_NamePtr					Clone();
	
private:

	CHE_PDF_Name( const CHE_ByteString& str,  CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_NAME, pAllocator ), mName(str) {}

	CHE_ByteString					mName;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

struct PDF_RefInfo
{
	HE_ULONG objNum;
	HE_ULONG genNum;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:

	static CHE_PDF_ReferencePtr		Create( HE_ULONG refNum, HE_ULONG genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );

	HE_ULONG						GetRefNum() const { return mRefObjNum; }

	HE_VOID							SetRefNum( HE_ULONG objNum ) { mRefObjNum = objNum; SetModified( TRUE ); }

	HE_ULONG						GetGenNum() const { return mRefGenNum; }

	HE_VOID							SetGenNum( HE_ULONG genNum ) { mRefGenNum = genNum; SetModified( TRUE ); }

	PDF_RefInfo						GetRefInfo() { PDF_RefInfo refInfo; refInfo.objNum = mRefObjNum; refInfo.genNum = mRefGenNum; return refInfo; }

	HE_VOID							SetRefInfo( PDF_RefInfo refInfo )  { mRefObjNum = refInfo.objNum; mRefGenNum = refInfo.genNum; SetModified( TRUE ); }

	CHE_PDF_ObjectPtr				GetRefObj();

	CHE_PDF_ObjectPtr				GetRefObj( PDF_OBJ_TYPE Type );

	CHE_PDF_File *					GetFile() const { return mpFile; }

	CHE_PDF_ReferencePtr			Clone();

private:

	CHE_PDF_Reference( HE_ULONG refNum, HE_ULONG genNum, CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL )
		: CHE_PDF_Object( OBJ_TYPE_REFERENCE, pAllocator ), mRefObjNum( refNum ), mRefGenNum( genNum ), mpFile( pFile ) {}

	HE_ULONG						mRefObjNum;

	HE_ULONG						mRefGenNum;

	CHE_PDF_File *					mpFile;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:

	static CHE_PDF_ArrayPtr			Create( CHE_Allocator * pAllocator = NULL );

	HE_BOOL							Append( const CHE_PDF_ObjectPtr & pObj );

	HE_ULONG						GetCount() const { return mArray.size(); }

	CHE_PDF_ObjectPtr				GetElement( HE_ULONG index ) const;

	CHE_PDF_ObjectPtr				GetElement( HE_ULONG index, PDF_OBJ_TYPE Type ) const;

	CHE_PDF_ObjectPtr				GetElementByType( PDF_OBJ_TYPE Type );

	CHE_PDF_ArrayPtr				Clone();

	HE_BOOL							IsModified();

	HE_BOOL							GetRect( CHE_Rect & rect ) const;

	HE_BOOL							GetMatrix( CHE_Matrix & maxtrix ) const;

private:

	CHE_PDF_Array( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Object( OBJ_TYPE_ARRAY, pAllocator ) {}

	std::vector<CHE_PDF_ObjectPtr>	mArray;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:

	static CHE_PDF_DictionaryPtr	Create( CHE_Allocator * pAllocator = NULL );

	HE_ULONG						GetCount() { return mKeys.size(); }

	CHE_PDF_ObjectPtr				GetElement( const CHE_ByteString & key ) const;

	CHE_PDF_ObjectPtr				GetElement( const CHE_ByteString & key, PDF_OBJ_TYPE type );

	CHE_PDF_ObjectPtr				GetElementByIndex( HE_ULONG index ) const { return mPtrs[index]; }

	HE_BOOL							GetKeyByIndex( HE_ULONG index, CHE_ByteString & strRet ) const { strRet = mKeys[index]; return TRUE; }

	HE_VOID							SetAtObj( const CHE_ByteString & key, const CHE_PDF_ObjectPtr & ptr );

	HE_VOID							SetAtNull( const CHE_ByteString & key );

	HE_VOID							SetAtBoolean( const CHE_ByteString & key, HE_BOOL value );

	HE_VOID							SetAtInteger( const CHE_ByteString & key, HE_INT32 value );

	HE_VOID							SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value );

	HE_VOID							SetAtString( const CHE_ByteString & key, const CHE_ByteString & string );

	HE_VOID							SetAtName( const CHE_ByteString & key, const CHE_ByteString & name );

	HE_VOID							SetAtArray( const CHE_ByteString & key, const CHE_PDF_ArrayPtr & pArray );

	HE_VOID							SetAtDictionary( const CHE_ByteString & key, const CHE_PDF_DictionaryPtr & pDict );

	HE_VOID							SetAtReference( const CHE_ByteString & key, HE_ULONG objNum, HE_ULONG genNum, CHE_PDF_File * pFile );

	CHE_PDF_DictionaryPtr			Clone();

	HE_BOOL							IsModified();

private:
	CHE_PDF_Dictionary( CHE_Allocator * pAllocator = NULL ) : CHE_PDF_Object( OBJ_TYPE_DICTIONARY, pAllocator ) {}

	std::vector<CHE_PDF_ObjectPtr>	mPtrs;

	std::vector<CHE_ByteString>		mKeys;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
};

class CHE_PDF_Stream : public CHE_PDF_Object
{
public:
	static CHE_PDF_StreamPtr		Create( HE_ULONG objNum, HE_ULONG genNum,
											CHE_PDF_Encrypt * pEncrypt = NULL,
											CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_StreamPtr		Create( HE_LPBYTE pData, HE_ULONG size,
											const CHE_PDF_DictionaryPtr & pDict, 
											HE_ULONG objNum, HE_ULONG genNum,
											CHE_PDF_Encrypt * pEncrypt = NULL,
											CHE_Allocator * pAllocator = NULL );

	static CHE_PDF_StreamPtr		Create( IHE_Read* pFile, HE_ULONG offset, HE_ULONG size, 
											const CHE_PDF_DictionaryPtr pDict,
											HE_ULONG objNum, HE_ULONG genNum, 
											CHE_PDF_Encrypt * pEncrypt = NULL,
											CHE_Allocator * pAllocator = NULL );

	HE_ULONG						GetObjNum() const { return m_dwObjNum; }

	HE_ULONG						GetGenNum() const { return m_dwGenNum; }

	HE_VOID							SetDict( const CHE_PDF_DictionaryPtr & ptr );

	CHE_PDF_DictionaryPtr			GetDictPtr() const { return mDictPtr; }

	HE_ULONG						GetRawSize() const { return m_dwSize; }

	HE_ULONG						GetRawData( HE_ULONG offset, HE_LPBYTE pBuf, HE_ULONG buf_size ) const;

	HE_BOOL							SetRawData( HE_LPBYTE pData, HE_ULONG dwDataSize, HE_BYTE filter = STREAM_FILTER_NULL );

	CHE_PDF_StreamPtr				Clone();

	HE_BOOL							IsModified();

private:
	CHE_PDF_Stream(	HE_LPBYTE pData, HE_ULONG size, const CHE_PDF_DictionaryPtr & pDict, HE_ULONG objNum, HE_ULONG genNum, 
					CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream( IHE_Read* pFile, HE_ULONG offset, HE_ULONG size, const CHE_PDF_DictionaryPtr & pDict, HE_ULONG objNum, HE_ULONG genNum,
					CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	CHE_PDF_Stream(	HE_ULONG objNum, HE_ULONG genNum, CHE_PDF_Encrypt * pEncrypt = NULL, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_Stream();

	CHE_PDF_Encrypt *				m_pEncrypt;

	CHE_PDF_DictionaryPtr			mDictPtr;
	HE_ULONG						m_dwSize;
	HE_BOOL							m_bMem;
	union {
		HE_LPBYTE					m_pDataBuf;
		IHE_Read*					m_pFile;	
	};
	HE_ULONG						m_FileOffset;

	HE_ULONG						m_dwObjNum;
	HE_ULONG						m_dwGenNum;

	friend class CHE_Allocator;
	friend class CHE_PDF_Object;
	friend class CHE_PDF_StreamAcc;
};

enum PDF_STREAM_DECODE_MODE
{
	STREAM_DECODE_NORMAL,
	STREAM_DECODE_NOTLASTFILTER
};

class CHE_PDF_StreamAcc : public CHE_Object
{
public:
	CHE_PDF_StreamAcc( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_StreamAcc();
	
	HE_BOOL							Attach( const CHE_PDF_StreamPtr & streamPtr, PDF_STREAM_DECODE_MODE mode = STREAM_DECODE_NORMAL );

	HE_VOID							Detach();

	CHE_PDF_StreamPtr				GetStream() const { return mStreamPtr; }

	HE_LPBYTE						GetData() const { return m_pDataBuf; }

	HE_ULONG						GetSize() const { return m_dwSize; }
	
private:
	HE_LPBYTE						m_pDataBuf;
	HE_ULONG						m_dwSize;;

	CHE_PDF_StreamPtr				mStreamPtr;
};

#endif