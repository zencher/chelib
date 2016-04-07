#ifndef _CHE_PDF_NEWFONT_H_
#define _CHE_PDF_NEWFONT_H_

#include "../che_base.h"
#include "che_pdf_cmap.h"
#include "che_pdf_objects.h"
#include "che_pdf_component.h"
#include "che_pdf_contentlist.h"


#include <unordered_map>


enum PDF_FONT_TYPE
{
	FONT_TYPE0		= 0x00,
	FONT_TYPE1		= 0x01,
	FONT_MMTYPE1	= 0x02,
	FONT_TRUETYPE	= 0x03,
	FONT_TYPE3		= 0x04
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
	FONT_ENCODING_BUILDINCMAP	= 0x08,
	FONT_ENCODING_IDENTITY		= 0x09,
	FONT_ENCODING_CUSTOM		= 0x10
};

enum PDF_FONT_LANGUAGE
{
    FONT_LANGUAGE_UNKNOWN               = 0x00,
    FONT_LANGUAGE_CHINESE_SIMPLIFIED    = 0x01,
    FONT_LANGUAGE_CHINESE_TRADITIONAL   = 0x02,
    FONT_LANGUAGE_JAPANESE              = 0x03,
    FONT_LANGUAGE_KOREAN                = 0x04
};


class CHE_PDF_TextItem;


//This Encoding class only handle the default build-in encoding
class CHE_PDF_Encoding : public CHE_Object
{
public:
	CHE_PDF_Encoding( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Encoding();

	PDF_FONT_ENCODING			GetType() const;
    PDF_FONT_ENCODING           GetBaseType() const;
	HE_BOOL						GetUnicode(HE_BYTE charCode, HE_WCHAR & codeRet) const;
    
    static PDF_FONT_LANGUAGE    GetLanguage( const CHE_ByteString & encodingName );

private:
	PDF_FONT_ENCODING			mType;
    PDF_FONT_ENCODING           mBaseType;
	HE_WCHAR*					mpCodeTable;
};


class CHE_PDF_FontDescriptor : public CHE_Object
{
public:
	CHE_PDF_FontDescriptor( const CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator = NULL ); 
	~CHE_PDF_FontDescriptor();

	//HE_BOOL					IsEmbedFont() const;
	CHE_PDF_ReferencePtr	GetEmbeddedStream() const { return mEmbedFont; }

	//Flags
	HE_BOOL					IsFixedPitch() const;
	HE_BOOL					IsSerif() const;
	HE_BOOL					IsSymbolic() const;
	HE_BOOL					IsScript() const;
	HE_BOOL					IsNonsymbolic() const;
	HE_BOOL					IsItalic() const;
	HE_BOOL					IsAllCap() const;
	HE_BOOL					IsSmallCap() const;
	HE_BOOL					IsForceBold() const;

	CHE_Rect				GetFontBBox() const { return mFontBBox; }

	HE_ULONG				GetWMode() const { return mWMode; }
    
    
    HE_FLOAT                GetItalicAngle() const { return mItalicAngle; }
    HE_FLOAT                GetAscent() const { return mAscent; }
    HE_FLOAT                GetDescent() const { return mDescent; }
    HE_FLOAT                GetLeading() const { return mLeading; }
    HE_FLOAT                GetCapHeight() const { return mCapHeight; }
    HE_FLOAT                GetXHeight() const { return mXHeight; }
    HE_FLOAT                GetStemV() const { return mStemV; }
    HE_FLOAT                GetStemH() const { return mStemH; }
private:
	HE_INT32				mFlags;
	HE_FLOAT				mItalicAngle;
	HE_FLOAT				mAscent;
	HE_FLOAT				mDescent;
    HE_FLOAT                mLeading;
	HE_FLOAT				mCapHeight;
	HE_FLOAT				mXHeight;
    HE_FLOAT                mStemV;
    HE_FLOAT                mStemH;
	HE_FLOAT				mMissingWidth;
	HE_BOOL					mEmbedded;
	HE_INT32 				mWMode;
	CHE_PDF_ReferencePtr	mEmbedFont;
	CHE_Rect				mFontBBox;

};


//∏√¿‡ «“ª∏ˆΩ”ø⁄¿‡£¨”Î∆ΩÃ®œ‡πÿ
//”√”⁄ µœ÷ªÒ»°◊÷ÃÂ¬∑æ∂ªÚ’ﬂ◊÷ÃÂŒƒº˛ ˝æ›µƒΩ”ø⁄
//∏˘æ›“ª–©±ÿ“™µƒ–≈œ¢¿¥ªÒ»°∆•≈‰µƒ◊÷ÃÂªÚ’ﬂƒ¨»œ◊÷ÃÂµƒΩ”ø⁄

class IHE_SystemFontMgr : public CHE_Object
{
public:
    static IHE_SystemFontMgr *	Create( CHE_Allocator * pAllocator = NULL );
    static HE_VOID				Destroy( IHE_SystemFontMgr * pSystemFontMgr );
    
    IHE_SystemFontMgr( CHE_Allocator * pAllocator = NULL ) : CHE_Object( pAllocator ) {};
    
    virtual ~IHE_SystemFontMgr() {};
    
    virtual CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName ) = 0;
    
    virtual CHE_ByteString GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent ) = 0;
    
    virtual CHE_ByteString GetFontFilePath( CHE_PDF_FontDescriptor * pFontDes ) = 0;
};

IHE_SystemFontMgr * HE_GetSystemFontMgr( CHE_Allocator * pAllocator = NULL );



class CHE_PDF_Font;

class CHE_PDF_FontPtr : public CHE_PDF_ComponentPtr
{
public:
    CHE_PDF_Font * operator->() const;
};

