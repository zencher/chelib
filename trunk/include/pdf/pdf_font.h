#ifndef _CHELIB_PDF_NEWFONT_H_
#define _CHELIB_PDF_NEWFONT_H_

#include "pdf_cmap.h"
#include "pdf_component.h"
#include "pdf_contentlist.h"

#include <unordered_map>

namespace chelib {

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


class CPDF_TextItem;


//This Encoding class only handle the default build-in encoding
class CPDF_Encoding : public CObject
{
public:
    static bool IsPredefinedCJKCMapNames( const CByteString & name );
    
    static PDF_FONT_LANGUAGE GetLanguage( const CByteString & encodingName );
    
    CPDF_Encoding( const CPDF_DictionaryPtr & fontDict, CAllocator * pAllocator = nullptr );
	~CPDF_Encoding();

	PDF_FONT_ENCODING GetType() const;
    PDF_FONT_ENCODING GetBaseType() const;
	bool GetUnicode( BYTE charCode, wchar_t & codeRet ) const;

private:
	PDF_FONT_ENCODING mType;
    PDF_FONT_ENCODING mBaseType;
	wchar_t * mpCodeTable;
};


class CPDF_FontDescriptor : public CObject
{
public:
	CPDF_FontDescriptor( const CPDF_DictionaryPtr & fontDesDict, CAllocator * pAllocator = nullptr ); 
	~CPDF_FontDescriptor();

	//bool IsEmbedFont() const;
	CPDF_ReferencePtr GetEmbeddedStream() const { return mEmbedFont; }

	//Flags
	bool IsFixedPitch() const;
	bool IsSerif() const;
	bool IsSymbolic() const;
	bool IsScript() const;
	bool IsNonsymbolic() const;
	bool IsItalic() const;
	bool IsAllCap() const;
	bool IsSmallCap() const;
	bool IsForceBold() const;

	CRect GetFontBBox() const { return mFontBBox; }
	uint32 GetWMode() const { return mWMode; }
    FLOAT GetItalicAngle() const { return mItalicAngle; }
    FLOAT GetAscent() const { return mAscent; }
    FLOAT GetDescent() const { return mDescent; }
    FLOAT GetLeading() const { return mLeading; }
    FLOAT GetCapHeight() const { return mCapHeight; }
    FLOAT GetXHeight() const { return mXHeight; }
    FLOAT GetStemV() const { return mStemV; }
    FLOAT GetStemH() const { return mStemH; }
private:
	int32 mFlags;
	FLOAT mItalicAngle;
	FLOAT mAscent;
	FLOAT mDescent;
    FLOAT mLeading;
	FLOAT mCapHeight;
	FLOAT mXHeight;
    FLOAT mStemV;
    FLOAT mStemH;
	FLOAT mMissingWidth;
	bool mEmbedded;
	int32 mWMode;
	CPDF_ReferencePtr mEmbedFont;
	CRect mFontBBox;
};


//∏√¿‡ «“ª∏ˆΩ”ø⁄¿‡£¨”Î∆ΩÃ®œ‡πÿ
//”√”⁄ µœ÷ªÒ»°◊÷ÃÂ¬∑æ∂ªÚ’ﬂ◊÷ÃÂŒƒº˛ ˝æ›µƒΩ”ø⁄
//∏˘æ›“ª–©±ÿ“™µƒ–≈œ¢¿¥ªÒ»°∆•≈‰µƒ◊÷ÃÂªÚ’ﬂƒ¨»œ◊÷ÃÂµƒΩ”ø⁄

class IHE_SystemFontMgr : public CObject
{
public:
    static IHE_SystemFontMgr *	Create( CAllocator * pAllocator = nullptr );
    static void				Destroy( IHE_SystemFontMgr * pSystemFontMgr );
    
    IHE_SystemFontMgr( CAllocator * pAllocator = nullptr ) : CObject( pAllocator ) {};
    
    virtual ~IHE_SystemFontMgr() {};
    
    virtual CByteString GetFontFilePath( const CByteString & fontName ) = 0;
    
    virtual CByteString GetFontFilePath( FLOAT ascent, FLOAT descent ) = 0;
    
    virtual CByteString GetFontFilePath( CPDF_FontDescriptor * pFontDes ) = 0;
};

IHE_SystemFontMgr * HE_GetSystemFontMgr( CAllocator * pAllocator = nullptr );



class CPDF_Font;

class CPDF_FontPtr : public CPDF_ComponentPtr
{
public:
    CPDF_Font * operator->() const;
};

typedef void (*PlatformFontInfoCleanCallBack)(void *);


class CPDF_Font : public CPDF_Component
{
public:
	static CPDF_FontPtr Create( const CPDF_DictionaryPtr & fontDict, CAllocator * pAllocator = nullptr );

	PDF_FONT_TYPE GetFontType() const;
	CByteString GetBaseFont() const;
	PDF_FONT_ENCODING GetEncodingType() const;
    PDF_FONT_LANGUAGE GetLanguage() const;
	CPDF_FontDescriptor * GetFontDescriptor() const { return mpFontDescriptor; }

	//dictionary
	CPDF_DictionaryPtr GetFontDict() const;
	CPDF_DictionaryPtr GetFontDescDict() const;

	uint32 GetWMode() const;


	virtual bool Decode(wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid) const = 0;
	virtual FLOAT GetWidth(const CPDF_TextItem & item, const CMatrix & matrix = CMatrix()) = 0;

	void Lock();
	void UnLock();
    
    uint32 GetFontDesender();
    uint32 GetFontAscender();
    
    

