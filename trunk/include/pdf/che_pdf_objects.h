#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"
#include "../che_string.h"

#define PDFOBJ_INVALID		0
#define	PDFOBJ_BOOLEAN		1
#define PDFOBJ_NUMBER		2
#define PDFOBJ_STRING		3
#define PDFOBJ_NAME			4
#define PDFOBJ_ARRAY		5
#define PDFOBJ_DICTIONARY	6
#define PDFOBJ_STREAM		7
#define PDFOBJ_NULL			8
#define PDFOBJ_REFERENCE	9


class CHE_PDF_Object;
class CHE_PDF_Boolean;
class CHE_PDF_Number;
class CHE_PDF_String;
class CHE_PDF_Name;
class CHE_PDF_Dictionary;
class CHE_PDF_Stream;
class CHE_PDF_IndirectObjects;

class CHE_PDF_Object : public CHE_Object
{

public:
	HE_BYTE		GetType() const { return m_Type; }

	HE_DWORD	GetObjNum() const { return m_ObjNum; }

	//HE_BOOL		IsIdentical( CHE_PDF_Object& pObj ) const;

	//CHE_PDF_Object*		Clone(/*FX_BOOL bDirect = FALSE*/) const;

	//CHE_PDF_Object*		CloneRef( CPDF_IndirectObjects* pObjs ) const;

	//CHE_PDF_Object*		GetDirect() const;

	//HE_VOID				Release();

	//CHE_ByteString		GetString() const;

	//HE_FLOAT			GetFloatNumber() const;

	//HE_DWORD			GetInteger() const;

	//CHE_PDF_Dictionary*	GetDict() const;

	//CHE_PDF_Array*		GetArray() const;

protected:

	CHE_PDF_Object() { m_ObjNum = 0; m_Type = 0; }

	HE_BYTE 				m_Type;

	HE_DWORD 				m_ObjNum;

	void					Destroy() {};
	
	~CHE_PDF_Object() {}

private:
	CHE_PDF_Object(const CHE_PDF_Object& src) {}
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:

	static CHE_PDF_Boolean*	Create( HE_BOOL value ) { return new CHE_PDF_Boolean( value ); }

	CHE_PDF_Boolean()  { m_Type = PDFOBJ_BOOLEAN; }

	CHE_PDF_Boolean( HE_BOOL value )  { m_Type = PDFOBJ_BOOLEAN; m_bValue = value; }
	
protected:
	HE_BOOL	m_bValue;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:

	static CHE_PDF_Number*		Create( HE_INT32 value ) { return new CHE_PDF_Number(value); }

	static CHE_PDF_Number*		Create( HE_FLOAT value ) { return new CHE_PDF_Number(value); }

	CHE_PDF_Number()  { m_Type = PDFOBJ_NUMBER; }

	CHE_PDF_Number( HE_INT32 value ) { m_bInteger = TRUE; m_Integer = value; }

	CHE_PDF_Number( HE_FLOAT value ) { m_bInteger = FALSE; m_Float = value; }

	HE_BOOL					IsInteger() const { return m_bInteger; }

	HE_INT32				GetInteger() const { return m_bInteger ? m_Integer : (HE_INT32)m_Float; }

	HE_FLOAT				GetFloatNumber() const { return m_bInteger ? (HE_FLOAT)m_Integer : m_Float; }

protected:

	HE_BOOL					m_bInteger;
	union {
		HE_INT32			m_Integer;
		HE_FLOAT			m_Float;
	};
};

class CHE_PDF_String : public CHE_PDF_Object
{
public:
	static CHE_PDF_String*		Create( const CHE_ByteString& str) { return new CHE_PDF_String(str); }
	
	CHE_PDF_String() { m_Type = PDFOBJ_STRING; }

	CHE_PDF_String( const CHE_ByteString& str ) : m_String(str) { m_Type = PDFOBJ_STRING; }
	
protected:
	CHE_ByteString			m_String;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:
	static CHE_PDF_Name*		Create( const CHE_ByteString& str ) { return new CHE_PDF_Name(str); }

	static CHE_PDF_Name*		Create( HE_LPCSTR str ) { return new CHE_PDF_Name(str); }

	static CHE_PDF_Name*		Create( HE_LPSTR str) { return new CHE_PDF_Name(str); }
	
	CHE_PDF_Name( const CHE_ByteString& str ) : m_Name(str) { m_Type = PDFOBJ_NAME; }

	CHE_PDF_Name( HE_LPCSTR str ) : m_Name(str) { m_Type = PDFOBJ_NAME; }

	CHE_PDF_Name( HE_LPSTR str ) : m_Name(str) { m_Type = PDFOBJ_NAME; }
	
	CHE_ByteString				GetString() { return m_Name; }
	
protected:

	CHE_ByteString			m_Name;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:

	static CHE_PDF_Array*		Create() { return new CHE_PDF_Array(); }

	CHE_PDF_Array()  { m_Type = PDFOBJ_ARRAY; }

	HE_VOID						Append( CHE_PDF_Object * pObj );

	HE_DWORD					GetCount() const { return m_array.GetCount(); }

	CHE_PDF_Object*				GetElement( HE_DWORD index ) const;

protected:
	CHE_PtrArray m_array;

	~CHE_PDF_Array();
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:
	static CHE_PDF_Dictionary*	Create() { return new CHE_PDF_Dictionary(); }

