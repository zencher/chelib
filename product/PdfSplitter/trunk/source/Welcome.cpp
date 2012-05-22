#include "stdafx.h"
#include "PdfSplitter.h"
#include "Welcome.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CWelcomeDlg, CDialogEx)

CWelcomeDlg * gpWelComeDlg = NULL;

static void EventClickBuyBtn( CHE_WD_Area * pArea )
{
	ShellExecute( NULL, L"open", L"http://www.peroit.com/pdfSplitter/Buy", NULL, NULL, SW_SHOWNORMAL );
};

static void EventClickActiveBtn( CHE_WD_Area * pArea )
{
	CRegsitrationDlg dlg;
	if ( dlg.DoModal() == 0 )
	{
		gpWelComeDlg->EndDialog( 0 );
	}
}

static void EventClickTryBtn( CHE_WD_Area * pArea )
{
	gpWelComeDlg->EndDialog( 0 );
}

CWelcomeDlg::CWelcomeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWelcomeDlg::IDD, pParent)
{
	mClientWidth = 0;
	mClientHeight = 0;

	gpWelComeDlg = this;

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

	CHE_WD_Area * pImageArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pImageArea->SetWidth( 530 );
	pImageArea->SetHeight( 150 );
	pImageArea->SetPositionX( 0 );
	pImageArea->SetPositionY( 0 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\welcome.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pImageArea->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( pImageArea );

	CHE_WD_Button * pTmpButtn = new CHE_WD_Button( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPositionX( 60 );
	pTmpButtn->SetPositionY( 170 );
	pTmpButtn->SetClickEvent( EventClickBuyBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BuyBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\BuyBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetMouseOverAppear( pTmpApper );
	mpMainArea->AppendChild( pTmpButtn );

	pTmpButtn = new CHE_WD_Button( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPositionX( 210 );
	pTmpButtn->SetPositionY( 170 );
	pTmpButtn->SetClickEvent( EventClickActiveBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\ActiveBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\ActiveBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetMouseOverAppear( pTmpApper );
	mpMainArea->AppendChild( pTmpButtn );

	pTmpButtn = new CHE_WD_Button( mpInterActive );
	pTmpButtn->SetWidth( 96 );
	pTmpButtn->SetHeight( 35 );
	pTmpButtn->SetPositionX( 360 );
	pTmpButtn->SetPositionY( 170 );
	pTmpButtn->SetClickEvent( EventClickTryBtn );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\TryBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\TryBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpButtn->SetMouseOverAppear( pTmpApper );
	mpMainArea->AppendChild( pTmpButtn );
}

CWelcomeDlg::~CWelcomeDlg()
{
	CHE_WD_Appearance * pTmpAppear = mpMainArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;

 	delete mpInterActive;
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
	CDialogEx::OnSize(nType, cx, cy);

	mClientWidth = cx;
	mClientHeight = cy;

	mpMainArea->SetWidth( cx );
	mpMainArea->SetHeight( cy );

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
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CWelcomeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CWelcomeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
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
	CWnd *		pWnd = GetDlgItem( IDC_WELCOME );
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