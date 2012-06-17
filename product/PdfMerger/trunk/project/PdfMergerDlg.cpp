
// PdfMergerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "PdfMergerDlg.h"
#include "EditDlg.h"
#include "FileLoadDlg.h"
#include "PasswordDlg.h"
#include "ProcessDlg.h"
#include "Welcome.h"
#include "OptionsDlg.h"
#include "OptionPropertyPage.h"
#include "OptionSecurity.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI ThreadLoadFile( LPVOID lpParameter )
{
	theApp.LoadDocument();
	return 0;
}

void EventAddFiles( CHE_WDM_Area * pArea )
{
	//构造打开文件对话框需要的数据结构
	static wchar_t fileName[1024*4], fileTitleName[1024*4];
	static OPENFILENAME ofn;
	ofn.lStructSize		= sizeof ( OPENFILENAME );
	ofn.hwndOwner		= theApp.m_pMainWnd->GetSafeHwnd();
	ofn.lpstrFilter		= L"PDF Files(*.pdf)\0*.pdf\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	= 0;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= fileName;
	ofn.nMaxFile		= 1024*4;
	ofn.lpstrFileTitle	= fileTitleName;
	ofn.nMaxFileTitle	= 1024*4;
	ofn.Flags			= OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.nFileOffset		= 16;
	ofn.nFileExtension	= 0;
	ofn.lCustData		= NULL;
	ofn.lpfnHook		= NULL;
	ofn.lpTemplateName	= NULL;

	memset( fileName, 0, 1024 * 4 );
	memset( fileTitleName, 0, 1024 * 4 );

	if ( GetOpenFileName( &ofn ) )
	{
		//处理获得的数据，将文件路径和文件保存备用

		std::wstring dirPath;
		std::wstring tmpStr;

		fileName[ofn.nFileOffset-1] = '\0';
		dirPath = fileName;
		dirPath += '\\';

		wchar_t * pWCHAR = fileName + ofn.nFileOffset;
		while ( true )
		{
			if ( *pWCHAR != '\0' )
			{
				tmpStr += *pWCHAR;
			}else{
				theApp.mFileNameToLoad.push_back( tmpStr );
				std::wstring tmp = dirPath;
				tmp += tmpStr;
				theApp.mFilePathToLoad.push_back( tmp );
				tmpStr.clear();
				
				if ( *(pWCHAR + 1) == '\0' )
				{
					break;
				}
			}
			++pWCHAR;
		}

		//根据文件名和文件路径进行加载和登记操作

		CFileLoadDlg loadDlg;
		theApp.mpLoadDlg = &loadDlg;
		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
		loadDlg.DoModal();
		theApp.mpLoadDlg = NULL;

		theApp.mpMainDlg->UpdateBtn();
	}
}

static void EventOptionBtnClick( CHE_WDM_Area * pArea )
{
	COptionsDlg dlg( L"Options" );
	dlg.DoModal();
}

void EventEditBtnClick( CHE_WDM_Area * pArea )
{
	CEditDlg dlg;
	if ( dlg.DoModal() == 0 )
	{
		theApp.mpMainDlg->CancelSelection();
		theApp.mpMainDlg->UpdateCurItem();
		theApp.mpMainDlg->UpdateSelection();
	}
}

void EventClearBtnClick( CHE_WDM_Area * pArea )
{
	theApp.ClearPageListItem();
}


void EventDelBtnClick( CHE_WDM_Area * pArea )
{
	theApp.DelCurPageListItem();
}


static void EventUpBtnClick( CHE_WDM_Area * pArea )
{
	theApp.UpCurPageListItem();
}


void EventDownBtnClick( CHE_WDM_Area * pArea )
{
	theApp.DownCurPagaListItem();
}

void EventListItemClick( CHE_WDM_Area * pArea )
{
	theApp.mpMainDlg->mpCurItem = pArea;
	unsigned int iCount = theApp.mpMainDlg->mpItemList->GetChildrenCount();
	CHE_WDM_Area * pTmpArea = NULL;

	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pTmpArea = theApp.mpMainDlg->mpItemList->GetChild( i );
		if ( pTmpArea == pArea )
		{
			theApp.mpMainDlg->CancelSelection();
			theApp.mCurItem = i + 1;
			theApp.mpMainDlg->UpdateSelection();
			theApp.mpMainDlg->UpdateBtn();
			theApp.mpMainDlg->UpdateList();
			pArea->Refresh();
			break;
		}
	}
}

