// EditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "EditDlg.h"
#include "afxdialogex.h"
#include <cassert>

CEditDlg * gpDlg = NULL;

static void EventOkBtn( CHE_WDM_Area * pArea )
{
	gpDlg->mpCurItem->type = gpDlg->mItemType;

	int curFilePageCount = theApp.mFileCache[ gpDlg->mpCurItem->index ].mpPageTree->GetPageCount();

	switch ( gpDlg->mItemType )
	{
	case ALL_PAGES:
		gpDlg->mpCurItem->pageIndex = 1;
		gpDlg->mpCurItem->pageCount = curFilePageCount;
		break;
	case SINGLE_PAGE:
		gpDlg->mpCurItem->pageIndex = gpDlg->mCurPage;
		gpDlg->mpCurItem->pageCount = 1;
		break;
	case PAGE_RANGE:
		gpDlg->mpCurItem->pageIndex = gpDlg->mPageBegin;
		gpDlg->mpCurItem->pageCount = gpDlg->mPageEnd - gpDlg->mPageBegin + 1;
		break;
	case ODD_PAGES:
		gpDlg->mpCurItem->pageIndex = 1;
		gpDlg->mpCurItem->pageCount = curFilePageCount / 2 + curFilePageCount % 2;
		break;
	case EVEN_PAGES:
		gpDlg->mpCurItem->pageIndex = 1;
		gpDlg->mpCurItem->pageCount = curFilePageCount /2;
		break;
	default:;
	}
	gpDlg->EndDialog( 0 );
}

static void EventCancelBtn( CHE_WDM_Area * pArea )
{
	if ( gpDlg )
	{
		gpDlg->EndDialog( 1 );
	}
}

static void UpdataNumberBox()
{
	CHE_WDM_AppearTextPtr textPtr = gpDlg->mpNumberBox1->GetAppear().mBackground[1].GetTextPtr();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mNumber1 );
	textPtr->SetText( tmpStr );
	gpDlg->mpNumberBox1->Refresh();

	textPtr = gpDlg->mpNumberBox2->GetAppear().mBackground[1].GetTextPtr();
	wsprintf( tmpStr, L"%d", gpDlg->mNumber2 );
	textPtr->SetText( tmpStr );
	gpDlg->mpNumberBox2->Refresh();
}

static void UpdateScroll1()
{
	gpDlg->mpSingleDragBox->SetPosiX( 50 + ( gpDlg->mNumber1 - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 501 );
	gpDlg->mpSinglePageScrollBar->Refresh();
}

static void UpdateScroll2_Begin()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	gpDlg->mpPageRangeBeginDragBox->SetPosiX( 50 + ( gpDlg->mNumber1 - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 469 );

	BeginPosi = gpDlg->mpPageRangeBeginDragBox->GetPosiX() + 15;
	EndPosi = gpDlg->mpPageRangeEndDragBox->GetPosiX() + 15;

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 208, EndPosi-45, 208 );
	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 208, 549, 208 );

	gpDlg->mpPageRangeScrollFill->SetPosiX( BeginPosi );
	gpDlg->mpPageRangeScrollFill->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpPageRangeArea->Refresh();
}

static void UpdateScroll2_End()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	gpDlg->mpPageRangeEndDragBox->SetPosiX( 80 + ( gpDlg->mNumber2 - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 469 );

	BeginPosi = gpDlg->mpPageRangeBeginDragBox->GetPosiX() + 15;
	EndPosi = gpDlg->mpPageRangeEndDragBox->GetPosiX() + 15;

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 208, EndPosi-45, 208 );
	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 208, 549, 208 );

	gpDlg->mpPageRangeScrollFill->SetPosiX( BeginPosi );
	gpDlg->mpPageRangeScrollFill->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpPageRangeArea->Refresh();
}

static void EventUpBtnClick1( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mNumber1 >= gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mNumber1++;
	gpDlg->mCurPage = gpDlg->mNumber1;
	UpdataNumberBox();
	UpdateScroll1();
	UpdateScroll2_Begin();
}

