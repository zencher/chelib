#ifndef _CHE_PDF_FONT_H_
#define _CHE_PDF_FONT_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_cmap.h"

#include "../../extlib/freetype/ft2build.h"
#include "../../extlib/freetype/freetype/freetype.h"

HE_VOID HE_InitFTLibrary();
HE_VOID HE_DestroyFTLibrary();

enum PDF_FONT_TYPE
{
	FONT_TYPE0		= 0x00,
	FONT_TYPE1		= 0x01,
	FONT_TRUETYPE	= 0x02,
	FONT_TYPE3		= 0x03
};

enum PDF_FONT_TYPE1_STANDARD14
{
	FONT_TYPE1_STANDARD14_TIMES_ROMAN			= 0x00,
	FONT_TYPE1_STANDARD14_TIMES_BOLD			= 0x01,
	FONT_TYPE1_STANDARD14_TIMES_ITALIC			= 0x02,
	FONT_TYPE1_STANDARD14_TIMES_BOLDITALIC		= 0x03,
	FONT_TYPE1_STANDARD14_HELVETICA				= 0x04,
	FONT_TYPE1_STANDARD14_HELVETICA_BOLD		= 0x05,
	FONT_TYPE1_STANDARD14_HELVETICA_OBILQUE		= 0x06,
	FONT_TYPE1_STANDARD14_HELVETICA_BOLDOBILQUE	= 0x07,
	FONT_TYPE1_STANDARD14_COURIER				= 0x08,
	FONT_TYPE1_STANDARD14_COURIER_BOLD			= 0x09,
	FONT_TYPE1_STANDARD14_COURIER_OBILQUE		= 0x0a,
	FONT_TYPE1_STANDARD14_COURIER_BOLDOBILQUE	= 0x0b,
	FONT_TYPE1_STANDARD14_SYMBOL				= 0x0c,
	FONT_TYPE1_STANDARD14_ZAPFDINGBATS			= 0x0d
};

enum PDF_FONT_ENCODING
{
	FONT_ENCODING_NONE			= 0x00,
	FONT_ENCODING_STANDARD		= 0x01,
	FONT_ENCODING_PDFDOC		= 0x02,
	FONT_ENCODING_WINANSI		= 0x03,
	FONT_ENCODING_MACROMAN		= 0x04,
	FONT_ENCODING_MACEXPERT		= 0x05,
	FONT_ENCODING_BUILDINCMAP	= 0x06,
	FONT_ENCODING_SELFDEF		= 0x07
};

class CHE_PDF_Font : public CHE_Object
{
public:
	CHE_PDF_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Font();

	PDF_FONT_TYPE			GetType() { return m_FontType; }
	HE_BOOL					IsSimpleFont() { return ( m_FontType == FONT_TYPE0 ) ? FALSE:TRUE; }
	HE_BOOL					IsCompositeFont() { return !IsSimpleFont(); }

	PDF_FONT_ENCODING		GetEncodingType() { return m_EncodingType; }
	HE_BOOL					IsDefaultEncoding() { return ( m_EncodingType == FONT_ENCODING_SELFDEF ) ? FALSE:TRUE; }

	HE_INT32				GetUnicode( HE_INT32 code );
	HE_INT32				GetCID( HE_INT32 code );

private:
	CHE_NumToPtrMap	*		GetToUnicodeMap( const CHE_PDF_StreamPtr & pToUnicodeStream );

	PDF_FONT_TYPE			m_FontType;
	PDF_FONT_ENCODING		m_EncodingType;

	CHE_PDF_DictionaryPtr	m_pFontDict;
	CHE_PDF_CMap *			m_pCIDMap;
	CHE_PDF_CMap *			m_pUnicodeMap;

	bool					m_bEncodingToDes;
 	HE_WCHAR *				m_pEncodingTable;
 	CHE_NumToPtrMap *		m_pToUnicodeMap;
};

#endif
