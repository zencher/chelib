#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_objects.h"
#include "che_pdf_xref.h"
#include "che_pdf_encrypt.h"
#include "che_pdf_collector.h"
#include <vector>

enum PDF_VERSION 
{
	PDF_VERSION_UNKNOWN	= 0x00,
	PDF_VERSION_1_0		= 0x10,
	PDF_VERSION_1_1		= 0x11,
	PDF_VERSION_1_2		= 0x12,
	PDF_VERSION_1_3		= 0x13,
	PDF_VERSION_1_4		= 0x14,
	PDF_VERSION_1_5		= 0x15,
	PDF_VERSION_1_6		= 0x16,
	PDF_VERSION_1_7		= 0x17
};

enum PDF_PARSE_WORD
{
	PARSE_WORD_UNKNOWN	= 0x00,
	PARSE_WORD_INTEGER	= 0x01,
	PARSE_WORD_FLOAT	= 0x02,
	PARSE_WORD_STRING	= 0x03,
	PARSE_WORD_NAME		= 0x04,
	PARSE_WORD_DICT_B	= 0x05,
	PARSE_WORD_DICT_E	= 0x06,
	PARSE_WORD_ARRAY_B	= 0x07,
	PARSE_WORD_ARRAY_E	= 0x08
};


class CHE_PDF_File;

class CHE_PDF_ParseWordDes : public CHE_Object
{
public:
	CHE_PDF_ParseWordDes( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ), str( pAllocator ) {};

	PDF_PARSE_WORD	type;
	HE_DWORD		offset;
	CHE_ByteString	str;
};

class CHE_PDF_SyntaxParser : public CHE_Object
{
public:
	CHE_PDF_SyntaxParser( CHE_PDF_File * pFile, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_SyntaxParser();

	HE_BOOL				InitParser( IHE_Read* pFileAccess );
	HE_DWORD			GetFileSize() { return m_lFileSize; };

	HE_VOID				SetFile( CHE_PDF_File * pFile ) { mpFile = pFile; }
	CHE_PDF_File *		GetFile()const { return mpFile; }

	/*	当前位置移动和设置的相关操作	*/
	HE_DWORD			GetPos() { return m_lFilePos; };
	HE_VOID				SetPos( HE_DWORD pos) { m_lFilePos = pos; };
	HE_DWORD			Seek( HE_DWORD bytes );
	HE_VOID				SeekToPrevLine();
	HE_VOID				SeekToNextLine();
	HE_VOID				SeekToPrevWord();
	HE_VOID				SeekToNextWord();
 	HE_VOID				SeekToEndStream();
 	HE_VOID				SeekToEndobj();

	HE_VOID				SeekToMark( CHE_ByteString markStr );
	HE_VOID				SetEncrypt( CHE_PDF_Encrypt * pEncrypt ) { m_pStrEncrypt = pEncrypt; }
	HE_VOID				SetCurObjNum( HE_DWORD objNum ) { m_dwCurObjNum = objNum; }
	HE_VOID				SetCurGenNum( HE_DWORD genNum ) { m_dwCurGenNum = genNum; }
	CHE_PDF_Encrypt *	GetEncrypt() { return m_pStrEncrypt; }
	HE_DWORD			ReadBytes( HE_LPBYTE pBuffer, HE_DWORD length );
	HE_BOOL				GetWord( CHE_PDF_ParseWordDes & des );
	/* 从当前位置开始解析一个数组，如果当前位置不是一个数组，则返回空（当前位置必须是数组开始"["） */
	CHE_PDF_ArrayPtr	GetArrayPtr();
	/*	从当前位置开始解析一个字典，如果当前位置不是一个字典，则返回空（当前位置必须是字典开始"<<"）	*/
	CHE_PDF_DictionaryPtr	GetDictionaryPtr();

private:
	HE_VOID				SubmitBufferStr( CHE_ByteString & str );

	HE_DWORD			m_lFilePos;
	HE_DWORD			m_lFileSize;
	IHE_Read*			m_pFileAccess;

	CHE_PDF_Encrypt	*	m_pStrEncrypt;
	HE_DWORD			m_dwCurObjNum;
	HE_DWORD			m_dwCurGenNum;

	HE_BOOL				m_bBegin;
	HE_BOOL				m_bPoint;
	HE_BOOL				m_bSign;

	HE_BYTE				m_WordBuffer[32770];
	HE_DWORD			m_lBufferSize;
	HE_DWORD			m_lBufferPos;
	CHE_PDF_File *		mpFile;

	friend class CHE_PDF_Parser;
};

class CHE_PDF_Parser : public CHE_Object
{
public:
	static CHE_PDF_Parser * Create( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XREF_Table * pXrefTable, CHE_Allocator * pAllocator = NULL );

	~CHE_PDF_Parser();

	//Basic information
	HE_DWORD					GetFileSize() const;
	PDF_VERSION					GetPDFVersion() const;

	//Encryption
	HE_BOOL						Authenticate( const CHE_ByteString & password );

	HE_DWORD					GetReadPos() { return m_sParser.GetPos(); }
	HE_VOID						SetReadPos( HE_DWORD pos) { m_sParser.SetPos( pos ); }

	CHE_PDF_ObjectPtr			GetObject();
	CHE_PDF_ObjectPtr			GetObjectInObjStm( CHE_PDF_StreamPtr & pStream, const PDF_RefInfo & ObjrefInfo, HE_DWORD index );

/*	CHE_PDF_ReferencePtr		GetEncryptRef() const { return mpEncryptRef; } */

private:
	CHE_PDF_Parser( CHE_PDF_File * pFile, IHE_Read * pRead, CHE_PDF_XREF_Table * pXrefTable, CHE_Allocator * pAllocator = NULL );

	HE_DWORD					GetStartxref( HE_DWORD range );

	HE_DWORD					ParseXRef();
	HE_DWORD					ParseXRefTable( HE_DWORD offset, CHE_PDF_DictionaryPtr & pTrailerDictRet );
	HE_DWORD					ParseXRefStream( HE_DWORD offset, CHE_PDF_DictionaryPtr & pTrailerDictRet );
	HE_DWORD					FullParseForXRef();

	HE_BOOL						ParseEncrypt( const CHE_PDF_DictionaryPtr & pEncryptDict, const CHE_PDF_ArrayPtr & pIdArray );

private:
	CHE_PDF_SyntaxParser		m_sParser;
	CHE_PDF_File *				mpFile;
	IHE_Read *					m_pIHE_FileRead;
	HE_DWORD					m_lStartxref;
	
	CHE_PDF_XREF_Table *		mpXRefTable;

	CHE_PDF_Encrypt	*			m_pStrEncrypt;
	CHE_PDF_Encrypt	*			m_pStmEncrypt;
	CHE_PDF_Encrypt	*			m_pEefEncrypt;

	CHE_PDF_ReferencePtr		mEncryptRef;
	CHE_PDF_ArrayPtr			mIDArrayPtr;

	friend class CHE_PDF_File;
	friend class CHE_Allocator;
};

#endif