static void EventDownBtnClick1( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mNumber1 == 1 )
	{
		return;
	}
	gpDlg->mNumber1--;
	gpDlg->mCurPage = gpDlg->mNumber1;
	UpdataNumberBox();
	UpdateScroll1();
	UpdateScroll2_Begin();
}

static void EventUpBtnClick2( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mNumber2 >= gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mNumber2++;
	gpDlg->mPageEnd = gpDlg->mNumber2;
	UpdataNumberBox();
	UpdateScroll2_End();
}

static void EventDownBtnClick2( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mNumber2 == gpDlg->mNumber1  + 1 )
	{
		return;
	}
	gpDlg->mNumber2--;
	gpDlg->mPageEnd = gpDlg->mNumber2;
	UpdataNumberBox();
	UpdateScroll2_End();
}

static void EventDragScroolBlock_SinglePage( CHE_WDM_Area * pArea )
{
	unsigned int tmpPosi = pArea->GetPosiX();
	gpDlg->mNumber1 = (unsigned int)( 1.5 + ( tmpPosi - 50 ) * 1.0 / ( 499 * 1.0 / (gpDlg->mPageCount - 1) ) );
	gpDlg->mCurPage = gpDlg->mNumber1;
	gpDlg->mpSinglePageArea->Refresh();
	UpdataNumberBox();
}

static void EventDragScroolBlock_PageRange_Begin( CHE_WDM_Area * pArea )
{
	int BeginPosi = gpDlg->mpPageRangeBeginDragBox->GetPosiX() + 15;
	gpDlg->mpPageRangeScrollFill->SetPosiX( BeginPosi );
	int EndPosi = gpDlg->mpPageRangeEndDragBox->GetPosiX() + 15;
	gpDlg->mpPageRangeScrollFill->SetWidth( EndPosi - BeginPosi );

	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 208, 549, 208 );
	unsigned int tmpPosi = pArea->GetPosiX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 50 ) * 1.0 / ( 469 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( tmpPageIndex < gpDlg->mNumber2 )
	{
		gpDlg->mNumber1 = tmpPageIndex;
		gpDlg->mPageBegin = gpDlg->mNumber1;
	}else if ( tmpPageIndex >= gpDlg->mNumber2 )
	{
		gpDlg->mNumber1 = gpDlg->mNumber2-1;
		gpDlg->mPageBegin = gpDlg->mNumber1;
	}
	gpDlg->mpPageRangeArea->Refresh();
	UpdataNumberBox();
}

static void EventDragScroolBlock_PageRange_End( CHE_WDM_Area * pArea )
{
	int BeginPosi = gpDlg->mpPageRangeBeginDragBox->GetPosiX() + 15;
	gpDlg->mpPageRangeScrollFill->SetPosiX( BeginPosi );
	int EndPosi = pArea->GetPosiX() + 15;
	gpDlg->mpPageRangeScrollFill->SetWidth( EndPosi - BeginPosi );

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 208, EndPosi-45, 208 );
	unsigned int tmpPosi = pArea->GetPosiX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 80 ) * 1.0 / ( 469 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( ( tmpPageIndex <= gpDlg->mPageCount ) && tmpPageIndex > gpDlg->mNumber1 )
	{
		gpDlg->mNumber2 = tmpPageIndex;
		gpDlg->mPageEnd = gpDlg->mNumber2;
	}else if ( tmpPageIndex > gpDlg->mPageCount )
	{
		gpDlg->mNumber2 = gpDlg->mPageCount;
		gpDlg->mPageEnd = gpDlg->mNumber2;
	}else if ( tmpPageIndex <= gpDlg->mNumber1 )
	{
		gpDlg->mNumber2 = gpDlg->mNumber1+1;
		gpDlg->mPageEnd = gpDlg->mNumber2;
	}

	gpDlg->mpPageRangeArea->Refresh();
	UpdataNumberBox();
}

