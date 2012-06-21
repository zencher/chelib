#include "stdafx.h"
#include "PdfMerger.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"

#include "../../../../trunk/include/pdf/che_pdf_creator.h"
#include "../../../../trunk/include/pdf/che_pdf_objclone.h"
#include "../../../../trunk/include/pdf/che_pdf_image.h"
#include "../../../../trunk/include/pdf/che_pdf_contentresmgr.h"

CProcessDlg * gpDlg = NULL;

static void EventCancelBtnClick( CHE_WDM_Area * pArea )
{
}

static void EventCloseDialog( CHE_WDM_Area * pArea )
{
	gpDlg->EndDialog( 0 );
}

static void EventOpenFile( CHE_WDM_Area* pArea )
{
	ShellExecute( gpDlg->GetSafeHwnd(), L"open", theApp.mNewFile.c_str(), NULL, NULL, SW_SHOWNORMAL );

	gpDlg->EndDialog( 0 );
}

static void EventOpenFolder( CHE_WDM_Area * pArea )
{
	std::wstring cmdStr = L"/select,";
	cmdStr += theApp.mNewFile;
	ShellExecute( gpDlg->GetSafeHwnd(), L"open", L"Explorer.exe", cmdStr.c_str(), NULL, SW_SHOWNORMAL );
	gpDlg->EndDialog( 0 );
}

DWORD WINAPI ThreadMerge( LPVOID lpParameter )
{
	//flag for working
	theApp.mbWork = true;

	CHE_PDF_File newFile;
	newFile.SetPDFVersion( PDF_VERSION_1_7 );

	CHE_PDF_Document * pNewDocument = CHE_PDF_Document::CreateDocument( &newFile );

	CHE_PDF_PageTree * pNewPageTree = pNewDocument->GetPageTree();

	std::vector< CListItem >::iterator it;

	unsigned int iCurPage = 0;
	unsigned int iPageCount = 0;
	unsigned int indexFile = 0;

	gpDlg->mpMainArea->Refresh();

	for ( it = theApp.mList.begin(); it != theApp.mList.end(); ++it )
	{
		iPageCount += (*it).pageCount;
	}

	for ( it = theApp.mList.begin(); it != theApp.mList.end(); ++it, ++iCurPage, ++indexFile )
	{
		unsigned int iIndex = (*it).pageIndex;
		unsigned int iCount = (*it).pageCount;
		CListItemType type = (*it).type;
		CPDFFileInfo * pFileInfo = &( theApp.mFileCache[ (*it).index ] );

		CHE_ObjectCloneMgr ObjCloneMgr;

		for ( unsigned int i = 0; i < iCount; i++ )
		{
			if ( theApp.mpProcessDlg )
			{
				theApp.mpProcessDlg->SetProcessBarValue( (unsigned int)( (iCurPage+1) * 100.0 / iPageCount ) );
				theApp.mpProcessDlg->mpMainArea->Refresh();
			}
			iCurPage++;

			CHE_PDF_DictionaryPtr OldPageDictPtr;
			
			CHE_PDF_Page * pOldPage = NULL;
			
			if ( type == EVEN_PAGES )
			{
				pOldPage = pFileInfo->mpPageTree->GetPage( i * 2 );
			}else if ( type == EVEN_PAGES )
			{
				pOldPage = pFileInfo->mpPageTree->GetPage( i * 2 + 1 );
			}else{
				pOldPage = pFileInfo->mpPageTree->GetPage( iIndex + i - 1 );
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
			if ( tmpDict )
			{
				NewPageDictPtr->SetAtDictionary( "Resources", tmpDict );
			}
			CHE_PDF_ArrayPtr tmpArray = CloneDirectArrayObj( pOldPage->GetMediaBoxArray(), &newFile, &ObjCloneMgr );
			if ( tmpArray )
			{
				NewPageDictPtr->SetAtArray( "MediaBox", tmpArray );
			}
			HE_INT32 rotate = pOldPage->GetRotate();
			NewPageDictPtr->SetAtInteger( "Rotate", rotate );

			//release pages
			CHE_PDF_Page::ReleasePage( pOldPage );
			CHE_PDF_Page::ReleasePage( pNewPage );
		}
	}

	if ( ! theApp.CheckRefInfo() )
	{
		IHE_Read * pTmpRead = HE_CreateFileRead( "resData.dat" );
		if ( pTmpRead )
		{
			CHE_PDF_File file;
			if ( file.Open( pTmpRead ) )
			{
				CHE_PDF_Document * pDoc = CHE_PDF_Document::CreateDocument( &file );
				
				if ( pDoc )
				{
					CHE_PDF_PageTree * pPageTree = pDoc->GetPageTree();

					CHE_ObjectCloneMgr ObjCloneMgr;
					CHE_PDF_DictionaryPtr OldPageDictPtr;

					CHE_PDF_Page * pOldPage= pPageTree->GetPage( 0 );

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
		}
	}

	newFile.SetInfo( DOCUMENT_INFO_CREATOR, "Peroit PDF Merger 1.0" );

	newFile.SetInfo( DOCUMENT_INFO_PRODUCER, "Peroit PDF Merger 1.0" );

	char tmpStr[1024];
	if ( theApp.mFileInfoTitle.size() > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, theApp.mFileInfoTitle.c_str(), -1, tmpStr, 1024, NULL, NULL );
		newFile.SetInfo( DOCUMENT_INFO_TITLE, tmpStr );
	}

	if ( theApp.mFileInfoSubject.size() > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, theApp.mFileInfoSubject.c_str(), -1, tmpStr, 1024, NULL, NULL );
		newFile.SetInfo( DOCUMENT_INFO_SUBJECT, tmpStr );
	}

	if ( theApp.mFileInfoAuthor.size() > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, theApp.mFileInfoAuthor.c_str(), -1, tmpStr, 1024, NULL, NULL );
		newFile.SetInfo( DOCUMENT_INFO_AUTHOR, tmpStr );
	}

	if ( theApp.mFileInfoKeywords.size() > 0 )
	{
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, theApp.mFileInfoKeywords.c_str(), -1, tmpStr, 1024, NULL, NULL );
		newFile.SetInfo( DOCUMENT_INFO_KEYWORDS, tmpStr );
	}

	theApp.mpProcessDlg->mpMainArea->Refresh();

	memset( tmpStr, 0, 1024 );
	WideCharToMultiByte( CP_ACP, 0, theApp.mNewFile.c_str(), -1, tmpStr, 1024, NULL, NULL );
	IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );

	newFile.Save( pWrite );

	HE_DestoryIHEWrite( pWrite );

	theApp.mbWork = false;

	theApp.mpProcessDlg->ShowTips();

	return 0;
}

