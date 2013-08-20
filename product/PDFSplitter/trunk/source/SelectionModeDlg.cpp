#include "stdafx.h"
#include "PdfSplitter.h"
#include "SelectionModeDlg.h"
#include "afxdialogex.h"

CSelectionModeDlg * gpDlg = NULL;

static void EventCancelBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventSinglePageBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 1 );
}

static void EventPageRangeBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 2 );
}

static void EventOddPagesBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 3 );
}

static void EventEvenPagesBtn( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 4 );
}

IMPLEMENT_DYNAMIC(CSelectionModeDlg, CDialogEx)

CSelectionModeDlg::CSelectionModeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectionModeDlg::IDD, pParent)
{
	gpDlg = this;

	CHE_WDM_AppearImagePtr imagePtr;
	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );
	mpMainArea->SetPosiX( 0 );
	mpMainArea->SetPosiY( 0 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpBtnSinglePage = CHE_WDM_Button::Create( mpInterActive );
	mpBtnSinglePage->SetWidth( 73 );
	mpBtnSinglePage->SetHeight( 72 );
	mpBtnSinglePage->SetPosiX( 30 );
	mpBtnSinglePage->SetPosiY( 30 );
	mpBtnSinglePage->SetMouseLBDEvent( EventSinglePageBtn );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnSinglePage.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnSinglePage->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnSinglePageHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnSinglePage->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpMainArea->AppendChild( mpBtnSinglePage );

	mpBtnPageRange = CHE_WDM_Button::Create( mpInterActive );
	mpBtnPageRange->SetWidth( 73 );
	mpBtnPageRange->SetHeight( 72 );
	mpBtnPageRange->SetPosiX( 130 );
	mpBtnPageRange->SetPosiY( 30 );
	mpBtnPageRange->SetMouseLBDEvent( EventPageRangeBtn );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnPageRange.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnPageRange->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnPageRangeHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnPageRange->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnPageRangeDisable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnPageRange->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpMainArea->AppendChild( mpBtnPageRange );

	mpBtnOddPages = CHE_WDM_Button::Create( mpInterActive );
	mpBtnOddPages->SetWidth( 73 );
	mpBtnOddPages->SetHeight( 72 );
	mpBtnOddPages->SetPosiX( 230 );
	mpBtnOddPages->SetPosiY( 30 );
	mpBtnOddPages->SetMouseLBDEvent( EventOddPagesBtn );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnOddPages.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnOddPages->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnOddPagesHOver.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnOddPages->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnOddPagesDisable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnOddPages->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpMainArea->AppendChild( mpBtnOddPages );

	mpBtnEvenPages = CHE_WDM_Button::Create( mpInterActive );
	mpBtnEvenPages->SetWidth( 73 );
	mpBtnEvenPages->SetHeight( 72 );
	mpBtnEvenPages->SetPosiX( 330 );
	mpBtnEvenPages->SetPosiY( 30 );
	mpBtnEvenPages->SetMouseLBDEvent( EventEvenPagesBtn );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnEvenPages.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnEvenPages->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnEvenPagesHOver.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnEvenPages->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BtnEvenPagesDisable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnEvenPages->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpMainArea->AppendChild( mpBtnEvenPages );

	mpBtnCancel = CHE_WDM_Button::Create( mpInterActive );
	mpBtnCancel->SetWidth( 88 );
	mpBtnCancel->SetHeight( 27 );
	mpBtnCancel->SetPosiX( 330 );
	mpBtnCancel->SetPosiY( 120 );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnCancel->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CancelBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBtnCancel->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpBtnCancel->SetMouseLBDEvent( EventCancelBtn );
	mpMainArea->AppendChild( mpBtnCancel );

	if ( theApp.mpPageTree->GetPageCount() <= 1 )
	{
		mpBtnPageRange->SetEnable( false );
		mpBtnOddPages->SetEnable( false );
		mpBtnEvenPages->SetEnable( false );
	}
}

CSelectionModeDlg::~CSelectionModeDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
}

void CSelectionModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelectionModeDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CSelectionModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
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

void CSelectionModeDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}

void CSelectionModeDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CSelectionModeDlg::OnSize(UINT nType, int cx, int cy)
{
// 	CDialogEx::OnSize(nType, 500, 250);
// 
// 	mClientWidth = 500;
// 	mClientHeight = 250;
// 
// 	mpMainArea->SetWidth( 500 );
// 	mpMainArea->SetHeight( 250 );
// 
// 	CWnd * pWnd = GetDlgItem( IDC_MAIN );
// 	if ( pWnd )
// 	{
// 		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
// 	}

	mClientWidth = 450;
	mClientHeight = 190;

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


void CSelectionModeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CSelectionModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CSelectionModeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CSelectionModeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CSelectionModeDlg::OnOK()
{
	//CDialogEx::OnOK();
}


void CSelectionModeDlg::OnCancel()
{
	EndDialog(0);
}


BOOL CSelectionModeDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}


void CSelectionModeDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}