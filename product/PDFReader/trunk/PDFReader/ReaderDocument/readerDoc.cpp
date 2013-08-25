#include "readerDoc.h"

#include <Windows.h>

DWORD WINAPI parseContentThread( PVOID pParam )
{
	CReaderDocument * pReaderDoc = (CReaderDocument*)(pParam);
	if ( pReaderDoc == NULL )
	{
		return 0;
	}

	HE_BOOL bRet = FALSE;
	HE_ULONG pageCount = pReaderDoc->GetPageCount();
	for ( HE_ULONG i = 0; i < pageCount; ++i )
	{
		bRet = pReaderDoc->IsPageContentParse( i );
		if ( ! bRet )
		{
			pReaderDoc->ParseContentParse( i );
		}
	}

	return 0;
}

DWORD WINAPI renderPageThread( PVOID pParam )
{
	return 0;
}






CReaderDocument::CReaderDocument( CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mpRead(NULL), mpFile(NULL), mpDocument(NULL),
	mpPageTree(NULL), mpFontMgr(NULL),mCompatibleDC(NULL), mpThumbnailDrawer(NULL),
	mpPageDrawer(NULL), mpHugeDrawer(NULL)
{
}

CReaderDocument::~CReaderDocument()
{
	CHE_Bitmap * pTmpBitmap = NULL;
	for ( HE_ULONG i = 0; i < mBitmaps.size(); ++i )
	{
		pTmpBitmap = mBitmaps[i];
		if ( pTmpBitmap )
		{
			pTmpBitmap->GetAllocator()->Delete( pTmpBitmap );
		}
	}

	CHE_PDF_ContentObjectList * pContents = NULL;
	for ( HE_ULONG i = 0; i < mContents.size(); ++i )
	{
		pContents = mContents[i];
		if ( pContents )
		{
			pContents->GetAllocator()->Delete( pContents );
		}
	}

	if ( mpFontMgr )
	{
		mpFontMgr->GetAllocator()->Delete( mpFontMgr );
	}

	if ( mpHugeDrawer )
	{
		GetAllocator()->Delete( mpHugeDrawer );
	}
	if ( mpPageDrawer )
	{
		GetAllocator()->Delete( mpPageDrawer );
	}
	if ( mpThumbnailDrawer )
	{
		GetAllocator()->Delete( mpThumbnailDrawer );
	}

	if ( mpDocument )
	{
		mpDocument->GetAllocator()->Delete( mpDocument );
	}
	if ( mpFile )
	{
		mpFile->GetAllocator()->Delete( mpFile );
	}
	if ( mpRead )
	{
		HE_DestoryIHERead( mpRead );
	}
}

HE_BOOL	CReaderDocument::OpenFile( char * fileName )
{
	if ( fileName == NULL )
	{
		return FALSE;
	}

	mpRead = HE_CreateFileRead( fileName, FILEREAD_MODE_DEFAULT, 0, GetAllocator() );
	if ( !mpRead )
	{
		return FALSE;
	}

	mpFile = GetAllocator()->New<CHE_PDF_File>( GetAllocator() );
	if ( !mpFile )
	{
		return FALSE;
	}

	if ( ! mpFile->Open( mpRead ) )
	{
		HE_DestoryIHERead( mpRead );
		mpRead = NULL;
		GetAllocator()->Delete( mpFile );
		mpFile = NULL;
		return FALSE;
	}

	mpDocument = CHE_PDF_Document::CreateDocument( mpFile );
	if ( ! mpDocument  )
	{
		return FALSE;
	}
	
	mpPageTree = mpDocument->GetPageTree();

	CHE_PDF_Page * pPage = NULL;
	mPageRects.resize( mpPageTree->GetPageCount() );
	mAllPageRect.bottom = 0;
	mAllPageRect.left = 0;
	mAllPageRect.width = 0;
	mAllPageRect.height = 0;
	for ( HE_ULONG i = 0; i < mpPageTree->GetPageCount(); ++i )
	{
		pPage = mpPageTree->GetPage( i );
		mPageRects[i] = pPage->GetPageRect();
		mPageRects[i].bottom = mAllPageRect.height;
		if ( mPageRects[i].width > mAllPageRect.width )
		{
			mAllPageRect.width = mPageRects[i].width;
		}
		mAllPageRect.height += mPageRects[i].height;
	}

	mpFontMgr = GetAllocator()->New<CHE_PDF_FontMgr>( GetAllocator() );

	//设置页面内容列表为空，标志信息为未解析
	mContents.resize( mpPageTree->GetPageCount(), NULL );
	mContentsFlag.resize( mpPageTree->GetPageCount(), FALSE );

	mBitmaps.resize( mpPageTree->GetPageCount() );
	for ( HE_ULONG i = 0; i < mpPageTree->GetPageCount(); ++i )
	{
		mBitmaps[i] = NULL;
	}
	
	return TRUE;
}

HE_ULONG CReaderDocument::GetPageCount()
{
	if ( mpPageTree )
	{
		return mpPageTree->GetPageCount();
	}
	return 0;
}

CHE_Rect CReaderDocument::GetAllPageRect()
{
	return mAllPageRect;
}

HE_ULONG CReaderDocument::GetPageIndex( HE_ULONG y )
{
	HE_ULONG iRet = 0;
	HE_ULONG iPosi = 0;
	for ( ; iRet < mPageRects.size(); ++iRet )
	{
		if ( y >= iPosi && y <= iPosi + mPageRects[iRet].height )
		{
			return iRet;
		}
		iPosi += mPageRects[iRet].height;
	}

	return iRet;
}

CHE_Bitmap * CReaderDocument::GetBitmap( HE_ULONG index )
{
	if ( index > mBitmaps.size() )
	{
		return NULL;
	}
	return mBitmaps[index];
}

CHE_Rect CReaderDocument::GetPageRect( HE_ULONG index )
{
	if ( index <= mPageRects.size() )
	{
		return mPageRects[index];
	}
	return CHE_Rect();
}

HE_BOOL CReaderDocument::RenderPage( HE_ULONG index )
{
	if ( index >= mpPageTree->GetPageCount() )
	{
		return FALSE;
	}
	if ( ! IsPageContentParse( index ) )
	{
		ParseContentParse( index );
	}

	CHE_PDF_ContentObjectList * pList = NULL;
	pList = mContents[index];
	if ( pList )
	{
		CHE_PDF_Page * pPage = mpPageTree->GetPage( index );
		if ( pPage )
		{
			CHE_Rect rect = pPage->GetPageRect();
			CHE_GraphicsDrawer * pDrawer = GetPageDrawer();
			CHE_PDF_Renderer::Render( *pList, *pDrawer, rect, 0, 1 /*, 96, 96, &clipRect*/ );
			CHE_Bitmap * pBitmap = GetAllocator()->New<CHE_Bitmap>( GetAllocator() );
			if ( pDrawer->GetBitmap( *pBitmap ) )
			{
				mBitmaps[index] = pBitmap;
			}	
			pPage->GetAllocator()->Delete( pPage );
			pPage = NULL;
			return TRUE;
		}
	}

	return TRUE;
}

HE_BOOL	CReaderDocument::SetCompatibleDC( HDC hdc )
{
	if ( hdc == NULL )
	{
		return FALSE;
	}
	mCompatibleDC = hdc;
	return TRUE;
}

CHE_GraphicsDrawer * CReaderDocument::GetPageDrawer()
{
	if ( mCompatibleDC == NULL )
	{
		return NULL;
	}
	if ( mpPageDrawer == NULL )
	{
		mpPageDrawer = GetAllocator()->New<CHE_GraphicsDrawer>( mCompatibleDC, 1, 1 );
	}
	return mpPageDrawer;
}

CHE_GraphicsDrawer * CReaderDocument::GetThumbnailDrawer()
{
	if ( mCompatibleDC == NULL )
	{
		return NULL;
	}
	if ( mpThumbnailDrawer == NULL )
	{
		mpThumbnailDrawer = GetAllocator()->New<CHE_GraphicsDrawer>( mCompatibleDC, 1, 1 );
	}
	return mpThumbnailDrawer;
}

CHE_GraphicsDrawer * CReaderDocument::GetHugeDrawer()
{
	if ( mCompatibleDC == NULL )
	{
		return NULL;
	}
	if ( mpHugeDrawer == NULL )
	{
		mpHugeDrawer = GetAllocator()->New<CHE_GraphicsDrawer>( mCompatibleDC, 1, 1 );
	}
	return mpHugeDrawer;
}

void CReaderDocument::StartParsePageContentThread()
{
	DWORD threadId = 0;
	CreateThread( NULL, 0, parseContentThread, (LPVOID)this, 0, &threadId );
}

void CReaderDocument::StartThumbnailRenderThread()
{

}

void CReaderDocument::StartPageRenderThread()
{

}

HE_BOOL	CReaderDocument::IsPageContentParse( HE_ULONG pageIndex )
{
	if ( pageIndex >= mContents.size() )
	{
		return FALSE;
	}
	if ( pageIndex >= mContentsFlag.size() )
	{
		return FALSE;
	}
	if ( mContentsFlag[pageIndex] == FALSE )
	{
		return FALSE;
	}
	if ( mContents[pageIndex] == NULL )
	{
		return FALSE;
	}
	return TRUE;
}

HE_BOOL CReaderDocument::ParseContentParse( HE_ULONG pageIndex )
{
	if ( pageIndex >= mContents.size() )
	{
		return FALSE;
	}
	if ( pageIndex >= mContentsFlag.size() )
	{
		return FALSE;
	}

	CHE_PDF_Page * pPage = mpPageTree->GetPage( pageIndex );
	if ( pPage == NULL )
	{
		return FALSE;
	}

	CHE_PDF_ContentObjectList *  pContentObjList = NULL;
	pContentObjList = GetAllocator()->New<CHE_PDF_ContentObjectList>( CHE_PDF_DictionaryPtr(), GetAllocator() );
	if ( pContentObjList == NULL )
	{
		return FALSE;
	}

	HE_BOOL bRet = GetPageContent( pPage->GetPageDict(), pContentObjList, mpFontMgr );
	if ( bRet )
	{
		mContents[pageIndex] = pContentObjList;
		mContentsFlag[pageIndex] = TRUE;
		pPage->GetAllocator()->Delete( pPage );
		return TRUE;
	}else{
		GetAllocator()->Delete( pContentObjList );
	}
	pPage->GetAllocator()->Delete( pPage );

	return bRet;
}