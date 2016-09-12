
#include "../../../include/pdf/pdf_font.h"

#include <list>

#import <Foundation/Foundation.h>
#include "../../../../extlib/freetype/include/ft2build.h"
#include "../../../../extlib/freetype/include/freetype/freetype.h"

namespace chelib {

class CWindowsFontInfo : public CObject
{
public:
	CWindowsFontInfo( CAllocator * pAllocator = NULL );
    
	CByteString	mFamilyName;
	CByteString	mStyle;
	CByteString	mPostScriptName;
	CByteString	mFilePath;
	//CRect        mFontBBox;
    FLOAT        mItalicAngle;
	FLOAT		mAscent;
	FLOAT		mDescent;
    FLOAT        mLeading;
	FLOAT		mCapHeight;
    FLOAT        mXHeight;
    FLOAT        mStemV;
    FLOAT        mStemH;
};

CWindowsFontInfo::CWindowsFontInfo( CAllocator * pAllocator /*= NULL*/ )
: CObject( pAllocator ), mFamilyName( pAllocator ), mPostScriptName( pAllocator ), mFilePath( pAllocator ),
 mItalicAngle( 0 ), mAscent( 0 ), mDescent( 0 ), mLeading( 0 ), mCapHeight( 0 ), mXHeight( 0 ), mStemV( 0 ), mStemH( 0 )  {}


class CMacOSXFontMgr : public IHE_SystemFontMgr
{
public:
	CMacOSXFontMgr( CAllocator * pAllocator = NULL );
	~CMacOSXFontMgr();

	CByteString GetFontFilePath( const CByteString & fontName );

	CByteString GetFontFilePath( FLOAT ascent, FLOAT descent );
    
    CByteString GetFontFilePath( CPDF_FontDescriptor * pFontDes );
    
private:
	std::list<CWindowsFontInfo>	mFontList;
};


IHE_SystemFontMgr * IHE_SystemFontMgr::Create( CAllocator * pAllocator /*= NULL*/ )
{
    if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	IHE_SystemFontMgr * pSystemFontMgr = pAllocator->New<CMacOSXFontMgr>( pAllocator );
	
	return pSystemFontMgr;
}


void IHE_SystemFontMgr::Destroy( IHE_SystemFontMgr * pSystemFontMgr )
{
	if ( pSystemFontMgr )
	{
		pSystemFontMgr->GetAllocator()->Delete<IHE_SystemFontMgr>( pSystemFontMgr );
	}
}

CMacOSXFontMgr::CMacOSXFontMgr( CAllocator * pAllocator /*= NULL*/ )
    : IHE_SystemFontMgr( pAllocator )
{
    FT_Error err;
	FT_Face face;
	FT_Library lib;
    
	const char * pPostScriptName = NULL;
    
    err = FT_Init_FreeType( &lib );
    
    NSString * pathStr = [[NSString alloc] initWithCString:"/Library/Fonts/" encoding:NSASCIIStringEncoding];
    NSArray *contentOfFolder = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:pathStr error:NULL];
    for (NSString *aPath in contentOfFolder) {
        NSString * fullPath = [pathStr stringByAppendingPathComponent:aPath];
        CWindowsFontInfo fontInfo( pAllocator );
        
        //memset( fontFileStr, 0, sizeof(char) * 512 );
        //strcat( fontFileStr, fontDirStr );
        //strcat( fontFileStr, "\\" );
        //strcat( fontFileStr, FindFileData.cFileName );
        
        const char * pathCStr = [fullPath UTF8String];
        
        err = FT_New_Face( lib, pathCStr, 0, &face );
        if ( ! err )
        {
            fontInfo.mFamilyName = face->family_name;
            fontInfo.mStyle = face->style_name;
            pPostScriptName = FT_Get_Postscript_Name( face );
            if ( pPostScriptName )
            {
                fontInfo.mPostScriptName = pPostScriptName;
            }
            
            /*fontInfo.mAscent = face->ascender * 1000.0f / face->units_per_EM;
            fontInfo.mDescent = face->descender * 1000.0f / face->units_per_EM;
            fontInfo.mHeight = face->height * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.left = face->bbox.xMin * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.bottom = face->bbox.yMin * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.width = face->bbox.xMax * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.height = face->bbox.yMax * 1000.0f / face->units_per_EM;*/
            
            fontInfo.mFilePath = pathCStr;
            
            
            {
                FILE * pFile = fopen( pathCStr, "rb" );
                if ( pFile )
                {
                    fseek( pFile, 0, SEEK_END );
                    long posi = ftell( pFile );
                    unsigned char * data = new unsigned char[posi];
                    fseek( pFile, 0, SEEK_SET);
                    fread( data, 1, posi, pFile);
                    
                    CFDataRef dataRef = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, data, posi, kCFAllocatorNull );
                    if ( dataRef )
                    {
                        CGDataProviderRef dataProviderRef = CGDataProviderCreateWithCFData( dataRef );
                        CGFontRef cgfontRef = CGFontCreateWithDataProvider( dataProviderRef );
                        if ( cgfontRef )
                        {
                            
                            fontInfo.mItalicAngle = CGFontGetItalicAngle( cgfontRef );
                            fontInfo.mAscent = CGFontGetAscent( cgfontRef );
                            fontInfo.mDescent = CGFontGetDescent( cgfontRef );
                            fontInfo.mLeading = CGFontGetLeading( cgfontRef );
                            fontInfo.mCapHeight = CGFontGetCapHeight( cgfontRef);
                            fontInfo.mXHeight = CGFontGetXHeight( cgfontRef );
                            fontInfo.mStemV = CGFontGetStemV( cgfontRef );
                            //fontInfo.mStemH = CGFontGetStemH( cgfontRef );
                            

                            
                            
                        }
                        CFRelease(dataProviderRef);
                        CFRelease(dataRef);
                    }
                    fclose(pFile);
                    
                    delete [] data;
                }
            }
            
            
            mFontList.push_back( fontInfo );
            FT_Done_Face( face );
        }
    }
    
    FT_Done_FreeType( lib );
}

