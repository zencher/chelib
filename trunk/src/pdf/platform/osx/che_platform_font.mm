#include "../../../../include/pdf/che_pdf_fontmgr.h"
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
	CHE_Rect        mFontBBox;
	HE_FLOAT		mAscent;
	HE_FLOAT		mDescent;
	HE_FLOAT		mHeight;
};

CHE_WindowsFontInfo::CHE_WindowsFontInfo( CHE_Allocator * pAllocator /*= NULL*/ )
: CHE_Object( pAllocator ), mFamilyName( pAllocator ), mPostScriptName( pAllocator ), mFilePath( pAllocator ),
mAscent( 0 ), mDescent( 0 ), mHeight( 0 ) {}


class CHE_MacOSXFontMgr : public IHE_SystemFontMgr
{
public:
	CHE_MacOSXFontMgr( CHE_Allocator * pAllocator = NULL );
	~CHE_MacOSXFontMgr();

	CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName );

	CHE_ByteString GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent );
    
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
            fontInfo.mAscent = face->ascender * 1000.0f / face->units_per_EM;
            fontInfo.mDescent = face->descender * 1000.0f / face->units_per_EM;
            fontInfo.mHeight = face->height * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.left = face->bbox.xMin * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.bottom = face->bbox.yMin * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.width = face->bbox.xMax * 1000.0f / face->units_per_EM;
            fontInfo.mFontBBox.height = face->bbox.yMax * 1000.0f / face->units_per_EM;
            fontInfo.mFilePath = pathCStr;
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