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
	CHE_PDF_Encoding( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_Encoding();
    
    static bool IsPredefinedCJKCMapNames(const CHE_ByteString & name);

	PDF_FONT_ENCODING			GetType() const;
    PDF_FONT_ENCODING           GetBaseType() const;
	bool						GetUnicode(BYTE charCode, wchar_t & codeRet) const;
    
    static PDF_FONT_LANGUAGE    GetLanguage( const CHE_ByteString & encodingName );

private:
	PDF_FONT_ENCODING			mType;
    PDF_FONT_ENCODING           mBaseType;
	wchar_t*					mpCodeTable;
};


class CHE_PDF_FontDescriptor : public CHE_Object
{
public:
	CHE_PDF_FontDescriptor( const CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator = nullptr ); 
	~CHE_PDF_FontDescriptor();

	//bool					IsEmbedFont() const;
	CHE_PDF_ReferencePtr	GetEmbeddedStream() const { return mEmbedFont; }

	//Flags
	bool					IsFixedPitch() const;
	bool					IsSerif() const;
	bool					IsSymbolic() const;
	bool					IsScript() const;
	bool					IsNonsymbolic() const;
	bool					IsItalic() const;
	bool					IsAllCap() const;
	bool					IsSmallCap() const;
	bool					IsForceBold() const;

	CHE_Rect				GetFontBBox() const { return mFontBBox; }

	uint32				GetWMode() const { return mWMode; }
    
    
    FLOAT                GetItalicAngle() const { return mItalicAngle; }
    FLOAT                GetAscent() const { return mAscent; }
    FLOAT                GetDescent() const { return mDescent; }
    FLOAT                GetLeading() const { return mLeading; }
    FLOAT                GetCapHeight() const { return mCapHeight; }
    FLOAT                GetXHeight() const { return mXHeight; }
    FLOAT                GetStemV() const { return mStemV; }
    FLOAT                GetStemH() const { return mStemH; }
private:
	int32				mFlags;
	FLOAT				mItalicAngle;
	FLOAT				mAscent;
	FLOAT				mDescent;
    FLOAT                mLeading;
	FLOAT				mCapHeight;
	FLOAT				mXHeight;
    FLOAT                mStemV;
    FLOAT                mStemH;
	FLOAT				mMissingWidth;
	bool					mEmbedded;
	int32 				mWMode;
	CHE_PDF_ReferencePtr	mEmbedFont;
	CHE_Rect				mFontBBox;

};


//∏√¿‡ «“ª∏ˆΩ”ø⁄¿‡£¨”Î∆ΩÃ®œ‡πÿ
//”√”⁄ µœ÷ªÒ»°◊÷ÃÂ¬∑æ∂ªÚ’ﬂ◊÷ÃÂŒƒº˛ ˝æ›µƒΩ”ø⁄
//∏˘æ›“ª–©±ÿ“™µƒ–≈œ¢¿¥ªÒ»°∆•≈‰µƒ◊÷ÃÂªÚ’ﬂƒ¨»œ◊÷ÃÂµƒΩ”ø⁄

class IHE_SystemFontMgr : public CHE_Object
{
public:
    static IHE_SystemFontMgr *	Create( CHE_Allocator * pAllocator = nullptr );
    static void				Destroy( IHE_SystemFontMgr * pSystemFontMgr );
    
    IHE_SystemFontMgr( CHE_Allocator * pAllocator = nullptr ) : CHE_Object( pAllocator ) {};
    
    virtual ~IHE_SystemFontMgr() {};
    
    virtual CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName ) = 0;
    
    virtual CHE_ByteString GetFontFilePath( FLOAT ascent, FLOAT descent ) = 0;
    
    virtual CHE_ByteString GetFontFilePath( CHE_PDF_FontDescriptor * pFontDes ) = 0;
};

IHE_SystemFontMgr * HE_GetSystemFontMgr( CHE_Allocator * pAllocator = nullptr );



class CHE_PDF_Font;

class CHE_PDF_FontPtr : public CHE_PDF_ComponentPtr
{
public:
    CHE_PDF_Font * operator->() const;
};

typedef void (*PlatformFontInfoCleanCallBack)(void *);





class CHE_PDF_Font : public CHE_PDF_Component
{
public:
	static CHE_PDF_FontPtr 	Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = nullptr );

	PDF_FONT_TYPE			GetFontType() const;
	CHE_ByteString			GetBaseFont() const;
	PDF_FONT_ENCODING		GetEncodingType() const;
    PDF_FONT_LANGUAGE       GetLanguage() const;
	CHE_PDF_FontDescriptor*	GetFontDescriptor() const { return mpFontDescriptor; }

	//dictionary
	CHE_PDF_DictionaryPtr	GetFontDict() const;
	CHE_PDF_DictionaryPtr	GetFontDescDict() const;

	uint32				GetWMode() const;


	virtual bool         Decode(wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid) const = 0;
	virtual FLOAT		GetWidth(const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix()) = 0;

	void					Lock();
	void					UnLock();
    
    uint32                GetFontDesender();
    uint32                GetFontAscender();
    
    

    PBYTE               GetEmbededFont() { return mpEmbeddedFontFile; }
    size_t                GetEmbededFontSize() { return mFontFileSize; }
    
    void *               GetPlatformFontInfo() { return mPlatformFontInfo; }
    void                 SetPlatformFontInfo( void * pInfo ) { mPlatformFontInfo = pInfo; }
    void                 SetPlatformFontInfoCleanCallBack( PlatformFontInfoCleanCallBack fun ) { mCleanCallBack = fun; }
	
