#include "stdafx.h"
#include "PdfSplitter.h"
#include "FileLoadDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CFileLoadDlg, CDialogEx)


CFileLoadDlg::CFileLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileLoadDlg::IDD, pParent)
{
	CHE_WD_Appearance * pTmpApper = NULL;
	CHE_WD_AppearImage * pTmpImage = NULL;
	CHE_WD_AppearText * pTmpText = NULL;
	mpInterActive = new MyIHE_WD_InterActive( this, theApp.m_hInstance );
	mpMainArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpApper = new CHE_WD_Appearance;
	pTmpImage = new CHE_WD_AppearImage;
	pTmpImage->SetImageFile( L"images\\background.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	pTmpApper->mItems.push_back( pTmpImage );

	pTmpImage = new CHE_WD_AppearImage;
	pTmpImage->SetImageFile( L"images\\loadingProcessBar01.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpImage->SetPositionX( 60 );
	pTmpImage->SetPositionY( 60 );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpImage = new CHE_WD_AppearImage;
	pTmpImage->SetImageFile( L"images\\loadingProcessBar00.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpImage->SetPositionX( 60 );
	pTmpImage->SetPositionY( 60 );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText;
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_CENTER );
	pTmpText->SetVertAlign(  APPEAR_TEXT_VALIGNMENT_TOP );
	pTmpText->SetText( L"Loading file, please wait!" );
	pTmpText->SetSize( 12 );
	pTmpText->SetWidth( 500 );
	pTmpText->SetHeight( 50 );
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 20 );
	pTmpApper->mItems.push_back( pTmpText );
 	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpAnimation = new CHE_WD_Animation;
	mpAnimation->SetTarget( pTmpImage );
	CHE_WD_AnimationData Data;
	Data.mPositionX = 60;
	Data.mPositionY = 60;
	mpAnimation->SetBeginState( Data );
	Data.mPositionX = 220;
	Data.mPositionY = 60;
	mpAnimation->SetEndState( Data );
	mpAnimation->SetFrames( 48 );
	mpMainArea->SetDefaultAnimation( mpAnimation );

	mTimerId = 0;
}


CFileLoadDlg::~CFileLoadDlg()
{
	CHE_WD_Appearance * pTmpAppear = mpMainArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear->mItems[2];
	delete pTmpAppear->mItems[3];
	delete pTmpAppear;
	delete mpAnimation;
	delete mpMainArea;
	delete mpInterActive;
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
	mpMainArea->ExecuteFrame();
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
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CFileLoadDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CFileLoadDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
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