// PagesSelectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PagesSelectionDlg.h"
#include "afxdialogex.h"

CPagesSelectionDlg * gpDlg = NULL;

static void UpdataBeginNumberBox()
{
	CHE_WDM_AppearTextPtr textPtr = gpDlg->mpBeginNumberBox->GetAppear().mBackground[1].GetTextPtr();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mPageBegin );
	textPtr->SetText( tmpStr );
	gpDlg->mpBeginNumberBox->Refresh();
}

static void UpdataEndNumberBox()
{
	CHE_WDM_AppearTextPtr textPtr = gpDlg->mpEndNumberBox->GetAppear().mBackground[1].GetTextPtr();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mPageEnd );
	textPtr->SetText( tmpStr );
	gpDlg->mpEndNumberBox->Refresh();
}

static void UpdateScrollBeginBlock()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	CHE_WDM_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	CHE_WDM_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	pBeginBlock->SetPosiX( 25 + ( gpDlg->mPageBegin - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	BeginPosi = pBeginBlock->GetPosiX() + 15;
	//pEndBlock->SetPosiX(  55 + ( gpDlg->mPageEnd - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	EndPosi = pEndBlock->GetPosiX() + 15;

	CHE_WDM_DragArea * pDragArea = (CHE_WDM_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	pDragArea = (CHE_WDM_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );

	CHE_WDM_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPosiX( BeginPosi );
	pScrollf->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpScrollBar->Refresh();
}

static void UpdateScrollEndBlock()
{
	unsigned int BeginPosi = 0, EndPosi = 0;
	CHE_WDM_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	CHE_WDM_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	//pBeginBlock->SetPosiX( 25 + ( gpDlg->mPageBegin - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	BeginPosi = pBeginBlock->GetPosiX() + 15;
	pEndBlock->SetPosiX(  55 + ( gpDlg->mPageEnd - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 381 );
	EndPosi = pEndBlock->GetPosiX() + 15;

	CHE_WDM_DragArea * pDragArea = (CHE_WDM_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	pDragArea = (CHE_WDM_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );

	CHE_WDM_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPosiX( BeginPosi );
	pScrollf->SetWidth( EndPosi - BeginPosi );
	gpDlg->mpScrollBar->Refresh();
}

static void EventBeginNumberUpBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mPageBegin == gpDlg->mPageEnd - 1 )
	{
		return;
	}
	gpDlg->mPageBegin++;
	UpdataBeginNumberBox();
	UpdateScrollBeginBlock();
}

static void EventBeginNumberDownBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mPageBegin == 1 )
	{
		return;
	}
	gpDlg->mPageBegin--;
	UpdataBeginNumberBox();
	UpdateScrollBeginBlock();
}

static void EventEndNumberUpBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mPageEnd == gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mPageEnd++;
	UpdataEndNumberBox();
	UpdateScrollEndBlock();
}

static void EventEndNumberDownBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mPageEnd == gpDlg->mPageBegin + 1 )
	{
		return;
	}
	gpDlg->mPageEnd--;
	UpdataEndNumberBox();
	UpdateScrollEndBlock();
}

static void EventDragBeginBlock( CHE_WDM_Area * pArea )
{
	int BeginPosi = pArea->GetPosiX() + 15;
	CHE_WDM_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	pScrollf->SetPosiX( BeginPosi );
	CHE_WDM_Area * pEndBlock = gpDlg->mpScrollBar->GetChild( 3 );
	int EndPosi = pEndBlock->GetPosiX() + 15;
	pScrollf->SetWidth( EndPosi - BeginPosi );
	CHE_WDM_DragArea * pDragArea = (CHE_WDM_DragArea*)pEndBlock;
	pDragArea->SetRange( BeginPosi+15, 63, 436, 63 );
	unsigned int tmpPosi = pArea->GetPosiX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 25 ) * 1.0 / ( 381 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( tmpPageIndex < gpDlg->mPageEnd )
	{
		gpDlg->mPageBegin = tmpPageIndex;
	}
	UpdataBeginNumberBox();
}

