#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"
#include "che_pdf_xref.h"

#define PDFPARSER_WORD_UNKNOWN	0
#define PDFPARSER_WORD_INTEGER	1
#define PDFPARSER_WORD_FLOAT	2
#define PDFPARSER_WORD_STRING	3
#define PDFPARSER_WORD_NAME		4
#define PDFPARSER_WORD_DICT_B	5
#define PDFPARSER_WORD_DICT_E	6
#define PDFPARSER_WORD_ARRAY_B	7
#define PDFPARSER_WORD_ARRAY_E	8

class CHE_PDF_SyntaxParser : public CHE_Object
{
public:
	CHE_PDF_SyntaxParser();
	~CHE_PDF_SyntaxParser();

	HE_BOOL				InitParser( IHE_FileRead* pFileAccess );
	HE_DWORD			GetFileSize() { return m_lFileSize; };

	/*	当前位置移动和设置的相关操作	*/
	HE_DWORD			GetPos() { return m_lFilePos; };
	HE_VOID				SetPos( HE_DWORD pos) { m_lFilePos = pos; };
	HE_DWORD			Seek( HE_DWORD bytes );
	/*	移动当前位置，分别为跳到下一个词的开头和新的行的开头	*/
	HE_VOID				SeekToNextLine();
	HE_VOID				SeekToNextWord();

	/*	从当前位置开始获取一个词（语法上的）	*/
	CHE_ByteString		GetWord();

	/*	返回最近一次返回的词的类型，目前只区分字符串，名称和未知三种类型，其余类型需要上一层参与运算	*/
	HE_BYTE				GetType() { return m_byteType; };

	CHE_PDF_Array *		GetArray();

	/*	从当前位置开始解析一个字典，如果当前位置不是一个字典，则返回空（当前位置必须是字典开始"<<"）	*/
	CHE_PDF_Dictionary * GetDictionary();



private:
	CHE_ByteString		SubmitBufferStr() { 
								CHE_ByteString strTmp;
								m_WordBuffer[m_lBufferPos] = '\0';
								strTmp = (char const *)(&(m_WordBuffer[0]));
								m_WordBuffer[m_lBufferPos=0] = '\0';
								m_bBegin = TRUE;
								m_bPoint = FALSE;
								m_bSign = FALSE;
								//m_byteType = PDFPARSER_WORD_UNKNOWN;
								return strTmp; }

	HE_DWORD			m_lFilePos;
	HE_DWORD			m_lFileSize;
	IHE_FileRead*		m_pFileAccess;

	HE_BOOL				m_bBegin;
	HE_BOOL				m_bPoint;
	HE_BOOL				m_bSign;

	HE_BYTE				m_byteType;
	HE_BYTE				m_WordBuffer[32770];
	HE_DWORD			m_lBufferSize;
	HE_DWORD			m_lBufferPos;
};

class CHE_PDF_Parser : public CHE_Object
{
public:
	CHE_PDF_Parser();

	HE_BOOL StartParse( IHE_FileRead * file );

	HE_VOID CloseParser();

	HE_DWORD GetFileSize();

	HE_PDF_VERSION GetPDFVersion();

	HE_DWORD GetStartxrefOffset( HE_DWORD range );
	HE_BOOL GetXRefTable();

	CHE_PDF_IndirectObject *	GetIndirectObject();
	CHE_PDF_IndirectObject *	GetIndirectObject( HE_DWORD objNum );

	//bool IsLinearized() const;
	//void GetObj( HE_DWORD objNum );
	//CHE_PDF_Dictionary * GetTrailer();
	//CHE_PDF_Dictionary * GetOtherTrailers();
private:
	IHE_FileRead * m_pIHE_FileRead;

	CHE_PDF_XREF_Table m_xrefTable;

	CHE_PDF_Dictionary*	m_pTrailerDict;

	CHE_PDF_SyntaxParser m_sParser;

	HE_DWORD m_lstartxref;
};

#endif