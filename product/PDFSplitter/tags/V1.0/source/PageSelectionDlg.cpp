// PageSelectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PageSelectionDlg.h"
#include "afxdialogex.h"


CPageSelectionDlg * gpDlg = NULL;

static void UpdataNumberBox()
{
	CHE_WDM_AppearTextPtr textPtr = gpDlg->mpNumberBox->GetAppear().mBackground[1].GetTextPtr();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mCurPage );
	textPtr->SetText( tmpStr );
	gpDlg->mpNumberBox->Refresh();
}

static void UpdateScroll()
{
	CHE_WDM_Area * pTmpArea = gpDlg->mpScrollBar->GetChild( 0 );
	pTmpArea->SetPosiX( 25 + ( gpDlg->mCurPage - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 411 );
	gpDlg->mpScrollBar->Refresh();
}

static void EventUpBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mCurPage == gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mCurPage++;
	UpdataNumberBox();
	UpdateScroll();
}

static void EventDownBtnClick( CHE_WDM_Area * pArea )
{
	if ( gpDlg->mCurPage == 1 )
	{
		return;
	}
	gpDlg->mCurPage--;
	UpdataNumberBox();
	UpdateScroll();
}

static void EventDragScroolBlock( CHE_WDM_Area * pArea )
{
	unsigned int tmpPosi = pArea->GetPosiX();
	gpDlg->mCurPage = (unsigned int)( 1 + ( tmpPosi - 25 ) * 1.0 / ( 411 * 1.0 / (gpDlg->mPageCount - 1) ) );
	UpdataNumberBox();
}

static void EventCancelBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventOkBtn( CHE_WDM_Area * pArea )
{
	CListItem listItem;
	listItem.type = SINGLE_PAGE;
	listItem.pageIndex = gpDlg->mCurPage;
	listItem.pageCount = 1;
	theApp.AddPageListItem( listItem );
	gpDlg->EndDialog( 1 );
}

IMPLEMENT_DYNAMIC(CPageSelectionDlg, CDialogEx)

CPageSelectionDlg::CPageSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageSelectionDlg::IDD, pParent)
{
	gpDlg = this;
	mCurPage = 1;
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
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetSize( 12 );
	textPtr->SetText( L"Page" );
	textPtr->SetPosiX( 35 );
	textPtr->SetPosiY( 15 );
	textPtr->SetWidth( 80 );
	textPtr->SetHeight( 30 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	mpMainArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetSize( 12 );
	textPtr->SetText( L"" );
	textPtr->SetPosiX( 150 );
	textPtr->SetPosiY( 20 );
	textPtr->SetWidth( 60 );
	textPtr->SetHeight( 30 );
	mpMainArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpNumberBox = CHE_WDM_Area::Create( mpInterActive );
	mpNumberBox->SetWidth( 45 );
	mpNumberBox->SetHeight( 24 );
	mpNumberBox->SetPosiX( 100 );
	mpNumberBox->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBox.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpNumberBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", mCurPage );
	textPtr->SetText( tmpStr );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 45 );
	textPtr->SetHeight( 24 );
	textPtr->SetSize( 12 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_RIGHT_OR_BOTTOM, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetColor( 0xFFFFFFFF );
	mpNumberBox->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpNumberBox );

	CHE_WDM_Button * pArea = CHE_WDM_Button::Create( mpInterActive );
	pArea->SetWidth( 18 );
	pArea->SetHeight( 11 );
	pArea->SetPosiX( 152 );
	pArea->SetPosiY( 20 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pArea->SetMouseLBDEvent( EventUpBtnClick );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pArea );

	pArea = CHE_WDM_Button::Create( mpInterActive );
	pArea->SetWidth( 18 );
	pArea->SetHeight( 13 );
	pArea->SetPosiX( 152 );
	pArea->SetPosiY( 31 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pArea->SetMouseLBDEvent( EventDownBtnClick );

	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pArea );

	mpScrollBar = CHE_WDM_Area::Create( mpInterActive );
	mpScrollBar->SetWidth( 441 );
	mpScrollBar->SetHeight( 10 );
	mpScrollBar->SetPosiX( 25 );
	mpScrollBar->SetPosiY( 65 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\Scroll.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	CHE_WDM_DragArea * pTmpDragArea = CHE_WDM_DragArea::Create( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPosiX( 25 );
	pTmpDragArea->SetPosiY( 63 );
	pTmpDragArea->SetRange( 25, 63, 436, 63 );
	pTmpDragArea->SetDragEvent( EventDragScroolBlock );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ScrollBlock.png" ); 
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );

	pTmpDragArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
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

CPageSelectionDlg::~CPageSelectionDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
}

void CPageSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPageSelectionDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CPageSelectionDlg::OnInitDialog()
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

void CPageSelectionDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}

void CPageSelectionDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CPageSelectionDlg::OnSize(UINT nType, int cx, int cy)
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

void CPageSelectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CPageSelectionDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CPageSelectionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPageSelectionDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}

void CPageSelectionDlg::OnOK()
{
	EventOkBtn( NULL );
}

BOOL CPageSelectionDlg::OnEraseBkgnd(CDC* pDC)
{
	mpInterActive->Invalidate();
	return TRUE;
}


void CPageSelectionDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	mpInterActive->Invalidate();
}