void EventMoveScrollBox( CHE_WDM_Area * pArea )
{
	static unsigned int point = 0;
	if ( pArea->GetPosiY() != point )
	{
		theApp.mfViewPoint = ( pArea->GetPosiY() - 131 ) * 1.0 /233;
		point = pArea->GetPosiY();
		theApp.mpMainDlg->UpdateList();
	}
}

void EventStartBtnClick( CHE_WDM_Area * pArea )
{
	static wchar_t fileName[1024], fileTitleName[1024];
	static OPENFILENAME ofn;
	ofn.lStructSize		= sizeof ( OPENFILENAME );
	ofn.hwndOwner		= theApp.mpMainDlg->GetSafeHwnd();
	ofn.lpstrFilter		= L"PDF Files(*.pdf)\0*.pdf\0\0";
	ofn.lpstrDefExt		= L".pdf";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter	= 0;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= fileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrFileTitle	= fileTitleName;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.Flags			= OFN_NOCHANGEDIR ;
	ofn.nFileOffset		= 16 ;
	ofn.nFileExtension	= 0 ;
	ofn.lCustData		= NULL ;
	ofn.lpfnHook		= NULL ;
	ofn.lpTemplateName	= NULL ;

	if ( GetSaveFileName( &ofn ) )
	{
		theApp.mNewFile = fileName;

		theApp.mpProcessDlg = new CProcessDlg;
		theApp.mpProcessDlg->DoModal();
		delete theApp.mpProcessDlg;
		theApp.mpProcessDlg = NULL;
	}
}

void EventListItemDelBtnClick( CHE_WDM_Area * pArea )
{
	theApp.DelCurPageListItem();
}

void EventListItemMouseOver( CHE_WDM_Area * pArea )
{
	if ( theApp.mpMainDlg->mpDelBtn->GetParent() )
	{
		theApp.mpMainDlg->mpDelBtn->GetParent()->PopChild( 0 );
	}
	theApp.mpMouseOverItem = pArea;
	theApp.mpMainDlg->mpDelBtn->SetPosiX( pArea->GetPosiX() + 580 );
	theApp.mpMainDlg->mpDelBtn->SetPosiY( pArea->GetPosiY() + 15 );
	pArea->AppendChild( theApp.mpMainDlg->mpDelBtn );
	theApp.mpMainDlg->mpDelBtn->SetVisable( true );
	theApp.mpMainDlg->mpDelBtn->SetEnable( true );
	theApp.mpMainDlg->mpDelBtn->Refresh();
}

void EventListItemMouseOut( CHE_WDM_Area * pArea )
{
}

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
public:
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

