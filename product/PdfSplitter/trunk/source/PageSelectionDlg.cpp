// PageSelectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PageSelectionDlg.h"
#include "afxdialogex.h"


CPageSelectionDlg * gpDlg = NULL;

static void UpdataNumberBox()
{
	CHE_WD_Appearance * pTmpAppear = gpDlg->mpNumberBox->GetBackGroundAppear();
	CHE_WD_AppearItem * pTmpItem = pTmpAppear->mItems[1];
	CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText*)( pTmpItem );
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", gpDlg->mCurPage );
	pTmpText->SetText( tmpStr );
	gpDlg->mpNumberBox->Refresh();
}

static void UpdateScroll()
{
	CHE_WD_Area * pTmpArea = gpDlg->mpScrollBar->GetChild( 0 );
	pTmpArea->SetPositionX( 25 + ( gpDlg->mCurPage - 1 ) * 1.0 / ( gpDlg->mPageCount - 1 ) * 411 );
	gpDlg->mpScrollBar->Refresh();
}

static void EventUpBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mCurPage == gpDlg->mPageCount )
	{
		return;
	}
	gpDlg->mCurPage++;
	UpdataNumberBox();
	UpdateScroll();
}

static void EventDownBtnClick( CHE_WD_Area * pArea )
{
	if ( gpDlg->mCurPage == 1 )
	{
		return;
	}
	gpDlg->mCurPage--;
	UpdataNumberBox();
	UpdateScroll();
}

static void EventDragScroolBlock( CHE_WD_Area * pArea )
{
	unsigned int tmpPosi = pArea->GetPositionX();
	gpDlg->mCurPage = (unsigned int)( 1 + ( tmpPosi - 25 ) * 1.0 / ( 411 * 1.0 / (gpDlg->mPageCount - 1) ) );
	UpdataNumberBox();
}

