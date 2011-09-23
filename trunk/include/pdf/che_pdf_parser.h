#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"
#include "che_pdf_xref.h"
#include "che_pdf_encrypt.h"
#include "che_pdf_collector.h"

#include <vector>

class CHE_PDF_Creator;

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
	CHE_PDF_SyntaxParser( CHE_PDF_Parser * pParser, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_SyntaxParser();

	HE_BOOL				InitParser( IHE_Read* pFileAccess );
	HE_DWORD			GetFileSize() { return m_lFileSize; };

	HE_VOID				SetParser( CHE_PDF_Parser * pParser ) { m_pParser = pParser; }
	CHE_PDF_Parser *	GetParser()const { return m_pParser; }

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
	CHE_PDF_Parser *	m_pParser;

	friend class CHE_PDF_Parser;
};

class CHE_PDF_Parser : public CHE_Object
{
public:
	CHE_PDF_Parser( CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Parser() { Close(); }

	HE_BOOL						Open( IHE_Read * file );
	HE_VOID						Close();

	//Basic information
	HE_DWORD					GetFileSize() const;
	PDF_VERSION					GetPDFVersion() const;
	CHE_PDF_Dictionary *		GetTrailerDict() const { return m_xrefTable.GetTrailer(); }
	CHE_PDF_Dictionary *		GetRootDict();
	CHE_PDF_Dictionary *		GetInfoDict();
	CHE_PDF_Array *				GetIDArray();
	//HE_BOOL					IsLinearized();

	//Page tree information
	HE_DWORD					GetPageCount();
	HE_DWORD					GetPageObjNum( HE_DWORD pageIndex );

	//Encryption
	HE_BOOL						Authenticate( CHE_ByteString & password ) const 
									{ return m_pStmEncrypt ? m_pStmEncrypt->Authenticate( password ): FALSE; }

	//Object operation
	CHE_PDF_Object *			GetObject( HE_DWORD objNum );
	PDF_OBJ_STATUS				GetObjectStatus( HE_DWORD objNum );

	CHE_PDF_Object *			CreateObject( PDF_OBJ_TYPE type );
	HE_DWORD					GetNewObjectCount();
	CHE_PDF_Object *			GetNewObject( HE_DWORD index );

	HE_BOOL						SetObjectModified( HE_DWORD objNum );
	HE_DWORD					GetModifiedObjectCount();
	CHE_PDF_Object *			GetModifiedObject( HE_DWORD index );

private:
	HE_DWORD					GetStartxref( HE_DWORD range );

	HE_DWORD					ParseXRef();
	HE_DWORD					ParseXRefTable( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet );
	HE_DWORD					ParseXRefStream( HE_DWORD offset, CHE_PDF_Dictionary ** pTrailerDictRet );
	HE_DWORD					FullParseForXRef();

	HE_BOOL						ParseEncrypt( CHE_PDF_Dictionary * pEncryptDict );

	CHE_PDF_Object *			GetObject();
	CHE_PDF_Object *			GetObjectInObjStm( HE_DWORD stmObjNum, HE_DWORD objNum, HE_DWORD index );

	HE_DWORD					GetFreeObjNum();

private:
	IHE_Read *					m_pIHE_FileRead;
	HE_DWORD					m_lStartxref;
	
	HE_DWORD					m_lPageCount;
	HE_DWORD					m_lCurPageIndex;
	std::vector<HE_DWORD>		m_pPageObjList;
	CHE_PtrStack				m_PageNodeStack;

	CHE_PDF_Encrypt	*			m_pStrEncrypt;
	CHE_PDF_Encrypt	*			m_pStmEncrypt;
	CHE_PDF_Encrypt	*			m_pEefEncrypt;

	CHE_PDF_SyntaxParser		m_sParser;
	CHE_PDF_XREF_Table			m_xrefTable;				//结构化的交叉索引表信息
	CHE_PDF_Collector			m_objCollector;				//对象收集器，被加载的都被放入收集器，某些尾字典不会
	CHE_PDF_Collector			m_NewObjCollector;			//新建的对象的收集器
	CHE_PDF_Collector			m_ModifiedObjCollector;		//被修改的对象的收集器

	friend class CHE_PDF_Creator;
};

#endif