	CHE_PDF_Dictionary()  { m_Type = PDFOBJ_DICTIONARY; }

	CHE_PDF_Object*			GetElement( CHE_ByteString & key ) const;

	HE_VOID					SetAtBoolean( CHE_ByteString & key, bool value );

	HE_VOID					SetAtInteger( CHE_ByteString & key, HE_INT32 value );
	
	HE_VOID					SetAtFloatNumber( CHE_ByteString & key, HE_FLOAT value );

	HE_VOID					SetAtString( CHE_ByteString & key, const CHE_ByteString& string );

	HE_VOID					SetAtName( CHE_ByteString & key, const CHE_ByteString& name );

	HE_VOID					SetAtArray( CHE_ByteString & key, CHE_PDF_Array * pArray );

	HE_VOID					SetAtDictionary( CHE_ByteString & key, CHE_PDF_Dictionary * pDict );

	HE_VOID					SetAtReference( CHE_ByteString & key, HE_DWORD objnum );

protected:
	~CHE_PDF_Dictionary();

	CHE_ByteStringToPtrMap m_Map;
};

// class CHE_PDF_Stream : public CHE_PDF_Object
// {
// public:
// 
// 	static CHE_PDF_Stream*		Create( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary& dict ) 
// 		{ return new CHE_PDF_Stream( pData, size, dict ); }
// 
// 	CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary& dict );
// 
// 	CHE_PDF_Stream( IHE_FileRead* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary& dict );
// 
// 	CHE_PDF_Dictionary*		GetDict() const { return m_pDict; }
// 
// 	HE_VOID					SetData( HE_LPBYTE pData, HE_DWORD size, HE_BOOL bCompressed );
// 
// 	HE_VOID					InitStream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary& dict );
// 
// 	HE_BOOL					Identical( CHE_PDF_Stream & obj ) const;
// 
// 	//CPDF_StreamFilter*		GetStreamFilter(FX_BOOL bRaw = FALSE) const;
// 
// 	HE_DWORD				GetRawSize() const { return m_dwSize; }
// 
// 	HE_BOOL					ReadRawData( HE_DWORD start_pos, HE_LPBYTE pBuf, HE_DWORD buf_size ) const;
// 
// protected:
// 
// 	~CHE_PDF_Stream();
// 
// 	CHE_PDF_Dictionary*		m_pDict;
// 	HE_DWORD				m_dwSize;
// 	HE_DWORD				m_GenNum;
// 	union {
// 		HE_LPBYTE			m_pDataBuf;
// 		IHE_FileRead*		m_pFile;	
// 	};
// 	HE_DWORD				m_FileOffset;
// };

// class CHE_PDF_StreamAcc : public CHE_PDF_Object
// {
// public:
// 	CHE_PDF_StreamAcc();
// 	~CHE_PDF_StreamAcc();
// 	
// 	HE_VOID					LoadAllData( const CHE_PDF_Stream& stream, FX_BOOL bRawAccess = FALSE, FX_DWORD estimated_size = 0, FX_BOOL bImageAcc = FALSE);
// 	
// 	/** Get the stream object attached to. */
// 	const CPDF_Stream*		GetStream() const { return m_pStream; }
// 	/** Get the dictionary of the stream object attached to. */
// 	CPDF_Dictionary*		GetDict() const { return m_pStream->GetDict(); }
// 	/** Get the loaded data pointer.*/
// 	FX_LPCBYTE				GetData() const { return m_pData; }
// 	/** Get the loaded data size in bytes. */
// 	FX_DWORD				GetSize() const { return m_dwSize; }
// 	
// 	/**
// 	* Detach the data buffer from this stream accessor.
// 	* After this call, the caller is now responsible for releasing the data buffer.
// 	*/
// 	FX_LPBYTE				DetachData();
// 	
// 	/** Get the image decoder name. */
// 	const CFX_ByteString&	GetImageDecoder() { return m_ImageDecoder; }
// 	/** Get the image parameters dictionary. */
// 	const CPDF_Dictionary*	GetImageParam() { return m_pImageParam; }
// 	
// protected:
// 	/** The loaded data. */
// 	FX_LPBYTE				m_pData;
// 	/** The size in bytes of the loaded buffer. */
// 	FX_DWORD				m_dwSize;
// 	/** Whether the loaded data pointer is newly allocated or just a reference. */
// 	FX_BOOL					m_bNewBuf;
// 	/** The cached image decoder name. */
// 	CFX_ByteString			m_ImageDecoder;
// 	/** The cached image decoder parameters dictionary. */
// 	CPDF_Dictionary*		m_pImageParam;
// 	/** The stream object attached to. */
// 	const CPDF_Stream*		m_pStream;
// 	/** The cached source buffer pointer. */
// 	FX_LPBYTE				m_pSrcData;
// };

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:

	static CHE_PDF_Null*		Create() { return new CHE_PDF_Null(); }
	
	CHE_PDF_Null()  { m_Type = PDFOBJ_NULL; }
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
	static CHE_PDF_Reference*	Create( int objnum ) { return new CHE_PDF_Reference( objnum ); }

	CHE_PDF_Reference( int objnum ) { m_Type = PDFOBJ_REFERENCE; m_RefObjNum = objnum; }

protected:

	HE_DWORD m_RefObjNum;
};

#endif