CPdfMergerDlg::CPdfMergerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPdfMergerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mpCurItem = NULL;

	listItemLine = CHE_WDM_AppearPath::Create();
	listItemLine->AddLine( 0, 49, 630, 49 );
	listItemLine->SetOperator( APPEAR_PATH_STROKE );
	listItemLine->SetStrokeColor( 0x88888888 );
	listItemLine->SetLineWidth( 1 );

	listItemSelectRect = CHE_WDM_AppearPath::Create();
	listItemSelectRect->AddRect( 1, 1, 630, 48 );
	listItemSelectRect->SetOperator( APPEAR_PATH_FILL );
	listItemSelectRect->SetFillColor( 0xFF6fd15e );

	listIcon1 = CHE_WDM_AppearImage::Create();
	listIcon1->SetPosiX( 15 );
	listIcon1->SetPosiY( 12 );
	listIcon1->SetImageFile( L"images\\PageIcon.png" );

	listIcon2 = CHE_WDM_AppearImage::Create();
	listIcon2->SetPosiX( 12 );
	listIcon2->SetPosiY( 12 );
	listIcon2->SetImageFile( L"images\\PagesIcon.png" );

	listIcon3 = CHE_WDM_AppearImage::Create();
	listIcon3->SetPosiX( 10 );
	listIcon3->SetPosiY( 12 );
	listIcon3->SetImageFile( L"images\\EvenPagesIcon.png" );

	listIcon4 = CHE_WDM_AppearImage::Create();
	listIcon4->SetPosiX( 10 );
	listIcon4->SetPosiY( 12 );
	listIcon4->SetImageFile( L"images\\OddPagesIcon.png" );

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
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
	mpAddFileBtn->SetMouseLBUEvent( EventAddFiles );
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
	mpOptionsBtn->SetMouseLBUEvent( EventOptionBtnClick );
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
	pTmpArea->SetWidth( 300 );
	pTmpArea->SetHeight( 20 );

	mpEditBtn = CHE_WDM_Button::Create( mpInterActive );
	mpEditBtn->SetPosiX( 28 );
	mpEditBtn->SetPosiY( 92 );
	mpEditBtn->SetWidth( 20 + 35 );
	mpEditBtn->SetHeight( 20 );
	mpEditBtn->SetMouseLBUEvent( EventEditBtnClick );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtn.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtnHover.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\EditBtnDisable.png" );
	mpEditBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"Edit" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 25 );
	textPtr->SetPosiY( 0 );
	textPtr->SetColor( 0xFF222222 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 40 );
	textPtr->SetHeight( 20 );
	mpEditBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	pTmpArea->AppendChild( mpEditBtn );

	mpClearBtn = CHE_WDM_Button::Create( mpInterActive );
	mpClearBtn->SetPosiX( 68 + 35 );
	mpClearBtn->SetPosiY( 92 );
	mpClearBtn->SetWidth( 20 + 35 );
	mpClearBtn->SetHeight( 20 );
	mpClearBtn->SetMouseLBUEvent( EventClearBtnClick );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtn.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtnHover.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\ClearBtnDisable.png" );
	mpClearBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"Clear" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 25 );
	textPtr->SetPosiY( 0 );
	textPtr->SetColor( 0xFF222222 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 40 );
	textPtr->SetHeight( 20 );
	mpClearBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	pTmpArea->AppendChild( mpClearBtn );

	mpUpBtn = CHE_WDM_Button::Create( mpInterActive );
	mpUpBtn->SetPosiX( 108 + 70 );
	mpUpBtn->SetPosiY( 92 );
	mpUpBtn->SetWidth( 20 + 35 );
	mpUpBtn->SetHeight( 20 );
	mpUpBtn->SetMouseLBUEvent( EventUpBtnClick );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtn.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtnHover.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\UpBtnDisable.png" );
	mpUpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"Up" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 25 );
	textPtr->SetPosiY( 0 );
	textPtr->SetColor( 0xFF222222 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 40 );
	textPtr->SetHeight( 20 );
	mpUpBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	pTmpArea->AppendChild( mpUpBtn );

	mpDownBtn = CHE_WDM_Button::Create( mpInterActive );
	mpDownBtn->SetPosiX( 148 + 105 );
	mpDownBtn->SetPosiY( 92 );
	mpDownBtn->SetWidth( 20 + 35 );
	mpDownBtn->SetHeight( 20 );
	mpDownBtn->SetMouseLBUEvent( EventDownBtnClick );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtn.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtnHover.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\DownBtnDisable.png" );
	mpDownBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetText( L"Down" );
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 25 );
	textPtr->SetPosiY( 0 );
	textPtr->SetColor( 0xFF222222 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_LEFT_OR_TOP, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetWidth( 40 );
	textPtr->SetHeight( 20 );
	mpDownBtn->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
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
	mpItemList->SetPosiY( 125 );
	mpItemList->SetWidth( 624 );
	mpItemList->SetHeight( 295 );
	mpItemList->EnableClip();
	pListBox->AppendChild( mpItemList );

	//list scroll bar
	mpScrollBar = CHE_WDM_Area::Create( mpInterActive );
	mpScrollBar->SetPosiX( 653 );
	mpScrollBar->SetPosiY( 120 );
	mpScrollBar->SetWidth( 21 );
	mpScrollBar->SetHeight( 310 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 9 );
	imagePtr->SetPosiY( 11 );
	imagePtr->SetImageFile( L"images\\listBoxSrocllLine.png" );
	mpScrollBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pListBox->AppendChild( mpScrollBar );

	//scroll block
	CHE_WDM_DragArea * pScrollBlock = CHE_WDM_DragArea::Create( mpInterActive );
	pScrollBlock->SetPosiX( 652 );
	pScrollBlock->SetPosiY( 131 );
	pScrollBlock->SetWidth( 21 );
	pScrollBlock->SetHeight( 50 );
	pScrollBlock->SetRange( 652, 131, 652, 364 );
	pScrollBlock->SetDragEvent( EventMoveScrollBox );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 4 );
	imagePtr->SetImageFile( L"images\\listBoxScrollBox.png" );
	pScrollBlock->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpScrollBar->AppendChild( pScrollBlock );

	mpMainArea->AppendChild( pListBox );

	mpDelBtn = CHE_WDM_Button::Create( mpInterActive );
	mpDelBtn->SetPosiX( 0 );
	mpDelBtn->SetPosiY( 0 );
	mpDelBtn->SetWidth( 24 );
	mpDelBtn->SetHeight( 24 );
 	mpDelBtn->SetVisable( false );
 	mpDelBtn->SetEnable( false );
	mpDelBtn->SetMouseLBUEvent( EventListItemDelBtnClick ); 
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\delBtn.png" );
	mpDelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\delBtnHover.png" );
	mpDelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

