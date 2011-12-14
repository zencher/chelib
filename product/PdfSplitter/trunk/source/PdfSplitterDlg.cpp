
// PdfSpliterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PdfSplitterDlg.h"
#include "FileLoadDlg.h"
#include "SelectionModeDlg.h"
#include "PageSelectionDlg.h"
#include "PagesSelectionDlg.h"
#include "afxdialogex.h"

#include "../../../../trunk//include/pdf/che_pdf_creator.h"
#include "../../../../trunk//include/pdf/che_pdf_objclone.h"

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
	if ( theApp.mTargetFile.size() > 0 )
	{
		char tmpStr[512];
		memset( tmpStr, 0, 512 );
		WideCharToMultiByte( CP_ACP, 0, theApp.mTargetFile.c_str(), -1, tmpStr, 512, NULL, NULL );
		theApp.mpFileRead = HE_CreateFileRead( tmpStr, FILEREAD_MODE_DEFAULT, 4096 );
		if ( theApp.mpFileRead )
		{
			theApp.mParser.Close();
			theApp.mParser.Open( theApp.mpFileRead );
			theApp.mParser.GetPageCount();
		}
		theApp.mbLoadOver = true;
		theApp.mpMainDlg->UpdateToolBtn();
	}
	return 0;
}

DWORD WINAPI ThreadSplit( LPVOID lpParameter )
{
	theApp.mpMainDlg->SetProcessBarValue( 0 );

	CHE_PDF_Creator creator;
	if ( creator.NewDocument() == FALSE )
	{
		return 0;
	}

	CHE_ObjectCloneMgr ObjCloneMgr;

	std::list< CListItem >::iterator it;

	unsigned int iCurPage = 0;
	unsigned int iPageCount = 0;

	for ( it = theApp.mPageList.begin(); it != theApp.mPageList.end(); ++it )
	{
		iPageCount += (*it).pageCount;
	}

	for ( it = theApp.mPageList.begin(); it != theApp.mPageList.end(); ++it, ++iCurPage )
	{
		unsigned int iIndex = (*it).pageIndex - 1;
		unsigned int iCount = (*it).pageCount;

		for ( unsigned int i = 0; i < iCount; i++ )
		{
			theApp.mpMainDlg->SetProcessBarValue( (unsigned int)( (iCurPage+1) * 100.0 / iPageCount ) );
			iCurPage++;
			CHE_PDF_Dictionary * pFirstPageDict = theApp.mParser.GetObject( theApp.mParser.GetPageObjNum( iIndex + i ) )->ToDict();
			if ( pFirstPageDict == NULL )
			{
				return 0;
			}
			CHE_PDF_Dictionary * pNewPageDict = creator.NewPage( 0, 0 );
			if ( pNewPageDict == NULL )
			{
				return 0;
			}

			creator.SetVersion( theApp.mParser.GetPDFVersion() );

			CHE_PDF_Object * pTmpObj = NULL;
			CHE_ByteString key;
			for ( HE_DWORD i = 0; i < pFirstPageDict->GetCount(); i++ )
			{
				pTmpObj = pFirstPageDict->GetElementByIndex( i );
				pFirstPageDict->GetKeyByIndex( i, key );
				if ( key == "Type" || key == "Parent"  )
				{
					continue;
				}
				switch( pTmpObj->GetType() )
				{
				case OBJ_TYPE_NULL:
					pNewPageDict->SetAtNull( key );
					break;
				case OBJ_TYPE_BOOLEAN:
					pNewPageDict->SetAtBoolean( key, pTmpObj->ToBoolean()->GetValue() );
					break;
				case OBJ_TYPE_NUMBER:
					if ( pTmpObj->ToNumber()->IsInteger() )
					{
						pNewPageDict->SetAtInteger( key, pTmpObj->ToNumber()->GetInteger() );
					}else{
						pNewPageDict->SetAtFloatNumber( key, pTmpObj->ToNumber()->GetFloat() );
					}
					break;
				case OBJ_TYPE_STRING:
					pNewPageDict->SetAtString( key, pTmpObj->ToString()->GetString() );
					break;
				case OBJ_TYPE_NAME:
					pNewPageDict->SetAtName( key, pTmpObj->ToName()->GetString() );
					break;
				case OBJ_TYPE_ARRAY:
					pNewPageDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), &creator, pNewPageDict->GetObjNum(), pNewPageDict->GetObjNum(), &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewPageDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), &creator, pNewPageDict->GetObjNum(), pNewPageDict->GetGenNum(), &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_DWORD refNum = CloneIndirectObject( pTmpObj->ToReference(), &creator, &ObjCloneMgr );
						pNewPageDict->SetAtReference( key, refNum );
						break;
					}
				default:
					break;
				}
			}
		}
	}

	IHE_Write * pWrite = HE_CreateFileWrite( "D:\\newPdfPage.pdf" );

	creator.Save( pWrite );

	HE_DestoryIHEWrite( pWrite );

	theApp.mpMainDlg->mpProcess->SetVisable( false );
	theApp.mpMainDlg->mpProcess->Refresh();

	//theApp.mpMainDlg->KillTimer( theApp.mpMainDlg->mTimerId );

	theApp.mpMainDlg->MessageBox(  L"分割完成！", L"消息", MB_OK | MB_ICONINFORMATION );

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

		theApp.mbLoadOver = false;
		theApp.mTargetFile = fileName;
		CHE_WD_Appearance * pTmpAppear = theApp.mpMainDlg->mpFilePathText->GetBackGroundAppear();
		CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
		pTmpText->SetText(  theApp.mTargetFile.c_str() );
		//theApp.mpMainDlg->mpInterActive->Invalidate();
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
					theApp.mpMainDlg->UpdataList();
				}
				break;
			}
		case 2:
			{
				CPagesSelectionDlg pageDlg;
				if ( pageDlg.DoModal() == 1 )
				{
					theApp.mpMainDlg->UpdataList();
				}
			}
		}
	}
