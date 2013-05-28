#include "../../../../include/pdf/che_pdf_fontmgr.h"
#include "../../../../extlib/freetype/include/ft2build.h"
#include "../../../../extlib/freetype/include/freetype/freetype.h"
#include <list>
#include <ShlObj.h>
#include <Windows.h>
#include <Gdiplus.h>

using namespace Gdiplus;

ULONG_PTR				gdiplusToken;
GdiplusStartupInput		gdiplusStartupInput;

class CHE_WindowsFontInfo : public CHE_Object
{
public:
	CHE_WindowsFontInfo( CHE_Allocator * pAllocator = NULL );

	CHE_ByteString	mFamilyName;
	CHE_ByteString	mStyle;
	CHE_ByteString	mPostScriptName;
	CHE_ByteString	mFilePath;
	CHE_Rect	mFontBBox;
	HE_FLOAT		mAscent;
	HE_FLOAT		mDescent;
	HE_FLOAT		mHeight;
};


class CHE_WindowsFontMgr : public IHE_SystemFontMgr
{
public:
	CHE_WindowsFontMgr( CHE_Allocator * pAllocator = NULL );
	~CHE_WindowsFontMgr();

	CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName );

	CHE_ByteString GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent );

private:
	std::list<CHE_WindowsFontInfo>	mFontList;
};


IHE_SystemFontMgr * IHE_SystemFontMgr::Create( CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( gdiplusToken == NULL )
	{
		GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL );
	}

	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	IHE_SystemFontMgr * pSystemFontMgr = pAllocator->New<CHE_WindowsFontMgr>( pAllocator );
	
	return pSystemFontMgr;
}


HE_VOID IHE_SystemFontMgr::Destroy( IHE_SystemFontMgr * pSystemFontMgr )
{
// 	if ( gdiplusToken )
// 	{
// 		GdiplusShutdown( gdiplusToken );
// 		gdiplusToken = NULL;
// 		gdiplusStartupInput = NULL;
// 	}
	if ( pSystemFontMgr )
	{
		pSystemFontMgr->GetAllocator()->Delete<IHE_SystemFontMgr>( pSystemFontMgr );
	}
}


CHE_WindowsFontInfo::CHE_WindowsFontInfo( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mFamilyName( pAllocator ), mPostScriptName( pAllocator ), mFilePath( pAllocator ),
	mAscent( 0 ), mDescent( 0 ), mHeight( 0 ) {}


CHE_WindowsFontMgr::CHE_WindowsFontMgr( CHE_Allocator * pAllocator /*= NULL*/ )
	: IHE_SystemFontMgr( pAllocator )
{
	//load all system fonts to get information

	FT_Error err;
	FT_Face face;
	FT_Library lib;
	
	char fontFileStr[512];
	char fontDirStr[512];
	char searchStr[512];
	const char * pPostScriptName = NULL;

	err = FT_Init_FreeType( &lib );

	memset( fontDirStr, 0, sizeof(char) * 512 );
	memset( searchStr, 0, sizeof(char) * 512 );
	
	SHGetSpecialFolderPathA( NULL, fontDirStr, CSIDL_FONTS, 0 );
	
	strcat( searchStr, fontDirStr );
	strcat( searchStr, "\\*.*" );

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA( searchStr, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE ) 
	{
		while( FindNextFileA( hFind, &FindFileData ) )
		{
			if ( FindFileData.nFileSizeLow != 0 || FindFileData.nFileSizeHigh != 0 )
			{
				CHE_WindowsFontInfo fontInfo( pAllocator );

				memset( fontFileStr, 0, sizeof(char) * 512 );
				strcat( fontFileStr, fontDirStr );
				strcat( fontFileStr, "\\" );
				strcat( fontFileStr, FindFileData.cFileName );

				err = FT_New_Face( lib, fontFileStr, 0, &face );
				if ( ! err )
				{
					fontInfo.mFamilyName = face->family_name;
					fontInfo.mStyle = face->style_name;
					pPostScriptName = FT_Get_Postscript_Name( face );
					if ( pPostScriptName )
					{
						fontInfo.mPostScriptName = pPostScriptName;
					}
					fontInfo.mAscent = face->ascender * 1000.0 / face->units_per_EM;
					fontInfo.mDescent = face->descender * 1000.0 / face->units_per_EM;
					fontInfo.mHeight = face->height * 1000.0 / face->units_per_EM;
					fontInfo.mFontBBox.left = face->bbox.xMin * 1000.0 / face->units_per_EM;
					fontInfo.mFontBBox.bottom = face->bbox.yMin * 1000.0 / face->units_per_EM;
					fontInfo.mFontBBox.width = face->bbox.xMax * 1000.0 / face->units_per_EM;
					fontInfo.mFontBBox.height = face->bbox.yMax * 1000.0 / face->units_per_EM;
					fontInfo.mFilePath = fontFileStr;
					mFontList.push_back( fontInfo );

					FT_Done_Face( face );
				}
			}
		}
		FindClose( hFind );
	}

	FT_Done_FreeType( lib );
}


CHE_WindowsFontMgr::~CHE_WindowsFontMgr()
{
	mFontList.clear();
}


CHE_ByteString CHE_WindowsFontMgr::GetFontFilePath( const CHE_ByteString & fontName )
{
	HE_BOOL bStyle = FALSE;
	HE_BOOL bBold = FALSE;
	HE_BOOL bItalic = FALSE;
	CHE_ByteString name;
	CHE_ByteString style;
	for ( HE_DWORD i = 0; i < fontName.GetLength(); ++i )
	{
		if ( fontName[i] == '+' )
		{
			if ( bStyle )
			{
				break;
			}else{
				//去掉前修饰
				name.Clear();
			}
		}else if ( fontName[i] == ',' )
		{
			bStyle = TRUE;
			continue;
		}else if ( fontName[i] == '-' )
		{
			break;
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

	//解决不用语言下字体名称不同的问题
	char	enFontName[128];	
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
	}

	CHE_ByteString fontPath;
	std::list<CHE_WindowsFontInfo>::iterator it;
	for ( it = mFontList.begin(); it != mFontList.end(); ++it )
	{
		if ( it->mPostScriptName.GetLength() == 0 )
		{
			//某些字体没有PostScriptName
			continue;
		}

		//1.PostScript Name完全相等
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
	return fontPath;
}


CHE_ByteString CHE_WindowsFontMgr::GetFontFilePath( HE_FLOAT ascent, HE_FLOAT descent )
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