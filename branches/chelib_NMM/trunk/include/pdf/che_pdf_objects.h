#ifndef _CHE_PDF_OBJECTS_H_
#define _CHE_PDF_OBJECTS_H_

#include "../che_base.h"
#include "../che_string.h"
#include "../che_datastructure.h"
#include "che_pdf_encrypt.h"

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
#define PDFOBJ_INDIRECTOBJ	10

class CHE_PDF_Object;
class CHE_PDF_Null;
class CHE_PDF_Boolean;
class CHE_PDF_Number;
class CHE_PDF_String;
class CHE_PDF_Name;
class CHE_PDF_Dictionary;
class CHE_PDF_Stream;
class CHE_PDF_IndirectObject;

class CHE_PDF_Object : public CHE_Object
{
public:
	HE_BYTE		GetType() const { return m_Type; }
	HE_DWORD	GetObjNum() const { return m_ObjNum; }
	HE_DWORD	GetGenNum() const { return m_GenNum; }

	HE_VOID		Release();

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
	CHE_PDF_Object() { m_Type = 0; m_ObjNum = 0; m_GenNum = 0; }
	~CHE_PDF_Object() {}

	void		Destroy() {};
	
	HE_BYTE 	m_Type;
	HE_DWORD 	m_ObjNum;
	HE_DWORD	m_GenNum;

private:
	CHE_PDF_Object(const CHE_PDF_Object& src) {}
};

class CHE_PDF_Boolean : public CHE_PDF_Object
{
public:
	static CHE_PDF_Boolean*	Create( HE_BOOL value, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Boolean( value, objNum, genNum ); }

	HE_BOOL	GetValue() { return m_bValue; }
	
protected:
	CHE_PDF_Boolean( HE_DWORD objNum, HE_DWORD genNum )  { m_Type = PDFOBJ_BOOLEAN; m_bValue = FALSE; m_ObjNum = objNum; m_GenNum = genNum; }
	
	CHE_PDF_Boolean( HE_BOOL value, HE_DWORD objNum, HE_DWORD genNum )  { m_Type = PDFOBJ_BOOLEAN;  m_bValue = value; m_ObjNum = objNum; m_GenNum = genNum; }

	HE_BOOL	m_bValue;

	friend class CHE_PDF_Object;
};

class CHE_PDF_Number : public CHE_PDF_Object
{
public:

	static CHE_PDF_Number*		Create( HE_INT32 value, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Number(value, objNum, genNum); }

	static CHE_PDF_Number*		Create( HE_FLOAT value, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Number(value, objNum, genNum); }

	HE_BOOL					IsInteger() const { return m_bInteger; }

	HE_INT32				GetInteger() const { return m_bInteger ? m_Integer : (HE_INT32)m_Float; }

	HE_FLOAT				GetFloatNumber() const { return m_bInteger ? (HE_FLOAT)m_Integer : m_Float; }

protected:
	CHE_PDF_Number( HE_INT32 value, HE_DWORD objNum, HE_DWORD genNum ) { m_bInteger = TRUE; m_Integer = value; m_Type = PDFOBJ_NUMBER; m_ObjNum = objNum; m_GenNum = genNum; }
	
	CHE_PDF_Number( HE_FLOAT value, HE_DWORD objNum, HE_DWORD genNum ) { m_bInteger = FALSE; m_Float = value; m_Type = PDFOBJ_NUMBER; m_ObjNum = objNum; m_GenNum = genNum; }


	HE_BOOL					m_bInteger;
	union {
		HE_INT32			m_Integer;
		HE_FLOAT			m_Float;
	};

	friend class CHE_PDF_Object;
};

class CHE_PDF_String : public CHE_PDF_Object
{
public:
	static CHE_PDF_String*		Create( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_String(str, objNum, genNum); }
	
	CHE_ByteString				GetString() { return m_String; }
	
protected:
	CHE_PDF_String( HE_DWORD objNum, HE_DWORD genNum ) { m_Type = PDFOBJ_STRING; m_ObjNum = objNum; m_GenNum = genNum; }
	
	CHE_PDF_String( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum ) : m_String(str) { m_Type = PDFOBJ_STRING; m_ObjNum = objNum; m_GenNum = genNum; }

	CHE_ByteString			m_String;

	friend class CHE_PDF_Object;
};

class CHE_PDF_Name : public CHE_PDF_Object
{
public:
	static CHE_PDF_Name*		Create( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Name(str, objNum, genNum); }
	
