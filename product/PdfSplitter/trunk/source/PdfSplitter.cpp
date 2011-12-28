
// PdfSpliter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PdfSplitterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPdfSpliterApp

BEGIN_MESSAGE_MAP(CPdfSpliterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPdfSpliterApp construction

CPdfSpliterApp::CPdfSpliterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	mbLoadOver = false;
	mbWork = false;
	mpMainDlg = NULL;
	mpProcessDlg = NULL;
	mpFileRead = NULL;
	mfViewPoint = 0.0;
	mItemCount = 0;
	mCurItem = 0;
}


// The one and only CPdfSpliterApp object

CPdfSpliterApp theApp;


// CPdfSpliterApp initialization

BOOL CPdfSpliterApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	mpMainDlg = new CPdfSpliterDlg;
	m_pMainWnd = mpMainDlg;
	INT_PTR nResponse = mpMainDlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CPdfSpliterApp::AddPageListItem( const CListItem & item )
{
	mPageList.push_back( item );
	++mItemCount;
	mpMainDlg->AppendListItem( item );
}

void CPdfSpliterApp::ClearPageListItem()
{
	std::vector< CListItem >::iterator it;
	mpMainDlg->CancelSelection();
	unsigned int iCount = mPageList.size();
	unsigned int i = 0;
	for ( ; i < iCount; ++i )
	{
		mpMainDlg->DeleteListItem( 0 );
	}
	mCurItem = 0;
	mItemCount = 0;
	mPageList.clear();
	mpMainDlg->UpdateSelection();
	mpMainDlg->UpdateToolBtn();
	mpMainDlg->UpdateList();
}

void CPdfSpliterApp::DelCurPageListItem()
{
	std::vector< CListItem >::iterator it;
	unsigned int i = 1;
	it = mPageList.begin();
	for ( ; it != mPageList.end(); ++it, ++i )
	{
		if ( i == mCurItem )
		{
			mpMainDlg->CancelSelection();
			mPageList.erase( it );
			mpMainDlg->DeleteListItem( i-1 );
			mItemCount--;
			if ( mCurItem > mItemCount )
			{
				--mCurItem;
			}
			if ( mItemCount == 0 )
			{
				mCurItem = 0;
			}
			mpMainDlg->UpdateSelection();
			mpMainDlg->UpdateToolBtn();
			mpMainDlg->UpdateList();
			break;
		}
	}
}

void CPdfSpliterApp::UpCurPageListItem()
{
	if ( 1 < mCurItem && mCurItem <= mItemCount )
	{
		mpMainDlg->CancelSelection();
		mpMainDlg->mpListBoxItems->ChildToLower( mCurItem-1 );
		CListItem item = mPageList[mCurItem-2];
		mPageList[mCurItem-2] = mPageList[mCurItem-1];
		mPageList[mCurItem-1] = item;
		mCurItem--;
		mpMainDlg->UpdateSelection();
		mpMainDlg->UpdateToolBtn();
		mpMainDlg->UpdateList();
	}
}

void CPdfSpliterApp::DownCurPagaListItem()
{
	if ( 0 < mCurItem && mCurItem < mItemCount )
	{
		mpMainDlg->CancelSelection();
		mpMainDlg->mpListBoxItems->ChildToUpper( mCurItem-1 );
		CListItem item = mPageList[mCurItem];
		mPageList[mCurItem] = mPageList[mCurItem-1];
		mPageList[mCurItem-1] = item;
		mCurItem++;
		mpMainDlg->UpdateSelection();
		mpMainDlg->UpdateToolBtn();
		mpMainDlg->UpdateList();
	}
}

void CPdfSpliterApp::LoadDocument()
{
	if ( mTargetFile.size() > 0 )
	{
		char tmpStr[1024];
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, mTargetFile.c_str(), -1, tmpStr, 1024, NULL, NULL );
		
		mpFileRead = HE_CreateFileRead( tmpStr, FILEREAD_MODE_DEFAULT, 4096 );
		if ( mpFileRead )
		{
			mParser.Open( theApp.mpFileRead );
			mParser.GetPageCount();
		}
		mbLoadOver = true;
	}
}

