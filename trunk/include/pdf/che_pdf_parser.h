#ifndef _CHE_PDF_PARSER_H_
#define _CHE_PDF_PARSER_H_

#include "../che_base.h"
#include "../che_string.h"
#include "che_pdf_define.h"
#include "che_pdf_xref.h"

class CHE_PDF_SyntaxParser : public CHE_Object
{
public:
	CHE_PDF_SyntaxParser();
	~CHE_PDF_SyntaxParser();

	HE_BOOL				InitParser( IHE_FileRead* pFileAccess );

	HE_DWORD			GetFileSize() { return m_lFileSize; };

	HE_DWORD			GetPos() { return m_lFilePos; };
	HE_VOID				SetPos( HE_DWORD pos) { m_lFilePos = pos; };
	
	HE_DWORD			Seek( HE_DWORD bytes );

	CHE_ByteString		GetWord();

	HE_VOID				NextLine();
	HE_VOID				NextWord();

private:
	HE_DWORD			m_lFilePos;
	HE_DWORD			m_lFileSize;
	IHE_FileRead*		m_pFileAccess;

	HE_BYTE				m_WordBuffer[32769];
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

	//bool IsLinearized() const;
	//HE_BOOL GetXRefTable();
	//void GetObj( HE_DWORD objNum );
	//CHE_PDF_Dictionary * GetTrailer();
	//CHE_PDF_Dictionary * GetOtherTrailers();
private:
	IHE_FileRead * m_pIHE_FileRead;

	CHE_PDF_XREF_Table m_xrefTable;

	CHE_PDF_SyntaxParser m_sParser;
};

#endif