static void EventToolBarBtnClick( CHE_WDM_Area * pArea )
{
	if ( pArea == gpDlg->mpToolBtn1 )
	{
		gpDlg->mItemType = ALL_PAGES;
		gpDlg->ShowAllPagesWeight();
	}else if ( pArea == gpDlg->mpToolBtn2 )
	{
		gpDlg->mItemType = SINGLE_PAGE;
		gpDlg->mNumber1 = gpDlg->mCurPage;
		gpDlg->ShowSinglePageWeight();
		UpdataNumberBox();
		UpdateScroll1();
	}else if ( pArea == gpDlg->mpToolBtn3 )
	{
		gpDlg->mItemType = PAGE_RANGE;
		gpDlg->mNumber1 = gpDlg->mPageBegin;
		gpDlg->mNumber2 = gpDlg->mPageEnd;
		gpDlg->ShowPageRangeWeight();
		UpdataNumberBox();
		UpdateScroll2_Begin();
		UpdateScroll2_End();
	}else if ( pArea == gpDlg->mpToolBtn4 )
	{
		gpDlg->mItemType = ODD_PAGES;
		gpDlg->ShowOddPageWeight();
	}else if ( pArea == gpDlg->mpToolBtn5 )
	{
		gpDlg->mItemType = EVEN_PAGES;
		gpDlg->ShowEvenPagesWeight();
	}
}

