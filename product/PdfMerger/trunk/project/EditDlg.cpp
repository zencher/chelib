// EditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "EditDlg.h"
#include "afxdialogex.h"


CEditDlg * gpDlg = NULL;

void EventDropBtn( CHE_WDM_Area * pArea )
{
	if ( gpDlg )
	{
		if ( gpDlg->mpDropListItems->IsVisable() )
		{
			gpDlg->mpDropListItems->SetVisable( FALSE );
			gpDlg->mpDropListItems->SetEnable( FALSE );
			/*pArea->GetParent()->ReleaseCapture();*/
		}else{
			gpDlg->mpDropListItems->SetVisable( TRUE );
			gpDlg->mpDropListItems->SetEnable( TRUE );
			/*pArea->GetParent()->SetCapture( pArea );*/
		}
		gpDlg->mpDropListItems->Refresh();
	}
}

void EventOkBtn( CHE_WDM_Area * pArea )
{

}

void EventCancelBtn( CHE_WDM_Area * pArea )
{
	if ( gpDlg )
	{
		gpDlg->EndDialog( 1 );
	}
}

void EventDropListItemClick( CHE_WDM_Area * pArea )
{
	CHE_ByteString name = pArea->GetName();
	if ( name == "All Pages" )
	{
		gpDlg->mDropListText->SetText( L"All Pages" );
	}else if ( name == "Single Page" )
	{
		gpDlg->mDropListText->SetText( L"Single Page" );
	}else if ( name == "Page Range" )
	{
		gpDlg->mDropListText->SetText( L"Page Range" );
	}else if ( name == "Even Pages" )
	{
		gpDlg->mDropListText->SetText( L"Even Pages" );
	}else{
		gpDlg->mDropListText->SetText( L"Odd Pages" );
	}
	pArea->OnMouseOut();
	gpDlg->mpDropListItems->SetVisable( FALSE );
	gpDlg->mpDropListItems->SetEnable( FALSE );
	gpDlg->mpDropList->Refresh();
	gpDlg->mpDropListItems->Refresh();

	CHE_WDM_AnimationData data;
	CHE_WDM_AreaAnimation animation;
	animation.SetTarget( gpDlg->mpSinglePageArea );
	animation.SetLoop( FALSE );
	data.mPosiX = 720;
	data.mPosiY = 120;
	animation.SetState( data );
	data.mPosiX = 20;
	data.mPosiY = 120;
	animation.InsertFrames( 24, data );
	
	gpDlg->mAnimationMgr.StartAreaAnimation( animation );
}

