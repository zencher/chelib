#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"
#include "che_pdf_xref.h"
#include "che_pdf_encrypt.h"

#define PDFPARSER_WORD_UNKNOWN	0
#define PDFPARSER_WORD_INTEGER	1
#define PDFPARSER_WORD_FLOAT	2
#define PDFPARSER_WORD_STRING	3
#define PDFPARSER_WORD_NAME		4
#define PDFPARSER_WORD_DICT_B	5
#define PDFPARSER_WORD_DICT_E	6
#define PDFPARSER_WORD_ARRAY_B	7
#define PDFPARSER_WORD_ARRAY_E	8

class CHE_PDF_PARSER_WORD_DES : public CHE_Object
{
public:
	CHE_ByteString	str;
	HE_BYTE			type;
	HE_DWORD		offset;
};

HE_INT32 HE_PDF_StringToInteger( CHE_ByteString & str );
HE_FLOAT HE_PDF_StringToFloat( CHE_ByteString & str );

class IHE_PDF_GetInObj : public CHE_Object
{
public:
	virtual ~IHE_PDF_GetInObj() {};
	
	virtual CHE_PDF_IndirectObject * GetInObj( HE_DWORD objNum ) = 0;

	virtual CHE_PDF_Object * GetObj( HE_DWORD objNum, HE_BYTE objType = 0 ) = 0;
};

class CHE_PDF_SyntaxParser : public CHE_Object
{
public:
	CHE_PDF_SyntaxParser();
	~CHE_PDF_SyntaxParser();

	HE_BOOL				InitParser( IHE_Read* pFileAccess );
	HE_DWORD			GetFileSize() { return m_lFileSize; };

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

	HE_DWORD			ReadBytes( /*HE_DWORD offset,*/ HE_LPBYTE pBuffer, HE_DWORD length );

	HE_BOOL				GetWord( CHE_PDF_PARSER_WORD_DES & des );

	/* 从当前位置开始解析一个数组，如果当前位置不是一个数组，则返回空（当前位置必须是数组开始"["） */
	CHE_PDF_Array *		GetArray();

	/*	从当前位置开始解析一个字典，如果当前位置不是一个字典，则返回空（当前位置必须是字典开始"<<"）	*/
	CHE_PDF_Dictionary * GetDictionary();

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

	friend class CHE_PDF_Parser;
};

class CHE_PDF_Parser : public CHE_Object
{
public:
	CHE_PDF_Parser();
	~CHE_PDF_Parser() { CloseParser(); }

	HE_BOOL						StartParse( IHE_Read * file );

	HE_VOID						CloseParser();

	HE_DWORD					GetFileSize();

	HE_PDF_VERSION				GetPDFVersion();

	HE_DWORD					GetStartxrefOffset( HE_DWORD range );

	HE_DWORD					ParseXRef();

	HE_VOID						VerifyXRef();

	HE_VOID						VerifyObjInStm();

	CHE_PDF_Dictionary*			GetRootDict();

	CHE_PDF_Dictionary*			GetInfoDict();

	CHE_PDF_Dictionary*			GetTrailerDict() { return m_pTrailerDict; }
	
	CHE_PDF_Array*				GetIDArray();

	HE_DWORD					GetPageCount();

	HE_DWORD					GetPageObjList( HE_DWORD* pList );

	HE_BOOL						Authenticate( CHE_ByteString & password ) { return m_pStmEncrypt ? m_pStmEncrypt->Authenticate( password ): FALSE; }

	CHE_PDF_IndirectObject *	GetIndirectObject();
	
	CHE_PDF_IndirectObject *	GetIndirectObject( HE_DWORD objNum );

	CHE_PDF_IndirectObject *	GetIndirectObjectInObjStm( HE_DWORD stmObjNum, HE_DWORD objNum, HE_DWORD index );

	IHE_PDF_GetInObj	*		GetIHE_GetPDFInObj() { return m_pIHE_GetPDFInObj; }

	//bool IsLinearized() const;

private:
	HE_DWORD					FullParseForXRef();
	
	HE_DWORD					ParseXRefTable( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet );
	
	HE_DWORD					ParseXRefStream( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet );

private:
	IHE_Read *					m_pIHE_FileRead;
	IHE_PDF_GetInObj *			m_pIHE_GetPDFInObj;

	CHE_PDF_Dictionary*			m_pTrailerDict;
	HE_BOOL						m_bTrailerDictNeedDestory;

	HE_DWORD					m_lstartxref;
	HE_DWORD					m_lPageCount;

	CHE_PDF_Encrypt	*			m_pStrEncrypt;
	CHE_PDF_Encrypt	*			m_pStmEncrypt;
	CHE_PDF_Encrypt	*			m_pEefEncrypt;

	CHE_PtrArray				m_arrObjStm;
	CHE_NumToPtrMap				m_XrefVerifyMap1;
	CHE_NumToPtrMap				m_XrefVerifyMap2;
	
	CHE_PDF_SyntaxParser		m_sParser;
	CHE_PDF_XREF_Table			m_xrefTable;
	CHE_PDF_IndirectObjectCollector	m_objCollector;
};

#endif