// 	mpTextBox = CHE_WDM_TextBox::Create( 400, 20, mpInterActive );
// 	mpTextBox->SetPosiX( 28 );
// 	mpTextBox->SetPosiY( 460 );
// 	mpTextBox->SetWidth( 400 );
// 	mpTextBox->SetHeight( 20 );
// 	mpMainArea->AppendChild( mpTextBox );


	//Start Btn Area
	mpStartBtn = CHE_WDM_Button::Create( mpInterActive );
	mpStartBtn->SetPosiX( 550 );
	mpStartBtn->SetPosiY( 440 );
	mpStartBtn->SetWidth( 125 );
	mpStartBtn->SetHeight( 46 );
	mpStartBtn->SetMouseLBUEvent( EventStartBtnClick );
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
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_ADDFILE, &CPdfMergerDlg::OnAddfile)
	ON_COMMAND(ID_FILE_QUIT, &CPdfMergerDlg::OnFileQuit)
	ON_COMMAND(ID_HELP_ABOUT, &CPdfMergerDlg::OnHelpAbout)
	ON_COMMAND(ID_HELP_BUY, &CPdfMergerDlg::OnHelpBuy)
	ON_COMMAND(ID_HELP_HOWTOUSE, &CPdfMergerDlg::OnHelpHowtouse)
	ON_COMMAND(ID_HELP_REGISTER, &CPdfMergerDlg::OnHelpRegister)
	ON_COMMAND(ID_HELP_OFFICALWEBSITE, &CPdfMergerDlg::OnHelpOfficalwebsite)
	ON_WM_DROPFILES()
	ON_WM_CHAR()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CPdfMergerDlg message handlers

BOOL CPdfMergerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if ( theApp.mbRegister == false )
	{
		CString str;
		GetWindowText( str );
		str += " (Unregistered)";
		this->SetWindowText( str );
	}

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	mpMainArea->OnDraw();

	UpdateBtn();
	UpdateList();

	SetTimer( 0, 500, NULL );

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
	CDialogEx::OnSize( nType, 700, 545 );

	if ( mClientWidth != 700 )
	{
		mClientWidth = 700;
		mClientHeight = 545;

		mpMainArea->SetWidth( 700 );
		mpMainArea->SetHeight( 545 );

		this->MoveWindow( 0, 0, 700, 545 );

		CWnd * pWnd = GetDlgItem( IDC_MAIN );
		if ( pWnd )
		{
			pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
		}
	}
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

void CPdfMergerDlg::SetWindowsTitleNormal()
{
	SetWindowText( L"Peroit PDF Merger" );
}

void CPdfMergerDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CPdfMergerDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}