void CPdfSpliterApp::CloseDocument()
{
	mbLoadOver = false;
	mbWork = false;
	mTargetFile.clear();
	mNewFile.clear();
	mfViewPoint = 0;
	mPageList.clear();
	mPasswrod.clear();
	mpMainDlg->CancelSelection();
	unsigned int i = 0;
	for ( ; i < mItemCount; ++i )
	{
		mpMainDlg->DeleteListItem( 0 );
	}
	mCurItem = 0;
	mItemCount = 0;
	mpMainDlg->UpdateToolBtn();
	mpMainDlg->UpdateList();
	mpMainDlg->UpdateTargetFileArea();
	mpMainDlg->UpdateNewFileArea();
	mpMainDlg->UpdateFileInfoArea();
	if ( mpFileRead )
	{
		mParser.Close();
		HE_DestoryIHERead( mpFileRead );
		mpFileRead = NULL;
		mbLoadOver = false;
	}
}

void MyIHE_WD_InterActive::Invalidate()
{
	if ( mpDlg )
	{
		mbRefleshAll = true;
		mpDlg->Invalidate(FALSE);
	}
}

void MyIHE_WD_InterActive::InvalidateRect( int left, int top, int right, int bottom )
{
	if ( !mbDirtyRect )
	{
		mbDirtyRect = true;
		mLeft = left;
		mTop = top;
		mRight = right;
		mBottom = bottom;
	}else{
		if ( left < mLeft )
		{
			mLeft = left;
		}
		if ( top < mTop )
		{
			mTop = top;
		}
		if ( right > mRight )
		{
			mRight = right;
		}
		if ( bottom > mBottom )
		{
			mBottom = bottom;
		}
	}
	if ( mpDlg )
	{
		mpDlg->Invalidate(FALSE);
	}
}

void MyIHE_WD_InterActive::SetTimer( unsigned int u )
{
	if ( mpDlg )
	{
		mpDlg->SetTimer( 999, u, NULL );
	}
}

void MyIHE_WD_InterActive::KillTimer()
{
	if ( mpDlg )
	{
		mpDlg->KillTimer( 999 );
	}
}

void MyIHE_WD_InterActive::SetClip( CHE_WD_Area * pArea )
{
	GraphicsContainer container = mpGraphics->BeginContainer();
	mContainerStack.push_back( container );
	Rect clipRect( pArea->GetPositionX(), pArea->GetPositionY(), pArea->GetWidth(), pArea->GetHeight() );
	Region clipRegion( clipRect ); 
	mpGraphics->SetClip( &clipRegion );
}

void MyIHE_WD_InterActive::SetClip( int left, int top, int right, int bottom )
{
	GraphicsContainer container = mpGraphics->BeginContainer();
	mContainerStack.push_back( container );
	Rect clipRect( left, top, right-left, bottom-top );
	Region clipRegion( clipRect );
	mpGraphics->SetClip( &clipRegion );
}

void MyIHE_WD_InterActive::ResetClip()
{
	if ( mContainerStack.size() > 0 )
	{
		mpGraphics->EndContainer( mContainerStack[mContainerStack.size()-1] );
		mContainerStack.pop_back();
	}
	//mpGraphics->ResetClip();
}

