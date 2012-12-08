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
	FONT_MMTYPE1	= 0x02,
	FONT_TRUETYPE	= 0x03,
	FONT_TYPE3		= 0x04
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
	FONT_ENCODING_SYMBOL		= 0x06,
	FONT_ENCODING_ZAPFDINGBAT	= 0x07,
	FONT_ENCODING_CUSTOM		= 0x08
};


struct pdf_hmtx
{
	unsigned short lo;
	unsigned short hi;
	int w;	/* type3 fonts can be big! */
};

struct pdf_vmtx
{
	unsigned short lo;
	unsigned short hi;
	short x;
	short y;
	short w;
};


class CHE_PDF_Encoding : public CHE_Object
{
public:
	static CHE_PDF_Encoding *	Create( CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );

	PDF_FONT_ENCODING			GetType() const;

	HE_BOOL						GetUnicode( HE_BYTE charCode, HE_WCHAR & codeRet ) const;

private:
	CHE_PDF_Encoding( CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Encoding();

	PDF_FONT_ENCODING			mType;
	HE_BOOL						mbCodeTableRelease;
	HE_WCHAR *					mpCodeTable;

	friend class CHE_Allocator;
};


class CHE_PDF_FontDescriptor : public CHE_Object
{
public:
	CHE_PDF_FontDescriptor( CHE_Allocator * pAllocator = NULL );
	CHE_PDF_FontDescriptor( CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator = NULL ); 

private:
	//fz_font *font;

	/* FontDescriptor */
	int flags;
	float italic_angle;
	float ascent;
	float descent;
	float cap_height;
	float x_height;
	float missing_width;

	/* Encoding (CMap) */
	CHE_PDF_CMap * encoding;
	CHE_PDF_CMap * to_ttf_cmap;
	//pdf_cmap *encoding;
	//pdf_cmap *to_ttf_cmap;
	int cid_to_gid_len;
	unsigned short *cid_to_gid;

	/* ToUnicode */
	CHE_PDF_CMap * to_unicode;
	//pdf_cmap *to_unicode;
	int cid_to_ucs_len;
	unsigned short *cid_to_ucs;

	/* Metrics (given in the PDF file) */
	int wmode;

	int hmtx_len, hmtx_cap;
	pdf_hmtx dhmtx;
	pdf_hmtx *hmtx;

	int vmtx_len, vmtx_cap;
	pdf_vmtx dvmtx;
	pdf_vmtx *vmtx;

	int is_embedded;
};


//字体作重要的包括三个内容，一个是基本的信息（类型，FontDescriptor等等），二是Encoding、Unicode、CMap，三是FT_FACE

class CHE_PDF_Font : public CHE_Object
{
public:
	static CHE_PDF_Font * 	Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );

	PDF_FONT_TYPE			GetType() const;
	HE_BOOL					IsSimpleFont() const;

	CHE_ByteString			GetBaseFont() const;

	HE_BOOL					GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const;
	HE_BOOL					GetCID( HE_WCHAR charCode, HE_DWORD & codeRet ) const;

private:
	CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Font();

	CHE_NumToPtrMap *		GetToUnicodeMap( const CHE_PDF_StreamPtr & pToUnicodeStream );

	PDF_FONT_TYPE			mType;
	CHE_ByteString			mBaseFont;
	CHE_PDF_DictionaryPtr	mFontDict;
	CHE_PDF_DictionaryPtr	mFontDescriptorDict;

	//for simple font
	CHE_PDF_Encoding *		mpEncoding;

	//for all font
	CHE_NumToPtrMap *		mpToUnicodeMap;

	//for type0(CIDFont) font
	CHE_PDF_CMap *			mpCIDMap;
	CHE_PDF_CMap *			mpUnicodeMap;

	friend class CHE_Allocator;

 	//FT_Face					mFace;

	//Font Descriptor
	//HE_INT32				mFlags;
	//HE_FLOAT				mItalicAngle;
	//HE_FLOAT				mAscent;
	//HE_FLOAT				mDescent;
	//HE_FLOAT				mCapHeight;
	//HE_FLOAT				mXHeight;
	//HE_FLOAT				mMissingWidth;
	//HE_BOOL				mEmbedded;
// 
// 	wmode = 0;
// 
// 	hmtx_cap = 0;
// 	vmtx_cap = 0;
// 	hmtx_len = 0;
// 	vmtx_len = 0;
// 	hmtx = NULL;
// 	vmtx = NULL;
// 
// 	dhmtx.lo = 0x0000;
// 	dhmtx.hi = 0xFFFF;
// 	dhmtx.w = 1000;
// 
// 	dvmtx.lo = 0x0000;
// 	dvmtx.hi = 0xFFFF;
// 	dvmtx.x = 0;
// 	dvmtx.y = 880;
// 	dvmtx.w = -1000;
};


#endif
