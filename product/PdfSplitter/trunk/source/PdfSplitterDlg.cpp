
// PdfSpliterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PdfSplitterDlg.h"
#include "FileLoadDlg.h"
#include "SelectionModeDlg.h"
#include "PageSelectionDlg.h"
#include "PagesSelectionDlg.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"

// #include "../../../../trunk//include/pdf/che_pdf_creator.h"
// #include "../../../../trunk//include/pdf/che_pdf_objclone.h"

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


CHE_WD_AppearPath * gpListItemHoverRect = NULL;
CHE_WD_AppearPath * gpListItemSelectRect = NULL;
CHE_WD_AppearPath * gpListItemLine = NULL;


DWORD WINAPI ThreadLoadFile( LPVOID lpParameter )
{
	theApp.LoadDocument();
	theApp.mpMainDlg->UpdateToolBtn();
	return 0;
}


static void EventBrowseBtnClick( CHE_WD_Area * pArea )
{
	static wchar_t fileName[1024], fileTitleName[1024];
	static OPENFILENAME ofn;
	ofn.lStructSize		= sizeof ( OPENFILENAME );
	ofn.hwndOwner		= theApp.mpMainDlg->GetSafeHwnd();
	ofn.lpstrFilter		= L"PDF Files(*.pdf)\0*.pdf\0\0";
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

	if ( GetOpenFileName( &ofn ) )
	{
		theApp.CloseDocument();

		theApp.mTargetFile = fileName;

		unsigned int i = 0;
		for ( i = theApp.mTargetFile.size()-1; i != 0; --i )
		{
			if ( L'.' == fileName[i] )
			{
				fileName[i++] = L'_';
				fileName[i++] = L'N';
				fileName[i++] = L'e';
				fileName[i++] = L'w';
				fileName[i++] = L'.';
				fileName[i++] = L'p';
				fileName[i++] = L'd';
				fileName[i++] = L'f';
				fileName[i++] = L'\0';
				break;
			}
		}
		theApp.mNewFile = fileName;
		CHE_WD_Appearance * pTmpAppear = theApp.mpMainDlg->mpTextBar2->GetBackGroundAppear();
		CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[1] );
		pTmpText->SetText(  theApp.mNewFile.c_str() );
		theApp.mpMainDlg->mpTextBar2->Refresh();

		pTmpAppear = theApp.mpMainDlg->mpFilePathText->GetBackGroundAppear();
		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
		pTmpText->SetText(  theApp.mTargetFile.c_str() );
		theApp.mpMainDlg->mpFilePathText->Refresh();
		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
		CFileLoadDlg loadDlg;
		loadDlg.DoModal();
		pTmpAppear = theApp.mpMainDlg->mpFilePageCountInfo->GetBackGroundAppear();
		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
		wchar_t tmpwstr[512];
		wsprintf( tmpwstr, L"总计：%d 页", theApp.mParser.GetPageCount() );
		pTmpText->SetText( tmpwstr );
		pTmpAppear = theApp.mpMainDlg->mpFileSizeInfo->GetBackGroundAppear();
		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
		if ( theApp.mpFileRead->GetSize() <= 10485 )
		{
			swprintf( tmpwstr, L"%4.2f KB", theApp.mpFileRead->GetSize() * 1.0 / 1024 ) ;
		}else{
			swprintf( tmpwstr, L"%4.2f MB", theApp.mpFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
		}
		pTmpText->SetText( tmpwstr );
		theApp.mpMainDlg->mpInterActive->Invalidate();

		CSelectionModeDlg selectModeDlg;
		unsigned int iRet = selectModeDlg.DoModal();
		switch ( iRet )
		{
		default:
		case 0:
			break;
		case 1:
			{
				CPageSelectionDlg pageDlg;
				if ( pageDlg.DoModal() == 1 )
				{
					theApp.mpMainDlg->UpdateList();
					theApp.mpMainDlg->UpdateToolBtn();
				}
				break;
			}
		case 2:
			{
				CPagesSelectionDlg pageDlg;
				if ( pageDlg.DoModal() == 1 )
				{
					theApp.mpMainDlg->UpdateList();
					theApp.mpMainDlg->UpdateToolBtn();
				}
			}
		}
	}
}


