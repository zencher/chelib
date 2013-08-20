#include "stdafx.h"
#include "PdfMerger.h"
#include "Welcome.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CWelcomeDlg, CDialogEx)

CWelcomeDlg * gpWelComeDlg = NULL;

static void EventClickBuyBtn( CHE_WDM_Area * pArea )
{
	ShellExecute( NULL, L"open", L"http://www.peroit.com/pdfMerger/Buy", NULL, NULL, SW_SHOWNORMAL );
};

static void EventClickActiveBtn( CHE_WDM_Area * pArea )
{
	CRegsitrationDlg dlg;
	if ( dlg.DoModal() == 0 )
	{
		gpWelComeDlg->EndDialog( 0 );
	}
}

static void EventClickTryBtn( CHE_WDM_Area * pArea )
{
	gpWelComeDlg->EndDialog( 0 );
}

CWelcomeDlg::CWelcomeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWelcomeDlg::IDD, pParent)
{
	mClientWidth = 0;
	mClientHeight = 0;

	gpWelComeDlg = this;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	CHE_WDM_AppearImagePtr imagePtr;
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );

	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\welcome.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );

	CHE_WDM_Area * pImageArea = CHE_WDM_Area::Create( mpInterActive );
	
	pImageArea->SetWidth( 530 );
	pImageArea->SetHeight( 150 );
	pImageArea->SetPosiX( 0 );
	pImageArea->SetPosiY( 0 );
	
	pImageArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( pImageArea );

	CHE_WDM_Button * pTmpButtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPosiX( 60 );
	pTmpButtn->SetPosiY( 170 );
	pTmpButtn->SetMouseLBUEvent( EventClickBuyBtn );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BuyBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\BuyBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpMainArea->AppendChild( pTmpButtn );

	pTmpButtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPosiX( 210 );
	pTmpButtn->SetPosiY( 170 );
	pTmpButtn->SetMouseLBUEvent( EventClickActiveBtn );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ActiveBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ActiveBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pTmpButtn );

	pTmpButtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPosiX( 360 );
	pTmpButtn->SetPosiY( 170 );
	pTmpButtn->SetMouseLBUEvent( EventClickTryBtn );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\TryBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\TryBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpButtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	mpMainArea->AppendChild( pTmpButtn );
}

CWelcomeDlg::~CWelcomeDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
}

void CWelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWelcomeDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


int CWelcomeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CWelcomeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_WELCOME_MAIN );
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

void CWelcomeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize( nType, 535, 250 );

	mClientWidth = 535;
	mClientHeight = 250;

	mpMainArea->SetWidth( mClientWidth );
	mpMainArea->SetHeight( mClientHeight );

	this->MoveWindow( 0, 0, mClientWidth, mClientHeight );

	CWnd * pWnd = GetDlgItem( IDC_WELCOME_MAIN );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}

void CWelcomeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}

void CWelcomeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CWelcomeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CWelcomeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CWelcomeDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}
 
void CWelcomeDlg::DrawMainArea()
{
	CWnd *		pWnd = GetDlgItem( IDC_WELCOME_MAIN );
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

void CWelcomeDlg::OnOK()
{
	//CDialogEx::OnOK();
}

void CWelcomeDlg::OnCancel()
{
	CDialogEx::OnCancel();
}

void CWelcomeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

BOOL CWelcomeDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}

void CWelcomeDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}

BEGIN_MESSAGE_MAP(CRegsitrationDlg, CDialogEx)
END_MESSAGE_MAP()

CRegsitrationDlg::CRegsitrationDlg() : CDialogEx( CRegsitrationDlg::IDD )
{
}

void CRegsitrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CRegsitrationDlg::OnInitDialog()
{
	wchar_t name[1024];
	wchar_t key[1024];

	memset( name, 0, 1024 * sizeof(wchar_t) );
	memset( key, 0, 1024 * sizeof(wchar_t) );

	MultiByteToWideChar( CP_ACP, 0, theApp.mName.c_str(), theApp.mName.length(), name, 1024 );

	MultiByteToWideChar( CP_ACP, 0, theApp.mKey.c_str(), theApp.mKey.length(), key, 1024 );

	CWnd * pWnd = GetDlgItem( IDC_NAME );
	pWnd->SetWindowText( name );
	pWnd = GetDlgItem( IDC_KEY );
	pWnd->SetWindowText( key );

	return TRUE;
}

void CRegsitrationDlg::OnOK()
{
	wchar_t name[1024];
	wchar_t key[1024];
	char tmpStr[1024];

	memset( name, 0, 1024 * sizeof(wchar_t) );
	memset( key, 0, 1024 * sizeof(wchar_t) );

	CWnd * pWnd = GetDlgItem( IDC_NAME );
	pWnd->GetWindowText( name, 1024 );
	pWnd = GetDlgItem( IDC_KEY );
	pWnd->GetWindowText( key, 1024 );

	if ( wcslen( name ) > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, name, -1, tmpStr, 1024, NULL, NULL );
		theApp.mName = tmpStr;
	}else{
		theApp.mName.clear();
	}

	if ( wcslen( key ) > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, key, -1, tmpStr, 1024, NULL, NULL );
		theApp.mKey = tmpStr;
	}else{
		theApp.mKey.clear();
	}

	theApp.WriteRegInfo();

	if ( theApp.CheckRefInfo() )
	{
		MessageBox( L"Thanks for register!", L"OK" );

		EndDialog( 0 );

		return;
	}

	EndDialog( 1 );
}

void CRegsitrationDlg::OnCancel()
{
	EndDialog( 1 );
}