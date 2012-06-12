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
	case EVEN_PAGES:
		gpDlg->mpCurItem->pageIndex = 1;
		gpDlg->mpCurItem->pageCount = curFilePageCount / 2 + curFilePageCount % 2;
		break;
	case ODD_PAGES:
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

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 158, EndPosi-45, 158 );
	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 158, 549, 158 );

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

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 158, EndPosi-45, 158 );
	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 158, 549, 158 );

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
	gpDlg->mNumber1 = (unsigned int)( 1 + ( tmpPosi - 50 ) * 1.0 / ( 499 * 1.0 / (gpDlg->mPageCount - 1) ) );
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

	gpDlg->mpPageRangeEndDragBox->SetRange( BeginPosi+15, 158, 549, 158 );
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

	gpDlg->mpPageRangeBeginDragBox->SetRange( 50, 158, EndPosi-45, 158 );
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

static void EventSModeBtnOver()
{
	gpDlg->mpSMode1->SetEnable( true );

	gpDlg->mpSMode2->SetEnable( true );

	if ( gpDlg->mPageCount > 1 )
	{
		gpDlg->mpSMode3->SetEnable( true );

		gpDlg->mpSMode4->SetEnable( true );

		gpDlg->mpSMode5->SetEnable( true );
	}
}

static void EventSModeClick( CHE_WDM_Area * pArea )
{
	gpDlg->mTipText->SetText( L"" );

	if ( gpDlg->mbBG2 == false )
	{
		gpDlg->mpSMode1->SetEnable( false );
		gpDlg->mpSMode1->SetVisable( true );
		gpDlg->mpSMode1->SetPosiX( 100 );

		gpDlg->mpSMode2->SetEnable( false );
		gpDlg->mpSMode2->SetVisable( true );
		gpDlg->mpSMode2->SetPosiX( 190 );

		gpDlg->mpSMode3->SetEnable( false );
		gpDlg->mpSMode3->SetVisable( true );
		gpDlg->mpSMode3->SetPosiX( 280 );

		gpDlg->mpSMode4->SetEnable( false );
		gpDlg->mpSMode4->SetVisable( true );
		gpDlg->mpSMode4->SetPosiX( 370 );

		gpDlg->mpSMode5->SetEnable( false );
		gpDlg->mpSMode5->SetVisable( true );
		gpDlg->mpSMode5->SetPosiX( 460 );

		gpDlg->mpBG2->SetVisable( true );
		gpDlg->mpBG3->SetVisable( true );
		gpDlg->mpModeBtn->SetVisable( false );
		gpDlg->mpModeBtn->SetEnable( false );
		gpDlg->mpBtnArea->SetEnable( true );
		gpDlg->mpBG3->SetEnable( true );
		gpDlg->mpBG2->SetEnable( true );
		gpDlg->mbBG2 = true;
		CHE_WDM_AreaAnimation animation;
		CHE_WDM_AnimationData data;
		data.mPosiX = 0;
		data.mPosiY = 140;
		animation.SetTarget( gpDlg->mpBtnArea );
		animation.SetState( data );
		animation.SetOverFunction( EventSModeBtnOver );
		data.mPosiY = 70;
		animation.InsertFrames( 20, data );
		gpDlg->mAnimationMgr.StartAreaAnimation( animation );
		gpDlg->mpBG2->Refresh();
	}
}

static void EventModeBtnOver()
{
	gpDlg->mpSMode1->SetEnable( false );
	gpDlg->mpSMode1->SetVisable( false );
	gpDlg->mpSMode1->SetPosiX( 100 );

	gpDlg->mpSMode2->SetEnable( false );
	gpDlg->mpSMode2->SetVisable( false );
	gpDlg->mpSMode2->SetPosiX( 190 );

	gpDlg->mpSMode3->SetEnable( false );
	gpDlg->mpSMode3->SetVisable( false );
	gpDlg->mpSMode3->SetPosiX( 280 );

	gpDlg->mpSMode4->SetEnable( false );
	gpDlg->mpSMode4->SetVisable( false );
	gpDlg->mpSMode4->SetPosiX( 370 );

	gpDlg->mpSMode5->SetEnable( false );
	gpDlg->mpSMode5->SetVisable( false );
	gpDlg->mpSMode5->SetPosiX( 460 );

	gpDlg->mpBG2->SetVisable( false );
	gpDlg->mpBG3->SetVisable( false );
	gpDlg->mpModeBtn->SetVisable( true );
	gpDlg->mpModeBtn->SetEnable( true );
	gpDlg->mpBtnArea->SetEnable( false );
	gpDlg->mpBG3->SetEnable( false );
	gpDlg->mpBG2->SetEnable( false );
	gpDlg->mpBG2->Refresh();
}