// 	CFileDialog fileDlg(TRUE, L"PDF", NULL, OFN_NOCHANGEDIR, L"PDF Files(*.pdf)|*.pdf||", theApp.mpMainDlg );
// 	//CFileDialog fileDlg( TRUE, L"PDF", NULL, OFN_NOCHANGEDIR, L"PDF Files(*.pdf)|*.pdf||", theApp.mpMainDlg,  )
// 	if ( fileDlg.DoModal() == IDOK )
// 	{
// 		theApp.mbLoadOver = false;
// 		theApp.mTargetFile = fileDlg.GetFolderPath();
// 		theApp.mTargetFile += L"\\";
// 		theApp.mTargetFile += fileDlg.GetFileName();
// 		CHE_WD_Appearance * pTmpAppear = theApp.mpMainDlg->mpFilePathText->GetBackGroundAppear();
// 		CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		pTmpText->SetText(  theApp.mTargetFile.c_str() );
// 		theApp.mpMainDlg->mpInterActive->Invalidate();
// 		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
// 		CFileLoadDlg loadDlg;
// 		loadDlg.DoModal();
// 		pTmpAppear = theApp.mpMainDlg->mpFilePageCountInfo->GetBackGroundAppear();
// 		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		wchar_t tmpwstr[512];
// 		wsprintf( tmpwstr, L"总计：%d 页", theApp.mParser.GetPageCount() );
// 		pTmpText->SetText( tmpwstr );
// 		pTmpAppear = theApp.mpMainDlg->mpFileSizeInfo->GetBackGroundAppear();
// 		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		if ( theApp.mpFileRead->GetSize() <= 10485 )
// 		{
// 			swprintf( tmpwstr, L"%4.2f KB", theApp.mpFileRead->GetSize() * 1.0 / 1024 ) ;
// 		}else{
// 			swprintf( tmpwstr, L"%4.2f MB", theApp.mpFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
// 		}
// 		pTmpText->SetText( tmpwstr );
// 		theApp.mpMainDlg->mpInterActive->Invalidate();
// 
// 		CSelectionModeDlg selectModeDlg;
// 		unsigned int iRet = selectModeDlg.DoModal();
// 		switch ( iRet )
// 		{
// 		default:
// 		case 0:
// 			break;
// 		case 1:
// 			{
// 				CPageSelectionDlg pageDlg;
// 				if ( pageDlg.DoModal() == 1 )
// 				{
// 					theApp.mpMainDlg->UpdataList();
// 				}
// 				break;
// 			}
// 		case 2:
// 			{
// 				CPagesSelectionDlg pageDlg;
// 				if ( pageDlg.DoModal() == 1 )
// 				{
// 					theApp.mpMainDlg->UpdataList();
// 				}
// 			}
// 		}
// 	}
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
			theApp.mpMainDlg->UpdataSelection();
			theApp.mpMainDlg->UpdateToolBtn();
			theApp.mpMainDlg->UpdataList();
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
				//theApp.mpMainDlg->CancelSelection();
				theApp.mpMainDlg->UpdateToolBtn();
				theApp.mpMainDlg->UpdataList();
			}
			break;
		}
	case 2:
		{
			CPagesSelectionDlg pageDlg;
			if ( pageDlg.DoModal() == 1 )
			{
				//theApp.mpMainDlg->CancelSelection();
				theApp.mpMainDlg->UpdateToolBtn();
				theApp.mpMainDlg->UpdataList();
			}
		}
	}
}

static void EventDelBtnClick( CHE_WD_Area * pArea )
{
	theApp.DelCurPageListItem();
}