// CEditDlg 对话框

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent)
{
	gpDlg = this;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mMainTipText = CHE_WDM_AppearText::Create();
	mMainTipText->SetPosiX( 130 );
	mMainTipText->SetPosiY( 145 );
	mMainTipText->SetWidth( 500 );
	mMainTipText->SetHeight( 20 );
	mMainTipText->SetSize( 12 );
	mMainTipText->SetColor( 0xFF000000 );
	mMainTipText->SetLayout(  CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	mpMainArea->AppendAppearItem( mMainTipText, AREA_APPEAR_BACKGROUND );

	mpCurItem = &( theApp.mList[ theApp.mCurItem - 1 ] );
	assert( mpCurItem );

	mItemType = mpCurItem->type;
	mPageCount = mpCurItem->filePageCount;
	if ( mItemType == EVEN_PAGES || mItemType == ODD_PAGES )
	{
		mNumber1 = 1;
		mNumber2 = mPageCount;
	}else{
		mNumber1 = mpCurItem->pageIndex;
		mNumber2 = mpCurItem->pageIndex + mpCurItem->pageCount - 1;
	}
	if ( mNumber1 == mNumber2 )
	{
		mNumber2 += 1;
	}
	mPageBegin = mNumber1;
	mPageEnd = mNumber2;
	mCurPage = mNumber1;

	mpFileInfo = CHE_WDM_Area::Create( mpInterActive );
	mpFileInfo->SetPosiX( 0 );
	mpFileInfo->SetPosiY( 0 );
	mpFileInfo->SetWidth( 630 );
	mpFileInfo->SetHeight( 70 );
	mpFileInfo->EnableClip();
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 15 );
	imagePtr->SetPosiY( 15 );
	imagePtr->SetImageFile( L"images\\FileIcon.png" );
	mpFileInfo->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 550 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 55 );
	textPtr->SetPosiY( 15 );
	textPtr->SetText( mpCurItem->fileName.c_str() );
	textPtr->SetSize( 14 );
	mpFileInfo->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	wchar_t tmpwstr[512];
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 160 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 55 );
	textPtr->SetPosiY( 40 );
	textPtr->SetSize( 12 );
	wsprintf( tmpwstr, L"Total：%d Page(s)", mPageCount );
	textPtr->SetText( tmpwstr );
	mpFileInfo->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 240 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 200 );
	textPtr->SetPosiY( 40 );
	textPtr->SetSize( 12 );
	if ( mpCurItem->bytes <= 10485 )
	{
		swprintf( tmpwstr, L"%4.2f KB", mpCurItem->bytes * 1.0 / 1024 ) ;
	}else{
		swprintf( tmpwstr, L"%4.2f MB", mpCurItem->bytes * 1.0 / ( 1024 * 1024 ) ) ;
	}
	textPtr->SetText( tmpwstr );
	mpFileInfo->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	mpMainArea->AppendChild( mpFileInfo );

	mpToolBar = CHE_WDM_Area::Create( mpInterActive );
	mpToolBar->SetPosiX( 0 );
	mpToolBar->SetPosiY( 70 );
	mpToolBar->SetWidth( 630 );
	mpToolBar->SetHeight( 39 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolbarBG.png" );
	mpToolBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mToolBarBtnBG = CHE_WDM_AppearImage::Create();
	mToolBarBtnBG->SetImageFile( L"images\\toolbarBtn.png" );

	mToolBarBtnHover = CHE_WDM_AppearImage::Create();
	mToolBarBtnHover->SetImageFile( L"images\\toolbarBtnHover.png" );

	mToolBarBtnActive = CHE_WDM_AppearImage::Create();
	mToolBarBtnActive->SetImageFile( L"images\\toolbarBtnActived.png" );

	mpToolBtn5 = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtn5->SetPosiX( 390 );
	mpToolBtn5->SetPosiY( 70 );
	mpToolBtn5->SetWidth( 90 );
	mpToolBtn5->SetHeight( 39 );
	mpToolBtn5->SetMouseLBDEvent( EventToolBarBtnClick );
	mEvenPagesText = CHE_WDM_AppearText::Create();
	mEvenPagesText->SetWidth( 90 );
	mEvenPagesText->SetHeight( 39 );
	mEvenPagesText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mEvenPagesText->SetColor( 0xFF000000 );
	mEvenPagesText->SetSize( 12 );
	mEvenPagesText->SetText( L"Even Pages" );
	mpToolBtn5->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn5->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_MOUSEOVER );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_DISABLE );
	mpToolBar->AppendChild( mpToolBtn5 );

	mpToolBtn4 = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtn4->SetPosiX( 300 );
	mpToolBtn4->SetPosiY( 70 );
	mpToolBtn4->SetWidth( 90 );
	mpToolBtn4->SetHeight( 39 );
	mpToolBtn4->SetMouseLBDEvent( EventToolBarBtnClick );
	mOddPagesText = CHE_WDM_AppearText::Create();
	mOddPagesText->SetWidth( 90 );
	mOddPagesText->SetHeight( 39 );
	mOddPagesText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mOddPagesText->SetColor( 0xFF000000 );
	mOddPagesText->SetSize( 12 );
	mOddPagesText->SetText( L"Odd Pages" );
	mpToolBtn4->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn4->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_MOUSEOVER );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_DISABLE );
	mpToolBar->AppendChild( mpToolBtn4 );

	mpToolBtn3 = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtn3->SetPosiX( 210 );
	mpToolBtn3->SetPosiY( 70 );
	mpToolBtn3->SetWidth( 90 );
	mpToolBtn3->SetHeight( 39 );
	mpToolBtn3->SetMouseLBDEvent( EventToolBarBtnClick );
	mPageRangeText = CHE_WDM_AppearText::Create();
	mPageRangeText->SetWidth( 90 );
	mPageRangeText->SetHeight( 39 );
	mPageRangeText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mPageRangeText->SetColor( 0xFF000000 );
	mPageRangeText->SetSize( 12 );
	mPageRangeText->SetText( L"Page Range" );
	mpToolBtn3->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_NORMAL );
	mpToolBtn3->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_MOUSEOVER );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_DISABLE );
	mpToolBar->AppendChild( mpToolBtn3 );

	mpToolBtn2 = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtn2->SetPosiX( 120 );
	mpToolBtn2->SetPosiY( 70 );
	mpToolBtn2->SetWidth( 90 );
	mpToolBtn2->SetHeight( 39 );
	mpToolBtn2->SetMouseLBDEvent( EventToolBarBtnClick );
	mSinglePageText = CHE_WDM_AppearText::Create();
	mSinglePageText->SetWidth( 90 );
	mSinglePageText->SetHeight( 39 );
	mSinglePageText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mSinglePageText->SetColor( 0xFF000000 );
	mSinglePageText->SetSize( 12 );
	mSinglePageText->SetText( L"Single Page" );
	mpToolBtn2->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_NORMAL );
	mpToolBtn2->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_MOUSEOVER );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_DISABLE );
	mpToolBar->AppendChild( mpToolBtn2 );

	mpToolBtn1 = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtn1->SetPosiX( 30 );
	mpToolBtn1->SetPosiY( 70 );
	mpToolBtn1->SetWidth( 90 );
	mpToolBtn1->SetHeight( 39 );
	mpToolBtn1->SetMouseLBDEvent( EventToolBarBtnClick );
	mAllPagesText = CHE_WDM_AppearText::Create();
	mAllPagesText->SetWidth( 90 );
	mAllPagesText->SetHeight( 39 );
	mAllPagesText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mAllPagesText->SetColor( 0xFF000000 );
	mAllPagesText->SetSize( 12 );
	mAllPagesText->SetText( L"All Pages" );
	mpToolBtn1->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn1->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_MOUSEOVER );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_DISABLE );
	mpToolBar->AppendChild( mpToolBtn1 );

	mpMainArea->AppendChild( mpToolBar );

	if ( mPageCount == 1 )
	{
		mpToolBtn3->SetEnable( FALSE );
		mpToolBtn4->SetEnable( FALSE );
		mpToolBtn5->SetEnable( FALSE );
	}
		
	//ok btn
	CHE_WDM_Button * pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 410 );
	pBtn->SetPosiY( 280 );
	pBtn->SetWidth( 88 );
	pBtn->SetHeight( 29 );
	pBtn->SetMouseLBUEvent( EventOkBtn );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OkBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OkBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpMainArea->AppendChild( pBtn );

	//cancel btn
	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 520 );
	pBtn->SetPosiY( 280 );
	pBtn->SetWidth( 88 );
	pBtn->SetHeight( 29 );
	pBtn->SetMouseLBUEvent( EventCancelBtn );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpMainArea->AppendChild( pBtn );


	textPtr = CHE_WDM_AppearText::Create();
	mpNumberBox1 = CHE_WDM_Area::Create( mpInterActive );
	mpNumberBox1->SetPosiX( 200 );
	mpNumberBox1->SetPosiY( 143 );
	mpNumberBox1->SetWidth( 71 );
	mpNumberBox1->SetHeight( 26 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBox.png" );
	mpNumberBox1->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 46 );
	textPtr->SetHeight( 25 );
	textPtr->SetSize( 12 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_RIGHT_OR_BOTTOM, LAYOUT_ALIGN_CENTER ) );
	mpNumberBox1->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	mpMainArea->AppendChild( mpNumberBox1 );

	CHE_WDM_Button * pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 253 );
	pTmpBtn->SetPosiY( 143 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 11 );
	pTmpBtn->SetMouseLBUEvent( EventUpBtnClick1 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox1->AppendChild( pTmpBtn );
	pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 253 );
	pTmpBtn->SetPosiY( 155 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 12 );
	pTmpBtn->SetMouseLBUEvent( EventDownBtnClick1 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox1->AppendChild( pTmpBtn );

	textPtr = CHE_WDM_AppearText::Create();
	mpNumberBox2 = CHE_WDM_Area::Create( mpInterActive );
	mpNumberBox2->SetPosiX( 380 );
	mpNumberBox2->SetPosiY( 143 );
	mpNumberBox2->SetWidth( 71 );
	mpNumberBox2->SetHeight( 26 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBox.png" );
	mpNumberBox2->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 46 );
	textPtr->SetHeight( 25 );
	textPtr->SetSize( 12 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_RIGHT_OR_BOTTOM, LAYOUT_ALIGN_CENTER ) );
	mpNumberBox2->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	mpMainArea->AppendChild( mpNumberBox2 );

	pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 433 );
	pTmpBtn->SetPosiY( 143 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 11 );
	pTmpBtn->SetMouseLBUEvent( EventUpBtnClick2 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox2->AppendChild( pTmpBtn );
	pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 433 );
	pTmpBtn->SetPosiY( 155 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 12 );
	pTmpBtn->SetMouseLBUEvent( EventDownBtnClick2 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox2->AppendChild( pTmpBtn );

	//Single page
	{
		mpSinglePageArea = CHE_WDM_Area::Create( mpInterActive );
		mpSinglePageArea->SetPosiX( 0 );
		mpSinglePageArea->SetPosiY( 205 );
		mpSinglePageArea->SetWidth( 630 );
		mpSinglePageArea->SetHeight( 25 );

		mpSinglePageScrollBar = CHE_WDM_Area::Create( mpInterActive );
		mpSinglePageScrollBar->SetWidth( 540 );
		mpSinglePageScrollBar->SetHeight( 20 );
		mpSinglePageScrollBar->SetPosiX( 50 );
		mpSinglePageScrollBar->SetPosiY( 210 );

		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\Scroll.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpSinglePageScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

		mpSinglePageArea->AppendChild( mpSinglePageScrollBar );

		CHE_WDM_DragArea * pTmpDragArea = CHE_WDM_DragArea::Create( mpInterActive );
		pTmpDragArea->SetWidth( 30 );
		pTmpDragArea->SetHeight( 14 );
		pTmpDragArea->SetPosiX( 50 );
		pTmpDragArea->SetPosiY( 208 );
		pTmpDragArea->SetRange( 50, 208, 549, 208 );
		pTmpDragArea->SetDragEvent( EventDragScroolBlock_SinglePage );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		pTmpDragArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpSinglePageArea->AppendChild( pTmpDragArea );
		mpSingleDragBox = pTmpDragArea;

		mpMainArea->AppendChild( mpSinglePageArea );
	}

	//page range
	{
		mpPageRangeArea = CHE_WDM_Area::Create( mpInterActive );
		mpPageRangeArea->SetPosiX( 0 );
		mpPageRangeArea->SetPosiY( 205 );
		mpPageRangeArea->SetWidth( 630 );
		mpPageRangeArea->SetHeight( 25 );

		mpPageRangeScrollBar = CHE_WDM_Area::Create( mpInterActive );
		mpPageRangeScrollBar->SetWidth( 540 );
		mpPageRangeScrollBar->SetHeight( 20 );
		mpPageRangeScrollBar->SetPosiX( 50 );
		mpPageRangeScrollBar->SetPosiY( 210 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\Scroll.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpSinglePageScrollBar );

		mpPageRangeScrollFill = CHE_WDM_Area::Create( mpInterActive );
		mpPageRangeScrollFill->SetWidth( 540 );
		mpPageRangeScrollFill->SetHeight( 20 );
		mpPageRangeScrollFill->SetPosiX( 50 );
		mpPageRangeScrollFill->SetPosiY( 210 );
		mpPageRangeScrollFill->EnableClip();
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\Scrollf.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeScrollFill->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpPageRangeScrollFill );

		mpPageRangeBeginDragBox = CHE_WDM_DragArea::Create( mpInterActive );
		mpPageRangeBeginDragBox->SetWidth( 30 );
		mpPageRangeBeginDragBox->SetHeight( 14 );
		mpPageRangeBeginDragBox->SetPosiX( 50 );
		mpPageRangeBeginDragBox->SetPosiY( 208 );
		mpPageRangeBeginDragBox->SetRange( 50, 208, 521, 208 );
		mpPageRangeBeginDragBox->SetDragEvent( EventDragScroolBlock_PageRange_Begin );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeBeginDragBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpPageRangeBeginDragBox );

		mpPageRangeEndDragBox = CHE_WDM_DragArea::Create( mpInterActive );
		mpPageRangeEndDragBox->SetWidth( 30 );
		mpPageRangeEndDragBox->SetHeight( 14 );
		mpPageRangeEndDragBox->SetPosiX( 50 );
		mpPageRangeEndDragBox->SetPosiY( 208 );
		mpPageRangeEndDragBox->SetRange( 80, 208, 551, 208 );
		mpPageRangeEndDragBox->SetDragEvent( EventDragScroolBlock_PageRange_End );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeEndDragBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpPageRangeEndDragBox );

		mpMainArea->AppendChild( mpPageRangeArea );
	}
}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CEditDlg 消息处理程序
void CEditDlg::DrawMainArea() 
{
	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	if ( mpInterActive->IsReflesh() )
	{
		mpMainArea->OnDraw();
	}else if ( mpInterActive->IsDirtyRect() )
	{
		mpMainArea->OnDraw( mpInterActive->GetLeft(), mpInterActive->GetTop(), mpInterActive->GetRight(), mpInterActive->GetBottom() );
	}
	mpInterActive->Reset();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CEditDlg::OnPaint()
{
	DrawMainArea();
	CPaintDC dc(this);
}


BOOL CEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer( 0, 10, NULL );

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();

	UpdateScroll1();
	UpdateScroll2_Begin();
	UpdateScroll2_End();
	UpdataNumberBox();

	switch ( mpCurItem->type )
	{
	case ALL_PAGES:
		ShowAllPagesWeight();
		break;
	case SINGLE_PAGE:
		ShowSinglePageWeight();
		break;
	case PAGE_RANGE:
		ShowPageRangeWeight();
		break;
	case EVEN_PAGES:
		ShowEvenPagesWeight();
		break;
	case ODD_PAGES:
		ShowOddPageWeight();
		break;
	default:
		break;
	}

	//mpModeBtn->Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CEditDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void CEditDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize( nType, 630, 350 );

	if ( mClientWidth != 630 )
	{

		this->MoveWindow( 0, 0, 630, 350 );

		mClientWidth = 630;
		mClientHeight = 350;

		mpMainArea->SetWidth( mClientWidth );
		mpMainArea->SetHeight( mClientHeight );

		CWnd * pWnd = GetDlgItem( IDC_MAIN );
		if ( pWnd )
		{
			pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
		}
	}
}


void CEditDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CEditDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CEditDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CEditDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mAnimationMgr.Execute();
	CDialogEx::OnTimer(nIDEvent);
}

void CEditDlg::HideAllWeight()
{
	mpNumberBox1->SetVisable( false );
	mpNumberBox1->SetEnable( false );

	mpNumberBox2->SetVisable( false );
	mpNumberBox2->SetEnable( false );

	mpSinglePageArea->SetVisable( false );
	mpSinglePageArea->SetEnable( false );

	mpPageRangeArea->SetVisable( false );
	mpPageRangeArea->SetEnable( false );

	mpToolBtn1->OnMouseOut();
	mpToolBtn1->GetAppear().mBackground.clear();
	mpToolBtn1->GetAppear().mNormal.clear();
	mpToolBtn1->GetAppear().mMouseOver.clear();
	mpToolBtn1->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn1->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_MOUSEOVER );

	mpToolBtn2->OnMouseOut();
	mpToolBtn2->GetAppear().mBackground.clear();
	mpToolBtn2->GetAppear().mNormal.clear();
	mpToolBtn2->GetAppear().mMouseOver.clear();
	mpToolBtn2->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_NORMAL );
	mpToolBtn2->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_MOUSEOVER );

	mpToolBtn3->OnMouseOut();
	mpToolBtn3->GetAppear().mBackground.clear();
	mpToolBtn3->GetAppear().mNormal.clear();
	mpToolBtn3->GetAppear().mMouseOver.clear();
	mpToolBtn3->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_NORMAL );
	mpToolBtn3->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_MOUSEOVER );

	mpToolBtn4->OnMouseOut();
	mpToolBtn4->GetAppear().mBackground.clear();
	mpToolBtn4->GetAppear().mNormal.clear();
	mpToolBtn4->GetAppear().mMouseOver.clear();
	mpToolBtn4->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn4->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_MOUSEOVER );

	mpToolBtn5->OnMouseOut();
	mpToolBtn5->GetAppear().mBackground.clear();
	mpToolBtn5->GetAppear().mNormal.clear();
	mpToolBtn5->GetAppear().mMouseOver.clear();
	mpToolBtn5->AppendAppearItem( mToolBarBtnBG, AREA_APPEAR_NORMAL );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_NORMAL );
	mpToolBtn5->AppendAppearItem( mToolBarBtnHover, AREA_APPEAR_MOUSEOVER );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_MOUSEOVER );
}