IMPLEMENT_DYNAMIC(CProcessDlg, CDialogEx)

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessDlg::IDD, pParent)
{
	gpDlg = this;

	mClientWidth = 0;
	mClientHeight = 0;

	CHE_WDM_AppearImagePtr imagePtr;
	CHE_WDM_AppearTextPtr textPtr;

	mpInterActive = new MyIHE_WDM_InterActive( this, theApp.m_hInstance );
	mpMainArea = CHE_WDM_Area::Create( mpInterActive );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\background.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_TILTING );
	mpMainArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpProcess = CHE_WDM_Area::Create( mpInterActive );
	mpProcess->SetWidth( 486 );
	mpProcess->SetHeight( 27 );
	mpProcess->SetPosiX( 30 );
	mpProcess->SetPosiY( 45 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\processbg.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	mpProcess->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	CHE_WDM_Area * pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetWidth( 15 );
	pTmpArea->SetHeight( 26 );
	pTmpArea->SetPosiX( 30 );
	pTmpArea->SetPosiY( 45 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\processleft.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpProcess->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetWidth( 100 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPosiX( 45 );
	pTmpArea->SetPosiY( 45 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\process.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	pTmpArea->EnableClip();

	mpProcess->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetWidth( 15 );
	pTmpArea->SetHeight( 26 );
	pTmpArea->SetPosiX( 144 );
	pTmpArea->SetPosiY( 45 );
	
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\processright.png" );
	imagePtr->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );

	mpProcess->AppendChild( pTmpArea );

	pTmpArea = CHE_WDM_Area::Create( mpInterActive );
	pTmpArea->SetWidth( 485 );
	pTmpArea->SetHeight( 27 );
	pTmpArea->SetPosiX( 30 );
	pTmpArea->SetPosiY( 45 );
	
	textPtr = CHE_WDM_AppearText::Create();
	textPtr->SetSize( 12 );
	textPtr->SetColor( 0xFFFFFFFF );
	textPtr->SetPosiX( 0 );
	textPtr->SetPosiY( 0 );
	textPtr->SetWidth( 485 );
	textPtr->SetHeight( 27 );
	textPtr->SetLayout( CHE_WDM_Layout( LAYOUT_ALIGN_CENTER, LAYOUT_ALIGN_CENTER ) );
	pTmpArea->AppendAppearItem( textPtr, AREA_APPEAR_BACKGROUND );

	mpProcess->AppendChild( pTmpArea );

	mpMainArea->AppendChild( mpProcess );

	mpTipArea = CHE_WDM_Area::Create( mpInterActive );

	mpTipArea->SetPosiX( 0 );
	mpTipArea->SetPosiY( 0 );
	mpTipArea->SetHeight( 150 );
	mpTipArea->SetWidth( 550 );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\FinishTip.png" );
	imagePtr->SetPosiX( 165 );
	imagePtr->SetPosiY( 10 );
	mpTipArea->AppendAppearItem( imagePtr, AREA_APPEAR_BACKGROUND );
	mpMainArea->AppendChild( mpTipArea );

	CHE_WDM_Button * pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 60 );
	pBtn->SetPosiY( 60 );
	pBtn->SetWidth( 107 );
	pBtn->SetHeight( 20 );
	pBtn->SetMouseLBUEvent( EventOpenFile );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OpenFileBtn.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OpenFileBtnHover.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpTipArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 210 );
	pBtn->SetPosiY( 60 );
	pBtn->SetWidth( 122 );
	pBtn->SetHeight( 20 );
	pBtn->SetMouseLBUEvent( EventOpenFolder );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OpenFolderBtn.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\OpenFolderBtnHover.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpTipArea->AppendChild( pBtn );

	pBtn = CHE_WDM_Button::Create( mpInterActive );
	pBtn->SetPosiX( 360 );
	pBtn->SetPosiY( 60 );
	pBtn->SetWidth( 118 );
	pBtn->SetHeight( 18 );
	pBtn->SetMouseLBUEvent( EventCloseDialog );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CloseDialogBtn.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_NORMAL );
	imagePtr = CHE_WDM_AppearImage::Create();
	imagePtr->SetImageFile( L"images\\CloseDialogBtnHover.png" );
	pBtn->AppendAppearItem( imagePtr, AREA_APPEAR_MOUSEOVER );
	mpTipArea->AppendChild( pBtn );

	mpTipArea->SetPosiY( 150 );

	mProcessBarValue = 0;
}

