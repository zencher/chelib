#include "../../../../include/pdf/che_pdf_fontmgr.h"
#include "../../../../extlib/freetype/ft2build.h"
#include "../../../../extlib/freetype/freetype/freetype.h"
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
	CHE_ByteString	mPostScriptName;
	CHE_ByteString	mFilePath;
};


class CHE_WindowsFontMgr : public IHE_SystemFontMgr
{
public:
	CHE_WindowsFontMgr( CHE_Allocator * pAllocator = NULL );
	~CHE_WindowsFontMgr();

	CHE_ByteString GetFontFilePath( const CHE_ByteString & fontName );

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
	: CHE_Object( pAllocator ), mFamilyName( pAllocator ), mPostScriptName( pAllocator ), mFilePath( pAllocator ) {}


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
					pPostScriptName = FT_Get_Postscript_Name( face );
					if ( pPostScriptName )
					{
						fontInfo.mPostScriptName = pPostScriptName;
					}
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
	CHE_ByteString str;
	CHE_ByteString fontNameForMatch;

	for ( HE_DWORD i = 0; i < fontName.GetLength(); ++i )
	{
		if ( fontName[i] == ',' || fontName[i] == '-' )
		{
			break;
		}
		fontNameForMatch += fontName[i];
	}

	char	enFontName[128];	
	wchar_t	tmpFontName[128];
	int nLen = MultiByteToWideChar( CP_ACP, 0, fontNameForMatch.GetData(), -1, NULL, 0 );
	MultiByteToWideChar( CP_ACP, 0, fontNameForMatch.GetData(), -1, tmpFontName, nLen );

	LANGID language = MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US );
	FontFamily fontFamily( tmpFontName );
	fontFamily.GetFamilyName( tmpFontName, language );

	nLen = WideCharToMultiByte( CP_ACP, 0, tmpFontName, -1, NULL, 0, NULL, FALSE );
	WideCharToMultiByte( CP_ACP, 0, tmpFontName, -1, enFontName, nLen, NULL, FALSE );

	if ( fontNameForMatch != enFontName && strlen( enFontName ) > 0 )
	{
		fontNameForMatch = enFontName;
	}

	std::list<CHE_WindowsFontInfo>::iterator it;
	for ( it = mFontList.begin(); it != mFontList.end(); ++it )
	{
		if ( it->mFamilyName == fontNameForMatch || it->mPostScriptName == fontNameForMatch )
		{
			str = it->mFilePath;
			break;
		}
	}
	return str;
}