static void EventListItemClick( CHE_WD_Area * pArea )
{
	unsigned int iCount = theApp.mpMainDlg->mpListBoxItems->GetChildrenCount();
	CHE_WD_Area * pTmpArea = NULL;
	
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pTmpArea = theApp.mpMainDlg->mpListBoxItems->GetChild( i );
		if ( pTmpArea == pArea )
		{
			theApp.mpMainDlg->CancelSelection();
			theApp.mCurItem = i + 1;
			theApp.mpMainDlg->UpdateSelection();
			theApp.mpMainDlg->UpdateToolBtn();
			theApp.mpMainDlg->UpdateList();
			pArea->Refresh();
			break;
		}
	}
}


static void EventAddBtnClick( CHE_WD_Area * pArea )
{
	CSelectionModeDlg selectModeDlg;
	unsigned int iRet = selectModeDlg.DoModal();
	switch ( iRet )
	{
	default:
	case 0:
		break;
	case 1:
		{
			CPageSelectionDlg pageDlg;
			if ( pageDlg.DoModal() == 1 )
			{
				theApp.mpMainDlg->UpdateToolBtn();
				theApp.mpMainDlg->UpdateList();
			}
			break;
		}
	case 2:
		{
			CPagesSelectionDlg pageDlg;
			if ( pageDlg.DoModal() == 1 )
			{
				theApp.mpMainDlg->UpdateToolBtn();
				theApp.mpMainDlg->UpdateList();
			}
		}
	}
}


static void EventDelBtnClick( CHE_WD_Area * pArea )
{
	theApp.DelCurPageListItem();
}


static void EventUpBtnClick( CHE_WD_Area * pArea )
{
	theApp.UpCurPageListItem();
}


static void EventDownBtnClick( CHE_WD_Area * pArea )
{
	theApp.DownCurPagaListItem();
}


static void EventMoveScrollBox( CHE_WD_Area * pArea )
{
	static unsigned int point = 0;
	if ( pArea->GetPositionY() != point )
	{
		theApp.mfViewPoint = ( pArea->GetPositionY() - 165 ) * 1.0 /233;
		point = pArea->GetPositionY();
		theApp.mpMainDlg->UpdateList();
	}
}


static void EventFileSavePathBrowseBtnClick( CHE_WD_Area * pArea )
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
		CHE_WD_Appearance * pTmpAppear = theApp.mpMainDlg->mpTextBar2->GetBackGroundAppear();
		CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[1] );
		pTmpText->SetText(  theApp.mNewFile.c_str() );
		theApp.mpMainDlg->mpTextBar2->Refresh();
	}
}


static void EventStartBtn( CHE_WD_Area * pArea )
{
	if( ! theApp.mPageList.size() )
	{
		theApp.mpMainDlg->MessageBox( L"请先选着需要输出的页面！", L"错误", MB_OK | MB_ICONINFORMATION );
		return;
	}

	theApp.mpProcessDlg = new CProcessDlg;
	theApp.mpProcessDlg->DoModal();
	theApp.mpMainDlg->MessageBox(  L"分割完成！", L"消息", MB_OK | MB_ICONINFORMATION );
}


