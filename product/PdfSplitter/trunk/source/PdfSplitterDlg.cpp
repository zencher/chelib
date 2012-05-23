
// PdfSpliterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PdfSplitterDlg.h"
#include "Welcome.h"
#include "FileLoadDlg.h"
#include "SelectionModeDlg.h"
#include "PageSelectionDlg.h"
#include "PagesSelectionDlg.h"
#include "ProcessDlg.h"
#include "PasswordDlg.h"
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
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

CHE_WDM_AppearPathPtr gpListItemHoverRect;
CHE_WDM_AppearPathPtr gpListItemSelectRect;
CHE_WDM_AppearPathPtr gpListItemLine;

CHE_WDM_AppearImagePtr gpListItemIcon1;
CHE_WDM_AppearImagePtr gpListItemIcon2;


DWORD WINAPI ThreadLoadFile( LPVOID lpParameter )
{
	theApp.LoadDocument();
	return 0;
}

static void EventBrowseBtnClick( CHE_WDM_Area * pArea )
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
		CHE_WDM_AppearTextPtr textPtr = theApp.mpMainDlg->mpTextBar2->GetAppear().mBackground[1].GetTextPtr();
		textPtr->SetText(  theApp.mNewFile.c_str() );
		theApp.mpMainDlg->mpTextBar2->Refresh();

		textPtr = theApp.mpMainDlg->mpFilePathText->GetAppear().mBackground[0].GetTextPtr();
		textPtr->SetText(  theApp.mTargetFile.c_str() );
		theApp.mpMainDlg->mpFilePathText->Refresh();

		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
		CFileLoadDlg loadDlg;
		loadDlg.DoModal();

		if ( theApp.mbLoadError )
		{
			theApp.mpProcessDlg->MessageBox( L"This is not a PDF Files ", L"Error", MB_OK | MB_ICONERROR );
			theApp.mbLoadError = false;
			return;
		}

		theApp.mpMainDlg->UpdateToolBtn();

		CHE_ByteString str;
		bool bPasswordError = false;
		str = "";
		while ( ! theApp.mFile.Authenticate( str ) )
		{
			CPasswordDlg dlg;
			if ( bPasswordError )
			{
				dlg.SetErrorFlag();
			}
			if ( dlg.DoModal() == 1 )
			{
				theApp.CloseDocument();
				return;
			}
			bPasswordError = true;
			str = theApp.mPasswrod.c_str();
		}

		textPtr = theApp.mpMainDlg->mpFilePageCountInfo->GetAppear().mBackground[0].GetTextPtr();
		wchar_t tmpwstr[512];
		wsprintf( tmpwstr, L"Total：%d Page(s)", theApp.mpDocument->GetPageCount() );
		textPtr->SetText( tmpwstr );

		textPtr = theApp.mpMainDlg->mpFileSizeInfo->GetAppear().mBackground[0].GetTextPtr();
		if ( theApp.mpFileRead->GetSize() <= 10485 )
		{
			swprintf( tmpwstr, L"%4.2f KB", theApp.mpFileRead->GetSize() * 1.0 / 1024 ) ;
		}else{
			swprintf( tmpwstr, L"%4.2f MB", theApp.mpFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
		}
		textPtr->SetText( tmpwstr );
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
				break;
			}
		case 3:
			{
				CListItem listItem;
				listItem.type = SINGLE_PAGE;
				for ( size_t i = 0; i < theApp.mpPageTree->GetPageCount(); i+=2 )
				{
					listItem.pageIndex = i+1;
					listItem.pageCount = 1;
					theApp.AddPageListItem( listItem );
				}
				theApp.mpMainDlg->UpdateList();
				theApp.mpMainDlg->UpdateToolBtn();
				break;
			}
		case 4:
			{
				CListItem listItem;
				listItem.type = SINGLE_PAGE;
				for ( size_t i = 1; i < theApp.mpPageTree->GetPageCount(); i+=2 )
				{
					listItem.pageIndex = i+1;
					listItem.pageCount = 1;
					theApp.AddPageListItem( listItem );
				}
				theApp.mpMainDlg->UpdateList();
				theApp.mpMainDlg->UpdateToolBtn();
				break;
			}
		}
	}

	theApp.mpMainDlg->mpBrowseBtn->OnMouseOut();
}


