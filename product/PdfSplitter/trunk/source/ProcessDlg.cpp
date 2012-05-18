#include "stdafx.h"
#include "PdfSplitter.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"

#include "../../../../trunk/include/pdf/che_pdf_creator.h"
#include "../../../../trunk/include/pdf/che_pdf_objclone.h"
#include "../../../../trunk/include/pdf/che_pdf_image.h"
#include "../../../../trunk/include/pdf/che_pdf_contentresmgr.h"

CProcessDlg * gpDlg = NULL;

static void EventCancelBtnClick( CHE_WD_Area * pArea )
{
}

DWORD WINAPI ThreadSplit( LPVOID lpParameter )
{
	//flag for working
	theApp.mbWork = true;

	CHE_PDF_File newFile;
	newFile.SetPDFVersion( theApp.mFile.GetPDFVersion() );

	CHE_PDF_Document * pNewDocument = CHE_PDF_Document::CreateDocument( &newFile );

	CHE_PDF_PageTree * pNewPageTree = pNewDocument->GetPageTree();

	CHE_ObjectCloneMgr ObjCloneMgr;

	std::vector< CListItem >::iterator it;

	unsigned int iCurPage = 0;
	unsigned int iPageCount = 0;

	gpDlg->mpMainArea->Refresh();

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
			if ( theApp.mpProcessDlg )
			{
				theApp.mpProcessDlg->SetProcessBarValue( (unsigned int)( (iCurPage+1) * 100.0 / iPageCount ) );
				theApp.mpProcessDlg->mpMainArea->Refresh();
			}
			iCurPage++;

			CHE_PDF_DictionaryPtr OldPageDictPtr;
			CHE_PDF_Page * pOldPage = theApp.mpPageTree->GetPage( iIndex + i );
			if ( pOldPage == NULL )
			{
				return 0;
			}
			OldPageDictPtr = pOldPage->GetPageDict();

			pNewPageTree->AppendPage( 0, 0 );

			CHE_PDF_DictionaryPtr NewPageDictPtr;
			CHE_PDF_Page * pNewPage = pNewPageTree->GetPage( pNewPageTree->GetPageCount() - 1 );
			if ( pNewPage == NULL )
			{
				return 0;
			}
			NewPageDictPtr = pNewPage->GetPageDict();

			CHE_ByteString key;
			CHE_PDF_ObjectPtr tmpObjPtr;
			for ( HE_DWORD i = 0; i < OldPageDictPtr->GetCount(); i++ )
			{
				tmpObjPtr = OldPageDictPtr->GetElementByIndex( i );
				OldPageDictPtr->GetKeyByIndex( i, key );
				if ( !tmpObjPtr || key == "Type" || key == "Parent" )
				{
					continue;
				}
				switch( tmpObjPtr->GetType() )
				{
				case OBJ_TYPE_NULL:
					NewPageDictPtr->SetAtNull( key );
					break;
				case OBJ_TYPE_BOOLEAN:
					NewPageDictPtr->SetAtBoolean( key, tmpObjPtr->GetBooleanPtr()->GetValue() );
					break;
				case OBJ_TYPE_NUMBER:
					if ( tmpObjPtr->GetNumberPtr()->IsInteger() )
					{
						NewPageDictPtr->SetAtInteger( key, tmpObjPtr->GetNumberPtr()->GetInteger() );
					}else{
						NewPageDictPtr->SetAtFloatNumber( key, tmpObjPtr->GetNumberPtr()->GetFloat() );
					}
					break;
				case OBJ_TYPE_STRING:
					NewPageDictPtr->SetAtString( key, tmpObjPtr->GetStringPtr()->GetString() );
					break;
				case OBJ_TYPE_NAME:
					NewPageDictPtr->SetAtName( key, tmpObjPtr->GetNamePtr()->GetString() );
					break;
				case OBJ_TYPE_ARRAY:
					NewPageDictPtr->SetAtArray( key, CloneDirectArrayObj( tmpObjPtr->GetArrayPtr(), &newFile, &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					NewPageDictPtr->SetAtDictionary( key, CloneDirectDictObj( tmpObjPtr->GetDictPtr(), &newFile, &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						PDF_RefInfo refInfo = CloneIndirectObj( tmpObjPtr->GetRefPtr(), &newFile, &ObjCloneMgr );
						NewPageDictPtr->SetAtReference( key, refInfo.objNum, refInfo.genNum, &newFile );
						break;
					}
				default:
					break;
				}
			}

			CHE_PDF_DictionaryPtr tmpDict = CloneDirectDictObj( pOldPage->GetResourcesDict(), &newFile, &ObjCloneMgr );
			NewPageDictPtr->SetAtDictionary( "Resources", tmpDict );

			CHE_PDF_ArrayPtr tmpArray = CloneDirectArrayObj( pOldPage->GetMediaBoxArray(), &newFile, &ObjCloneMgr );
			NewPageDictPtr->SetAtArray( "MediaBox", tmpArray );

			HE_INT32 rotate = pOldPage->GetRotate();
			NewPageDictPtr->SetAtInteger( "Rotate", rotate );

			//release pages
			CHE_PDF_Page::ReleasePage( pOldPage );
			CHE_PDF_Page::ReleasePage( pNewPage );
		}
	}

	if ( ! theApp.CheckRefInfo() )
	{
		pNewPageTree->AppendPage( 600, 800 );

		CHE_PDF_Page * pAdPage = pNewPageTree->GetPage( pNewPageTree->GetPageCount() - 1 );

		CHE_PDF_DictionaryPtr dictPtr = pAdPage->GetPageDict();

		CHE_PDF_DictionaryPtr resDict = CHE_PDF_Dictionary::Create();

		dictPtr->SetAtDictionary( "Resources", resDict );

		FILE * pImageFile = NULL;
		pImageFile = fopen( "resData.dat", "rb" );
		fseek( pImageFile, 0, SEEK_END );
		HE_DWORD imageSize = ftell( pImageFile );
		HE_LPBYTE pBuf = new HE_BYTE[imageSize];
		fseek( pImageFile, 0, SEEK_SET ); 
		fread( pBuf, 1, imageSize, pImageFile );

		CHE_PDF_ReferencePtr refPtr = CHE_PDF_Image::InsertImageToFile( &newFile, IMAGE_JPEG, 8, 600, 800, pBuf, imageSize );

		CHE_PDF_ContentResMgr resMgr( resDict );

		resMgr.CreateName( CONTENTRES_XOBJECT, refPtr );

		CHE_PDF_StreamPtr streamPtr;

		PDF_RefInfo refInfo = newFile.CreateStreamObject( streamPtr );

		if ( streamPtr )
		{
			streamPtr->SetRawData( (HE_LPBYTE)"600 0 0 800 0 0 cm\n/XOBJ Do", 27/*, STREAM_FILTER_FLATE*/ );
		}

		dictPtr->SetAtReference( "Contents", refInfo.objNum, refInfo.genNum, &newFile );
	}

	theApp.mpProcessDlg->mpMainArea->Refresh();

	char tmpStr[1024];
	memset( tmpStr, 0, 1024 );
	WideCharToMultiByte( CP_ACP, 0, theApp.mNewFile.c_str(), -1, tmpStr, 1024, NULL, NULL );
	IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );

	newFile.Save( pWrite );

	HE_DestoryIHEWrite( pWrite );

	theApp.mbWork = false;

	return 0;
}