// CEditDlg 对话框

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent)
{
	gpDlg = this;

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\headbar.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	imagePtr->SetPosiX( -10 );
	imagePtr->SetPosiY( -15 );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	CHE_WDM_Area * pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetPosiX( 20 );
	pArea->SetPosiY( 15 );
	pArea->SetWidth( 600 );
	pArea->SetHeight( 30 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\FileIcon.png" );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 550 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 45 );
	textPtr->SetText( theApp.mList[theApp.mCurItem-1].fileName.c_str() );
	textPtr->SetSize( 14 );
	pArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	wchar_t tmpwstr[512];
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 160 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 45 );
	textPtr->SetPosiY( 25 );
	textPtr->SetSize( 12 );
	wsprintf( tmpwstr, L"Total：%d Page(s)  ", theApp.mList[ theApp.mCurItem-1 ].pageCount );
	textPtr->SetText( tmpwstr );
	pArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 240 );
	textPtr->SetHeight( 20 );
	textPtr->SetPosiX( 200 );
	textPtr->SetPosiY( 25 );
	textPtr->SetSize( 12 );
	wsprintf( tmpwstr, L"Total：%d Page(s)  ", theApp.mList[ theApp.mCurItem-1 ].pageCount );
	textPtr->SetText( tmpwstr );
	if ( theApp.mList[ theApp.mCurItem-1 ].pPDFFile->GetFileSize() <= 10485 )
	{
		swprintf( tmpwstr, L"%4.2f KB", theApp.mList[ theApp.mCurItem-1 ].pFileRead->GetSize() * 1.0 / 1024 ) ;
	}else{
		swprintf( tmpwstr, L"%4.2f MB", theApp.mList[ theApp.mCurItem-1 ].pFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
	}
	textPtr->SetText( tmpwstr );
	pArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	mpMainArea->AppendChild( pArea );

	//drop list
	pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetPosiX( 20 );
	pArea->SetPosiY( 80 );
	pArea->SetWidth( 222 );
	pArea->SetHeight( 25 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListBG.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mDropListText = CHE_WDM_AppearText::Create();
	mDropListText->SetColor( 0xFF000000 );
	mDropListText->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	mDropListText->SetWidth( 180 );
	mDropListText->SetHeight( 25 );
	mDropListText->SetPosiX( 21 );

	mDropListText->SetText( L"All Pages" );
	mDropListText->SetSize( 12 );
	pArea->AppendAppearItem( mDropListText, AREA_APPEAR_BACKGROUND );
	CHE_WDM_Button * pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 213 );
	pBtn->SetPosiY( 80 );
	pBtn->SetWidth( 29 );
	pBtn->SetHeight( 25 );
	pBtn->SetClickEvent( EventDropBtn );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );
	mpMainArea->AppendChild( pArea );
	mpDropList = pArea;

	//main area
	pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetPosiX( 0 );
	pArea->SetPosiY( 120 );
	pArea->SetWidth( 650 );
	pArea->SetHeight( 137 );

	mpMainArea->AppendChild( pArea );

	//Single page
	{
		mpSinglePageArea = CHE_WDM_Area::Create( mpInterActive );
		mpSinglePageArea->SetPosiX( 20 );
		mpSinglePageArea->SetPosiY( 120 );
		mpSinglePageArea->SetWidth( 589 );
		mpSinglePageArea->SetHeight( 137 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\mainBG.png" );
		imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		mpSinglePageArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

		mpSingleNumberBox = CHE_WDM_Area::Create( mpInterActive );
		mpSingleNumberBox->SetPosiX( 60 );
		mpSingleNumberBox->SetPosiY( 140 );
		mpSingleNumberBox->SetWidth( 71 );
		mpSingleNumberBox->SetHeight( 26 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\numberBox.png" );
		mpSingleNumberBox->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
		mpSinglePageArea->AppendChild( mpSingleNumberBox );

		CHE_WDM_Button * pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
		pTmpBtn->SetPosiX( 113 );
		pTmpBtn->SetPosiY( 140 );
		pTmpBtn->SetWidth( 18 );
		pTmpBtn->SetHeight( 11 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\numberBoxUpHover.png" );
		pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
		mpSingleNumberBox->AppendChild( pTmpBtn );
		pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
		pTmpBtn->SetPosiX( 113 );
		pTmpBtn->SetPosiY( 152 );
		pTmpBtn->SetWidth( 18 );
		pTmpBtn->SetHeight( 12 );
		imagePtr = CHE_WDM_AppearImage::Create();
		imagePtr->SetImageFile( L"images\\numberBoxDownHover.png" );
		pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
		mpSingleNumberBox->AppendChild( pTmpBtn );

		pArea->AppendChild( mpSinglePageArea );
	}

	//drop list items
	pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetPosiX( 20 );
	pArea->SetPosiY( 105 );
	pArea->SetWidth( 222 );
	pArea->SetHeight( 152 );
	pArea->SetVisable( FALSE );
	pArea->SetEnable( FALSE );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropList.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 20 );
	pBtn->SetPosiY( 106 );
	pBtn->SetWidth( 222 );
	pBtn->SetHeight( 30 );
	pBtn->SetName( "All Pages" );
	pBtn->SetClickEvent( EventDropListItemClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 180 );
	textPtr->SetHeight( 25 );
	textPtr->SetPosiX( 21 );
	textPtr->SetText( L"All Pages" );
	textPtr->SetSize( 12 );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListHover0.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 20 );
	pBtn->SetPosiY( 136 );
	pBtn->SetWidth( 222 );
	pBtn->SetHeight( 30 );
	pBtn->SetName( "Single Page" );
	pBtn->SetClickEvent( EventDropListItemClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 180 );
	textPtr->SetHeight( 25 );
	textPtr->SetPosiX( 21 );
	textPtr->SetText( L"Single Page" );
	textPtr->SetSize( 12 );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListHover1.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 20 );
	pBtn->SetPosiY( 166 );
	pBtn->SetWidth( 222 );
	pBtn->SetHeight( 30 );
	pBtn->SetName( "Page Range" );
	pBtn->SetClickEvent( EventDropListItemClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 180 );
	textPtr->SetHeight( 25 );
	textPtr->SetPosiX( 21 );
	textPtr->SetText( L"Page Range" );
	textPtr->SetSize( 12 );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListHover1.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 20 );
	pBtn->SetPosiY( 196 );
	pBtn->SetWidth( 222 );
	pBtn->SetHeight( 30 );
	pBtn->SetName( "Even Pages" );
	pBtn->SetClickEvent( EventDropListItemClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 180 );
	textPtr->SetHeight( 25 );
	textPtr->SetPosiX( 21 );
	textPtr->SetText( L"Even Pages" );
	textPtr->SetSize( 12 );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListHover1.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
		pBtn->AppendAppearItem( textPtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 20 );
	pBtn->SetPosiY( 226 );
	pBtn->SetWidth( 222 );
	pBtn->SetHeight( 30 );
	pBtn->SetName( "Odd Pages" );
	pBtn->SetClickEvent( EventDropListItemClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetColor( 0xFF000000 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 180 );
	textPtr->SetHeight( 25 );
	textPtr->SetPosiX( 21 );
	textPtr->SetText( L"Odd Pages" );
	textPtr->SetSize( 12 );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DropListHover2.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pBtn->AppendAppearItem( textPtr, AREA_APPEAR_MOUSEOVER );
	pArea->AppendChild( pBtn );

	mpDropListItems = pArea;
	mpMainArea->AppendChild( mpDropListItems );

	//ok btn
	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 410 );
	pBtn->SetPosiY( 270 );
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
	pBtn->SetPosiY( 270 );
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

	SetTimer( 0, 20, NULL );

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();

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
	CDialogEx::OnSize( nType, 630, 340 );

	if ( mClientWidth != 630 )
	{

		this->MoveWindow( 0, 0, 630, 340 );

		mClientWidth = 630;
		mClientHeight = 340;

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