static void EventListItemClick( CHE_WDM_Area * pArea )
{
	unsigned int iCount = theApp.mpMainDlg->mpListBoxItems->GetChildrenCount();
	CHE_WDM_Area * pTmpArea = NULL;
	
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


static void EventAddBtnClick( CHE_WDM_Area * pArea )
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
			break;
		}
	case 3:
		{
			CListItem listItem;
			listItem.type = SINGLE_PAGE;
			for ( size_t i = 0; i < theApp.mpPageTree->GetPageCount(); i+=2 )
			{
				listItem.pageIndex = i+1;
				listItem.pageCount = 1;
				theApp.AddPageListItem( listItem );
			}
			theApp.mpMainDlg->UpdateList();
			theApp.mpMainDlg->UpdateToolBtn();
			break;
		}
	case 4:
		{
			CListItem listItem;
			listItem.type = SINGLE_PAGE;
			for ( size_t i = 1; i < theApp.mpPageTree->GetPageCount(); i+=2 )
			{
				listItem.pageIndex = i+1;
				listItem.pageCount = 1;
				theApp.AddPageListItem( listItem );
			}
			theApp.mpMainDlg->UpdateList();
			theApp.mpMainDlg->UpdateToolBtn();
			break;
		}
	}
	pArea->OnMouseOut();
}


static void EventClearBtnClick( CHE_WDM_Area * pArea )
{
	theApp.ClearPageListItem();
}


static void EventDelBtnClick( CHE_WDM_Area * pArea )
{
	theApp.DelCurPageListItem();
}


static void EventUpBtnClick( CHE_WDM_Area * pArea )
{
	theApp.UpCurPageListItem();
}


static void EventDownBtnClick( CHE_WDM_Area * pArea )
{
	theApp.DownCurPagaListItem();
}


static void EventMoveScrollBox( CHE_WDM_Area * pArea )
{
	static unsigned int point = 0;
	if ( pArea->GetPosiY() != point )
	{
		theApp.mfViewPoint = ( pArea->GetPosiY() - 165 ) * 1.0 /233;
		point = pArea->GetPosiY();
		theApp.mpMainDlg->UpdateList();
	}
}


static void EventFileSavePathBrowseBtnClick( CHE_WDM_Area * pArea )
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
		CHE_WDM_AppearTextPtr textPtr = theApp.mpMainDlg->mpTextBar2->GetAppear().mBackground[1].GetTextPtr();
		textPtr->SetText(  theApp.mNewFile.c_str() );
		theApp.mpMainDlg->mpTextBar2->Refresh();
	}
}


static void EventStartBtn( CHE_WDM_Area * pArea )
{
	if( ! theApp.mPageList.size() )
	{
		theApp.mpMainDlg->MessageBox( L"Please select the pages want to split.", L"Error", MB_OK | MB_ICONINFORMATION );
		return;
	}

	theApp.mpProcessDlg = new CProcessDlg;
	theApp.mpProcessDlg->DoModal();
	delete theApp.mpProcessDlg;
	theApp.mpProcessDlg = NULL;
}

static void EventListItemMouseOver( CHE_WDM_Area * pArea )
{
	if ( pArea )
	{
		CHE_WDM_Area * pTmpBtn = pArea->GetChild( 0 );
		pTmpBtn->SetVisable( true );
		pTmpBtn->SetEnable( true );
		pArea->Refresh();
	}
}

static void EventListItemMouseOut( CHE_WDM_Area * pArea )
{
	if ( pArea )
	{
		CHE_WDM_Area * pTmpBtn = pArea->GetChild( 0 );
		pTmpBtn->SetEnable( false );
		pTmpBtn->SetVisable( false );
		pArea->Refresh();
	}
}

static void EventListItemDelBtnClick( CHE_WDM_Area * pArea )
{
	if ( pArea )
	{
		CHE_WDM_Area * pTarget = pArea->GetParent();
		CHE_WDM_Area * pTmp = NULL;

		if ( pTarget )
		{
			bool bFlag = false;
			unsigned int i = 0;
			unsigned int iCount = theApp.mpMainDlg->mpListBoxItems->GetChildrenCount();
			std::vector< CListItem >::iterator it = theApp.mPageList.begin();
			for ( ; i < iCount; ++i, ++it )
			{
				pTmp = theApp.mpMainDlg->mpListBoxItems->GetChild( i );
				if ( pTmp == pTarget )
				{
					bFlag = true;
					break;
				}
			}
			if ( bFlag )
			{
				pArea->ReleaseFocus();
				theApp.mpMainDlg->CancelSelection();
				theApp.mPageList.erase( it );
				--theApp.mItemCount;
				if ( theApp.mCurItem > theApp.mItemCount )
				{
					--theApp.mCurItem;
				}
				if ( theApp.mItemCount == 0 )
				{
					theApp.mCurItem = 0;
				}
				theApp.mpMainDlg->DeleteListItem( i );
				theApp.mpMainDlg->UpdateSelection();
				theApp.mpMainDlg->UpdateToolBtn();
				theApp.mpMainDlg->UpdateList();
			}
		}
	}
}


