#include "stdafx.h"
#include "PdfSplitter.h"
#include "SelectionModeDlg.h"
#include "afxdialogex.h"

CSelectionModeDlg * gpDlg = NULL;

static void EventCancelBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventSinglePageBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 1 );
}

static void EventPageRangeBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 2 );
}

static void EventOddPagesBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 3 );
}

static void EventEvenPagesBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 4 );
}

IMPLEMENT_DYNAMIC(CSelectionModeDlg, CDialogEx)

CSelectionModeDlg::CSelectionModeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectionModeDlg::IDD, pParent)
{
	gpDlg = this;

	CHE_WD_Appearance * pTmpApper = NULL;
	CHE_WD_AppearImage * pTmpImage = NULL;
	mpInterActive = new MyIHE_WD_InterActive( this, theApp.m_hInstance );
	mpMainArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\background.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	pTmpApper->mItems.push_back( pTmpImage );
	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpBtnSinglePage = new CHE_WD_Button( mpInterActive );
	mpBtnSinglePage->SetWidth( 73 );
	mpBtnSinglePage->SetHeight( 72 );
	mpBtnSinglePage->SetPositionX( 30 );
	mpBtnSinglePage->SetPositionY( 30 );
	mpBtnSinglePage->SetClickEvent( EventSinglePageBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnSinglePage.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnSinglePage->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnSinglePageHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnSinglePage->SetMouseOverAppear( pTmpApper );
	mpMainArea->AppendChild( mpBtnSinglePage );

	mpBtnPageRange = new CHE_WD_Button( mpInterActive );
	mpBtnPageRange->SetWidth( 73 );
	mpBtnPageRange->SetHeight( 72 );
	mpBtnPageRange->SetPositionX( 130 );
	mpBtnPageRange->SetPositionY( 30 );
	mpBtnPageRange->SetClickEvent( EventPageRangeBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnPageRange.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnPageRange->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnPageRangeHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnPageRange->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnPageRangeDisable.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnPageRange->SetDisableAppear( pTmpApper );
	mpMainArea->AppendChild( mpBtnPageRange );

	mpBtnOddPages = new CHE_WD_Button( mpInterActive );
	mpBtnOddPages->SetWidth( 73 );
	mpBtnOddPages->SetHeight( 72 );
	mpBtnOddPages->SetPositionX( 230 );
	mpBtnOddPages->SetPositionY( 30 );
	mpBtnOddPages->SetClickEvent( EventOddPagesBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnOddPages.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnOddPages->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnOddPagesHOver.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnOddPages->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnOddPagesDisable.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnOddPages->SetDisableAppear( pTmpApper );
	mpMainArea->AppendChild( mpBtnOddPages );

	mpBtnEvenPages = new CHE_WD_Button( mpInterActive );
	mpBtnEvenPages->SetWidth( 73 );
	mpBtnEvenPages->SetHeight( 72 );
	mpBtnEvenPages->SetPositionX( 330 );
	mpBtnEvenPages->SetPositionY( 30 );
	mpBtnEvenPages->SetClickEvent( EventEvenPagesBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnEvenPages.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnEvenPages->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnEvenPagesHOver.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnEvenPages->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BtnEvenPagesDisable.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnEvenPages->SetDisableAppear( pTmpApper );
	mpMainArea->AppendChild( mpBtnEvenPages );

	mpBtnCancel = new CHE_WD_Button( mpInterActive );
	mpBtnCancel->SetWidth( 88 );
	mpBtnCancel->SetHeight( 27 );
	mpBtnCancel->SetPositionX( 330 );
	mpBtnCancel->SetPositionY( 120 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnCancel->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBtnCancel->SetMouseOverAppear( pTmpApper );
	mpBtnCancel->SetClickEvent( EventCancelBtn );
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
	CHE_WD_Button * pTmpArea = (CHE_WD_Button *)( mpMainArea->GetChild( 0 ) );
	CHE_WD_Appearance * pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;


	pTmpArea = (CHE_WD_Button *)( mpMainArea->GetChild( 1 ) );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetDisableAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	pTmpArea = (CHE_WD_Button *)( mpMainArea->GetChild( 2 ) );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetDisableAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	pTmpArea = (CHE_WD_Button *)( mpMainArea->GetChild( 3 ) );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetDisableAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	pTmpArea = (CHE_WD_Button *)( mpMainArea->GetChild( 4 ) );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	delete mpInterActive;
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

void CSelectionModeDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}

void CSelectionModeDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}

void CSelectionModeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	mClientWidth = cx;
	mClientHeight = cy;

	mpMainArea->SetWidth( cx );
	mpMainArea->SetHeight( cy );

	CWnd * pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}


void CSelectionModeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CSelectionModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CSelectionModeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
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