CMacOSXFontMgr::~CMacOSXFontMgr()
{
}


CByteString CMacOSXFontMgr::GetFontFilePath( const CByteString & fontName )
{
    NSLog( @"GetFontFilePath for %s", fontName.GetData() );
	bool bStyle = FALSE;
	bool bBold = FALSE;
	bool bItalic = FALSE;
	CByteString name;
	CByteString style;
	for ( size_t i = 0; i < fontName.GetLength(); ++i )
	{
		if ( fontName[i] == '+' )
		{
			if ( bStyle )
			{
				break;
			}else{
				//»•µÙ«∞–ﬁ Œ
				name.Clear();
			}
		}else if ( fontName[i] == ',' )
		{
			bStyle = TRUE;
			continue;
		}else if ( fontName[i] == '-' )
		{
            bStyle = TRUE;
			continue;
		}
        
		if ( bStyle )
		{
			style += fontName[i];
		}else{
			name += fontName[i];
		}
	}
    
	if ( bStyle )
	{
		if ( strstr( (const char *)( style.GetData() ), "Bold" ) != NULL )
		{
			bBold = TRUE;
		}
		if ( strstr( (const char *)( style.GetData() ), "Italic" ) != NULL )
		{
			bItalic = TRUE;
		}
	}
    
	//Ω‚æˆ≤ª”√”Ô—‘œ¬◊÷ÃÂ√˚≥∆≤ªÕ¨µƒŒ Ã‚
	/*char	enFontName[128];
	wchar_t	tmpFontName[128];
	int nLen = MultiByteToWideChar( CP_ACP, 0, name.GetData(), -1, NULL, 0 );
	MultiByteToWideChar( CP_ACP, 0, name.GetData(), -1, tmpFontName, nLen );
	LANGID language = MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US );
	FontFamily fontFamily( tmpFontName );
	fontFamily.GetFamilyName( tmpFontName, language );
	nLen = WideCharToMultiByte( CP_ACP, 0, tmpFontName, -1, NULL, 0, NULL, FALSE );
	WideCharToMultiByte( CP_ACP, 0, tmpFontName, -1, enFontName, nLen, NULL, FALSE );
	if ( name != enFontName && strlen( enFontName ) > 0 )
	{
		name = enFontName;
	}*/
    
	CByteString fontPath;
	std::list<CWindowsFontInfo>::iterator it;
	for ( it = mFontList.begin(); it != mFontList.end(); ++it )
	{
		if ( it->mPostScriptName.GetLength() == 0 )
		{
			//ƒ≥–©◊÷ÃÂ√ª”–PostScriptName
			continue;
		}
        
		//1.PostScript NameÕÍ»´œ‡µ»
		if ( it->mPostScriptName == fontName || it->mPostScriptName == name )
		{
			fontPath = it->mFilePath;
			break;
		}
        
		if ( bStyle )
		{
			if ( strstr( (const char *)( it->mPostScriptName.GetData() ), (const char *)( name.GetData() ) ) != NULL &&
				strstr( (const char *)( it->mPostScriptName.GetData() ), (const char *)( style.GetData() ) ) != NULL  )
			{
				if ( bBold != ( strstr( (const char *)( it->mPostScriptName.GetData() ), "Bold" ) != NULL ) )
				{
					continue;
				}
				if ( bItalic != ( strstr( (const char *)( it->mPostScriptName.GetData() ), "Italic" ) != NULL ) )
				{
				}
				fontPath = it->mFilePath;
				break;
			}
		}
		else{
			if ( strstr( (const char *)( it->mPostScriptName.GetData() ), (const char *)( name.GetData() ) ) != NULL )
			{
				fontPath = it->mFilePath;
				break;
			}
		}
	}
    NSLog( @"return : %s", fontPath.GetData() );

    
	return fontPath;
}