protected:
	CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = nullptr );
	virtual ~CHE_PDF_Font();

	void 					ParseToUnicodeStream( const CHE_PDF_StreamPtr & stm );

	PDF_FONT_TYPE                   mFontType;
	CHE_ByteString                  mBaseFont;

    PDF_FONT_LANGUAGE               mLanguage;
	CHE_PDF_Encoding                mEncoding;
	CHE_PDF_FontDescriptor*         mpFontDescriptor;

	CHE_PDF_DictionaryPtr           mFontDict;
	CHE_PDF_DictionaryPtr           mFontDescDict;

	PBYTE                       mpEmbeddedFontFile;
	size_t                        mFontFileSize;

	
    void *                       mPlatformFontInfo;
    PlatformFontInfoCleanCallBack   mCleanCallBack;
	
	CHE_Lock                        mLock;
    
    bool                         mbBase14Font;
    
    void *                       mFTFace;

	std::unordered_map<uint32, uint32> mToUnicode;

	friend class CHE_Allocator;
};


class CHE_PDF_Type0_Font : public CHE_PDF_Font
{
public:
    bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
	FLOAT GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix() );
	bool IsCode( uint32 cpt, BYTE byteCount );

protected:
	CHE_PDF_Type0_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_Type0_Font();

	CHE_PDF_DictionaryPtr	mDescdtFontDict;
	CHE_PDF_CMap *			mpCIDMap;
	CHE_PDF_CMap *			mpUnicodeMap;
	wchar_t *				mCIDTOGID;
	size_t				mCIDTOGIDLength;
    unordered_map<size_t, size_t> mWidths;
    uint32               mDefaultWidth;

	friend class CHE_Allocator;
};

class CHE_PDF_SimpleFont : public CHE_PDF_Font
{
protected:
	CHE_PDF_SimpleFont(const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator = nullptr);

	BYTE					mFirstChar;
	BYTE					mLastChar;
	int32				mCharWidths[256];
	wchar_t				mGlyphId[256];
	CHE_ByteString			mGlyphNames[256];
};


class CHE_PDF_Type1_Font : public CHE_PDF_SimpleFont
{
public:
	virtual bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
	
	virtual FLOAT GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix() );

protected:
	CHE_PDF_Type1_Font( const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator = nullptr );
	CHE_PDF_Type1_Font( int noType1Handle, const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_Type1_Font();

	friend class CHE_Allocator;
};


class CHE_PDF_MMType1_Font : public CHE_PDF_Type1_Font
{
public:
//    virtual bool Decode( wchar_t charCode, wchar_t & ucs, size_t & gid, size_t & cid ) const;
    
private:
	CHE_PDF_MMType1_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_MMType1_Font();

	friend class CHE_Allocator;
};


class CHE_PDF_TrueType_Font : public CHE_PDF_Type1_Font
{
public:
    virtual bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
    
private:
	CHE_PDF_TrueType_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_TrueType_Font();

	friend class CHE_Allocator;
};


class CHE_PDF_Type3_Font : public CHE_PDF_Font
{
public:
	CHE_PDF_ArrayPtr				GetFontBBox() const;
	CHE_Matrix						GetFontMatrix() const;
	CHE_PDF_DictionaryPtr			GetResDict() const;
	CHE_PDF_ContentObjectList*		GetGlyph(BYTE charCode);
	bool							Decode(wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid) const;
	FLOAT						GetWidth(const CHE_PDF_TextItem & item, const CHE_Matrix & matrix = CHE_Matrix());

private:
	CHE_PDF_Type3_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator = nullptr );
	~CHE_PDF_Type3_Font();

	CHE_Matrix				mFontMatrix;
	CHE_PDF_ArrayPtr		mFontBBox;
	CHE_PDF_DictionaryPtr	mResDict;
	CHE_PDF_DictionaryPtr	mCharProcsDict;
	
	BYTE														mFirstChar;
	BYTE														mLastChar;
	std::vector<CHE_PDF_ReferencePtr>                           mCharCodeToRef;
    std::unordered_map<uint32, CHE_PDF_ContentObjectList*>      mGlyphMap;

	friend class CHE_Allocator;
};


bool HE_GetType1BaseFontFile( const CHE_ByteString & fontName, PBYTE & pFileBufRet, size_t & fileSizeRet );

bool HE_GetType1BaseFontFile( CHE_PDF_FontDescriptor & fontDescriptor, PBYTE & pFileBufRet, size_t & fileSizeRet );



#endif