CPdfSpliterDlg::CPdfSpliterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPdfSpliterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mClientWidth = 0;
	mClientHeight = 0;

	if ( ! gpListItemHoverRect )
	{
		gpListItemHoverRect = CHE_WDM_AppearPath::Create();
		gpListItemHoverRect->AddRect( 0, 1, 630, 47 );
		gpListItemHoverRect->SetOperator( APPEAR_PATH_FILL );
		gpListItemHoverRect->SetFillColor( 0x65EEEEEE );
	}
	if ( ! gpListItemSelectRect )
	{
		gpListItemSelectRect = CHE_WDM_AppearPath::Create();
		gpListItemSelectRect->AddRect( 0, 1, 630, 47 );
		gpListItemSelectRect->SetOperator( APPEAR_PATH_FILL );
		gpListItemSelectRect->SetFillColor( 0x650000FF );
	}
	if ( ! gpListItemLine )
	{
		gpListItemLine = CHE_WDM_AppearPath::Create();
		gpListItemLine->AddLine( 0, 48, 630, 48 );
		gpListItemLine->SetOperator( APPEAR_PATH_STROKE );
		gpListItemLine->SetStrokeColor( 0x88888888 );
		gpListItemLine->SetLineWidth( 1 );
	}
	if ( ! gpListItemIcon1 )
	{
		gpListItemIcon1 = CHE_WDM_AppearImage::Create();
		gpListItemIcon1->SetPosiX( 25 );
		gpListItemIcon1->SetPosiY( 10 );
		gpListItemIcon1->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		gpListItemIcon1->SetImageFile( L"images\\listItemIcon1.png" );
	}
	if ( ! gpListItemIcon2 )
	{
		gpListItemIcon2 = CHE_WDM_AppearImage::Create();
		gpListItemIcon2->SetPosiX( 25 );
		gpListItemIcon2->SetPosiY( 10 );
		gpListItemIcon2->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
		gpListItemIcon2->SetImageFile( L"images\\listItemIcon2.png" );
	}

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
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	imagePtr->SetPosiX( 0 );
	imagePtr->SetPosiY( 100 );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\line.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	imagePtr->SetPosiX( 0 );
	imagePtr->SetPosiY( 475 );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetPosiX( 22 );
	imagePtr->SetPosiY( 120 );
	imagePtr->SetImageFile( L"images\\listBox.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpTextBarTitle = CHE_WDM_Area::Create( mpInterActive );
	mpTextBarTitle->SetWidth( 227 );
	mpTextBarTitle->SetHeight( 29 );
	mpTextBarTitle->SetPosiX( 6 );
	mpTextBarTitle->SetPosiY( 2 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textbarTitle.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpTextBarTitle->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	mpMainArea->AppendChild( mpTextBarTitle );

	mpTextBar = CHE_WDM_Area::Create( mpInterActive );
	mpTextBar->SetWidth( 653 );
	mpTextBar->SetHeight( 41 );
	mpTextBar->SetPosiX( 15 );
	mpTextBar->SetPosiY( 30 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textbar.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_FIT );
	mpTextBar->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpTextBar );

	mpFilePathText = CHE_WDM_Area::Create( mpInterActive );
	mpFilePathText->SetWidth( 580 );
	mpFilePathText->SetHeight( 20 );
	mpFilePathText->SetPosiX( 30 );
	mpFilePathText->SetPosiY( 42 );
	
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetSize( 16 );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 580 );
	textPtr->SetHeight( 20 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_LEFT_OR_TOP ) );
	mpFilePathText->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpFilePathText );

	mpBrowseBtn = CHE_WDM_Button::Create( mpInterActive );
	mpBrowseBtn->SetWidth( 96 );
	mpBrowseBtn->SetHeight( 36 );
	mpBrowseBtn->SetPosiX( 575 );
	mpBrowseBtn->SetPosiY( 35 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\browseBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\browseBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpBrowseBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSELBDOWN );

	mpBrowseBtn->SetClickEvent( EventBrowseBtnClick );
	mpMainArea->AppendChild( mpBrowseBtn );

	mpFilePageCountInfo = CHE_WDM_Area::Create( mpInterActive );
	mpFilePageCountInfo->SetWidth( 150 );
	mpFilePageCountInfo->SetHeight( 15 );
	mpFilePageCountInfo->SetPosiX( 30 );
	mpFilePageCountInfo->SetPosiY( 80 );
	
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 150 );
	textPtr->SetHeight( 15 );
	textPtr->SetSize( 12 );
	textPtr->SetColor( 0xFF333333 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_LEFT_OR_TOP ) );
	mpFilePageCountInfo->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpFilePageCountInfo );

	mpFileSizeInfo = CHE_WDM_Area::Create( mpInterActive );
	mpFileSizeInfo->SetWidth( 150 );
	mpFileSizeInfo->SetHeight( 15 );
	mpFileSizeInfo->SetPosiX( 180 );
	mpFileSizeInfo->SetPosiY( 80 );
	
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 150 );
	textPtr->SetHeight( 15 );
	textPtr->SetSize( 12 );
	textPtr->SetColor( 0xFF333333 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_LEFT_OR_TOP ) );
	mpFileSizeInfo->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpMainArea->AppendChild( mpFileSizeInfo );

	mpToolBtnAdd = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtnAdd->SetWidth( 20 );
	mpToolBtnAdd->SetHeight( 20 );
	mpToolBtnAdd->SetPosiX( 40 );
	mpToolBtnAdd->SetPosiY( 130 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn1.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnAdd->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn1Hover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnAdd->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn1Disable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnAdd->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpToolBtnAdd->SetClickEvent( EventAddBtnClick );
	mpToolBtnAdd->SetEnable( false );

	mpMainArea->AppendChild( mpToolBtnAdd );

	mpToolBtnClear = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtnClear->SetWidth( 20 );
	mpToolBtnClear->SetHeight( 20 );
	mpToolBtnClear->SetPosiX( 80 );
	mpToolBtnClear->SetPosiY( 130 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn2.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnClear->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn2Hover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnClear->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn2Disable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnClear->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpToolBtnClear->SetEnable( false );
	mpToolBtnClear->SetClickEvent( EventClearBtnClick );
	
	mpMainArea->AppendChild( mpToolBtnClear );

	mpToolBtnUp = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtnUp->SetWidth( 20 );
	mpToolBtnUp->SetHeight( 20 );
	mpToolBtnUp->SetPosiX( 120 );
	mpToolBtnUp->SetPosiY( 130 );
	mpToolBtnUp->SetClickEvent( EventUpBtnClick );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn3.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnUp->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn3Hover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnUp->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn3Disable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnUp->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpToolBtnUp->SetEnable( false );
	mpToolBtnUp->SetClickEvent( EventUpBtnClick );

	mpMainArea->AppendChild( mpToolBtnUp );

	mpToolBtnDown = CHE_WDM_Button::Create( mpInterActive );
	mpToolBtnDown->SetWidth( 20 );
	mpToolBtnDown->SetHeight( 20 );
	mpToolBtnDown->SetPosiX( 160 );
	mpToolBtnDown->SetPosiY( 130 );
	mpToolBtnDown->SetClickEvent( EventDownBtnClick );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn4.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnDown->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn4Hover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnDown->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\toolBarBtn4Disable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpToolBtnDown->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpToolBtnDown->SetEnable( false );
	mpToolBtnDown->SetClickEvent( EventDownBtnClick );
	
	mpMainArea->AppendChild( mpToolBtnDown );

	mpListBoxItems = CHE_WDM_Area::Create( mpInterActive );
	mpListBoxItems->SetWidth( 625 );
	mpListBoxItems->SetHeight( 290 );
	mpListBoxItems->SetPosiX( 25 );
	mpListBoxItems->SetPosiY( 160 );
	mpListBoxItems->EnableClip();
	
	mpMainArea->AppendChild( mpListBoxItems );

	mpListScrollBar = CHE_WDM_Button::Create( mpInterActive );
	mpListScrollBar->SetWidth( 17 );
	mpListScrollBar->SetHeight( 283 );
	mpListScrollBar->SetPosiX( 653 );
	mpListScrollBar->SetPosiY( 165 );

	CHE_WDM_Area * pArea = CHE_WDM_Area::Create( mpInterActive );
	pArea->SetWidth( 5 );
	pArea->SetHeight( 283 );
	pArea->SetPosiX( 658 );
	pArea->SetPosiY( 165 );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\listBoxSrocllLine.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpListScrollBar->AppendChild( pArea );

	CHE_WDM_DragArea * pDragArea = CHE_WDM_DragArea::Create( mpInterActive );
	pDragArea->SetWidth( 17 );
	pDragArea->SetHeight( 50 );
	pDragArea->SetPosiX( 653 );
	pDragArea->SetPosiY( 165 );
	pDragArea->SetRange( 653, 165, 653, 398 );
	pDragArea->SetDragEvent( EventMoveScrollBox );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\listBoxScrollBox.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pDragArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpListScrollBar->AppendChild( pDragArea );
	mpListScrollBar->SetVisable( false );

	mpMainArea->AppendChild( mpListScrollBar );

	mpStartBtn = CHE_WDM_Button::Create( mpInterActive );
	mpStartBtn->SetWidth( 125 );
	mpStartBtn->SetHeight( 46 );
	mpStartBtn->SetPosiX( 540 );
	mpStartBtn->SetPosiY( 498 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\startBtn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\startBtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSELBDOWN );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\startBtnDiable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpStartBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	mpStartBtn->SetEnable( false );
	mpStartBtn->SetClickEvent( EventStartBtn );

	mpMainArea->AppendChild( mpStartBtn );

	mpTextBar2 = CHE_WDM_Area::Create( mpInterActive );
	mpTextBar2->SetPosiX( 21 );
	mpTextBar2->SetPosiY( 505 );
	mpTextBar2->SetWidth( 509 );
	mpTextBar2->SetHeight( 29 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textBar2.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpTextBar2->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_LEFT_OR_TOP ) );
	textPtr->SetPosiX( 10 );
	textPtr->SetPosiY( 8 );
	textPtr->SetWidth( 465 );
	textPtr->SetHeight( 15 );
	textPtr->SetSize( 12 );
	textPtr->SetColor( 0xFF000000 );
	mpTextBar2->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	CHE_WDM_Button * pTmpBtn = CHE_WDM_Button::Create( mpInterActive );
	mpTextBar2Btn = pTmpBtn;
	pTmpBtn->SetPosiX( 474 );
	pTmpBtn->SetPosiY( 505 );
	pTmpBtn->SetWidth( 51 );
	pTmpBtn->SetHeight( 29 );
	pTmpBtn->SetClickEvent( EventFileSavePathBrowseBtnClick );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textBar2Btn.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textBar2BtnHover.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\textBar2BtnDisable.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpBtn->AppendAppearItem( imagePtr, AREA_APPEAR_DISABLE );
	pTmpBtn->SetEnable( false );
	mpTextBar2->AppendChild( pTmpBtn );

	mpMainArea->AppendChild( mpTextBar2 );
}