CPdfSpliterDlg::CPdfSpliterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPdfSpliterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mClientWidth = 0;
	mClientHeight = 0;

	if ( ! gpListItemHoverRect )
	{
		gpListItemHoverRect = new CHE_WD_AppearPath;
		gpListItemHoverRect->AddRect( 0, 0, 630, 30 );
		gpListItemHoverRect->SetOperator( APPEAR_PATH_FILL );
		gpListItemHoverRect->SetFillColor( 0x65EEEEEE );
	}
	if ( ! gpListItemSelectRect )
	{
		gpListItemSelectRect = new CHE_WD_AppearPath;
		gpListItemSelectRect->AddRect( 0, 0, 630, 30 );
		gpListItemSelectRect->SetOperator( APPEAR_PATH_FILL );
		gpListItemSelectRect->SetFillColor( 0x650000FF );
	}
	if ( ! gpListItemLine )
	{
		gpListItemLine = new CHE_WD_AppearPath;
		gpListItemLine->AddLine( 0, 31, 630, 31 );
		gpListItemLine->SetOperator( APPEAR_PATH_STROKE );
		gpListItemLine->SetStrokeColor( 0xFF88888888 );
		gpListItemLine->SetLineWidth( 1 );
	}

	CHE_WD_Appearance * pTmpApper = NULL;
	CHE_WD_AppearImage * pTmpImage = NULL;
	CHE_WD_AppearText * pTmpText = NULL;

	mpInterActive = new MyIHE_WD_InterActive( this, theApp.m_hInstance );
	mpMainArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpApper = new CHE_WD_Appearance;
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\background.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\line.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpImage->SetPositionX( 0 );
	pTmpImage->SetPositionY( 100 );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\line.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpImage->SetPositionX( 0 );
	pTmpImage->SetPositionY( 475 );
	pTmpApper->mItems.push_back( pTmpImage );
	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpTextBarTitle = new CHE_WD_Area( 0, 0, mpInterActive );
	mpTextBarTitle->SetWidth( 158 );
	mpTextBarTitle->SetHeight( 34 );
	mpTextBarTitle->SetPositionX( 15 );
	mpTextBarTitle->SetPositionY( 2 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textbarTitle.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_FIT );
	pTmpApper->mItems.push_back( pTmpImage );
	mpTextBarTitle->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( mpTextBarTitle );

	mpTextBar = new CHE_WD_Area( 0, 0, mpInterActive );
	mpTextBar->SetWidth( 653 );
	mpTextBar->SetHeight( 41 );
	mpTextBar->SetPositionX( 15 );
	mpTextBar->SetPositionY( 30 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textbar.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_FIT );
	pTmpApper->mItems.push_back( pTmpImage );
	mpTextBar->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( mpTextBar );

	mpFilePathText = new CHE_WD_Area( 0, 0, mpInterActive );
	mpFilePathText->SetWidth( 580 );
	mpFilePathText->SetHeight( 20 );
	mpFilePathText->SetPositionX( 30 );
	mpFilePathText->SetPositionY( 42 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpText = new CHE_WD_AppearText();
	pTmpText->SetSize( 16 );
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 0 );
	pTmpText->SetWidth( 580 );
	pTmpText->SetHeight( 20 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_LEFT );
	pTmpApper->mItems.push_back( pTmpText );
	mpFilePathText->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( mpFilePathText );

	mpBrowseBtn = new CHE_WD_Button( mpInterActive );
	mpBrowseBtn->SetWidth( 96 );
	mpBrowseBtn->SetHeight( 36 );
	mpBrowseBtn->SetPositionX( 575 );
	mpBrowseBtn->SetPositionY( 35 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\browseBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBrowseBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\browseBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBrowseBtn->SetMouseOverAppear( pTmpApper );
	mpBrowseBtn->SetMouseLButtonDownAppear( pTmpApper );

	mpBrowseBtn->SetClickEvent( EventBrowseBtnClick );
	mpMainArea->AppendChild( mpBrowseBtn );

	mpFilePageCountInfo = new CHE_WD_Area( 0, 0, mpInterActive );
	mpFilePageCountInfo->SetWidth( 150 );
	mpFilePageCountInfo->SetHeight( 15 );
	mpFilePageCountInfo->SetPositionX( 30 );
	mpFilePageCountInfo->SetPositionY( 80 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpText = new CHE_WD_AppearText();
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 0 );
	pTmpText->SetWidth( 150 );
	pTmpText->SetHeight( 15 );
	pTmpText->SetSize( 12 );
	pTmpText->SetColor( 0xFF333333 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_LEFT );
	pTmpApper->mItems.push_back( pTmpText );
	mpFilePageCountInfo->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( mpFilePageCountInfo );

	mpFileSizeInfo = new CHE_WD_Area( 0, 0, mpInterActive );
	mpFileSizeInfo->SetWidth( 150 );
	mpFileSizeInfo->SetHeight( 15 );
	mpFileSizeInfo->SetPositionX( 130 );
	mpFileSizeInfo->SetPositionY( 80 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpText = new CHE_WD_AppearText();
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 0 );
	pTmpText->SetWidth( 150 );
	pTmpText->SetHeight( 15 );
	pTmpText->SetSize( 12 );
	pTmpText->SetColor( 0xFF333333 );
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_LEFT );
	pTmpApper->mItems.push_back( pTmpText );
	mpFileSizeInfo->SetBackGroundAppear( pTmpApper );
	mpMainArea->AppendChild( mpFileSizeInfo );

	mpListBox = new CHE_WD_Area( 0, 0, mpInterActive );
	mpListBox->SetWidth( 650 );
	mpListBox->SetHeight( 345 );
	mpListBox->SetPositionX( 22 );
	mpListBox->SetPositionY( 120 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\listBox.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpListBox->SetBackGroundAppear( pTmpApper );

	mpListBoxItems = new CHE_WD_Area( 0, 0, mpInterActive );
	mpListBoxItems->SetWidth( 625 );
	mpListBoxItems->SetHeight( 290 );
	mpListBoxItems->SetPositionX( 25 );
	mpListBoxItems->SetPositionY( 160 );
	mpListBoxItems->EnableClip();
	mpListBox->AppendChild( mpListBoxItems );
	mpListBox->EnableClip();

	mpToolBtnAdd = new CHE_WD_Button( mpInterActive );
	mpToolBtnAdd->SetWidth( 20 );
	mpToolBtnAdd->SetHeight( 20 );
	mpToolBtnAdd->SetPositionX( 40 );
	mpToolBtnAdd->SetPositionY( 130 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn1.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnAdd->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn1Hover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnAdd->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnAdd->SetMouseLButtonDownAppear( pTmpApper );
	mpToolBtnAdd->SetClickEvent( EventAddBtnClick );
	mpToolBtnAdd->SetVisable( false );
	mpToolBtnAdd->SetEnable( false );
	mpListBox->AppendChild( mpToolBtnAdd );

	mpToolBtnDel = new CHE_WD_Button( mpInterActive );
	mpToolBtnDel->SetWidth( 20 );
	mpToolBtnDel->SetHeight( 20 );
	mpToolBtnDel->SetPositionX( 80 );
	mpToolBtnDel->SetPositionY( 130 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn2.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDel->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn2Hover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDel->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDel->SetMouseLButtonDownAppear( pTmpApper );
	mpToolBtnDel->SetVisable( false );
	mpToolBtnDel->SetEnable( false );
	mpToolBtnDel->SetClickEvent( EventDelBtnClick );
	mpListBox->AppendChild( mpToolBtnDel );

	mpToolBtnUp = new CHE_WD_Button( mpInterActive );
	mpToolBtnUp->SetWidth( 20 );
	mpToolBtnUp->SetHeight( 20 );
	mpToolBtnUp->SetPositionX( 120 );
	mpToolBtnUp->SetPositionY( 130 );
	mpToolBtnUp->SetClickEvent( EventUpBtnClick );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn3.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnUp->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn3Hover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnUp->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnUp->SetMouseLButtonDownAppear( pTmpApper );
	mpToolBtnUp->SetVisable( false );
	mpToolBtnUp->SetEnable( false );
	mpListBox->AppendChild( mpToolBtnUp );

	mpToolBtnDown = new CHE_WD_Button( mpInterActive );
	mpToolBtnDown->SetWidth( 20 );
	mpToolBtnDown->SetHeight( 20 );
	mpToolBtnDown->SetPositionX( 160 );
	mpToolBtnDown->SetPositionY( 130 );
	mpToolBtnDown->SetClickEvent( EventDownBtnClick );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn4.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDown->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\toolBarBtn4Hover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDown->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpApper->mItems.push_back( pTmpImage );
	mpToolBtnDown->SetMouseLButtonDownAppear( pTmpApper );
	mpToolBtnDown->SetVisable( false );
	mpToolBtnDown->SetEnable( false );
	mpListBox->AppendChild( mpToolBtnDown );

	mpMainArea->AppendChild( mpListBox );

	mpListScrollBar = new CHE_WD_Button( mpInterActive );
	mpListScrollBar->SetWidth( 17 );
	mpListScrollBar->SetHeight( 283 );
	mpListScrollBar->SetPositionX( 653 );
	mpListScrollBar->SetPositionY( 165 );

	CHE_WD_Area * pArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pArea->SetWidth( 5 );
	pArea->SetHeight( 283 );
	pArea->SetPositionX( 658 );
	pArea->SetPositionY( 165 );
	pTmpApper = new CHE_WD_Appearance;
	pTmpImage = new CHE_WD_AppearImage;
	pTmpImage->SetImageFile( L"images\\listBoxSrocllLine.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pArea->SetBackGroundAppear( pTmpApper );
	mpListScrollBar->AppendChild( pArea );

	CHE_WD_DragArea * pDragArea = new CHE_WD_DragArea( mpInterActive );
	pDragArea->SetWidth( 17 );
	pDragArea->SetHeight( 50 );
	pDragArea->SetPositionX( 653 );
	pDragArea->SetPositionY( 165 );
	pDragArea->SetRange( 653, 165, 653, 398 );
	pDragArea->SetDragEvent( EventMoveScrollBox );
	pTmpApper = new CHE_WD_Appearance;
	pTmpImage = new CHE_WD_AppearImage;
	pTmpImage->SetImageFile( L"images\\listBoxScrollBox.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pDragArea->SetBackGroundAppear( pTmpApper );
	mpListScrollBar->AppendChild( pDragArea );

	mpListScrollBar->SetVisable( false );

	mpMainArea->AppendChild( mpListScrollBar );

	mpStartBtn = new CHE_WD_Button( mpInterActive );
	mpStartBtn->SetWidth( 125 );
	mpStartBtn->SetHeight( 46 );
	mpStartBtn->SetPositionX( 540 );
	mpStartBtn->SetPositionY( 498 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\startBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpStartBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\startBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpStartBtn->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpApper->mItems.push_back( pTmpImage );
	mpStartBtn->SetMouseLButtonDownAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\startBtnDiable.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpStartBtn->SetDisableAppear( pTmpApper );
	mpStartBtn->SetEnable( false );
	mpStartBtn->SetClickEvent( EventStartBtn );

	mpMainArea->AppendChild( mpStartBtn );

	mpTextBar2 = new CHE_WD_Area( 0, 0, mpInterActive );
	mpTextBar2->SetPositionX( 21 );
	mpTextBar2->SetPositionY( 505 );
	mpTextBar2->SetWidth( 509 );
	mpTextBar2->SetHeight( 29 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textBar2.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpText = new CHE_WD_AppearText();
	pTmpText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_LEFT );
	pTmpText->SetPositionX( 10 );
	pTmpText->SetPositionY( 8 );
	pTmpText->SetWidth( 465 );
	pTmpText->SetHeight( 15 );
	pTmpText->SetSize( 12 );
	pTmpText->SetColor( 0xFF000000 );
	pTmpApper->mItems.push_back( pTmpText );
	mpTextBar2->SetBackGroundAppear( pTmpApper );
	CHE_WD_Button * pTmpBtn = new CHE_WD_Button( mpInterActive );
	mpTextBar2Btn = pTmpBtn;
	pTmpBtn->SetPositionX( 474 );
	pTmpBtn->SetPositionY( 505 );
	pTmpBtn->SetWidth( 51 );
	pTmpBtn->SetHeight( 29 );
	pTmpBtn->SetClickEvent( EventFileSavePathBrowseBtnClick );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textBar2Btn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textBar2BtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpBtn->SetMouseOverAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\textBar2BtnDisable.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpBtn->SetDisableAppear( pTmpApper );
	pTmpBtn->SetEnable( false );
	mpTextBar2->AppendChild( pTmpBtn );

	mpMainArea->AppendChild( mpTextBar2 );
}

void CPdfSpliterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPdfSpliterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_CMD_OPEN, &CPdfSpliterDlg::OnOpenCmd )
	ON_COMMAND(ID_CMD_CLOSE, &CPdfSpliterDlg::OnCloseCmd )
	ON_COMMAND(ID_CMD_QUIT, &CPdfSpliterDlg::OnQuitCmd )
END_MESSAGE_MAP()


// CPdfSpliterDlg message handlers

BOOL CPdfSpliterDlg::OnInitDialog()
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

	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

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

void CPdfSpliterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPdfSpliterDlg::OnPaint()
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
HCURSOR CPdfSpliterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CPdfSpliterDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


void CPdfSpliterDlg::OnTimer(UINT_PTR nIDEvent)
{
	mpMainArea->ExecuteFrame();	Invalidate(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


void CPdfSpliterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	mClientWidth = cx;
	mClientHeight = cy;

	mpMainArea->SetWidth( cx );
	mpMainArea->SetHeight( cy );

	CWnd * pWnd = GetDlgItem( IDC_MAIN );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}


void CPdfSpliterDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}


void CPdfSpliterDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CPdfSpliterDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPdfSpliterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPdfSpliterDlg::DrawMainArea()
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


void CPdfSpliterDlg::AppendListItem( const CListItem & item )
{
	CHE_WD_Button * pTmpItem = NULL;
	CHE_WD_Appearance * pAppear = NULL;
	CHE_WD_AppearText * pText = NULL;
	pTmpItem = new CHE_WD_Button( mpInterActive );
	pTmpItem->SetWidth( 605 );
	pTmpItem->SetHeight( 32 );
	pTmpItem->SetPositionX( 25 );
	pTmpItem->SetPositionY( 0 );
	pAppear = new CHE_WD_Appearance;
	pAppear->mItems.push_back( gpListItemHoverRect );
	pTmpItem->SetMouseOverAppear( pAppear );
	pAppear = new CHE_WD_Appearance;
	pAppear->mItems.push_back( gpListItemLine );
	pText = new CHE_WD_AppearText;
	pText->SetPositionX( 10 );
	pText->SetPositionY( 0 );
	pText->SetWidth( 200 );
	pText->SetHeight( 32 );
	pText->SetHoriAlign( APPEAR_TEXT_HALIGNMENT_LEFT );
	pText->SetVertAlign( APPEAR_TEXT_VALIGNMENT_CENTER );
	pText->SetSize( 12 );
	wchar_t tmpStr[128];
	switch( item.type )
	{
	case SINGLE_PAGE:
		{
			wsprintf( tmpStr, L"单页：第 %d 页", item.pageIndex );
			break;
		}
	case PAGE_RANGE:
		{
			wsprintf( tmpStr, L"多页：第 %d 页 到 第 %d 页", item.pageIndex, item.pageIndex + item.pageCount );
			break;
		}
	default:;
	}
	pText->SetText( tmpStr );
	pAppear->mItems.push_back( pText );
	pTmpItem->SetBackGroundAppear( pAppear );
	pTmpItem->SetClickEvent( EventListItemClick );
	mpListBoxItems->AppendChild( pTmpItem );
}


void CPdfSpliterDlg::DeleteListItem( unsigned int index )
{
	CHE_WD_Area * pTmp = mpListBoxItems->PopChild( index );
	if ( pTmp )
	{
		CHE_WD_Appearance * pTmpAppear = pTmp->GetBackGroundAppear();
		delete pTmpAppear->mItems[1];
		delete pTmpAppear;
		pTmpAppear = pTmp->GetMouseOverAppear();
		delete pTmpAppear;
		delete pTmp;
	}
}

void CPdfSpliterDlg::CancelSelection()
{
	CHE_WD_Area * pTmpArea = NULL;
	CHE_WD_Appearance * pTmpAppear = NULL;
	unsigned int iCount = mpListBoxItems->GetChildrenCount();
	if ( theApp.mCurItem != 0 )
	{
		for ( unsigned int i = 0; i < iCount; ++i )
		{
			if (  theApp.mCurItem == i + 1 )
			{
				pTmpArea = theApp.mpMainDlg->mpListBoxItems->GetChild( i );
				pTmpAppear = pTmpArea->GetBackGroundAppear();
				pTmpAppear->mItems.erase( pTmpAppear->mItems.begin() );
				break;
			}
		}
	}
}

void CPdfSpliterDlg::UpdateSelection(void)
{
	CHE_WD_Area * pTmpItem = NULL;
	unsigned int iCount = theApp.mItemCount;
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pTmpItem = mpListBoxItems->GetChild( i );
		if ( i + 1 == theApp.mCurItem )
		{
			CHE_WD_Appearance * pTmpAppear = pTmpItem->GetBackGroundAppear();
			pTmpAppear->mItems.insert( pTmpAppear->mItems.begin(), gpListItemSelectRect );
		}
	}
}

void CPdfSpliterDlg::UpdateList()
{
	CHE_WD_Area * pTmpItem = NULL;
	int offset = 0;
	int newPosition = 0;
	unsigned int iCount = theApp.mItemCount;
	if ( iCount > 9 )
	{
		mpListScrollBar->SetVisable( true );
		offset = (iCount - 9) * 32 * theApp.mfViewPoint;
	}else
	{
		mpListScrollBar->SetVisable( false );
	}

	CHE_WD_Button * pBtn = NULL;
	CHE_WD_Appearance * pAppear = NULL;
	CHE_WD_AppearText * pText = NULL;
	CListItem item;
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pBtn = (CHE_WD_Button*)( mpListBoxItems->GetChild( i ) );
		pAppear = pBtn->GetBackGroundAppear();
		pText = (CHE_WD_AppearText *)( pAppear->mItems[pAppear->mItems.size()-1] );
		item = theApp.mPageList[i];
		wchar_t tmpStr[128];
		switch( item.type )
		{
		case SINGLE_PAGE:
			{
				wsprintf( tmpStr, L"单页：第 %d 页", item.pageIndex );
				break;
			}
		case PAGE_RANGE:
			{
				wsprintf( tmpStr, L"多页：第 %d 页 到 第 %d 页", item.pageIndex, item.pageIndex + item.pageCount - 1 );
				break;
			}
		default:;
		}
		pText->SetText( tmpStr );
		newPosition = mpListBoxItems->GetPositionY() + i * 32 - offset;
		pTmpItem = mpListBoxItems->GetChild( i );
		pTmpItem->SetPositionY( newPosition );
		if ( ( newPosition > mpListBoxItems->GetPositionY() + 288 ) || ( newPosition < -32 ) )
		{
			pTmpItem->SetVisable( false );
		}else{
			pTmpItem->SetVisable( true );
		}
	}
	mpListBoxItems->Refresh();
}

void CPdfSpliterDlg::UpdateTargetFileArea()
{
	CHE_WD_Appearance * pTmpAppear = mpFilePathText->GetBackGroundAppear();
	CHE_WD_AppearText * pAppearText = (CHE_WD_AppearText*)( pTmpAppear->mItems[0] );
	pAppearText->SetText( theApp.mTargetFile.c_str() ); 
	mpFilePathText->Refresh();
}

void CPdfSpliterDlg::UpdateFileInfoArea()
{
	CHE_WD_Appearance * pTmpAppear = mpFileSizeInfo->GetBackGroundAppear();
	CHE_WD_AppearText * pAppearText = (CHE_WD_AppearText*)( pTmpAppear->mItems[0] );
	pAppearText->SetText( L"" );
	pTmpAppear = mpFilePageCountInfo->GetBackGroundAppear();
	pAppearText = (CHE_WD_AppearText*)( pTmpAppear->mItems[0] );
	pAppearText->SetText( L"" ); 
	mpFilePageCountInfo->Refresh();
}

void CPdfSpliterDlg::UpdateNewFileArea()
{
	CHE_WD_Appearance * pTmpAppear = mpTextBar2->GetBackGroundAppear();
	CHE_WD_AppearText * pAppearText = (CHE_WD_AppearText*)( pTmpAppear->mItems[1] );
	pAppearText->SetText( theApp.mTargetFile.c_str() );
	mpTextBar2->Refresh();
}

void CPdfSpliterDlg::UpdateToolBtn()
{
	if ( ! theApp.mbLoadOver )
	{
		mpToolBtnAdd->SetVisable( false );
		mpToolBtnAdd->SetEnable( false );
		mpToolBtnDel->SetVisable( false );
		mpToolBtnDel->SetEnable( false );
		mpToolBtnUp->SetVisable( false );
		mpToolBtnUp->SetEnable( false );
		mpToolBtnDown->SetVisable( false );
		mpToolBtnDown->SetEnable( false );
		mpStartBtn->SetEnable( false );
		mpTextBar2Btn->SetEnable( false );
	}
	else{
		mpToolBtnAdd->SetVisable( true );
		mpToolBtnAdd->SetEnable( true );
		mpToolBtnDel->SetVisable( false );
		mpToolBtnDel->SetEnable( false );
		mpToolBtnUp->SetVisable( false );
		mpToolBtnUp->SetEnable( false );
		mpToolBtnDown->SetVisable( false );
		mpToolBtnDown->SetEnable( false );
		mpTextBar2Btn->SetEnable( true );
		mpStartBtn->SetEnable( false );
		if ( theApp.mItemCount > 0 )
		{
			mpStartBtn->SetEnable( true );
		}
		if ( theApp.mCurItem != 0 )
		{
			mpToolBtnDel->SetVisable( true );
			mpToolBtnDel->SetEnable( true );
			mpToolBtnUp->SetVisable( true );
			mpToolBtnUp->SetEnable( true );
			mpToolBtnDown->SetVisable( true );
			mpToolBtnDown->SetEnable( true );
			if ( theApp.mCurItem == 1 )
			{
				mpToolBtnUp->SetVisable( false );
				mpToolBtnUp->SetEnable( false );
			}
			if ( theApp.mCurItem == theApp.mItemCount )
			{
				mpToolBtnDown->SetVisable( false );
				mpToolBtnDown->SetEnable( false );
			}
		}
	}
	mpToolBtnAdd->Refresh();
	mpToolBtnDel->Refresh();
	mpToolBtnDown->Refresh();
	mpToolBtnUp->Refresh();
	mpTextBar2->Refresh();
	mpStartBtn->Refresh();
}

void CPdfSpliterDlg::OnOK()
{
	//CDialogEx::OnOk();
}


void CPdfSpliterDlg::OnCancel()
{
	if ( MessageBox( L"是否确定要退出？", L"确定", MB_YESNO | MB_ICONQUESTION ) == IDYES )
	{
		EndDialog(0);
	}
}

void CPdfSpliterDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer( mTimerId );
	theApp.mParser.Close();
	if( theApp.mpFileRead )
	{
		HE_DestoryIHERead( theApp.mpFileRead );
		theApp.mpFileRead = NULL;
	}
}


BOOL CPdfSpliterDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}


void CPdfSpliterDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}

void CPdfSpliterDlg::OnOpenCmd()
{
	EventBrowseBtnClick( NULL );
}

void CPdfSpliterDlg::OnCloseCmd()
{
	theApp.CloseDocument();
}

void CPdfSpliterDlg::OnQuitCmd()
{
	EndDialog( 0 );
}