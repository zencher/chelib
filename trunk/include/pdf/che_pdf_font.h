#ifndef _CHE_PDF_FONT_H_
#define _CHE_PDF_FONT_H_

#include "../che_base.h"
#include "che_pdf_define.h"
#include "che_pdf_objects.h"
#include "che_pdf_parser.h"
#include "che_pdf_cmap.h"

#include "../../extlib/freetype/ft2build.h"
#include "../../extlib/freetype/freetype/freetype.h"

HE_VOID HE_InitFTLibrary();
HE_VOID HE_DestroyFTLibrary();

class CHE_PDF_Font : public CHE_Object
{
public:
	CHE_PDF_Font( CHE_PDF_Dictionary * pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Font();

	PDF_FONT_TYPE			GetType() { return m_FontType; }
	HE_BOOL					IsSimpleFont() { return ( m_FontType == FONT_TYPE0 ) ? FALSE:TRUE; }
	HE_BOOL					IsCompositeFont() { return !IsSimpleFont(); }

	PDF_FONT_ENCODING		GetEncodingType() { return m_EncodingType; }
	HE_BOOL					IsDefaultEncoding() { return ( m_EncodingType == FONT_ENCODING_SELFDEF ) ? FALSE:TRUE; }

	HE_BOOL					IsFontFileLoaded() { return ( m_pFontProgramData == NULL ) ? FALSE : TRUE; }
	HE_BOOL					LoadFontFile();
	HE_LPBYTE				GetFontFile() { return m_pFontProgramData; }
	HE_LONG					GetFontFileSize() { return m_lFontDataLength; }

	HE_WCHAR				GetUnicode( HE_WCHAR wch );
	HE_BOOL					GetUnicodeString( CHE_ByteString & str, CHE_WideString & wstrRet );

private:
	CHE_NumToPtrMap	*		GetToUnicodeMap( CHE_PDF_Stream * pToUnicodeStream );

	HE_WCHAR				GetUnicodeFromFace( HE_WCHAR wch );

private:
	PDF_FONT_TYPE			m_FontType;
	PDF_FONT_ENCODING		m_EncodingType;

	CHE_PDF_Dictionary *	m_pFontDict;

	HE_LPBYTE				m_pFontProgramData;
	HE_LONG					m_lFontDataLength;
	FT_Face					m_FontFace;
	
	HE_WCHAR *				m_pUnicodeTable;
	CHE_NumToPtrMap*		m_pMap;
};

#endif