void MyIHE_WD_InterActive::Draw( CHE_WD_Area * pArea, CHE_WD_Appearance * pAppear )
{
	if ( pArea && pAppear && mpGraphics )
	{
		GraphicsContainer container = mpGraphics->BeginContainer();
		if ( mbDirtyRect )
		{
			Rect clipRect( mLeft, mTop, mRight-mLeft, mBottom-mTop );
			Region clipRegion( clipRect );
			mpGraphics->SetClip( &clipRegion );
		}
 		if ( pArea->IsClipEnable() )
 		{
 			Rect clipRect( pArea->GetPositionX(), pArea->GetPositionY(), pArea->GetWidth(), pArea->GetHeight() );
 			Region clipRegion( clipRect );
 			mpGraphics->SetClip( &clipRegion );
 		}
		for ( size_t i = 0; i < pAppear->mItems.size(); ++i )
		{
			CHE_WD_AppearItem * tmp = pAppear->mItems[i];
			if( tmp == NULL )
			{
				continue;
			}
			switch( tmp->GetType() )
			{
			case APPEAR_TYPE_TEXT:
				{
					CHE_WD_AppearText * pTmp = (CHE_WD_AppearText *)tmp;
					StringFormat strFormat;
					switch ( pTmp->GetHoriAlign() )
					{
					case APPEAR_TEXT_HALIGNMENT_LEFT:
						strFormat.SetAlignment( StringAlignmentNear );
						break;
					case APPEAR_TEXT_HALIGNMENT_CENTER:
						strFormat.SetAlignment( StringAlignmentCenter );
						break;
					case APPEAR_TEXT_HALIGNMENT_RIGHT:
						strFormat.SetAlignment( StringAlignmentFar );
						break;
					default:;
					}
					switch( pTmp->GetVertAlign() )
					{
					case APPEAR_TEXT_VALIGNMENT_TOP:
						strFormat.SetLineAlignment( StringAlignmentNear );
						break;
					case APPEAR_TEXT_VALIGNMENT_CENTER:
						strFormat.SetLineAlignment( StringAlignmentCenter );
						break;
					case APPEAR_TEXT_VALIGNMENT_BOTTOM:
						strFormat.SetLineAlignment( StringAlignmentFar );
						break;
					default:;
					}
					if ( pTmp->GetFontFile() )
					{
						PrivateFontCollection fontCollection;
						fontCollection.AddFontFile( pTmp->GetFontFile() );
						INT fontFamilyCount = fontCollection.GetFamilyCount();
						FontFamily * pFontFamily = ::new FontFamily[fontFamilyCount];
						fontCollection.GetFamilies( fontFamilyCount, pFontFamily, &fontFamilyCount );
						Gdiplus::Font font( &(pFontFamily[0]), pTmp->GetSize(), FontStyleRegular, UnitPixel );
						RectF rectF(	pArea->GetPositionX() + pTmp->GetPositionX(),
										pArea->GetPositionY()+pTmp->GetPositionY(),
										pTmp->GetWidth(), pTmp->GetHeight() );
						SolidBrush solidBrush( Color( pTmp->GetAlpha() * ( (pTmp->GetColor()>>24) & 0xFF ), (pTmp->GetColor()>>16) & 0xFF, (pTmp->GetColor()>>8) & 0xFF, (pTmp->GetColor()) & 0xFF ) );
						mpGraphics->DrawString( pTmp->GetText(), -1, &font, rectF, &strFormat, &solidBrush );
						delete [] pFontFamily;
					}else{
						FontFamily fontFamily( L"Arial" );
						Gdiplus::Font font( &fontFamily, pTmp->GetSize(), FontStyleRegular, UnitPixel );
						RectF rectF(	pArea->GetPositionX() + pTmp->GetPositionX(),
										pArea->GetPositionY()+pTmp->GetPositionY(),
										pTmp->GetWidth(), pTmp->GetHeight() );
						SolidBrush solidBrush( Color( pTmp->GetAlpha() * ( (pTmp->GetColor()>>24) & 0xFF ), (pTmp->GetColor()>>16) & 0xFF, (pTmp->GetColor()>>8) & 0xFF, (pTmp->GetColor()) & 0xFF ) );
						mpGraphics->DrawString( pTmp->GetText(), -1, &font, rectF, &strFormat, &solidBrush );
					}break;
				}
			case APPEAR_TYPE_PATH:
				{
					CHE_WD_AppearPath * pTmp = (CHE_WD_AppearPath *)tmp;
					unsigned int fillColorVal = pTmp->GetFillColor();
					unsigned int strokeColorVal = pTmp->GetStrokeColor();
					Color fillColor( pTmp->GetAlpha() * ( (fillColorVal>>24) & 0xFF ), (fillColorVal>>16) & 0xFF, (fillColorVal>>8) & 0xFF, fillColorVal & 0xFF );
					Color strokeColor( pTmp->GetAlpha() * ( (strokeColorVal>>24) & 0xFF ), (strokeColorVal>>16) & 0xFF, (strokeColorVal>>8) & 0xFF, strokeColorVal & 0xFF );
					SolidBrush brush( fillColor );
					Pen pen( strokeColor, pTmp->GetLineWidth() );

					GraphicsPath path;
					CHE_WD_AppearPathItem pathItem;
					float a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
					size_t iCount = pTmp->GetItemCount();
					for ( unsigned int i = 0; i < iCount; ++i )
					{
						pTmp->GetItem( i, pathItem );
						switch ( pathItem.GetType() )
						{
						case APPEAR_PATH_ITEM_LINE:
							{
								pTmp->GetItem( ++i, pathItem );
								a = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								pTmp->GetItem( ++i, pathItem );
								c = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								d = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								path.AddLine( a, b, c, d );
								break;
							}
						case APPEAR_PATH_ITEM_RECT:
							{
								pTmp->GetItem( ++i, pathItem );
								a = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								pTmp->GetItem( ++i, pathItem );
								c = pathItem.GetValue();
								pTmp->GetItem( ++i, pathItem );
								d = pathItem.GetValue();
								RectF rectF( a, b, c, d );
								path.AddRectangle( rectF );
								break;
							}
						case APPEAR_PATH_ITEM_CURVE:
							{
								pTmp->GetItem( ++i, pathItem );
								a = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								pTmp->GetItem( ++i, pathItem );
								c = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								d = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								pTmp->GetItem( ++i, pathItem );
								e = pathItem.GetValue() + pTmp->GetPositionX() + pArea->GetPositionX();
								pTmp->GetItem( ++i, pathItem );
								f = pathItem.GetValue() + pTmp->GetPositionY() + pArea->GetPositionY();
								PointF point[3];
								point[0].X = a;
								point[0].Y = b;
								point[1].X = c;
								point[1].Y = d;
								point[2].X = e;
								point[2].Y = f;
								path.AddCurve( point, 3 );
								break;
							}
						default:;
						}
					}
					if ( pTmp->GetFillMode() == APPEAR_PATH_FILL_EVENODD )
					{
						path.SetFillMode( FillModeWinding );
					}
					else
					{
						path.SetFillMode( FillModeAlternate );
					}
					switch ( pTmp->GetOperator() )
					{
					case APPEAR_PATH_FILL:
						{
							mpGraphics->FillPath( &brush, &path );
							break;
						}
					case APPEAR_PATH_STROKE:
						{
							mpGraphics->DrawPath( &pen, &path );
							break;
						}
					case APPEAR_PATH_FILL_STROKE:
						{
							mpGraphics->FillPath( &brush, &path );
							mpGraphics->DrawPath( &pen, &path );
							break;
						}
					default:;
					}
					break;
				}
			case APPEAR_TYPE_IMAGE:
				{
					CHE_WD_AppearImage * pTmp = (CHE_WD_AppearImage *)tmp;
					Image * tmpImage = NULL;
					if ( pTmp->GetExtData() == NULL )
					{
						tmpImage = ::new Image( pTmp->GetFile() );
						pTmp->SetExtData( (void*)( tmpImage ) );
					}else{
						tmpImage = (Image*)( pTmp->GetExtData() );
					}
					switch ( pTmp->GetStyle() )
					{
					case APPEAR_IMAGE_STYLE_SINGLE:
						{
							ImageAttributes imageAtr;
							ColorMatrix colorMatrix = {	1, 0, 0, 0, 0,
								0, 1, 0, 0, 0,
								0, 0, 1, 0, 0,
								0, 0, 0, pTmp->GetAlpha(), 0,
								0, 0, 0, 0, 1 };
							imageAtr.SetColorMatrix( &colorMatrix );
							mpGraphics->DrawImage(	tmpImage, 
								Rect( pArea->GetPositionX() + pTmp->GetPositionX(), pArea->GetPositionY() + pTmp->GetPositionY(), tmpImage->GetWidth(), tmpImage->GetHeight() ),
								(INT)0,
								0,
								tmpImage->GetWidth(),
								tmpImage->GetHeight(),
								UnitPixel,
								&imageAtr );
							break;
						}
					case APPEAR_IMAGE_STYLE_TILTING:
						{
							TextureBrush brush( tmpImage );
							mpGraphics->FillRectangle( &brush, pArea->GetPositionX(), pArea->GetPositionY(), pArea->GetWidth(), pArea->GetHeight() );
							break;
						}
					case APPEAR_IMAGE_STYLE_FIT:
						{
							mpGraphics->DrawImage( tmpImage, pArea->GetPositionX(), pArea->GetPositionY(), pArea->GetWidth(), pArea->GetHeight() );
							break;
						}
					default:;
					}
					break;
				}
			default:;
			}
		}
		mpGraphics->EndContainer(container);
	}
}