void CEditDlg::ShowSinglePageWeight()
{
	HideAllWeight();
	mpNumberBox1->SetVisable( true );
	mpNumberBox1->SetEnable( true );
	mpSinglePageArea->SetVisable( true );
	mpSinglePageArea->SetEnable( true );

	mpToolBtn2->GetAppear().mBackground.clear();
	mpToolBtn2->GetAppear().mNormal.clear();
	mpToolBtn2->GetAppear().mMouseOver.clear();
	mpToolBtn2->AppendAppearItem( mToolBarBtnActive, AREA_APPEAR_NORMAL );
	mpToolBtn2->AppendAppearItem( mSinglePageText, AREA_APPEAR_NORMAL );

	mMainTipText->SetText( L"       Page" );

	mpMainArea->Refresh();
}

void CEditDlg::ShowPageRangeWeight()
{
	HideAllWeight();
	mpNumberBox1->SetVisable( true );
	mpNumberBox1->SetEnable( true );
	mpNumberBox2->SetVisable( true );
	mpNumberBox2->SetEnable( true );
	mpPageRangeArea->SetVisable( true );
	mpPageRangeArea->SetEnable( true );

	mpToolBtn3->GetAppear().mBackground.clear();
	mpToolBtn3->GetAppear().mNormal.clear();
	mpToolBtn3->GetAppear().mMouseOver.clear();
	mpToolBtn3->AppendAppearItem( mToolBarBtnActive, AREA_APPEAR_BACKGROUND );
	mpToolBtn3->AppendAppearItem( mPageRangeText, AREA_APPEAR_BACKGROUND );

	mMainTipText->SetText( L"From Page                                         To Page" );

	mpMainArea->Refresh();
}

