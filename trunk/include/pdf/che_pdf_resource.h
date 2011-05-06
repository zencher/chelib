#ifndef _CHE_PDF_RESOURCE_H_
#define _CHE_PDF_RESOURCE_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"

#define PDFFONT_UNKNOWN			0
#define PDFFONT_TYPE0			1
#define PDFFONT_TYPE1			2
#define PDFFONT_TRUETYPE		3
#define PDFFONT_TYPE3			4

#define PDFENCODING_NONE		0
#define PDFENCODING_STANDARD	1
#define PDFENCODING_WINANSI		2
#define	PDFENCODING_MACROMAN	3
#define PDFENCODING_MACEXPERT	4
#define PDFENCODING_PDFDOC		5
#define PDFENCODING_GBK			6
#define PDFENCODING_GBK_UCS2	7
#define PDFENCODING_SELFDEF		8
#define PDFENCODING_OTHER		9

class CHE_PDF_FontCharCodeMgr : public CHE_Object
{
public:
	CHE_PDF_FontCharCodeMgr( IHE_PDF_GetInObj * pIHE_GetPDFInObj, CHE_PDF_Dictionary * pFontDict );
	~CHE_PDF_FontCharCodeMgr();

	HE_WCHAR	GetUnicode( HE_WCHAR wch );

	HE_BYTE		GetEncodingType() { return m_EncodingType; }
	HE_BYTE		GetFontType() { return m_FontType; }
	HE_BOOL		IsDefaultEncoding() { return m_bDefaultEncoding; }

private:
	IHE_PDF_GetInObj *		m_pIHE_GetPDFInObj;
	CHE_PDF_Dictionary *	m_pFontDict;
	HE_WCHAR *				m_pUnicodeTable;
	CHE_NumToPtrMap*		m_pMap;

	HE_BYTE					m_EncodingType;
	HE_BYTE					m_FontType;
	HE_BOOL					m_bDefaultEncoding;
};

#endif