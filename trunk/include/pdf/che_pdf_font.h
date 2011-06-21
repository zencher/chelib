#ifndef _CHE_PDF_FONT_H_
#define _CHE_PDF_FONT_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#define USE_FREETYPE2_STATIC
#include "../../extlib/freetype/ft2build.h"
#include "../../extlib/freetype/freetype/freetype.h"

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
#define PDFENCODING_UCS2		7
#define PDFENCODINT_UTF16		8
#define PDFENCODING_SELFDEF		9
#define PDFENCODING_OTHER		10

HE_VOID HE_InitFTLibrary();
HE_VOID HE_DestroyFTLibrary();

class CHE_PDF_Font : public CHE_Object
{
public:
	CHE_PDF_Font( CHE_PDF_Dictionary * pFontDict,/* IHE_PDF_GetInObj * pIHE_GetPDFInObj,*/ CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Font();

	HE_BYTE		GetType() { return m_FontType; }
	HE_BOOL		IsSimpleFont() { return ( m_FontType == PDFFONT_TYPE0 || m_FontType == PDFFONT_UNKNOWN ) ? FALSE:TRUE; }
	HE_BOOL		IsCompositeFont() { return !IsSimpleFont(); }

	HE_BYTE		GetEncodingType() { return m_EncodingType; }
	HE_BOOL		IsDefaultEncoding() { return ( m_EncodingType == PDFENCODING_SELFDEF || m_EncodingType == PDFENCODING_NONE ) ? FALSE:TRUE; }

	HE_BOOL		IsFontFileLoaded() { return ( m_pFontProgramData == NULL ) ? FALSE : TRUE; }
	HE_BOOL		LoadFontFile();
	HE_LPBYTE	GetFontFile() { return m_pFontProgramData; }
	HE_LONG		GetFontFileSize() { return m_lFontDataLength; }

	HE_WCHAR	GetUnicode( HE_WCHAR wch );
	HE_BOOL		GetUnicodeString( CHE_ByteString & str, CHE_WideString & wstrRet );

private:
	CHE_NumToPtrMap	*	GetToUnicodeMap( CHE_PDF_Stream * pToUnicodeStream );

	HE_WCHAR			GetUnicodeFromFace( HE_WCHAR wch );

private:
	HE_BYTE					m_FontType;
	HE_BYTE					m_EncodingType;

	CHE_PDF_Dictionary *	m_pFontDict;
	//IHE_PDF_GetInObj *		m_pIHE_GetPDFInObj;

	HE_BYTE *				m_pFontProgramData;
	HE_LONG					m_lFontDataLength;
	FT_Face					m_FontFace;
	
	HE_WCHAR *				m_pUnicodeTable;
	CHE_NumToPtrMap*		m_pMap;
};

#endif