CProcessDlg::~CProcessDlg()
{
	mMemdc.SelectObject( &mpOldBitmap );
	::delete mGraphics;
	delete mpInterActive;
	delete mpMainArea;
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
	mAnimationMgr.Execute();
	CDialogEx::OnTimer(nIDEvent);
}


int CProcessDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	mTimerId = SetTimer( 4, 10, NULL );
	return 0;
}

BOOL CProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd *		pWnd = GetDlgItem( IDC_MAIN );
	CPaintDC	dc( pWnd );
	mMemdc.CreateCompatibleDC( &dc );
	mBitmap.CreateCompatibleBitmap( &dc, mpMainArea->GetWidth(), mpMainArea->GetHeight() );
	CBitmap * olbBitmap = mMemdc.SelectObject( &mBitmap );
	mGraphics = ::new Graphics( mMemdc.GetSafeHdc() );
	mGraphics->SetSmoothingMode( SmoothingModeAntiAlias );
	mpInterActive->SetGraphics( mGraphics );
	SetProcessBarValue( 0 );
	mpMainArea->OnDraw();

	CreateThread( NULL, 0, ThreadMerge, 0, 0, 0 );

	return TRUE;
}

void CProcessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize( nType, 550, 150 );

	mClientWidth = 550;
	mClientHeight = 150;

	mpMainArea->SetWidth( mClientWidth );
	mpMainArea->SetHeight( mClientHeight );

	this->MoveWindow( 0, 0, mClientWidth, mClientHeight );

	CWnd * pWnd = GetDlgItem( IDC_MAIN );
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
	mpMainArea->OnMouseLBDown( point.x, point.y );
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CProcessDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBDown( point.x, point.y );
	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CProcessDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpMainArea->OnMouseLBUp( point.x, point.y );
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
	unsigned int iTmp = mProcessBarValue / 100.0 * 455;
	mpProcess->GetChild(1)->SetWidth( iTmp );
	mpProcess->GetChild(2)->SetPosiX( iTmp + 45 );
	CHE_WDM_Area * pTmpArea = NULL;
	CHE_WDM_AppearTextPtr textPtr;
	wchar_t tmpStr[32];
	pTmpArea = mpProcess->GetChild( mpProcess->GetChildrenCount() - 1 );
	textPtr =  pTmpArea->GetAppear().mBackground[0].GetTextPtr();
	if ( mProcessBarValue == 100 )
	{
		textPtr->SetText( L"Writing file, Please Wait!" );
	}else{
		wsprintf( tmpStr, L"%d%%", mProcessBarValue );
		textPtr->SetText( tmpStr );
	}
}

void CProcessDlg::ShowTips()
{
	mpProcess->SetEnable( false );
	mpProcess->SetVisable( false );

	mpTipArea->SetPosiY( 150 );
	mpTipArea->Refresh();

	CHE_WDM_AnimationData data;
	data.mPosiX = 0;
	data.mPosiY = 150;
	CHE_WDM_AreaAnimation animation;
	animation.SetTarget( mpTipArea );
	animation.SetState( data );
	data.mPosiY = 0;
	animation.InsertFrames( 20, data );
	mAnimationMgr.StartAreaAnimation( animation );
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