static void EventDragEndBlock( CHE_WDM_Area * pArea )
{
	int EndPosi = pArea->GetPosiX() + 15;
	CHE_WDM_Area * pScrollf = gpDlg->mpScrollBar->GetChild( 1 );
	CHE_WDM_Area * pBeginBlock = gpDlg->mpScrollBar->GetChild( 2 );
	int BeginPosi = pBeginBlock->GetPosiX() + 15;
	pScrollf->SetWidth( EndPosi - BeginPosi );
	CHE_WDM_DragArea * pDragArea = (CHE_WDM_DragArea*)pBeginBlock;
	pDragArea->SetRange( 25, 63, EndPosi-45, 63 );
	unsigned int tmpPosi = pArea->GetPosiX();
	unsigned int tmpPageIndex = (unsigned int)( 1.5 + ( tmpPosi - 55 ) * 1.0 / ( 381 * 1.0 / (gpDlg->mPageCount - 1) ) );
	if ( tmpPageIndex > gpDlg->mPageBegin )
	{
		gpDlg->mPageEnd = tmpPageIndex;
	}
	UpdataEndNumberBox();
}

static void EventCancelBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventOkBtn( CHE_WDM_Area * pArea )
{
	CListItem listItem;
	listItem.type = PAGE_RANGE;
	listItem.pageIndex = gpDlg->mPageBegin;
	listItem.pageCount = gpDlg->mPageEnd - gpDlg->mPageBegin + 1;
	theApp.AddPageListItem( listItem );
	gpDlg->EndDialog( 1 );
}

IMPLEMENT_DYNAMIC(CPagesSelectionDlg, CDialogEx)

