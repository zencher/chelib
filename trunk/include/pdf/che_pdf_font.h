#ifndef _CHE_PDF_FONT_H_
#define _CHE_PDF_FONT_H_

#include "../che_base.h"
#include "che_pdf_objects.h"
#include "che_pdf_matrix.h"
#include "che_pdf_cmap.h"

#include "../../extlib/freetype/ft2build.h"
#include "../../extlib/freetype/freetype/freetype.h"


FT_Library HE_GetFTLibrary();
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


//该类是一个接口类，与平台相关
//用于实现获取字体路径或者字体文件数据的接口
//根据一些必要的信息来获取匹配的字体或者默认字体的接口

class IHE_SystemFontMgr : public CHE_Object
{
public:
	static IHE_SystemFontMgr *	Create( CHE_Allocator * pAllocator = NULL );
	static HE_VOID				Destroy( IHE_SystemFontMgr * pSystemFontMgr );

	IHE_SystemFontMgr( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};

	virtual ~IHE_SystemFontMgr() {};

	virtual CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName ) = 0;
};


IHE_SystemFontMgr * HE_GetSystemFontMgr( CHE_Allocator * pAllocator = NULL );


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
	CHE_PDF_FontDescriptor( const CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator = NULL ); 
	~CHE_PDF_FontDescriptor();
	
private:
	HE_INT32				mFlags;
	HE_FLOAT				mItalicAngle;
	HE_FLOAT				mAscent;
	HE_FLOAT				mDescent;
	HE_FLOAT				mCapHeight;
	HE_FLOAT				mXHeight;
	HE_FLOAT				mMissingWidth;
	HE_BOOL					mEmbedded;
	HE_INT32 				mWMode;
	CHE_PDF_ReferencePtr	mEmbedFont;

// 	/* Encoding (CMap) */
// 	CHE_PDF_CMap * encoding;
// 	CHE_PDF_CMap * to_ttf_cmap;
// 	//pdf_cmap *encoding;
// 	//pdf_cmap *to_ttf_cmap;
// 	int cid_to_gid_len;
// 	unsigned short *cid_to_gid;
// 
// 	/* ToUnicode */
// 	CHE_PDF_CMap * to_unicode;
// 	//pdf_cmap *to_unicode;
// 	int cid_to_ucs_len;
// 	unsigned short *cid_to_ucs;
// 
// 	/* Metrics (given in the PDF file) */
// 	int wmode;
// 
// 	int hmtx_len, hmtx_cap;
// 	pdf_hmtx dhmtx;
// 	pdf_hmtx *hmtx;
// 
// 	int vmtx_len, vmtx_cap;
// 	pdf_vmtx dvmtx;
// 	pdf_vmtx *vmtx;
};


class CHE_PDF_Font : public CHE_Object
{
public:
	static CHE_PDF_Font * 	Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );

	PDF_FONT_TYPE			GetType() const;
	CHE_ByteString			GetBaseFont() const;
	CHE_PDF_DictionaryPtr	GetFontDictPtr() const;
	CHE_PDF_DictionaryPtr	GetFontDescriptorDictPtr() const;
	FT_Face					GetFTFace();
	virtual HE_RECT			GetGraphBox( HE_WCHAR charCode, CHE_PDF_Matrix matrix = CHE_PDF_Matrix() ) const = 0;
	virtual HE_BOOL			GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const = 0;
	
protected:
	CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	virtual ~CHE_PDF_Font();

	CHE_NumToPtrMap	*		GetToUnicodeMap( const CHE_PDF_StreamPtr & pToUnicodeStream );

	PDF_FONT_TYPE			mType;
	CHE_ByteString			mBaseFont;
	CHE_PDF_DictionaryPtr	mFontDict;
	CHE_PDF_DictionaryPtr	mFontDescriptorDict;
	FT_Face					mFace;
	CHE_NumToPtrMap *		mpToUnicodeMap;
	CHE_PDF_FontDescriptor*	mpFontDescriptor;

	friend class CHE_Allocator;
};


class CHE_PDF_Type0_Font : public CHE_PDF_Font
{
public:
	HE_BOOL	GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const;
	HE_RECT GetGraphBox( HE_WCHAR charCode, CHE_PDF_Matrix matrix = CHE_PDF_Matrix() ) const;
	HE_BOOL GetCID( HE_WCHAR charCode, HE_DWORD & codeRet ) const;

protected:
	CHE_PDF_Type0_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type0_Font();

	CHE_PDF_DictionaryPtr	mDescdtFontDict;
	CHE_PDF_CMap *			mpCIDMap;
	CHE_PDF_CMap *			mpUnicodeMap;

	friend class CHE_Allocator;
};


class CHE_PDF_Type1_Font : public CHE_PDF_Font
{
public:
	HE_BOOL	GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const;
	HE_RECT GetGraphBox( HE_WCHAR charCode, CHE_PDF_Matrix matrix = CHE_PDF_Matrix() ) const;

protected:
	CHE_PDF_Type1_Font( const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type1_Font();

	CHE_PDF_Encoding *		mpEncoding;
	HE_BYTE					mFirstChar;
	HE_BYTE					mLastChar;
	HE_INT32				mCharWidths[256];

	friend class CHE_Allocator;
};


class CHE_PDF_MMType1_Font : public CHE_PDF_Type1_Font
{
private:
	CHE_PDF_MMType1_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_MMType1_Font();

	friend class CHE_Allocator;
};


class CHE_PDF_TrueType_Font : public CHE_PDF_Type1_Font
{
private:
	CHE_PDF_TrueType_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_TrueType_Font();

	friend class CHE_Allocator;
};


class CHE_PDF_Type3_Font : public CHE_PDF_Type1_Font
{
private:
	CHE_PDF_Type3_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type3_Font();

	friend class CHE_Allocator;
};


#endif
