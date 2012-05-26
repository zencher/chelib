
// PdfMergerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "PdfMergerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPdfMergerDlg dialog




CPdfMergerDlg::CPdfMergerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPdfMergerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\line.png" );
	imagePtr->SetPosiX( 0 );
	imagePtr->SetPosiY( 455 );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	//headbar
	CHE_WDM_Area * pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 0 );
	pTmpArea->SetPosiY( 0 );
	pTmpArea->SetWidth( 695 );
	pTmpArea->SetHeight( 85 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\headbar.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_FIT );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpAddFileBtn = CHE_WDM_Button::Create( mpInterActive );
	mpAddFileBtn->SetPosiX( 28 );
	mpAddFileBtn->SetPosiY( 20 );
	mpAddFileBtn->SetWidth( 118 );
	mpAddFileBtn->SetHeight( 47 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\AddFileBtn.png" );
	mpAddFileBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\AddFileBtnHover.png" );
	mpAddFileBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	pTmpArea->AppendChild( mpAddFileBtn );

	mpOptionsBtn = CHE_WDM_Button::Create( mpInterActive );
	mpOptionsBtn->SetPosiX( 178 );
	mpOptionsBtn->SetPosiY( 20 );
	mpOptionsBtn->SetWidth( 118 );
	mpOptionsBtn->SetHeight( 47 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OptionsBtn.png" );
	mpOptionsBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OptionsBtnHover.png" );
	mpOptionsBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OptionsBtnDisable.png" );
	mpOptionsBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpArea->AppendChild( mpOptionsBtn );

	mpMainArea->AppendChild( pTmpArea );


	//toolbar
	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 92 );
	pTmpArea->SetWidth( 140 );
	pTmpArea->SetHeight( 20 );

	mpEditBtn = CHE_WDM_Button::Create( mpInterActive );
	mpEditBtn->SetPosiX( 28 );
	mpEditBtn->SetPosiY( 92 );
	mpEditBtn->SetWidth( 20 );
	mpEditBtn->SetHeight( 20 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtn.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtnHover.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtnDisable.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpArea->AppendChild( mpEditBtn );

	mpClearBtn = CHE_WDM_Button::Create( mpInterActive );
	mpClearBtn->SetPosiX( 68 );
	mpClearBtn->SetPosiY( 92 );
	mpClearBtn->SetWidth( 20 );
	mpClearBtn->SetHeight( 20 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtn.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtnHover.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtnDisable.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpArea->AppendChild( mpClearBtn );

	mpUpBtn = CHE_WDM_Button::Create( mpInterActive );
	mpUpBtn->SetPosiX( 108 );
	mpUpBtn->SetPosiY( 92 );
	mpUpBtn->SetWidth( 20 );
	mpUpBtn->SetHeight( 20 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtn.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtnHover.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtnDisable.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpArea->AppendChild( mpUpBtn );

	mpDownBtn = CHE_WDM_Button::Create( mpInterActive );
	mpDownBtn->SetPosiX( 148 );
	mpDownBtn->SetPosiY( 92 );
	mpDownBtn->SetWidth( 20 );
	mpDownBtn->SetHeight( 20 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtn.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtnHover.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtnDisable.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpArea->AppendChild( mpDownBtn );

	mpMainArea->AppendChild( pTmpArea );


	//listbox area
	CHE_WDM_Area * pListBox = CHE_WDM_Area::Create( mpInterActive );
	pListBox->SetPosiX( 28 );
	pListBox->SetPosiY( 120 );
	pListBox->SetWidth( 645 );
	pListBox->SetHeight( 310 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\listBox.png" );
	pListBox->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	
	//itemlist area
	mpItemList = CHE_WDM_Area::Create( mpInterActive );
	mpItemList->SetPosiX( 28 );
	mpItemList->SetPosiY( 120 );
	mpItemList->SetWidth( 624 );
	mpItemList->SetHeight( 310 );
	mpItemList->EnableClip();
	pListBox->AppendChild( mpItemList );

	//item area
	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 121 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\listboxline.png" );
	imagePtr->SetPosiX( 0 );
	imagePtr->SetPosiY( 49 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 171 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 221 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 271 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 321 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 371 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 50 );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );

	//list scroll bar
	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetPosiX( 653 );
	pTmpArea->SetPosiY( 120 );
	pTmpArea->SetWidth( 21 );
	pTmpArea->SetHeight( 310 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 9 );
	imagePtr->SetPosiY( 11 );
	imagePtr->SetImageFile( L"images\\listBoxSrocllLine.png" );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pListBox->AppendChild( pTmpArea );

	//scroll block
	CHE_WDM_DragArea * pScrollBlock = CHE_WDM_DragArea::Create( mpInterActive );
	pScrollBlock->SetPosiX( 652 );
	pScrollBlock->SetPosiY( 131 );
	pScrollBlock->SetWidth( 21 );
	pScrollBlock->SetHeight( 50 );
	pScrollBlock->SetRange( 652, 131, 652, 364 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 4 );
	imagePtr->SetImageFile( L"images\\listBoxScrollBox.png" );
	pScrollBlock->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pListBox->AppendChild( pScrollBlock );

	mpMainArea->AppendChild( pListBox );


	//filePath area
	mpFilePath = CHE_WDM_Area::Create( mpInterActive );
	mpFilePath->SetPosiX( 28 );
	mpFilePath->SetPosiY( 485 );
	mpFilePath->SetWidth( 500 );
	mpFilePath->SetHeight( 29 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\filePathBox.png" );
	mpFilePath->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpBrowseBtn = CHE_WDM_Button::Create( mpInterActive );
	mpBrowseBtn->SetPosiX( 470 );
	mpBrowseBtn->SetPosiY( 485 );
	mpBrowseBtn->SetWidth( 52 );
	mpBrowseBtn->SetHeight( 31 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\browseBtn.png" );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\browseBtnHover.png" );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\browseBtnDisable.png" );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpFilePath->AppendChild( mpBrowseBtn );
	mpMainArea->AppendChild( mpFilePath );


	//Start Btn Area
	mpStartBtn = CHE_WDM_Button::Create( mpInterActive );
	mpStartBtn->SetPosiX( 550 );
	mpStartBtn->SetPosiY( 478 );
	mpStartBtn->SetWidth( 125 );
	mpStartBtn->SetHeight( 46 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\StartBtn.png" );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\StartBtnHover.png" );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\StartBtnDisable.png" );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpMainArea->AppendChild( mpStartBtn );
}

void CPdfMergerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPdfMergerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPdfMergerDlg message handlers

BOOL CPdfMergerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPdfMergerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPdfMergerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		DrawMainArea();
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPdfMergerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPdfMergerDlg::DrawMainArea()
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


void CPdfMergerDlg::OnSize(UINT nType, int cx, int cy)
{
		CDialogEx::OnSize( nType, 700, 600 );

		mClientWidth = 700;
		mClientHeight = 600;

		mpMainArea->SetWidth( 700 );
		mpMainArea->SetHeight( 600 );

		this->MoveWindow( 0, 0, 700, 600 );

// 		CWnd * pWnd = GetDlgItem( IDC_MAIN );
// 		if ( pWnd )
// 		{
// 			pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
// 		}
}


void CPdfMergerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPdfMergerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPdfMergerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	mpMainArea->OnMouseLBUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}