CPagesSelectionDlg::CPagesSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPagesSelectionDlg::IDD, pParent)
{
	gpDlg = this;
	mPageBegin = 1;
	mPageEnd = theApp.mpPageTree->GetPageCount();
	mPageCount = theApp.mpPageTree->GetPageCount();

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;
	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"From" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 20 );
	textPtr->SetPosiY( 18 );
	textPtr->SetWidth( 60 );
	textPtr->SetHeight( 30 );
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	mpMainArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"To" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 165 );
	textPtr->SetPosiY( 18 );
	textPtr->SetWidth( 60 );
	textPtr->SetHeight( 30 );
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	mpMainArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpBeginNumberBox = CHE_WDM_Area::Create( mpInterActive );
	mpBeginNumberBox->SetWidth( 45 );
	mpBeginNumberBox->SetHeight( 24 ); 
	mpBeginNumberBox->SetPosiX( 75 );
	mpBeginNumberBox->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBox.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBeginNumberBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", mPageBegin );
	textPtr->SetText( tmpStr );
	textPtr->SetSize( 12 );
	textPtr->SetWidth( 45 );
	textPtr->SetHeight( 24 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_RIGHT_OR_BOTTOM, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetColor( 0xFFFFFFFF );
	mpBeginNumberBox->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpBeginNumberBox );

	CHE_WDM_Button * pButton = CHE_WDM_Button::Create( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 11 );
	pButton->SetPosiX( 127 );
	pButton->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pButton->SetMouseLBDEvent( EventBeginNumberUpBtnClick );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pButton );

	pButton = CHE_WDM_Button::Create( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 13 );
	pButton->SetPosiX( 127 );
	pButton->SetPosiY( 31 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pButton->SetMouseLBDEvent( EventBeginNumberDownBtnClick );

	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER ); 

	mpMainArea->AppendChild( pButton );

	mpEndNumberBox = CHE_WDM_Area::Create( mpInterActive );
	mpEndNumberBox->SetWidth( 45 );
	mpEndNumberBox->SetHeight( 24 );
	mpEndNumberBox->SetPosiX( 220 );
	mpEndNumberBox->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBox.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpEndNumberBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	wsprintf( tmpStr, L"%d", mPageEnd );
	textPtr->SetText( tmpStr );
	textPtr->SetSize( 12 );
	textPtr->SetWidth( 45 );
	textPtr->SetHeight( 24 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_RIGHT_OR_BOTTOM, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetColor( 0xFFFFFFFF );

	mpEndNumberBox->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpEndNumberBox );

	pButton = CHE_WDM_Button::Create( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 11 );
	pButton->SetPosiX( 272 );
	pButton->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pButton->SetMouseLBDEvent( EventEndNumberUpBtnClick );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pButton );

	pButton = CHE_WDM_Button::Create( mpInterActive );
	pButton->SetWidth( 18 );
	pButton->SetHeight( 13 );
	pButton->SetPosiX( 272 );
	pButton->SetPosiY( 31 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pButton->SetMouseLBDEvent( EventEndNumberDownBtnClick );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pButton->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pButton );

	mpScrollBar = CHE_WDM_Area::Create( mpInterActive );
	mpScrollBar->SetWidth( 441 );
	mpScrollBar->SetHeight( 10 );
	mpScrollBar->SetPosiX( 25 );
	mpScrollBar->SetPosiY( 65 );

	CHE_WDM_Area * pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetWidth( 441 );
	pArea->SetHeight( 10 );
	pArea->SetPosiX( 25 );
	pArea->SetPosiY( 65 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\Scroll.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpScrollBar->AppendChild( pArea );

	pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetWidth( 421 );
	pArea->SetHeight( 10 );
	pArea->SetPosiX( 40 );
	pArea->SetPosiY( 65 );
	pArea->SetClipEnable( TRUE );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\Scrollf.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	mpScrollBar->AppendChild( pArea );

	CHE_WDM_DragArea * pTmpDragArea = CHE_WDM_DragArea::Create( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPosiX( 25 );
	pTmpDragArea->SetPosiY( 63 );
	pTmpDragArea->SetRange( 25, 63, 406, 63 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpDragArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pTmpDragArea->SetDragEvent( EventDragBeginBlock );
	mpScrollBar->AppendChild( pTmpDragArea );
	pTmpDragArea = CHE_WDM_DragArea::Create( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPosiX( 436 );
	pTmpDragArea->SetPosiY( 63 );
	pTmpDragArea->SetRange( 55, 63, 436, 63 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpDragArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pTmpDragArea->SetDragEvent( EventDragEndBlock );
	mpScrollBar->AppendChild( pTmpDragArea );

	mpMainArea->AppendChild( mpScrollBar );

	mpOkBtn = CHE_WDM_Button::Create( mpInterActive );
	mpOkBtn->SetWidth( 88 );
	mpOkBtn->SetHeight( 27 );
	mpOkBtn->SetPosiX( 280 );
	mpOkBtn->SetPosiY( 100 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OkBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpOkBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OkBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpOkBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpOkBtn->SetMouseLBDEvent( EventOkBtn );
	mpMainArea->AppendChild( mpOkBtn );

	mpCancelBtn = CHE_WDM_Button::Create( mpInterActive );
	mpCancelBtn->SetWidth( 88 );
	mpCancelBtn->SetHeight( 27 );
	mpCancelBtn->SetPosiX( 380 );
	mpCancelBtn->SetPosiY( 100 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpCancelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpCancelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpCancelBtn->SetMouseLBDEvent( EventCancelBtn );
	mpMainArea->AppendChild( mpCancelBtn );
}

CPagesSelectionDlg::~CPagesSelectionDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
}

void CPagesSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPagesSelectionDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CPagesSelectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();
	return TRUE;
}

void CPagesSelectionDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}

void CPagesSelectionDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CPagesSelectionDlg::OnSize(UINT nType, int cx, int cy)
{
	mClientWidth = 500;
	mClientHeight = 180;

	CDialogEx::OnSize( nType, mClientWidth, mClientHeight );

	mpMainArea->SetWidth( mClientWidth );
	mpMainArea->SetHeight( mClientHeight );

	this->MoveWindow( 0, 0, mClientWidth, mClientHeight );

	CWnd * pWnd = GetDlgItem( IDC_MAIN );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}

void CPagesSelectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPagesSelectionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CPagesSelectionDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CPagesSelectionDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPagesSelectionDlg::OnOK()
{
	EventOkBtn( NULL );
}


BOOL CPagesSelectionDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}


void CPagesSelectionDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}