IMPLEMENT_DYNAMIC(CProcessDlg, CDialogEx)

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessDlg::IDD, pParent)
{
	gpDlg = this;

	mClientWidth = 0;
	mClientHeight = 0;

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
	mpMainArea->SetBackGroundAppear( pTmpApper );

	mpProcess = new CHE_WD_Area( 0, 0, mpInterActive );
	mpProcess->SetWidth( 486 );
	mpProcess->SetHeight( 27 );
	mpProcess->SetPositionX( 30 );
	mpProcess->SetPositionY( 40 );
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
	pTmpArea->SetPositionY( 40 );
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
	pTmpArea->SetPositionY( 40 );
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
	pTmpArea->SetPositionY( 40 );
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
	pTmpArea->SetPositionY( 40 );
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

	mpMainArea->AppendChild( mpProcess );

	mProcessBarValue = 0;
}

CProcessDlg::~CProcessDlg()
{
	CHE_WD_Appearance * pTmpAppear = mpMainArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;

	CHE_WD_Area * pTmpArea = mpMainArea->GetChild( 0 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;

	CHE_WD_Area * pOtherTmpArea = pTmpArea->GetChild( 0 );
	pTmpAppear = pOtherTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pOtherTmpArea;
	pOtherTmpArea = pTmpArea->GetChild( 1 );
	pTmpAppear = pOtherTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pOtherTmpArea;
	pOtherTmpArea = pTmpArea->GetChild( 2 );
	pTmpAppear = pOtherTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pOtherTmpArea;

	delete pTmpArea;

	pTmpArea = mpMainArea->GetChild( 1 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	pTmpAppear = pTmpArea->GetMouseOverAppear();
	delete pTmpAppear->mItems[0];
	delete pTmpAppear;
	delete pTmpArea;

	delete mpInterActive;
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProcessDlg, CDialogEx)
	ON_WM_TIMER()
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

void CProcessDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( theApp.mbWork == false )
	{
		this->EndDialog( 0 );
	}
	CDialogEx::OnTimer(nIDEvent);
}


int CProcessDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	mTimerId = SetTimer( 4, 50, NULL );
	return 0;
}