typedef HE_VOID (*PlatformFontInfoCleanCallBack)(HE_LPVOID);





class CHE_PDF_Font : public CHE_PDF_Component
{
public:
	static CHE_PDF_FontPtr 	Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );

	PDF_FONT_TYPE			GetFontType() const;
	CHE_ByteString			GetBaseFont() const;
	PDF_FONT_ENCODING		GetEncodingType() const;
    PDF_FONT_LANGUAGE       GetLanguage() const;
	CHE_PDF_FontDescriptor*	GetFontDescriptor() const { return mpFontDescriptor; }

	//dictionary
	CHE_PDF_DictionaryPtr	GetFontDict() const;
	CHE_PDF_DictionaryPtr	GetFontDescDict() const;

	HE_ULONG				GetWMode() const;


	virtual HE_BOOL         Decode(HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid) const = 0;
	virtual HE_FLOAT		GetWidth(const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix()) = 0;

	HE_VOID					Lock();
	HE_VOID					UnLock();
    
    HE_ULONG                GetFontDesender();
    HE_ULONG                GetFontAscender();
    
    

    HE_LPBYTE               GetEmbededFont() { return mpEmbeddedFontFile; }
    HE_ULONG                GetEmbededFontSize() { return mFontFileSize; }
    
    HE_LPVOID               GetPlatformFontInfo() { return mPlatformFontInfo; }
    HE_VOID                 SetPlatformFontInfo( HE_LPVOID pInfo ) { mPlatformFontInfo = pInfo; }
    HE_VOID                 SetPlatformFontInfoCleanCallBack( PlatformFontInfoCleanCallBack fun ) { mCleanCallBack = fun; }
	
protected:
	CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	virtual ~CHE_PDF_Font();

	void 					ParseToUnicodeStream( const CHE_PDF_StreamPtr & stm );

	PDF_FONT_TYPE                   mFontType;
	CHE_ByteString                  mBaseFont;

    PDF_FONT_LANGUAGE               mLanguage;
	CHE_PDF_Encoding                mEncoding;
	CHE_PDF_FontDescriptor*         mpFontDescriptor;

	CHE_PDF_DictionaryPtr           mFontDict;
	CHE_PDF_DictionaryPtr           mFontDescDict;

	HE_LPBYTE                       mpEmbeddedFontFile;
	HE_ULONG                        mFontFileSize;

	
    HE_LPVOID                       mPlatformFontInfo;
    PlatformFontInfoCleanCallBack   mCleanCallBack;
	
	CHE_Lock                        mLock;
    
    HE_BOOL                         mbBase14Font;

	std::unordered_map<HE_UINT32, HE_UINT32> mToUnicode;

	friend class CHE_Allocator;
};


class CHE_PDF_Type0_Font : public CHE_PDF_Font
{
public:
    HE_BOOL Decode( HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid ) const;
	HE_FLOAT GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix() );
	HE_BOOL IsCode( HE_ULONG cpt, HE_BYTE byteCount );

protected:
	CHE_PDF_Type0_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type0_Font();

	CHE_PDF_DictionaryPtr	mDescdtFontDict;
	CHE_PDF_CMap *			mpCIDMap;
	CHE_PDF_CMap *			mpUnicodeMap;
	HE_WCHAR *				mCIDTOGID;
	HE_ULONG				mCIDTOGIDLength;
    unordered_map<HE_ULONG, HE_ULONG> mWidths;
    HE_UINT32               mDefaultWidth;

	friend class CHE_Allocator;
};

class CHE_PDF_SimpleFont : public CHE_PDF_Font
{
protected:
	CHE_PDF_SimpleFont(const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = NULL);

	HE_BYTE					mFirstChar;
	HE_BYTE					mLastChar;
	HE_INT32				mCharWidths[256];
	HE_WCHAR				mGlyphId[256];
	CHE_ByteString			mGlyphNames[256];
};


class CHE_PDF_Type1_Font : public CHE_PDF_SimpleFont
{
public:
	virtual HE_BOOL Decode( HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid ) const;
	
	virtual HE_FLOAT GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix() );

protected:
	CHE_PDF_Type1_Font( const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator = NULL );
	CHE_PDF_Type1_Font( int noType1Handle, const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type1_Font();

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


class CHE_PDF_Type3_Font : public CHE_PDF_Font
{
public:
	CHE_PDF_ArrayPtr				GetFontBBox() const;
	CHE_Matrix						GetFontMatrix() const;
	CHE_PDF_DictionaryPtr			GetResDict() const;
	CHE_PDF_ContentObjectList*		GetGlyph(HE_BYTE charCode);
	HE_BOOL							Decode(HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid) const;
	HE_FLOAT						GetWidth(const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix());

private:
	CHE_PDF_Type3_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = NULL );
	~CHE_PDF_Type3_Font();

	CHE_Matrix				mFontMatrix;
	CHE_PDF_ArrayPtr		mFontBBox;
	CHE_PDF_DictionaryPtr	mResDict;
	CHE_PDF_DictionaryPtr	mCharProcsDict;
	
	HE_BYTE														mFirstChar;
	HE_BYTE														mLastChar;
	std::vector<CHE_PDF_ReferencePtr>                           mCharCodeToRef;
	std::unordered_map<HE_ULONG, CHE_PDF_ContentObjectList*>	mGlyphMap;

	friend class CHE_Allocator;
};


HE_BOOL HE_GetType1BaseFontFile( const CHE_ByteString & fontName, HE_LPBYTE & pFileBufRet, HE_ULONG & fileSizeRet );

HE_BOOL HE_GetType1BaseFontFile( CHE_PDF_FontDescriptor & fontDescriptor, HE_LPBYTE & pFileBufRet, HE_ULONG & fileSizeRet );



#endif