static void EventModeBtnClick( CHE_WDM_Area * pArea )
{
	gpDlg->mTipText->SetText( L"" );
	gpDlg->mpBG2->Refresh();

	gpDlg->mpSMode1->SetEnable( false );
	gpDlg->mpSMode1->SetVisable( false );
	gpDlg->mpSMode1->SetPosiX( 100 );

	gpDlg->mpSMode2->SetEnable( false );
	gpDlg->mpSMode2->SetVisable( false );
	gpDlg->mpSMode2->SetPosiX( 190 );

	gpDlg->mpSMode3->SetEnable( false );
	gpDlg->mpSMode3->SetVisable( false );
	gpDlg->mpSMode3->SetPosiX( 280 );

	gpDlg->mpSMode4->SetEnable( false );
	gpDlg->mpSMode4->SetVisable( false );
	gpDlg->mpSMode4->SetPosiX( 370 );

	gpDlg->mpSMode5->SetEnable( false );
	gpDlg->mpSMode5->SetVisable( false );
	gpDlg->mpSMode5->SetPosiX( 460 );

	pArea->SetEnable( true );
	pArea->SetVisable( true );

	gpDlg->mbBG2 = false;
	CHE_WDM_AreaAnimation animation;
	CHE_WDM_AnimationData data;
	data.mPosiX = pArea->GetPosiX();
	data.mPosiY = 81;
	animation.SetTarget( pArea );
	animation.SetState( data );
	data.mPosiX = 30;
	animation.InsertFrames( 20, data );
	animation.SetOverFunction( EventModeBtnOver );
	gpDlg->mAnimationMgr.StartAreaAnimation( animation );

	if ( pArea == gpDlg->mpSMode1 )
	{
		gpDlg->mItemType = ALL_PAGES;
		gpDlg->ShowAllPagesWeight();
	}else if ( pArea == gpDlg->mpSMode2 )
	{
		gpDlg->mItemType = SINGLE_PAGE;
		gpDlg->mNumber1 = gpDlg->mCurPage;
		gpDlg->ShowSinglePageWeight();
		UpdataNumberBox();
		UpdateScroll1();
	}else if ( pArea == gpDlg->mpSMode3 )
	{
		gpDlg->mItemType = PAGE_RANGE;
		gpDlg->mNumber1 = gpDlg->mPageBegin;
		gpDlg->mNumber2 = gpDlg->mPageEnd;
		gpDlg->ShowPageRangeWeight();
		UpdataNumberBox();
		UpdateScroll2_Begin();
		UpdateScroll2_End();
	}else if ( pArea == gpDlg->mpSMode4 )
	{
		gpDlg->mItemType = EVEN_PAGES;
		gpDlg->ShowEvenPagesWeight();
	}else if ( pArea == gpDlg->mpSMode5 )
	{
		gpDlg->mItemType = ODD_PAGES;
		gpDlg->ShowOddPageWeight();
	}
}

static void EventClickBG2( CHE_WDM_Area * pArea )
{
	gpDlg->mTipText->SetText( L"" );
	gpDlg->mpBG2->Refresh();

	if ( gpDlg->mbBG2 == true )
	{
		gpDlg->mbBG2 = false;

		gpDlg->mpSMode1->SetEnable( false );
		gpDlg->mpSMode1->SetVisable( true );
		gpDlg->mpSMode1->SetPosiX( 100 );

		gpDlg->mpSMode2->SetEnable( false );
		gpDlg->mpSMode2->SetVisable( true );
		gpDlg->mpSMode2->SetPosiX( 190 );

		gpDlg->mpSMode3->SetEnable( false );
		gpDlg->mpSMode3->SetVisable( true );
		gpDlg->mpSMode3->SetPosiX( 280 );

		gpDlg->mpSMode4->SetEnable( false );
		gpDlg->mpSMode4->SetVisable( true );
		gpDlg->mpSMode4->SetPosiX( 370 );

		gpDlg->mpSMode5->SetEnable( false );
		gpDlg->mpSMode5->SetVisable( true );
		gpDlg->mpSMode5->SetPosiX( 460 );

		CHE_WDM_AreaAnimation animation;
		CHE_WDM_AnimationData data;
		data.mPosiX = 0;
		data.mPosiY = 70;
		animation.SetTarget( gpDlg->mpBtnArea );
		animation.SetState( data );
		data.mPosiY = 150;
		animation.InsertFrames( 20, data );
		animation.SetOverFunction( EventModeBtnOver );
		gpDlg->mAnimationMgr.StartAreaAnimation( animation );
		gpDlg->mpBG2->Refresh();
	}
}