	CHE_ByteString				GetString() { return m_Name; }
	
protected:
	CHE_PDF_Name( const CHE_ByteString& str, HE_DWORD objNum, HE_DWORD genNum ) : m_Name(str) { m_Type = PDFOBJ_NAME; m_ObjNum = objNum; m_GenNum = genNum; }

	CHE_ByteString			m_Name;

	friend class CHE_PDF_Object;
};

class CHE_PDF_Array : public CHE_PDF_Object
{
public:

	static CHE_PDF_Array*		Create( HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Array( objNum, genNum ); }

	HE_VOID						Append( CHE_PDF_Object * pObj );

	HE_DWORD					GetCount() const { return m_array.GetCount(); }

	CHE_PDF_Object*				GetElement( HE_DWORD index ) const;

protected:
	CHE_PDF_Array( HE_DWORD objNum, HE_DWORD genNum )  { m_Type = PDFOBJ_ARRAY; m_ObjNum = objNum; m_GenNum = genNum; }

	CHE_PtrArray m_array;

	~CHE_PDF_Array();

	friend class CHE_PDF_Object;
};

class CHE_PDF_Dictionary : public CHE_PDF_Object
{
public:
	static CHE_PDF_Dictionary*	Create( HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Dictionary( objNum, genNum ); }

	CHE_PDF_Object*			GetElement( const CHE_ByteString & key ) const;

	HE_VOID					SetAtNull( const CHE_ByteString & key );

	HE_VOID					SetAtBoolean( const CHE_ByteString & key, bool value );

	HE_VOID					SetAtInteger( const CHE_ByteString & key, HE_INT32 value );
	
	HE_VOID					SetAtFloatNumber( const CHE_ByteString & key, HE_FLOAT value );

	HE_VOID					SetAtString( const CHE_ByteString & key, const CHE_ByteString& string );

	HE_VOID					SetAtName( const CHE_ByteString & key, const CHE_ByteString& name );

	HE_VOID					SetAtArray( const CHE_ByteString & key, CHE_PDF_Array * pArray );

	HE_VOID					SetAtDictionary( const CHE_ByteString & key, CHE_PDF_Dictionary * pDict );

	HE_VOID					SetAtReference( const CHE_ByteString & key, HE_DWORD objnum );

protected:
	CHE_PDF_Dictionary( HE_DWORD objNum, HE_DWORD genNum )  { m_Type = PDFOBJ_DICTIONARY; m_ObjNum = objNum; m_GenNum = genNum; }

	~CHE_PDF_Dictionary();

	CHE_ByteStringToPtrMap m_Map;

	friend class CHE_PDF_Object;
};

class CHE_PDF_Stream : public CHE_PDF_Object
{
public:

	static CHE_PDF_Stream*		Create( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL ) 
									{ return new CHE_PDF_Stream( pData, size, pDict, objNum, genNum, pEncrypt ); }

	static CHE_PDF_Stream*		Create( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL ) 
									{ return new CHE_PDF_Stream( pFile, offset, size, pDict, objNum, genNum, pEncrypt ); }

	CHE_PDF_Dictionary*		GetDict() const { return m_pDict; }

	//HE_VOID					SetData( HE_LPBYTE pData, HE_DWORD size, HE_BOOL bCompressed );

	//HE_VOID					InitStream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary& dict );

	//HE_BOOL					Identical( CHE_PDF_Stream & obj ) const;

	//CPDF_StreamFilter*		GetStreamFilter(FX_BOOL bRaw = FALSE) const;

	HE_DWORD				GetRawSize() const { return m_dwSize; }

	HE_DWORD				ReadRawData( HE_DWORD offset, HE_LPBYTE pBuf, HE_DWORD buf_size ) const;

protected:
	CHE_PDF_Stream( HE_LPBYTE pData, HE_DWORD size, CHE_PDF_Dictionary * pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL );
	
	CHE_PDF_Stream( IHE_Read* pFile, HE_DWORD offset, HE_DWORD size, CHE_PDF_Dictionary* pDict, HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Encrypt * pEncrypt = NULL );

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

	friend class CHE_PDF_Object;
	friend class CHE_PDF_StreamAcc;
};

class CHE_PDF_StreamAcc : public CHE_Object
{
public:
	CHE_PDF_StreamAcc();
	~CHE_PDF_StreamAcc();
	