void CPdfMergerDlg::AppendListItem( const CListItem & item )
{
	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;
	CHE_WDM_MouseEventBtn * pTmpArea = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	pTmpArea->SetPosiX( 28 );
	pTmpArea->SetPosiY( 121 );
	pTmpArea->SetWidth( 624 );
	pTmpArea->SetHeight( 49 );
 	pTmpArea->SetMouseOverEvent( EventListItemMouseOver );
 	pTmpArea->SetMouseLBDEvent( EventListItemClick );
	pTmpArea->SetDBClickEvent( EventEditBtnClick );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 55 );
	textPtr->SetPosiY( 18 );
	textPtr->SetWidth( 380 );
	textPtr->SetHeight( 15 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_CENTER ) );
	textPtr->SetText( item.fileName.c_str() );	
	pTmpArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 460 );
	textPtr->SetPosiY( 18 );
	textPtr->SetWidth( 150 );
	textPtr->SetHeight( 15 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_CENTER ) );
	wchar_t tmpStr[128];
	switch( item.type )
	{
	case ALL_PAGES:
		{
			pTmpArea->AppendAppearItem( listIcon2, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"All Pages" );
			break;
		}
	case SINGLE_PAGE:
		{
			pTmpArea->AppendAppearItem( listIcon1, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"Page : %d", item.pageIndex );
			break;
		}
	case PAGE_RANGE:
		{
			pTmpArea->AppendAppearItem( listIcon2, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"Pages : %d-%d", item.pageIndex, item.pageIndex + item.pageCount - 1 );
			break;
		}
	case EVEN_PAGES:
		{
			pTmpArea->AppendAppearItem( listIcon3, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"All Even Pages" );
			break;
		}
	case ODD_PAGES:
		{
			pTmpArea->AppendAppearItem( listIcon4, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"All Odd Pages" );
			break;
		}
	default:;
	}
	textPtr->SetText( tmpStr );	
	pTmpArea->AppendAppearItem( listItemLine, AREA_APPEAR_BACKGROUND );
	pTmpArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
	mpItemList->AppendChild( pTmpArea );
}

void CPdfMergerDlg::UpdateBtn()
{
	if ( ! theApp.mbLoadOver )
	{
		mpEditBtn->SetEnable( false );
		mpClearBtn->SetEnable( false );
		mpUpBtn->SetEnable( false );
		mpDownBtn->SetEnable( false );
		mpStartBtn->SetEnable( false );
	}else{
		mpEditBtn->SetEnable( false );
		mpClearBtn->SetEnable( false );
		mpUpBtn->SetEnable( false );
		mpDownBtn->SetEnable( false );
		mpStartBtn->SetEnable( false );
		if ( theApp.mList.size() > 0 )
		{
			mpClearBtn->SetEnable( true );
			mpStartBtn->SetEnable( true );
		}
		if ( theApp.mCurItem != 0 )
		{
			mpEditBtn->SetEnable( true );
			mpUpBtn->SetEnable( true );
			mpDownBtn->SetEnable( true );
			if ( theApp.mCurItem == 1 )
			{
				mpUpBtn->SetEnable( false );
			}
			if ( theApp.mCurItem == theApp.mList.size() )
			{
				mpDownBtn->SetEnable( false );
			}
		}
	}
	mpEditBtn->Refresh();
	mpClearBtn->Refresh();
	mpUpBtn->Refresh();
	mpDownBtn->Refresh();
	mpStartBtn->Refresh();
}

void CPdfMergerDlg::UpdateList()
{
	CHE_WDM_Area * pTmpItem = NULL;
	int offset = 0;
	int newPosition = 0;
	unsigned int iCount = theApp.mList.size();
	if ( iCount > 6 )
	{
		mpScrollBar->SetVisable( true );
		offset = (iCount - 6) * 47 * theApp.mfViewPoint;
	}else
	{
		mpScrollBar->SetVisable( false );
	}
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		newPosition = mpItemList->GetPosiY() + i * 49 - offset;
		pTmpItem = mpItemList->GetChild( i );
		pTmpItem->SetPosiY( newPosition );
	}
	mpItemList->Refresh();
	mpScrollBar->Refresh();
}

void CPdfMergerDlg::DeleteListItem( unsigned int index )
{
	CHE_WDM_Area * pTmp = mpItemList->PopChild( index );
	if ( pTmp )
	{
		pTmp->GetAllocator()->Delete( pTmp );
	}
}