BOOL CProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_PROCESS );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	SetProcessBarValue( 0 );
	mpMainArea->OnDraw();

	CreateThread( NULL, 0, ThreadSplit, 0, 0, 0 );

	return TRUE;
}

void CProcessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	mClientWidth = cx;
	mClientHeight = cy;

	mpMainArea->SetWidth( cx );
	mpMainArea->SetHeight( cy );

	CWnd * pWnd = GetDlgItem( IDC_PROCESS );
	if ( pWnd )
	{
		pWnd->MoveWindow( 0, 0, mpMainArea->GetWidth(), mpMainArea->GetHeight(), TRUE );
	}
}

void CProcessDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseMove( point.x, point.y );
	CDialogEx::OnMouseMove(nFlags, point);
}

void CProcessDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CProcessDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CProcessDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLButtonUp( point.x, point.y );
	ReleaseCapture();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CProcessDlg::OnPaint()
{
	CPaintDC dc(this);
	UpdateProcessBar();
	DrawMainArea();
}

void CProcessDlg::DrawMainArea()
{
	CWnd *		pWnd = GetDlgItem( IDC_PROCESS );
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

void CProcessDlg::SetProcessBarValue( unsigned int val )
{
	if ( val > 100 )
	{
		val = 100;
	}
	mProcessBarValue = val;
}

void CProcessDlg::UpdateProcessBar()
{
	unsigned int iTmp = mProcessBarValue / 100.0 * 457;
	mpProcess->GetChild(1)->SetWidth( iTmp );
	mpProcess->GetChild(2)->SetPositionX( iTmp + 45 );
	CHE_WD_Area * pTmpArea = NULL;
	CHE_WD_Appearance * pTmpAppear = NULL;
	CHE_WD_AppearText * pTmpText = NULL;
	wchar_t tmpStr[32];
	pTmpArea = mpProcess->GetChild( mpProcess->GetChildrenCount() - 1 );
	pTmpAppear = pTmpArea->GetBackGroundAppear();
	pTmpText = (CHE_WD_AppearText*)( pTmpAppear->mItems[0] );
	if ( mProcessBarValue == 100 )
	{
		pTmpText->SetText( L"Writing file, Please Wait!" );
	}else{
		wsprintf( tmpStr, L"%d%%", mProcessBarValue );
		pTmpText->SetText( tmpStr );
	}
}

void CProcessDlg::OnOK()
{
	//CDialogEx::OnOk();
}

void CProcessDlg::OnCancel()
{
	//CDialogEx::OnCancel();
}

void CProcessDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer( mTimerId );
}

BOOL CProcessDlg::OnEraseBkgnd(CDC* pDC)
{
	Invalidate(FALSE);
	return TRUE;
}

void CProcessDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	Invalidate(FALSE);
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}