void CEditDlg::ShowAllPagesWeight()
{
	HideAllWeight();

	mpToolBtn1->GetAppear().mBackground.clear();
	mpToolBtn1->GetAppear().mNormal.clear();
	mpToolBtn1->GetAppear().mMouseOver.clear();
	mpToolBtn1->AppendAppearItem( mToolBarBtnActive, AREA_APPEAR_BACKGROUND );
	mpToolBtn1->AppendAppearItem( mAllPagesText, AREA_APPEAR_BACKGROUND );

	mMainTipText->SetText( L"Out put all pages of file." );

	mpMainArea->Refresh();
}

void CEditDlg::ShowEvenPagesWeight()
{
	HideAllWeight();

	mpToolBtn5->GetAppear().mBackground.clear();
	mpToolBtn5->GetAppear().mNormal.clear();
	mpToolBtn5->GetAppear().mMouseOver.clear();
	mpToolBtn5->AppendAppearItem( mToolBarBtnActive, AREA_APPEAR_BACKGROUND );
	mpToolBtn5->AppendAppearItem( mEvenPagesText, AREA_APPEAR_BACKGROUND );

	mMainTipText->SetText( L"Out put all even pages of file." );

	mpMainArea->Refresh();
}

void CEditDlg::ShowOddPageWeight()
{
	HideAllWeight();

	mpToolBtn4->GetAppear().mBackground.clear();
	mpToolBtn4->GetAppear().mNormal.clear();
	mpToolBtn4->GetAppear().mMouseOver.clear();
	mpToolBtn4->AppendAppearItem( mToolBarBtnActive, AREA_APPEAR_BACKGROUND );
	mpToolBtn4->AppendAppearItem( mOddPagesText, AREA_APPEAR_BACKGROUND );

	mMainTipText->SetText( L"Out put all odd pages of file." );

	mpMainArea->Refresh();
}