CByteString CMacOSXFontMgr::GetFontFilePath( FLOAT ascent, FLOAT descent )
{
	CByteString fontPath;
	std::list<CWindowsFontInfo>::iterator it;
	for ( it = mFontList.begin(); it != mFontList.end(); ++it )
	{
		if ( ascent - it->mAscent <= 1 && descent - it->mDescent <= 1 )
		{
			fontPath = it->mFilePath;
			break;
		}
	}
	return fontPath;
}

CByteString CMacOSXFontMgr::GetFontFilePath( CPDF_FontDescriptor * pFontDes )
{
    
    FLOAT italicAngle = 0.0f;
    FLOAT ascent = 0.0f;
    FLOAT descent = 0.0f;
    FLOAT leading = 0.0f;
    FLOAT capHeight = 0.0f;
    FLOAT xHeight = 0.0f;
    FLOAT stemV = 0.0f;
    FLOAT stemH = 0.0f;
    
    
    FLOAT v = 99999999.0f;
    FLOAT t = 0.0f;
    size_t index = -1;
    std::list<CWindowsFontInfo>::iterator target;
    
    std::list<size_t> optional;
    
    
    italicAngle = pFontDes->GetItalicAngle();
    ascent = pFontDes->GetAscent();
    descent = pFontDes->GetDescent();
    leading = pFontDes->GetLeading();
    capHeight = pFontDes->GetCapHeight();
    xHeight = pFontDes->GetXHeight();
    stemV = pFontDes->GetStemV();
    stemH = pFontDes->GetStemH();
    
    CByteString fontPath;
    std::list<CWindowsFontInfo>::iterator it;
    for ( it = mFontList.begin(); it != mFontList.end(); ++it )
    {
        index++;
        t = 0.0f;
        t += fabs(it->mAscent - ascent);
        t += fabs(it->mDescent - descent);
        t += fabs(it->mLeading - leading);
        t += fabs(it->mCapHeight - capHeight);
        t += fabs(it->mXHeight - xHeight);
        t += fabs(it->mStemV - stemV);
        
        if (t < v) {
            v = t;
            target = it;
            optional.push_back( index );
        }
    }
    fontPath = target->mFilePath;
    NSLog( @"return : %s", fontPath.GetData() );
    return fontPath;
}

}//namespace