static void EventCancelBtn( CHE_WD_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventOkBtn( CHE_WD_Area * pArea )
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

	CHE_WD_Appearance * pTmpApper = NULL;
	CHE_WD_AppearImage * pTmpImage = NULL;
	CHE_WD_AppearText * pTmpText = NULL;

	mpInterActive = new MyIHE_WD_InterActive( this, theApp.m_hInstance );
	mpMainArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\background.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText;
	pTmpText->SetColor( 0xFF000000 );
	pTmpText->SetSize( 12 );
	pTmpText->SetText( L"选取  第" );
	pTmpText->SetPositionX( 35 );
	pTmpText->SetPositionY( 20 );
	pTmpText->SetWidth( 80 );
	pTmpText->SetHeight( 30 );
	pTmpApper->mItems.push_back( pTmpText );
	pTmpText = new CHE_WD_AppearText;
	pTmpText->SetColor( 0xFF000000 );
	pTmpText->SetSize( 12 );
	pTmpText->SetText( L"页" );
	pTmpText->SetPositionX( 150 );
	pTmpText->SetPositionY( 20 );
	pTmpText->SetWidth( 60 );
	pTmpText->SetHeight( 30 );
	pTmpApper->mItems.push_back( pTmpText );
	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpNumberBox = new CHE_WD_Area( 45, 24, mpInterActive );
	mpNumberBox->SetPositionX( 100 );
	mpNumberBox->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBox.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText();
	wchar_t tmpStr[16];
	wsprintf( tmpStr, L"%d", mCurPage );
	pTmpText->SetText( tmpStr );
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 0 );
	pTmpText->SetWidth( 45 );
	pTmpText->SetHeight( 24 );
	pTmpText->SetSize( 12 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_RIGHT );
	pTmpText->SetVertAlign( APPEAR_TEXT_VALIGNMENT_CENTER );
	pTmpText->SetColor( 0xFFFFFFFF );
	mpNumberBox->SetBackGroundAppear( pTmpApper );
	pTmpApper->mItems.push_back( pTmpText );
	mpMainArea->AppendChild( mpNumberBox );

	CHE_WD_Button * pArea = new CHE_WD_Button( mpInterActive );
	pArea->SetWidth( 18 );
	pArea->SetHeight( 11 );
	pArea->SetPositionX( 152 );
	pArea->SetPositionY( 20 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetBackGroundAppear( pTmpApper );
	pArea->SetClickEvent( EventUpBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxUpBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pArea );

	pArea = new CHE_WD_Button( mpInterActive );
	pArea->SetWidth( 18 );
	pArea->SetHeight( 13 );
	pArea->SetPositionX( 152 );
	pArea->SetPositionY( 31 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtn.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetBackGroundAppear( pTmpApper );
	pArea->SetClickEvent( EventDownBtnClick );

	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\numberBoxDownBtnHover.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetMouseOverAppear( pTmpApper );

	mpMainArea->AppendChild( pArea );

	mpScrollBar = new CHE_WD_Area( 441, 10, mpInterActive );
	mpScrollBar->SetPositionX( 25 );
	mpScrollBar->SetPositionY( 65 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\Scroll.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpScrollBar->SetBackGroundAppear( pTmpApper );
	CHE_WD_DragArea * pTmpDragArea = new CHE_WD_DragArea( mpInterActive );
	pTmpDragArea->SetWidth( 30 );
	pTmpDragArea->SetHeight( 14 );
	pTmpDragArea->SetPositionX( 25 );
	pTmpDragArea->SetPositionY( 63 );
	pTmpDragArea->SetRange( 25, 63, 436, 63 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\ScrollBlock.png" ); 
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpDragArea->SetBackGroundAppear( pTmpApper );
	pTmpDragArea->SetDragEvent( EventDragScroolBlock );
	mpScrollBar->AppendChild( pTmpDragArea );

	mpMainArea->AppendChild( mpScrollBar );

	mpOkBtn = new CHE_WD_Button( mpInterActive );
	mpOkBtn->SetWidth( 88 );
	mpOkBtn->SetHeight( 27 );
	mpOkBtn->SetPositionX( 280 );
	mpOkBtn->SetPositionY( 100 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\OkBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpOkBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\OkBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpOkBtn->SetMouseOverAppear( pTmpApper );
	mpOkBtn->SetClickEvent( EventOkBtn );
	mpMainArea->AppendChild( mpOkBtn );

	mpCancelBtn = new CHE_WD_Button( mpInterActive );
	mpCancelBtn->SetWidth( 88 );
	mpCancelBtn->SetHeight( 27 );
	mpCancelBtn->SetPositionX( 380 );
	mpCancelBtn->SetPositionY( 100 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\CancelBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetMouseOverAppear( pTmpApper );
	mpCancelBtn->SetClickEvent( EventCancelBtn );
	mpMainArea->AppendChild( mpCancelBtn );
}

CPageSelectionDlg::~CPageSelectionDlg()
{
	//销毁主区域背景
	CHE_WD_Appearance * pTmpAppear = mpMainArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear->mItems[2];
	delete pTmpAppear;

	//数值框
	CHE_WD_Area * pTmpArea = mpMainArea->GetChild( 0 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear->mItems[1];
	delete pTmpAppear;
	delete pTmpArea;

	//数值框up按钮
	pTmpArea = mpMainArea->GetChild( 1 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//数值框down按钮
	pTmpArea = mpMainArea->GetChild( 2 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//scroll bar
	pTmpArea = mpMainArea->GetChild( 3 );
	CHE_WD_Area * pTmpArea2 = pTmpArea->GetChild( 0 );
	pTmpAppear = pTmpArea2->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea2;
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//ok btn
	pTmpArea = mpMainArea->GetChild( 4 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	//cancel btn
	pTmpArea = mpMainArea->GetChild( 5 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	delete mpMainArea;
	delete mpInterActive;
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
	Invalidate(TRUE);
}

void CPageSelectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CPageSelectionDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CPageSelectionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
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
