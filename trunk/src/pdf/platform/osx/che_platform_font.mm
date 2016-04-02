//#include "../../../../include/pdf/che_pdf_fontmgr.h"
#include_next "../../../include/pdf/che_pdf_font.h"
#include "../../../../extlib/freetype/include/ft2build.h"
#include "../../../../extlib/freetype/include/freetype/freetype.h"
#include <list>
#import <Foundation/Foundation.h>

class CHE_WindowsFontInfo : public CHE_Object
{
public:
	CHE_WindowsFontInfo( CHE_Allocator * pAllocator = NULL );
    
	CHE_ByteString	mFamilyName;
	CHE_ByteString	mStyle;
	CHE_ByteString	mPostScriptName;
	CHE_ByteString	mFilePath;
	//CHE_Rect        mFontBBox;
    HE_FLOAT        mItalicAngle;
	HE_FLOAT		mAscent;
	HE_FLOAT		mDescent;
    HE_FLOAT        mLeading;
	HE_FLOAT		mCapHeight;
    HE_FLOAT        mXHeight;
    HE_FLOAT        mStemV;
    HE_FLOAT        mStemH;
};

CHE_WindowsFontInfo::CHE_WindowsFontInfo( CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object( pAllocator ), mFamilyName( pAllocator ), mPostScriptName( pAllocator ), mFilePath( pAllocator ),
 mItalicAngle( 0 ), mAscent( 0 ), mDescent( 0 ), mLeading( 0 ), mCapHeight( 0 ), mXHeight( 0 ), mStemV( 0 ), mStemH( 0 )  {}


class CHE_MacOSXFontMgr : public IHE_SystemFontMgr
{
public:
	CHE_MacOSXFontMgr( CHE_Allocator * pAllocator = NULL );
	~CHE_MacOSXFontMgr();

	CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName );

	CHE_ByteString GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent );
    
    CHE_ByteString GetFontFilePath( CHE_PDF_FontDescriptor * pFontDes );
    
private:
	std::list<CHE_WindowsFontInfo>	mFontList;
};


IHE_SystemFontMgr * IHE_SystemFontMgr::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
    if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	IHE_SystemFontMgr * pSystemFontMgr = pAllocator->New<CHE_MacOSXFontMgr>( pAllocator );
	
	return pSystemFontMgr;
}


HE_VOID IHE_SystemFontMgr::Destroy( IHE_SystemFontMgr * pSystemFontMgr )
{
	if ( pSystemFontMgr )
	{
		pSystemFontMgr->GetAllocator()->Delete<IHE_SystemFontMgr>( pSystemFontMgr );
	}
}

CHE_MacOSXFontMgr::CHE_MacOSXFontMgr( CHE_Allocator * pAllocator /*= NULL*/ )
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
        CHE_WindowsFontInfo fontInfo( pAllocator );
        
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

CHE_MacOSXFontMgr::~CHE_MacOSXFontMgr()
{
}


CHE_ByteString CHE_MacOSXFontMgr::GetFontFilePath( const CHE_ByteString & fontName )
{
    NSLog( @"GetFontFilePath for %s", fontName.GetData() );
	HE_BOOL bStyle = FALSE;
	HE_BOOL bBold = FALSE;
	HE_BOOL bItalic = FALSE;
	CHE_ByteString name;
	CHE_ByteString style;
	for ( HE_ULONG i = 0; i < fontName.GetLength(); ++i )
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
    
	CHE_ByteString fontPath;
	std::list<CHE_WindowsFontInfo>::iterator it;
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


CHE_ByteString CHE_MacOSXFontMgr::GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent )
{
	CHE_ByteString fontPath;
	std::list<CHE_WindowsFontInfo>::iterator it;
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

CHE_ByteString CHE_MacOSXFontMgr::GetFontFilePath( CHE_PDF_FontDescriptor * pFontDes )
{
    
    HE_FLOAT italicAngle = 0.0f;
    HE_FLOAT ascent = 0.0f;
    HE_FLOAT descent = 0.0f;
    HE_FLOAT leading = 0.0f;
    HE_FLOAT capHeight = 0.0f;
    HE_FLOAT xHeight = 0.0f;
    HE_FLOAT stemV = 0.0f;
    HE_FLOAT stemH = 0.0f;
    
    
    HE_FLOAT v = 99999999.0f;
    HE_FLOAT t = 0.0f;
    HE_LONG index = -1;
    std::list<CHE_WindowsFontInfo>::iterator target;
    
    std::list<HE_LONG> optional;
    
    
    italicAngle = pFontDes->GetItalicAngle();
    ascent = pFontDes->GetAscent();
    descent = pFontDes->GetDescent();
    leading = pFontDes->GetLeading();
    capHeight = pFontDes->GetCapHeight();
    xHeight = pFontDes->GetXHeight();
    stemV = pFontDes->GetStemV();
    stemH = pFontDes->GetStemH();
    
    CHE_ByteString fontPath;
    std::list<CHE_WindowsFontInfo>::iterator it;
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