void CPdfMergerDlg::CancelSelection()
{
	CHE_WDM_Area * pTmpArea = NULL;
	unsigned int iCount = mpItemList->GetChildrenCount();
	if ( theApp.mCurItem != 0 )
	{
		for ( unsigned int i = 0; i < iCount; ++i )
		{
			if (  theApp.mCurItem == i + 1 )
			{
				pTmpArea = theApp.mpMainDlg->mpItemList->GetChild( i );
				pTmpArea->GetAppear().mBackground.erase(  pTmpArea->GetAppear().mBackground.begin() );
				break;
			}
		}
	}
}

void CPdfMergerDlg::UpdateSelection(void)
{
	CHE_WDM_Area * pTmpItem = NULL;
	unsigned int iCount = theApp.mList.size();
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pTmpItem = mpItemList->GetChild( i );
		if ( i + 1 == theApp.mCurItem )
		{
			pTmpItem->GetAppear().mBackground.insert( pTmpItem->GetAppear().mBackground.begin(), listItemSelectRect );
		}
	}
}

void CPdfMergerDlg::UpdateCurItem()
{
	if ( mpCurItem )
	{
		mpCurItem->GetAppear().mBackground.clear();

		CListItem * pItem = &( theApp.mList[ theApp.mCurItem - 1 ] );
		
		CHE_WDM_AppearTextPtr textPtr;
		textPtr = CHE_WDM_AppearText::Create();
		textPtr->SetSize( 12 );
		textPtr->SetPosiX( 55 );
		textPtr->SetPosiY( 18 );
		textPtr->SetWidth( 380 );
		textPtr->SetHeight( 15 );
		textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_CENTER ) );
		textPtr->SetText( pItem->fileName.c_str() );	
		mpCurItem->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );
		textPtr = CHE_WDM_AppearText::Create();
		textPtr->SetSize( 12 );
		textPtr->SetPosiX( 460 );
		textPtr->SetPosiY( 18 );
		textPtr->SetWidth( 150 );
		textPtr->SetHeight( 15 );
		textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_CENTER ) );
		wchar_t tmpStr[128];
		switch( pItem->type )
		{
		case ALL_PAGES:
			{
				mpCurItem->AppendAppearItem( listIcon2, AREA_APPEAR_BACKGROUND );
				wsprintf( tmpStr, L"All Pages" );
				break;
			}
		case SINGLE_PAGE:
			{
				mpCurItem->AppendAppearItem( listIcon1, AREA_APPEAR_BACKGROUND );
				wsprintf( tmpStr, L"Page : %d", pItem->pageIndex );
				break;
			}
		case PAGE_RANGE:
			{
				mpCurItem->AppendAppearItem( listIcon2, AREA_APPEAR_BACKGROUND );
				wsprintf( tmpStr, L"Pages : %d-%d", pItem->pageIndex, pItem->pageIndex + pItem->pageCount - 1 );
				break;
			}
		case EVEN_PAGES:
			{
				mpCurItem->AppendAppearItem( listIcon3, AREA_APPEAR_BACKGROUND );
				wsprintf( tmpStr, L"All Even Pages" );
				break;
			}
		case ODD_PAGES:
			{
				mpCurItem->AppendAppearItem( listIcon4, AREA_APPEAR_BACKGROUND );
				wsprintf( tmpStr, L"All Odd Pages" );
				break;
			}
		default:;
		}
		textPtr->SetText( tmpStr );	
		mpCurItem->AppendAppearItem( listItemLine, AREA_APPEAR_BACKGROUND );
		mpCurItem->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

		mpCurItem->Refresh();
	}
}


void CPdfMergerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLDBClick( point.x, point.y ); 
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CPdfMergerDlg::OnAddfile()
{
	EventAddFiles( mpAddFileBtn );
}


void CPdfMergerDlg::OnFileQuit()
{
	EndDialog( 0 );
}


void CPdfMergerDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CPdfMergerDlg::OnHelpBuy()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com/pdfMerger/Buy", NULL, NULL, SW_SHOWNORMAL );
}


void CPdfMergerDlg::OnHelpHowtouse()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com/pdfMerger/UserGuide", NULL, NULL, SW_SHOWNORMAL );
}