static void EventSModeBtnMouseOver( CHE_WDM_Area * pArea )
{
	if ( pArea == NULL )
	{
		return;
	}
	if ( pArea == gpDlg->mpSMode1 )
	{
		gpDlg->mTipText->SetText( L"Out put a whole file." );
	}else if ( pArea == gpDlg->mpSMode2 )
	{
		gpDlg->mTipText->SetText( L"Select a single page to out put." );
	}else if ( pArea == gpDlg->mpSMode3 )
	{
		gpDlg->mTipText->SetText( L"Select a page range to out put." ); 
	}else if ( pArea == gpDlg->mpSMode4 )
	{
		gpDlg->mTipText->SetText( L"Out put all even pages." );
	}else if ( pArea == gpDlg->mpSMode5 )
	{
		gpDlg->mTipText->SetText( L"Out put all odd pages." );
	}
	gpDlg->mpBG2->Refresh();
}

static void EventSModeBtnMouseOut( CHE_WDM_Area * pArea )
{
	gpDlg->mTipText->SetText( L"" );
	gpDlg->mpBG2->Refresh();
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
	mMainTipText->SetPosiY( 95 );
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
	imagePtr->SetImageFile( L"images\\headbar.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpFileInfo->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
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

	//mode btn
	mpModeBtn = CHE_WDM_Button::Create( mpInterActive );
	mpModeBtn->SetPosiX( 30 );
	mpModeBtn->SetPosiY( 80 );
	mpModeBtn->SetWidth( 82 );
	mpModeBtn->SetHeight( 50 );
	mpModeBtn->SetClickEvent( EventSModeClick );
	mpMainArea->AppendChild( mpModeBtn );

	mModeBtnGB = CHE_WDM_AppearImage::Create();
	mModeBtnGB->SetImageFile( L"images\\ModeBtn.png" );

	mModeBtnGBHL = CHE_WDM_AppearImage::Create();
	mModeBtnGBHL->SetImageFile( L"images\\ModeBtnHL.png" );
		
	//ok btn
	CHE_WDM_Button * pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 410 );
	pBtn->SetPosiY( 200 );
	pBtn->SetWidth( 88 );
	pBtn->SetHeight( 29 );
	pBtn->SetClickEvent( EventOkBtn );
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
	pBtn->SetPosiY( 200 );
	pBtn->SetWidth( 88 );
	pBtn->SetHeight( 29 );
	pBtn->SetClickEvent( EventCancelBtn );
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
	mpNumberBox1->SetPosiY( 93 );
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
	pTmpBtn->SetPosiY( 93 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 11 );
	pTmpBtn->SetClickEvent( EventUpBtnClick1 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox1->AppendChild( pTmpBtn );
	pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 253 );
	pTmpBtn->SetPosiY( 105 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 12 );
	pTmpBtn->SetClickEvent( EventDownBtnClick1 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox1->AppendChild( pTmpBtn );

	textPtr = CHE_WDM_AppearText::Create();
	mpNumberBox2 = CHE_WDM_Area::Create( mpInterActive );
	mpNumberBox2->SetPosiX( 380 );
	mpNumberBox2->SetPosiY( 93 );
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
	pTmpBtn->SetPosiY( 93 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 11 );
	pTmpBtn->SetClickEvent( EventUpBtnClick2 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox2->AppendChild( pTmpBtn );
	pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpBtn->SetPosiX( 433 );
	pTmpBtn->SetPosiY( 105 );
	pTmpBtn->SetWidth( 18 );
	pTmpBtn->SetHeight( 12 );
	pTmpBtn->SetClickEvent( EventDownBtnClick2 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownHover.png" );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpNumberBox2->AppendChild( pTmpBtn );

	//Single page
	{
		mpSinglePageArea = CHE_WDM_Area::Create( mpInterActive );
		mpSinglePageArea->SetPosiX( 0 );
		mpSinglePageArea->SetPosiY( 155 );
		mpSinglePageArea->SetWidth( 630 );
		mpSinglePageArea->SetHeight( 25 );

		mpSinglePageScrollBar = CHE_WDM_Area::Create( mpInterActive );
		mpSinglePageScrollBar->SetWidth( 540 );
		mpSinglePageScrollBar->SetHeight( 20 );
		mpSinglePageScrollBar->SetPosiX( 50 );
		mpSinglePageScrollBar->SetPosiY( 160 );

		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\Scroll.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpSinglePageScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

		mpSinglePageArea->AppendChild( mpSinglePageScrollBar );

		CHE_WDM_DragArea * pTmpDragArea = CHE_WDM_DragArea::Create( mpInterActive );
		pTmpDragArea->SetWidth( 30 );
		pTmpDragArea->SetHeight( 14 );
		pTmpDragArea->SetPosiX( 50 );
		pTmpDragArea->SetPosiY( 158 );
		pTmpDragArea->SetRange( 50, 158, 549, 158 );
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
		mpPageRangeArea->SetPosiY( 155 );
		mpPageRangeArea->SetWidth( 630 );
		mpPageRangeArea->SetHeight( 25 );

		mpPageRangeScrollBar = CHE_WDM_Area::Create( mpInterActive );
		mpPageRangeScrollBar->SetWidth( 540 );
		mpPageRangeScrollBar->SetHeight( 20 );
		mpPageRangeScrollBar->SetPosiX( 50 );
		mpPageRangeScrollBar->SetPosiY( 160 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\Scroll.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpSinglePageScrollBar );

		mpPageRangeScrollFill = CHE_WDM_Area::Create( mpInterActive );
		mpPageRangeScrollFill->SetWidth( 540 );
		mpPageRangeScrollFill->SetHeight( 20 );
		mpPageRangeScrollFill->SetPosiX( 50 );
		mpPageRangeScrollFill->SetPosiY( 160 );
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
		mpPageRangeBeginDragBox->SetPosiY( 158 );
		mpPageRangeBeginDragBox->SetRange( 50, 158, 521, 158 );
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
		mpPageRangeEndDragBox->SetPosiY( 158 );
		mpPageRangeEndDragBox->SetRange( 80, 158, 551, 158 );
		mpPageRangeEndDragBox->SetDragEvent( EventDragScroolBlock_PageRange_End );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpPageRangeEndDragBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpPageRangeArea->AppendChild( mpPageRangeEndDragBox );

		mpMainArea->AppendChild( mpPageRangeArea );
	}

	mbBG2 = false;
	mpBG2 = CHE_WDM_Button::Create( mpInterActive );
	mpBG2->SetPosiX( 0 );
	mpBG2->SetPosiY( 0 );
	mpBG2->SetWidth( 630 );
	mpBG2->SetHeight( 280 );
	mpBG2->SetClickEvent( EventClickBG2 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\bg2.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	imagePtr->SetAlpha( 0 );
	mpBG2->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mTipText = CHE_WDM_AppearText::Create();
	mTipText->SetPosiX( 0 );
	mTipText->SetPosiY( 155 );
	mTipText->SetWidth( 630 );
	mTipText->SetHeight( 20 );
	mTipText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mTipText->SetSize( 12 );
	mTipText->SetColor( 0xFFFFFFFF );
	mpBG2->AppendAppearItem( mTipText, AREA_APPEAR_BACKGROUND );
	CHE_WDM_AppearTextPtr tmpText = CHE_WDM_AppearText::Create();
	tmpText->SetPosiX( 10 );
	tmpText->SetPosiY( 220 );
	tmpText->SetWidth( 630 );
	tmpText->SetHeight( 20 );
	tmpText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	tmpText->SetSize( 12 );
	tmpText->SetColor( 0xFFFFFFFF );
	tmpText->SetText( L"Click the ICON to select mode. Click the black area to cancel." );
	mpBG2->AppendAppearItem( tmpText, AREA_APPEAR_BACKGROUND );
	mpBG2->SetEnable( false );
	mpBG2->SetVisable( false );
	mpMainArea->AppendChild( mpBG2 );

	mpBG3 = CHE_WDM_Area::Create( mpInterActive );
	mpBG3->SetPosiX( 0 );
	mpBG3->SetPosiY( 70 );
	mpBG3->SetWidth( 630 );
	mpBG3->SetHeight( 75 );
	mpBG3->EnableClip();
	CHE_WDM_AppearPathPtr pathPtr = CHE_WDM_AppearPath::Create();
	pathPtr->AddRect( 0, 0, 630, 75 );
	pathPtr->SetFillColor( 0xFFFFFFFF );
	pathPtr->SetOperator( APPEAR_PATH_FILL );
	mpBG3->AppendAppearItem( pathPtr, AREA_APPEAR_BACKGROUND );
	mpBG3->SetEnable( false );
	mpBG3->SetVisable( false );
	mpMainArea->AppendChild( mpBG3 );

	mNotAMark = CHE_WDM_AppearImage::Create();
	mNotAMark->SetImageFile( L"images\\notamark.png" );
	mNotAMark->SetPosiX( 3 );
	mNotAMark->SetPosiY( 18 );

	mpBtnArea = CHE_WDM_Area::Create( mpInterActive );
	mpBtnArea->SetPosiX( 0 );
	mpBtnArea->SetPosiY( 70 );
	mpBtnArea->SetWidth( 630 );
	mpBtnArea->SetHeight( 75 );
	mpBtnArea->EnableClip();
	mpBG3->AppendChild( mpBtnArea );

	mpSMode1 = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	mpSMode1->SetPosiX( 100 );
	mpSMode1->SetPosiY( 70 );
	mpSMode1->SetWidth( 52 );
	mpSMode1->SetHeight( 55 );
	mpSMode1->SetClickEvent( EventModeBtnClick );
	mpSMode1->SetMouseOverEvent( EventSModeBtnMouseOver );
	mpSMode1->SetMouseOutEvent( EventSModeBtnMouseOut );
	mSModeImage1 = CHE_WDM_AppearImage::Create();
	mSModeImage1->SetImageFile( L"images\\FileIcon.png" );
	mSModeImage1->SetPosiX( 20 );
	mSModeImage1->SetPosiY( 6 );
	mpSMode1->AppendAppearItem( mSModeImage1, AREA_APPEAR_NORMAL );
	mpBtnArea->AppendChild( mpSMode1 );

	mpSMode2 = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	mpSMode2->SetPosiX( 190 );
	mpSMode2->SetPosiY( 70 );
	mpSMode2->SetWidth( 52 );
	mpSMode2->SetHeight( 55 );
	mpSMode2->SetClickEvent( EventModeBtnClick );
	mpSMode2->SetMouseOverEvent( EventSModeBtnMouseOver );
	mpSMode2->SetMouseOutEvent( EventSModeBtnMouseOut );
	mSModeImage2 = CHE_WDM_AppearImage::Create();
	mSModeImage2->SetImageFile( L"images\\PageIcon.png" );
	mSModeImage2->SetPosiX( 23 );
	mSModeImage2->SetPosiY( 12 );
	mpSMode2->AppendAppearItem( mSModeImage2, AREA_APPEAR_NORMAL );
	mpBtnArea->AppendChild( mpSMode2 );

	mpSMode3 = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	mpSMode3->SetPosiX( 280 );
	mpSMode3->SetPosiY( 70 );
	mpSMode3->SetWidth( 52 );
	mpSMode3->SetHeight( 55 );
	mpSMode3->SetClickEvent( EventModeBtnClick );
	mpSMode3->SetMouseOverEvent( EventSModeBtnMouseOver );
	mpSMode3->SetMouseOutEvent( EventSModeBtnMouseOut );
	mSModeImage3 = CHE_WDM_AppearImage::Create();
	mSModeImage3->SetImageFile( L"images\\PagesIcon.png" );
	mSModeImage3->SetPosiX( 20 );
	mSModeImage3->SetPosiY( 12 );
	mpSMode3->AppendAppearItem( mSModeImage3, AREA_APPEAR_NORMAL );
	if ( mPageCount == 1 )
	{
		mpSMode3->AppendAppearItem( mNotAMark, AREA_APPEAR_NORMAL );
		mpSMode3->SetEnable( false );
	}
	mpBtnArea->AppendChild( mpSMode3 );

	mpSMode4 = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	mpSMode4->SetPosiX( 370 );
	mpSMode4->SetPosiY( 70 );
	mpSMode4->SetWidth( 52 );
	mpSMode4->SetHeight( 55 );
	mpSMode4->SetClickEvent( EventModeBtnClick );
	mpSMode4->SetMouseOverEvent( EventSModeBtnMouseOver );
	mpSMode4->SetMouseOutEvent( EventSModeBtnMouseOut );
	mSModeImage4 = CHE_WDM_AppearImage::Create();
	mSModeImage4->SetImageFile( L"images\\EvenPagesIcon.png" );
	mSModeImage4->SetPosiX( 16 );
	mSModeImage4->SetPosiY( 12 );
	mpSMode4->AppendAppearItem( mSModeImage4, AREA_APPEAR_NORMAL );
	if ( mPageCount == 1 )
	{
		mpSMode4->AppendAppearItem( mNotAMark, AREA_APPEAR_NORMAL );
		mpSMode4->SetEnable( false );
	}
	mpBtnArea->AppendChild( mpSMode4 );

	mpSMode5 = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	mpSMode5->SetPosiX( 460 );
	mpSMode5->SetPosiY( 70 );
	mpSMode5->SetWidth( 52 );
	mpSMode5->SetHeight( 55 );
	mpSMode5->SetClickEvent( EventModeBtnClick );
	mpSMode5->SetMouseOverEvent( EventSModeBtnMouseOver );
	mpSMode5->SetMouseOutEvent( EventSModeBtnMouseOut );
	mSModeImage5 = CHE_WDM_AppearImage::Create();
	mSModeImage5->SetImageFile( L"images\\OddPagesIcon.png" );
	mSModeImage5->SetPosiX( 16 );
	mSModeImage5->SetPosiY( 12 );
	mpSMode5->AppendAppearItem( mSModeImage5, AREA_APPEAR_NORMAL );
	if ( mPageCount == 1 )
	{
		mpSMode5->AppendAppearItem( mNotAMark, AREA_APPEAR_NORMAL );
		mpSMode5->SetEnable( false );
	}
	mpBtnArea->AppendChild( mpSMode5 );
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

	mpModeBtn->Refresh();

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
	CDialogEx::OnSize( nType, 630, 280 );

	if ( mClientWidth != 630 )
	{

		this->MoveWindow( 0, 0, 630, 280 );

		mClientWidth = 630;
		mClientHeight = 280;

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
}

void CEditDlg::ShowSinglePageWeight()
{
	HideAllWeight();
	mpNumberBox1->SetVisable( true );
	mpNumberBox1->SetEnable( true );
	mpSinglePageArea->SetVisable( true );
	mpSinglePageArea->SetEnable( true );
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_NORMAL );
	mpModeBtn->AppendAppearItem( mSModeImage2, AREA_APPEAR_NORMAL );
	mpModeBtn->GetAppear().mMouseOver.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mSModeImage2, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mModeBtnGBHL, AREA_APPEAR_MOUSEOVER );
	mMainTipText->SetText( L"       Page" );
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
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_NORMAL );
	mpModeBtn->AppendAppearItem( mSModeImage3, AREA_APPEAR_NORMAL );
	mpModeBtn->GetAppear().mMouseOver.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mSModeImage3, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mModeBtnGBHL, AREA_APPEAR_MOUSEOVER );
	mMainTipText->SetText( L"From Page                                         To Page" );
}

void CEditDlg::ShowAllPagesWeight()
{
	HideAllWeight();
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_NORMAL );
	mpModeBtn->AppendAppearItem( mSModeImage1, AREA_APPEAR_NORMAL );
	mpModeBtn->GetAppear().mMouseOver.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mSModeImage1, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mModeBtnGBHL, AREA_APPEAR_MOUSEOVER );
	mMainTipText->SetText( L"Out put all pages of file." );
}

void CEditDlg::ShowEvenPagesWeight()
{
	HideAllWeight();
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_NORMAL );
	mpModeBtn->AppendAppearItem( mSModeImage4, AREA_APPEAR_NORMAL );
	mpModeBtn->GetAppear().mMouseOver.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mSModeImage4, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mModeBtnGBHL, AREA_APPEAR_MOUSEOVER );
	mMainTipText->SetText( L"Out put all even pages of file." );
}

void CEditDlg::ShowOddPageWeight()
{
	HideAllWeight();
	mpModeBtn->GetAppear().mNormal.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_NORMAL );
	mpModeBtn->AppendAppearItem( mSModeImage5, AREA_APPEAR_NORMAL );
	mpModeBtn->GetAppear().mMouseOver.clear();
	mpModeBtn->AppendAppearItem( mModeBtnGB, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mSModeImage5, AREA_APPEAR_MOUSEOVER );
	mpModeBtn->AppendAppearItem( mModeBtnGBHL, AREA_APPEAR_MOUSEOVER );
	mMainTipText->SetText( L"Out put all odd pages of file." );
}