CPdfSpliterDlg::~CPdfSpliterDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;

	gpListItemHoverRect.reset();
	gpListItemSelectRect.reset();
	gpListItemLine.reset();
	gpListItemIcon1.reset();
	gpListItemIcon2.reset();
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
	ON_WM_DROPFILES()
	ON_COMMAND(ID_CMD_OPEN, &CPdfSpliterDlg::OnOpenCmd)
	ON_COMMAND(ID_CMD_CLOSE, &CPdfSpliterDlg::OnCloseCmd)
	ON_COMMAND(ID_CMD_QUIT, &CPdfSpliterDlg::OnQuitCmd)
	ON_COMMAND(ID_CMD_WEB, &CPdfSpliterDlg::OnWebCmd)
	ON_COMMAND(ID_USERGUIDE, &CPdfSpliterDlg::OnUserGuide)
	ON_COMMAND(ID_BUY, &CPdfSpliterDlg::OnBuy)
	ON_COMMAND(ID_REGISTER, &CPdfSpliterDlg::OnRegister)
	ON_COMMAND(ID_CMD_ABOUT, &CPdfSpliterDlg::OnAboutCmd)
END_MESSAGE_MAP()


// CPdfSpliterDlg message handlers

BOOL CPdfSpliterDlg::OnInitDialog()
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

	return TRUE;
}

void CPdfSpliterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
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
	mpMainArea->OnMouseLBDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CPdfSpliterDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPdfSpliterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
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
	CHE_WDM_AppearTextPtr textPtr;
	CHE_WDM_MouseEventBtn * pTmpItem = NULL;
	
	pTmpItem = CHE_WDM_MouseEventBtn::Create( mpInterActive );
	pTmpItem->SetWidth( 605 );
	pTmpItem->SetHeight( 48 );
	pTmpItem->SetPosiX( 25 );
	pTmpItem->SetPosiY( 0 );
	pTmpItem->SetMouseOverEvent( EventListItemMouseOver );
	pTmpItem->SetMouseOutEvent( EventListItemMouseOut );

	pTmpItem->AppendAppearItem( gpListItemHoverRect, AREA_APPEAR_MOUSEOVER );
	pTmpItem->AppendAppearItem( gpListItemLine, AREA_APPEAR_BACKGROUND );
	
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetSize( 12 );
	textPtr->SetPosiX( 75 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 200 );
	textPtr->SetHeight( 48 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_FIX, LAYOUT_ALIGN_CENTER ) );
	wchar_t tmpStr[128];
	switch( item.type )
	{
	case SINGLE_PAGE:
		{
			pTmpItem->AppendAppearItem( gpListItemIcon1, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"Page : %d", item.pageIndex );
			break;
		}
	case PAGE_RANGE:
		{
			pTmpItem->AppendAppearItem( gpListItemIcon2, AREA_APPEAR_BACKGROUND );
			wsprintf( tmpStr, L"Pages : %d - %d", item.pageIndex, item.pageIndex + item.pageCount - 1 );
			break;
		}
	default:;
	}
	textPtr->SetText( tmpStr );
	pTmpItem->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );	
	pTmpItem->SetClickEvent( EventListItemClick );

	CHE_WDM_Button * pTmpDelBtn = CHE_WDM_Button::Create( mpInterActive );
	pTmpDelBtn->SetPosiX( 400 );
	pTmpDelBtn->SetPosiY( 12 );
	pTmpDelBtn->SetWidth( 24 );
	pTmpDelBtn->SetHeight( 24 );
	pTmpDelBtn->SetVisable( false );
	pTmpDelBtn->SetEnable( false );
	pTmpDelBtn->SetClickEvent( EventListItemDelBtnClick ); 

	CHE_WDM_AppearImagePtr imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\delBtn.png" );
	pTmpDelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );

	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\delBtnHover.png" );
	pTmpDelBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );

	pTmpItem->AppendChild( pTmpDelBtn );

	mpListBoxItems->AppendChild( pTmpItem );
}


