#include "stdafx.h"
#include "PdfSplitter.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"

#include "../../../../trunk//include/pdf/che_pdf_creator.h"
#include "../../../../trunk//include/pdf/che_pdf_objclone.h"

CProcessDlg * gpDlg = NULL;

static void EventCancelBtnClick( CHE_WD_Area * pArea )
{
}

DWORD WINAPI ThreadSplit( LPVOID lpParameter )
{
	theApp.mbWork = true;

	CHE_PDF_Creator creator;
	if ( creator.NewDocument() == FALSE )
	{
		return 0;
	}

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
					pNewPageDict->SetAtArray( key, CloneDirectArrayObj( pTmpObj->ToArray(), &creator, &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_DICTIONARY:
					pNewPageDict->SetAtDictionary( key, CloneDirectDictObj( pTmpObj->ToDict(), &creator, &ObjCloneMgr ) );
					break;
				case OBJ_TYPE_REFERENCE:
					{
						HE_PDF_InObjectNumbers numbers = CloneIndirectObject( pTmpObj->ToReference(), &creator, &ObjCloneMgr );
						if ( numbers.objNum == 0 )
						{
							int x = 0;
						}
						pNewPageDict->SetAtReference( key, numbers.objNum, numbers.genNum, NULL );
						break;
					}
				default:
					break;
				}
			}
		}
	}

	theApp.mpProcessDlg->mpMainArea->Refresh();

	char tmpStr[1024];
	memset( tmpStr, 0, 1024 );
	WideCharToMultiByte( CP_ACP, 0, theApp.mNewFile.c_str(), -1, tmpStr, 1024, NULL, NULL );
	IHE_Write * pWrite = HE_CreateFileWrite( tmpStr );

	creator.Save( pWrite );

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

	mpCancelBtn = new CHE_WD_Button( mpInterActive );
	mpCancelBtn->SetPositionX( 215 );
	mpCancelBtn->SetPositionY( 80 );
	mpCancelBtn->SetWidth( 96 );
	mpCancelBtn->SetHeight( 32 );
	mpCancelBtn->SetClickEvent( EventCancelBtnClick );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\cancelTextBtn.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetBackGroundAppear( pTmpApper );
	pTmpApper = new CHE_WD_Appearance();
	pTmpImage = new CHE_WD_AppearImage();
	pTmpImage->SetImageFile( L"images\\cancelTextBtnHover.png" );
	pTmpImage->SetStyle( APPEAR_IMAGE_STYLE_SINGLE );
	pTmpApper->mItems.push_back( pTmpImage );
	mpCancelBtn->SetMouseOverAppear( pTmpApper );

	mpCancelBtn->SetVisable( false );

	mpMainArea->AppendChild( mpCancelBtn );

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
	pOtherTmpArea = pTmpArea->GetChild( 3 );
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
		pTmpText->SetText( L"正在写文件，即将完成。" );
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