void CPdfMergerDlg::OnHelpRegister()
{
	CRegsitrationDlg dlg;
	if ( dlg.DoModal() == 0 )
	{
		SetWindowsTitleNormal();
	}
}


void CPdfMergerDlg::OnHelpOfficalwebsite()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com", NULL, NULL, SW_SHOWNORMAL );
}


void CPdfMergerDlg::OnDropFiles(HDROP hDropInfo)
{
	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );

	char fileName[1024];
	char SecFileName[5];
	wchar_t szFileName[1024];
	int pathlength; 

	for ( int i = 0; i < count; ++i ) 
	{ 
		pathlength = DragQueryFile( hDropInfo, i, NULL, 0 );
		if ( pathlength < 1024 )
		{
			DragQueryFile( hDropInfo, i, szFileName, 1024 );
			
			memset( fileName, 0, 1024 );
			WideCharToMultiByte( CP_ACP, 0, szFileName, -1, fileName, 1024, NULL, NULL );
			int tmpStrlen = strlen( fileName );
			strcpy( SecFileName, fileName + tmpStrlen - 4 );
			for ( int j = 0; j < 4; ++j )
			{
				SecFileName[j] = toupper( SecFileName[j] );
			}

			if ( strcmp( SecFileName, ".PDF" ) == 0 )
			{
				std::wstring tmpStr = szFileName;
				theApp.mFilePathToLoad.push_back( tmpStr );
				for ( size_t i = tmpStr.size()-1; i != 0; --i )
				{
					if ( L'\\' == szFileName[i] )
					{
						tmpStr = szFileName+i+1;
						theApp.mFileNameToLoad.push_back( tmpStr );
						break;
					}
				}
			}
		}
	}

	DragFinish( hDropInfo );

	if ( theApp.mFileNameToLoad.size() > 0 && theApp.mFilePathToLoad.size() > 0 )
	{
		CFileLoadDlg loadDlg;
		theApp.mpLoadDlg = &loadDlg;
		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
		loadDlg.DoModal();
		theApp.mpLoadDlg = NULL;
		theApp.mpMainDlg->UpdateBtn();
	}else{
		theApp.mFilePathToLoad.clear();
		theApp.mFileNameToLoad.clear();
	}

	CDialogEx::OnDropFiles(hDropInfo);
}


void CPdfMergerDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	mpTextBox->OnChar( nChar );
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}

void CPdfMergerDlg::OnTimer(UINT_PTR nIDEvent)
{
//	mpTextBox->OnTimer();
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CPdfMergerDlg::PreTranslateMessage(MSG* pMsg)
{
// 	if ( pMsg->message == WM_CHAR )
// 	{
// 		if ( pMsg->wParam > 32 )
// 		{
// 			this->OnChar( pMsg->wParam, 0, 0 );
// 		}
// 	}else if ( pMsg->message == WM_KEYDOWN )
// 	{
// 		this->OnKeyDown( pMsg->wParam, 0, 0 );
// 	}else if ( pMsg->message == WM_KEYUP )
// 	{
// 		this->OnKeyUp( pMsg->wParam, 0, 0 );
// 	}
// 	return FALSE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPdfMergerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// 	HE_INT32 keyCode = 0;
// 	switch( nChar )
// 	{
// 	case VK_LEFT:
// 		keyCode = KEYCODE_LEFT;
// 		break;
// 	case VK_RIGHT:
// 		keyCode = KEYCODE_RIGHT;
// 		break;
// 	case VK_BACK:
// 		keyCode = KEYCODE_BACK_SPACE;
// 	}
// 	if ( keyCode > 0 )
// 	{
// 		mpTextBox->OnKeyDown( keyCode );
// 	}
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CPdfMergerDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// 	HE_INT32 keyCode = 0;
// 	switch( nChar )
// 	{
// 	case VK_LEFT:
// 		keyCode = KEYCODE_LEFT;
// 		break;
// 	case VK_RIGHT:
// 		keyCode = KEYCODE_RIGHT;
// 		break;
// 	case VK_BACK:
// 		keyCode = KEYCODE_BACK_SPACE;
// 	}
// 	if ( keyCode > 0 )
// 	{
// 		mpTextBox->OnKeyUp( keyCode );
// 	}
	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}