static void EventMoveScrollBox( CHE_WD_Area * pArea )
{
	static unsigned int point = 0;
	if ( pArea->GetPositionY() != point )
	{
		theApp.mfViewPoint = ( pArea->GetPositionY() - 165 ) * 1.0 /233;
		point = pArea->GetPositionY();
		theApp.mpMainDlg->UpdataList();
	}
}

static void EventStartBtn( CHE_WD_Area * pArea )
{
// 	CFileDialog fileDlg(TRUE, L"PDF", NULL, 0, L"PDF Files(*.pdf)|*.pdf||", theApp.mpMainDlg );
// 	if ( fileDlg.DoModal() == IDOK )
// 	{
// 		theApp.mbLoadOver = false;
// 		theApp.mTargetFile = fileDlg.GetFolderPath();
// 		theApp.mTargetFile += L"\\";
// 		theApp.mTargetFile += fileDlg.GetFileName();
// 		CHE_WD_Appearance * pTmpAppear = theApp.mpMainDlg->mpFilePathText->GetBackGroundAppear();
// 		CHE_WD_AppearText * pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		pTmpText->SetText(  theApp.mTargetFile.c_str() );
// 		theApp.mpMainDlg->mpInterActive->Invalidate();
// 		CreateThread( NULL, 0, ThreadLoadFile, 0, 0, 0 );
// 		CFileLoadDlg loadDlg;
// 		loadDlg.DoModal();
// 		pTmpAppear = theApp.mpMainDlg->mpFilePageCountInfo->GetBackGroundAppear();
// 		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		wchar_t tmpwstr[512];
// 		wsprintf( tmpwstr, L"总计：%d 页", theApp.mParser.GetPageCount() );
// 		pTmpText->SetText( tmpwstr );
// 		pTmpAppear = theApp.mpMainDlg->mpFileSizeInfo->GetBackGroundAppear();
// 		pTmpText = (CHE_WD_AppearText *)( pTmpAppear->mItems[0] );
// 		if ( theApp.mpFileRead->GetSize() <= 10485 )
// 		{
// 			swprintf( tmpwstr, L"%4.2f KB", theApp.mpFileRead->GetSize() * 1.0 / 1024 ) ;
// 		}else{
// 			swprintf( tmpwstr, L"%4.2f MB", theApp.mpFileRead->GetSize() * 1.0 / ( 1024 * 1024 ) ) ;
// 		}
// 		pTmpText->SetText( tmpwstr );
// 		theApp.mpMainDlg->mpInterActive->Invalidate();
// 
// 		CSelectionModeDlg selectModeDlg;
// 		unsigned int iRet = selectModeDlg.DoModal();
// 		switch ( iRet )
// 		{
// 		default:
// 		case 0:
// 			break;
// 		case 1:
// 			{
// 				CPageSelectionDlg pageDlg;
// 				if ( pageDlg.DoModal() == 1 )
// 				{
// 					theApp.mpMainDlg->UpdataList();
// 				}
// 				break;
// 			}
// 		case 2:
// 			{
// 				CPagesSelectionDlg pageDlg;
// 				if ( pageDlg.DoModal() == 1 )
// 				{
// 					theApp.mpMainDlg->UpdataList();
// 				}
// 			}
// 		}
// 	}
	//theApp.mpMainDlg->mTimerId = theApp.mpMainDlg->SetTimer( 2, 30, NULL );
	theApp.mpMainDlg->mpProcess->SetVisable( true );
	CreateThread( NULL, 0, ThreadSplit, 0, 0, 0 );
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
	CHE_WD_AppearText * pTmpText = new CHE_WD_AppearText();
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
	mpBrowseBtn->SetHeight( 34 );
	mpBrowseBtn->SetPositionX( 575 );
	mpBrowseBtn->SetPositionY( 35 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\browseBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_FIT );
	pTmpApper->mItems.push_back( pTmpImage );
	mpBrowseBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\browseBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_FIT );
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
	mpToolBtnDel->SetClickEvent( EventDelBtnClick );
	mpListBox->AppendChild( mpToolBtnDel );

	mpToolBtnUp = new CHE_WD_Button( mpInterActive );
	mpToolBtnUp->SetWidth( 20 );
	mpToolBtnUp->SetHeight( 20 );
	mpToolBtnUp->SetPositionX( 120 );
	mpToolBtnUp->SetPositionY( 130 );
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
	mpListBox->AppendChild( mpToolBtnUp );

	mpToolBtnDown = new CHE_WD_Button( mpInterActive );
	mpToolBtnDown->SetWidth( 20 );
	mpToolBtnDown->SetHeight( 20 );
	mpToolBtnDown->SetPositionX( 160 );
	mpToolBtnDown->SetPositionY( 130 );
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
	mpStartBtn->SetWidth( 88 );
	mpStartBtn->SetHeight( 27 );
	mpStartBtn->SetPositionX( 580 );
	mpStartBtn->SetPositionY( 500 );
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
	mpStartBtn->SetClickEvent( EventStartBtn );

	mpMainArea->AppendChild( mpStartBtn );

	mpProcess = new CHE_WD_Area( 0, 0, mpInterActive );
	mpProcess->SetWidth( 486 );
	mpProcess->SetHeight( 27 );
	mpProcess->SetPositionX( 30 );
	mpProcess->SetPositionY( 500 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\processbg.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpProcess->SetBackGroundAppear( pTmpApper );
	CHE_WD_Area * pTmpArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpArea->SetWidth( 14 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPositionX( 31 );
	pTmpArea->SetPositionY( 500 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\processleft.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpArea->SetBackGroundAppear( pTmpApper );
	mpProcess->AppendChild( pTmpArea );
	pTmpArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpArea->SetWidth( 100 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPositionX( 45 );
	pTmpArea->SetPositionY( 500 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\process.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpArea->SetBackGroundAppear( pTmpApper );
	pTmpArea->EnableClip();
	mpProcess->AppendChild( pTmpArea );
	pTmpArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpArea->SetWidth( 13 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPositionX( 144 );
	pTmpArea->SetPositionY( 500 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\processright.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	pTmpArea->SetBackGroundAppear( pTmpApper );
	mpProcess->AppendChild( pTmpArea );
	pTmpArea = new CHE_WD_Area( 0, 0, mpInterActive );
	pTmpArea->SetWidth( 486 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPositionX( 30 );
	pTmpArea->SetPositionY( 500 );
	pTmpApper = new CHE_WD_Appearance();
	pTmpText = new CHE_WD_AppearText();
	pTmpText->SetSize( 12 );
	pTmpText->SetColor( 0xFFFFFFFF );
	pTmpText->SetPositionX( 0 );
	pTmpText->SetPositionY( 0 );
	pTmpText->SetWidth( 486 );
	pTmpText->SetHeight( 27 );
	pTmpApper->mItems.push_back( pTmpText );
	pTmpArea->SetBackGroundAppear( pTmpApper );
	mpProcess->AppendChild( pTmpArea );
	mpProcess->SetVisable( false );

	mpMainArea->AppendChild( mpProcess );
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

	SetProcessBarValue( 0 );
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

void CPdfSpliterDlg::SetProcessBarValue( unsigned int val )
{
	if ( val > 100 )
	{
		val = 100;
	}
	unsigned int iTmp = val / 100.0 * 457;
	mpProcess->GetChild(1)->SetWidth( iTmp );
	mpProcess->GetChild(2)->SetPositionX( iTmp + 45 );
	CHE_WD_Area * pTmpArea = NULL;
	CHE_WD_Appearance * pTmpAppear = NULL;
	CHE_WD_AppearText * pTmpText = NULL;
	wchar_t tmpStr[32];
	pTmpArea = mpProcess->GetChild( mpProcess->GetChildrenCount() - 1 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	pTmpText = (CHE_WD_AppearText*)( pTmpAppear->mItems[0] );
	if ( val == 100 )
	{
		pTmpText->SetText( L"正在写文件，即将完成。" );
	}else{
		wsprintf( tmpStr, L"%d%%", val );
		pTmpText->SetText( tmpStr );
	}
	mpProcess->Refresh();
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

void CPdfSpliterDlg::UpdataSelection(void)
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

void CPdfSpliterDlg::UpdataList()
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
	for ( unsigned int i = 0; i < iCount; ++i )
	{
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

void CPdfSpliterDlg::UpdateToolBtn()
{
	if ( ! theApp.mbLoadOver )
	{
		mpToolBtnAdd->SetVisable( false );
		mpToolBtnDel->SetVisable( false );
		mpToolBtnUp->SetVisable( false );
		mpToolBtnDown->SetVisable( false );
	}
	else{
		mpToolBtnAdd->SetVisable( true );
		mpToolBtnDel->SetVisable( false );
		mpToolBtnUp->SetVisable( false );
		mpToolBtnDown->SetVisable( false );
		if ( theApp.mCurItem != 0 )
		{
			mpToolBtnDel->SetVisable( true );
			mpToolBtnUp->SetVisable( true );
			mpToolBtnDown->SetVisable( true );
			if ( theApp.mCurItem == 1 )
			{
				mpToolBtnUp->SetVisable( false );
			}
			if ( theApp.mCurItem == theApp.mItemCount )
			{
				mpToolBtnDown->SetVisable( false );
			}
		}
	}
	mpToolBtnAdd->Refresh();
	mpToolBtnDel->Refresh();
	mpToolBtnDown->Refresh();
	mpToolBtnUp->Refresh();
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