#include "stdafx.h"
#include "PdfSplitter.h"
#include "FileLoadDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CFileLoadDlg, CDialogEx)


CFileLoadDlg::CFileLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileLoadDlg::IDD, pParent)
{
	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;
	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );

	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\loadingProcessBar01.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	imagePtr->SetPosiX( 60 );
	imagePtr->SetPosiY( 60 );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\loadingProcessBar00.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	imagePtr->SetPosiX( 60 );
	imagePtr->SetPosiY( 60 );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_LEFT_OR_TOP ) );
	textPtr->SetText( L"Loading file, please wait!" );
	textPtr->SetSize( 12 );
	textPtr->SetWidth( 500 );
	textPtr->SetHeight( 50 );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 20 );
 	mpMainArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	CHE_WDM_AppearAnimation animation;
	animation.SetTarget( mpMainArea );
	animation.SetAppear( imagePtr );
	animation.SetLoop( TRUE );
	CHE_WDM_AnimationData data;
	data.mPosiX = 60;
	data.mPosiY = 60;
	animation.SetState( data );

	data.mPosiX = 220;
	data.mPosiY = 60;
	animation.InsertFrames( 48, data );

	data.mPosiX = 60;
	data.mPosiY = 60;
	animation.InsertFrames( 48, data );

	mAnimations.StartAppearAnimation( animation );
}


CFileLoadDlg::~CFileLoadDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
}


void CFileLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileLoadDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CFileLoadDlg::OnInitDialog()
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


void CFileLoadDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( theApp.mbLoadOver == true )
	{
		this->EndDialog( 0 );
	}
	mAnimations.Execute();
	CDialogEx::OnTimer(nIDEvent);
}


int CFileLoadDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	mTimerId = SetTimer( 1, 25, NULL );
	return 0;
}


void CFileLoadDlg::OnSize(UINT nType, int cx, int cy)
{
	mClientWidth = 500;
	mClientHeight = 150;

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


void CFileLoadDlg::OnPaint()
{
	CPaintDC dc(this);
	DrawMainArea();
}


void CFileLoadDlg::DrawMainArea(void)
{
	CWnd *		pWnd = GetDlgItem( IDC_LOADDLG_MAIN );
	CPaintDC	dc( pWnd );
	mpMainArea->OnDraw();
	dc.BitBlt( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), &mMemdc, 0, 0, SRCCOPY );
}


void CFileLoadDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CFileLoadDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CFileLoadDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CFileLoadDlg::OnOK()
{
	if( theApp.mbLoadOver )
	{
		CDialogEx::OnOK();
	}
}


void CFileLoadDlg::OnCancel()
{
	if( theApp.mbLoadOver )
	{
		CDialogEx::OnCancel();
	}
}


void CFileLoadDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	mpInterActive->Invalidate();
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}


BOOL CFileLoadDlg::OnEraseBkgnd(CDC* pDC)
{
	mpInterActive->Invalidate();
	return TRUE;
}

void CFileLoadDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer( mTimerId );
}