    PBYTE GetEmbededFont() { return mpEmbeddedFontFile; }
    size_t GetEmbededFontSize() { return mFontFileSize; }
    
    void * GetPlatformFontInfo() { return mPlatformFontInfo; }
    void SetPlatformFontInfo( void * pInfo ) { mPlatformFontInfo = pInfo; }
    void SetPlatformFontInfoCleanCallBack( PlatformFontInfoCleanCallBack fun ) { mCleanCallBack = fun; }
	
protected:
	CPDF_Font( const CPDF_DictionaryPtr & fontDict, CAllocator * pAllocator = nullptr );
	virtual ~CPDF_Font();

	void ParseToUnicodeStream( const CPDF_StreamPtr & stm );

	PDF_FONT_TYPE mFontType;
	CByteString mBaseFont;

    PDF_FONT_LANGUAGE mLanguage;
	CPDF_Encoding mEncoding;
	CPDF_FontDescriptor * mpFontDescriptor;

	CPDF_DictionaryPtr mFontDict;
	CPDF_DictionaryPtr mFontDescDict;

	PBYTE mpEmbeddedFontFile;
	size_t mFontFileSize;

	
    void * mPlatformFontInfo;
    PlatformFontInfoCleanCallBack mCleanCallBack;
	
	CLock mLock;
    
    bool mbBase14Font;
    
    void * mFTFace;

	std::unordered_map<uint32, uint32> mToUnicode;

	friend class CAllocator;
};


class CPDF_Type0_Font : public CPDF_Font
{
public:
    bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
	FLOAT GetWidth( const CPDF_TextItem & item, const CMatrix & matrix = CMatrix() );
	bool IsCode( uint32 cpt, BYTE byteCount );

protected:
	CPDF_Type0_Font( const CPDF_DictionaryPtr & fontDict, CAllocator * pAllocator = nullptr );
	~CPDF_Type0_Font();

	CPDF_DictionaryPtr	mDescdtFontDict;
	CPDF_CMap *			mpCIDMap;
	CPDF_CMap *			mpUnicodeMap;
	wchar_t *				mCIDTOGID;
	size_t				mCIDTOGIDLength;
    std::unordered_map<size_t, size_t> mWidths;
    uint32               mDefaultWidth;

	friend class CAllocator;
};

class CPDF_SimpleFont : public CPDF_Font
{
protected:
	CPDF_SimpleFont(const CPDF_DictionaryPtr & fontDict, CAllocator * pAllocator = nullptr);

	BYTE					mFirstChar;
	BYTE					mLastChar;
	int32				mCharWidths[256];
	wchar_t				mGlyphId[256];
	CByteString			mGlyphNames[256];
};


class CPDF_Type1_Font : public CPDF_SimpleFont
{
public:
	virtual bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
	
	virtual FLOAT GetWidth( const CPDF_TextItem & item, const CMatrix & matrix = CMatrix() );

protected:
	CPDF_Type1_Font( const CPDF_DictionaryPtr & pFontDcit, CAllocator * pAllocator = nullptr );
	CPDF_Type1_Font( int noType1Handle, const CPDF_DictionaryPtr & pFontDcit, CAllocator * pAllocator = nullptr );
	~CPDF_Type1_Font();

	friend class CAllocator;
};


class CPDF_MMType1_Font : public CPDF_Type1_Font
{
public:
//    virtual bool Decode( wchar_t charCode, wchar_t & ucs, size_t & gid, size_t & cid ) const;
    
private:
	CPDF_MMType1_Font( const CPDF_DictionaryPtr & pFontDict, CAllocator * pAllocator = nullptr );
	~CPDF_MMType1_Font();

	friend class CAllocator;
};


class CPDF_TrueType_Font : public CPDF_Type1_Font
{
public:
    virtual bool Decode( wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid ) const;
    
private:
	CPDF_TrueType_Font( const CPDF_DictionaryPtr & pFontDict, CAllocator * pAllocator = nullptr );
	~CPDF_TrueType_Font();

	friend class CAllocator;
};


class CPDF_Type3_Font : public CPDF_Font
{
public:
	CPDF_ArrayPtr				GetFontBBox() const;
	CMatrix						GetFontMatrix() const;
	CPDF_DictionaryPtr			GetResDict() const;
	CPDF_ContentObjectList*		GetGlyph(BYTE charCode);
	bool							Decode(wchar_t charCode, wchar_t & ucs, uint32 & gid, uint32 & cid) const;
	FLOAT						GetWidth(const CPDF_TextItem & item, const CMatrix & matrix = CMatrix());

private:
	CPDF_Type3_Font( const CPDF_DictionaryPtr & pFontDict, CAllocator * pAllocator = nullptr );
	~CPDF_Type3_Font();

	CMatrix				mFontMatrix;
	CPDF_ArrayPtr		mFontBBox;
	CPDF_DictionaryPtr	mResDict;
	CPDF_DictionaryPtr	mCharProcsDict;
	
	BYTE														mFirstChar;
	BYTE														mLastChar;
	std::vector<CPDF_ReferencePtr>                           mCharCodeToRef;
    std::unordered_map<uint32, CPDF_ContentObjectList*>      mGlyphMap;

	friend class CAllocator;
};


bool HE_GetType1BaseFontFile( const CByteString & fontName, PBYTE & pFileBufRet, size_t & fileSizeRet );

bool HE_GetType1BaseFontFile( CPDF_FontDescriptor & fontDescriptor, PBYTE & pFileBufRet, size_t & fileSizeRet );

}//namespace

#endif