void CPdfSpliterDlg::DeleteListItem( unsigned int index )
{
	CHE_WDM_Area * pTmp = mpListBoxItems->PopChild( index );
	if ( pTmp )
	{
		pTmp->GetAllocator()->Delete( pTmp );
	}
}

void CPdfSpliterDlg::CancelSelection()
{
	CHE_WDM_Area * pTmpArea = NULL;
	unsigned int iCount = mpListBoxItems->GetChildrenCount();
	if ( theApp.mCurItem != 0 )
	{
		for ( unsigned int i = 0; i < iCount; ++i )
		{
			if (  theApp.mCurItem == i + 1 )
			{
				pTmpArea = theApp.mpMainDlg->mpListBoxItems->GetChild( i );
				pTmpArea->GetAppear().mBackground.erase(  pTmpArea->GetAppear().mBackground.begin() );
				break;
			}
		}
	}
}

void CPdfSpliterDlg::UpdateSelection(void)
{
	CHE_WDM_Area * pTmpItem = NULL;
	unsigned int iCount = theApp.mItemCount;
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		pTmpItem = mpListBoxItems->GetChild( i );
		if ( i + 1 == theApp.mCurItem )
		{
			pTmpItem->GetAppear().mBackground.insert( pTmpItem->GetAppear().mBackground.begin(), gpListItemSelectRect );
		}
	}
}