	HE_BOOL					Attach( const CHE_PDF_Stream * stream /*, HE_BOOL bRawAccess = FALSE, FX_DWORD estimated_size = 0, FX_BOOL bImageAcc = FALSE*/);
	
	HE_VOID					Detach();

	/** Get the stream object attached to. */
	const CHE_PDF_Stream *	GetStream() const { return m_pStream; }
	/** Get the loaded data pointer.*/
	HE_LPCBYTE				GetData() const { return m_pData; }
	/** Get the loaded data size in bytes. */
	HE_DWORD				GetSize() const { return m_dwSize; }
	
	/**
	* Detach the data buffer from this stream accessor.
	* After this call, the caller is now responsible for releasing the data buffer.
	*/
	
	
// 	/** Get the image decoder name. */
// 	const CFX_ByteString&	GetImageDecoder() { return m_ImageDecoder; }
// 	/** Get the image parameters dictionary. */
// 	const CPDF_Dictionary*	GetImageParam() { return m_pImageParam; }
	
protected:
	/** The loaded data. */
	HE_LPBYTE				m_pData;
	/** The size in bytes of the loaded buffer. */
	HE_DWORD				m_dwSize;
	/** Whether the loaded data pointer is newly allocated or just a reference. */
	//FX_BOOL					m_bNewBuf;
	/** The cached image decoder name. */
	//CFX_ByteString			m_ImageDecoder;
	/** The cached image decoder parameters dictionary. */
	//CPDF_Dictionary*		m_pImageParam;
	/** The stream object attached to. */
	const CHE_PDF_Stream*		m_pStream;
	/** The cached source buffer pointer. */
	//FX_LPBYTE				m_pSrcData;
};

class CHE_PDF_Null	: public CHE_PDF_Object
{
public:
	static CHE_PDF_Null*		Create( HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Null( objNum, genNum ); }

protected:
	CHE_PDF_Null( HE_DWORD objNum, HE_DWORD genNum )  { m_Type = PDFOBJ_NULL; m_ObjNum = objNum; m_GenNum = genNum; }
	friend class CHE_PDF_Object;
};

class CHE_PDF_Reference : public CHE_PDF_Object
{
public:
	static CHE_PDF_Reference*	Create( HE_DWORD refNum, HE_DWORD objNum, HE_DWORD genNum ) { return new CHE_PDF_Reference( refNum, objNum, genNum ); }

	HE_DWORD	GetRefNuml() { return m_RefObjNum; }

protected:
	CHE_PDF_Reference( HE_DWORD refNum, HE_DWORD objNum, HE_DWORD genNum ) { m_Type = PDFOBJ_REFERENCE; m_RefObjNum = refNum; m_ObjNum = objNum; m_GenNum = genNum; }

	HE_DWORD m_RefObjNum;
	friend class CHE_PDF_Object;
};

class CHE_PDF_IndirectObject : public CHE_PDF_Object
{
public:
	static CHE_PDF_IndirectObject*	Create( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Object * pObj )
										{ return new CHE_PDF_IndirectObject( objNum, genNum, pObj ); }
										
	CHE_PDF_Dictionary	*	GetDict() const;

	CHE_PDF_Stream *		GetStream() const;

	CHE_PDF_Object *		GetObject() const { return m_pObj; }

protected:
	CHE_PDF_IndirectObject( HE_DWORD objNum, HE_DWORD genNum, CHE_PDF_Object * pObj );

	~CHE_PDF_IndirectObject();

	CHE_PDF_Object *	m_pObj;

	friend class CHE_PDF_Object;
	friend class CHE_PDF_IndirectObjectCollector;
};

class CHE_PDF_IndirectObjectCollector : public CHE_Object
{
public:
	CHE_PDF_IndirectObjectCollector() {}
	~CHE_PDF_IndirectObjectCollector() {}

	HE_BOOL Add( CHE_PDF_IndirectObject * pObj );

	HE_BOOL IsExist( HE_DWORD objNum ) { return m_map.GetItem( objNum ) ? TRUE : FALSE ; }

	CHE_PDF_IndirectObject * GetObj( HE_DWORD objNum ) { return (CHE_PDF_IndirectObject*)m_map.GetItem( objNum ); }

	HE_VOID Clear() { m_map.Clear(); }

	HE_VOID ReleaseObj();

private:
	CHE_NumToPtrMap m_map;
};

#endif