void CPdfSpliterDlg::UpdateList()
{
	CHE_WDM_Area * pTmpItem = NULL;
	int offset = 0;
	int newPosition = 0;
	unsigned int iCount = theApp.mItemCount;
	if ( iCount > 6 )
	{
		mpListScrollBar->SetVisable( true );
		offset = (iCount - 6) * 48 * theApp.mfViewPoint;
	}else
	{
		mpListScrollBar->SetVisable( false );
	}
	for ( unsigned int i = 0; i < iCount; ++i )
	{
		newPosition = mpListBoxItems->GetPosiY() + i * 48 - offset;
		pTmpItem = mpListBoxItems->GetChild( i );
		pTmpItem->SetPosiY( newPosition );
		if ( ( newPosition > mpListBoxItems->GetPosiY() + 288 ) || ( newPosition < -48 ) )
		{
			pTmpItem->SetVisable( false );
		}else{
			pTmpItem->SetVisable( true );
		}

	}
	mpListBoxItems->Refresh();
	mpListScrollBar->Refresh();
}

void CPdfSpliterDlg::UpdateTargetFileArea()
{
	CHE_WDM_AppearTextPtr pAppearText = mpFilePathText->GetAppear().mBackground[0].GetTextPtr();
	pAppearText->SetText( theApp.mTargetFile.c_str() ); 
	mpFilePathText->Refresh();
}

void CPdfSpliterDlg::UpdateFileInfoArea()
{
	CHE_WDM_AppearTextPtr pAppearText = mpFileSizeInfo->GetAppear().mBackground[0].GetTextPtr();
	pAppearText->SetText( L"" );
	pAppearText = mpFilePageCountInfo->GetAppear().mBackground[0].GetTextPtr();
	pAppearText->SetText( L"" ); 
	mpFilePageCountInfo->Refresh();
}

void CPdfSpliterDlg::UpdateNewFileArea()
{
	CHE_WDM_AppearTextPtr pAppearText = mpTextBar2->GetAppear().mBackground[1].GetTextPtr();
	pAppearText->SetText( theApp.mTargetFile.c_str() );
	mpTextBar2->Refresh();
}

void CPdfSpliterDlg::UpdateToolBtn()
{
	if ( ! theApp.mbLoadOver )
	{
		mpToolBtnAdd->SetEnable( false );
		mpToolBtnClear->SetEnable( false );
		mpToolBtnUp->SetEnable( false );
		mpToolBtnDown->SetEnable( false );
		mpStartBtn->SetEnable( false );
		mpTextBar2Btn->SetEnable( false );
	}
	else{
		mpToolBtnAdd->SetEnable( true );
		mpToolBtnClear->SetEnable( false );
		mpToolBtnUp->SetEnable( false );
		mpToolBtnDown->SetEnable( false );
		mpTextBar2Btn->SetEnable( true );
		mpStartBtn->SetEnable( false );
		if ( theApp.mItemCount > 0 )
		{
			mpToolBtnClear->SetEnable( true );
			mpStartBtn->SetEnable( true );
		}
		if ( theApp.mCurItem != 0 )
		{
			mpToolBtnUp->SetEnable( true );
			mpToolBtnDown->SetEnable( true );
			if ( theApp.mCurItem == 1 )
			{
				mpToolBtnUp->SetEnable( false );
			}
			if ( theApp.mCurItem == theApp.mItemCount )
			{
				mpToolBtnDown->SetEnable( false );
			}
		}
	}
	mpToolBtnAdd->Refresh();
	mpToolBtnClear->Refresh();
	mpToolBtnDown->Refresh();
	mpToolBtnUp->Refresh();
	mpTextBar2->Refresh();
	mpStartBtn->Refresh();
}

void CPdfSpliterDlg::SetWindowsTitleNormal()
{
	SetWindowText( L"Peroit PDF Splitter" );
}

void CPdfSpliterDlg::OnOK()
{
	//CDialogEx::OnOk();
}


void CPdfSpliterDlg::OnCancel()
{
	EndDialog( 0 );
}

void CPdfSpliterDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer( mTimerId );
	theApp.mFile.Close();
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

void CPdfSpliterDlg::OnWebCmd()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com", NULL, NULL, SW_SHOWNORMAL );
}

void CPdfSpliterDlg::OnAboutCmd()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CPdfSpliterDlg::OnBuy()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com/pdfSplitter/Buy", NULL, NULL, SW_SHOWNORMAL );
}

void CPdfSpliterDlg::OnUserGuide()
{
	ShellExecute( GetSafeHwnd(), L"open", L"http://www.peroit.com/pdfSplitter/UserGuide", NULL, NULL, SW_SHOWNORMAL );
}

void CPdfSpliterDlg::OnRegister()
{
	CRegsitrationDlg dlg;
	if ( dlg.DoModal() == 0 )
	{
		SetWindowsTitleNormal();
	}
}

void CPdfSpliterDlg::OnDropFiles( HDROP hDropInfo )
{
 	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );
 
	char fileName[1024];
	char SecFileName[5];
	wchar_t szFileName[1024];

	int i , pathlength; 
	
	for ( i = 0; i < count; ++i ) 
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
				theApp.CloseDocument();

				theApp.mTargetFile = szFileName;

				unsigned int i = 0;

				for ( i = theApp.mTargetFile.size()-1; i != 0; --i )
				{
					if ( L'.' == szFileName[i] )
					{
						szFileName[i++] = L'_';
						szFileName[i++] = L'N';
						szFileName[i++] = L'e';
						szFileName[i++] = L'w';
						szFileName[i++] = L'.';
						szFileName[i++] = L'p';
						szFileName[i++] = L'd';
						szFileName[i++] = L'f';
						szFileName[i++] = L'\0';
						break;
					}
				}
				theApp.mNewFile = szFileName;
				CHE_WDM_AppearTextPtr textPtr = theApp.mpMainDlg->mpTextBar2->GetAppear().mBackground[1].GetTextPtr();;
				textPtr->SetText(  theApp.mNewFile.c_str() );
				theApp.mpMainDlg->mpTextBar2->Refresh();

				textPtr = theApp.mpMainDlg->mpFilePathText->GetAppear().mBackground[0].GetTextPtr();
				textPtr->SetText(  theApp.mTargetFile.c_str() );
				theApp.mpMainDlg->mpFilePathText->Refresh();
				CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
				CFileLoadDlg loadDlg;
				loadDlg.DoModal();

				if ( theApp.mbLoadError )
				{
					theApp.mpProcessDlg->MessageBox( L"This is not a PDF Files ", L"Error", MB_OK | MB_ICONERROR );
					theApp.mbLoadError = false;
					return;
				}

				theApp.mpMainDlg->UpdateToolBtn();

				CHE_ByteString str;
				bool bPasswordError = false;
				str = "";
				while ( ! theApp.mFile.Authenticate( str ) )
				{
					CPasswordDlg dlg;
					if ( bPasswordError )
					{
						dlg.SetErrorFlag();
					}
					if ( dlg.DoModal() == 1 )
					{
						theApp.CloseDocument();
						return;
					}
					bPasswordError = true;
					str = theApp.mPasswrod.c_str();
				}

				textPtr = theApp.mpMainDlg->mpFilePageCountInfo->GetAppear().mBackground[0].GetTextPtr();
				wchar_t tmpwstr[512];
				wsprintf( tmpwstr, L"Total：%d Page(s)", theApp.mpDocument->GetPageCount() );
				textPtr->SetText( tmpwstr );
				textPtr = theApp.mpMainDlg->mpFileSizeInfo->GetAppear().mBackground[0].GetTextPtr();
				if ( theApp.mpFileRead->GetSize() <= 10485 )
				{
					swprintf( tmpwstr, L"%4.2f KB", theApp.mpFileRead->GetSize() * 1.0 / 1024 ) ;
				}else{
					swprintf( tmpwstr, L"%4.2f MB", theApp.mpFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
				}
				textPtr->SetText( tmpwstr );
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
						break;
					}
				case 3:
					{
						CListItem listItem;
						listItem.type = SINGLE_PAGE;
						for ( size_t i = 0; i < theApp.mpPageTree->GetPageCount(); i+=2 )
						{
							listItem.pageIndex = i+1;
							listItem.pageCount = 1;
							theApp.AddPageListItem( listItem );
						}
						theApp.mpMainDlg->UpdateList();
						theApp.mpMainDlg->UpdateToolBtn();
						break;
					}
				case 4:
					{
						CListItem listItem;
						listItem.type = SINGLE_PAGE;
						for ( size_t i = 1; i < theApp.mpPageTree->GetPageCount(); i+=2 )
						{
							listItem.pageIndex = i+1;
							listItem.pageCount = 1;
							theApp.AddPageListItem( listItem );
						}
						theApp.mpMainDlg->UpdateList();
						theApp.mpMainDlg->UpdateToolBtn();
						break;
					}
				}

				theApp.mpMainDlg->mpBrowseBtn->OnMouseOut();

				break;
			}
		}
	}

	DragFinish( hDropInfo );

	CDialogEx::OnDropFiles(hDropInfo);
}


void CAboutDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnDropFiles(hDropInfo);
}
