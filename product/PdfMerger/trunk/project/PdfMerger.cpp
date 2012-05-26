
// PdfMerger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "PdfMergerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPdfMergerApp

BEGIN_MESSAGE_MAP(CPdfMergerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPdfMergerApp construction

CPdfMergerApp::CPdfMergerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPdfMergerApp object

CPdfMergerApp theApp;


// CPdfMergerApp initialization

BOOL CPdfMergerApp::InitInstance()
{
	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

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

	CPdfMergerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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

MyIHE_WDM_InterActive::~MyIHE_WDM_InterActive()
{
	for ( size_t i = 0; i < mImageCache.size(); ++i )
	{
		Image * pImage = mImageCache[i];
		::delete pImage;
	}
}

void MyIHE_WDM_InterActive::Invalidate()
{
	if ( mpDlg )
	{
		mbRefleshAll = true;
		mpDlg->Invalidate(FALSE);
	}
}

void MyIHE_WDM_InterActive::InvalidateRect( int left, int top, int right, int bottom )
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

void MyIHE_WDM_InterActive::SetTimer( HE_DWORD u )
{
	if ( mpDlg )
	{
		mpDlg->SetTimer( 999, u, NULL );
	}
}

void MyIHE_WDM_InterActive::KillTimer()
{
	if ( mpDlg )
	{
		mpDlg->KillTimer( 999 );
	}
}

void MyIHE_WDM_InterActive::SetClip( CHE_WDM_Area * pArea )
{
	GraphicsContainer container = mpGraphics->BeginContainer();
	mContainerStack.push_back( container );
	Rect clipRect( pArea->GetPosiX(), pArea->GetPosiY(), pArea->GetWidth(), pArea->GetHeight() );
	Region clipRegion( clipRect ); 
	mpGraphics->SetClip( &clipRegion );
}

void MyIHE_WDM_InterActive::SetClip( int left, int top, int right, int bottom )
{
	GraphicsContainer container = mpGraphics->BeginContainer();
	mContainerStack.push_back( container );
	Rect clipRect( left, top, right-left, bottom-top );
	Region clipRegion( clipRect );
	mpGraphics->SetClip( &clipRegion );
}

void MyIHE_WDM_InterActive::ResetClip()
{
	if ( mContainerStack.size() > 0 )
	{
		mpGraphics->EndContainer( mContainerStack[mContainerStack.size()-1] );
		mContainerStack.pop_back();
	}
	//mpGraphics->ResetClip();
}

void MyIHE_WDM_InterActive::Draw( CHE_WDM_Area * pArea, WDM_AREA_APPEAR_TYPE type )
{
	if ( ! pArea || ! mpGraphics )
	{
		return;
	}

	CHE_WDM_Appearance & appear = pArea->GetAppear();

	std::vector<CHE_WDM_AppearItemPtr> * pAppearItemArr = NULL;

	switch ( type )
	{
	case AREA_APPEAR_NORMAL:
		pAppearItemArr = &( appear.mNormal );
		break;
	case AREA_APPEAR_DISABLE:
		pAppearItemArr = &( appear.mDisable );
		break;
	case AREA_APPEAR_BACKGROUND:
		pAppearItemArr = &( appear.mBackground );
		break;
	case AREA_APPEAR_MOUSEOVER:
		pAppearItemArr = &( appear.mMouseOver );
		break;
	case AREA_APPEAR_MOUSELBDOWN:
		pAppearItemArr = &( appear.mMouseLButtonDown );
		break;
	case AREA_APPEAR_MOUSERBDOWN:
		pAppearItemArr = &( appear.mMouseRButtonDown );
		break;
	default:
		break;
	}

	if ( pAppearItemArr && pAppearItemArr->size() > 0 )
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
			Rect clipRect( pArea->GetPosiX(), pArea->GetPosiY(), pArea->GetWidth(), pArea->GetHeight() );
			Region clipRegion( clipRect );
			mpGraphics->SetClip( &clipRegion );
		}
		for ( size_t i = 0; i < pAppearItemArr->size(); ++i )
		{
			CHE_WDM_AppearItemPtr tmp = (*pAppearItemArr)[i];
			if( tmp == NULL )
			{
				continue;
			}
			switch( tmp->GetType() )
			{
			case APPEAR_ITEM_TEXT:
				{
					CHE_WDM_AppearTextPtr pTmp = tmp.GetTextPtr();
					StringFormat strFormat;
					CHE_WDM_Layout layout = pTmp->GetLayout();
					switch ( layout.GetVertLayout() )
					{
					case LAYOUT_ALIGN_LEFT_OR_TOP:
						strFormat.SetAlignment( StringAlignmentNear );
						break;
					case LAYOUT_ALIGN_CENTER:
						strFormat.SetAlignment( StringAlignmentCenter );
						break;
					case LAYOUT_ALIGN_RIGHT_OR_BOTTOM:
						strFormat.SetAlignment( StringAlignmentFar );
						break;
					default:;
					}
					switch( layout.GetHoriLayout() )
					{
					case LAYOUT_ALIGN_LEFT_OR_TOP:
						strFormat.SetLineAlignment( StringAlignmentNear );
						break;
					case LAYOUT_ALIGN_CENTER:
						strFormat.SetLineAlignment( StringAlignmentCenter );
						break;
					case LAYOUT_ALIGN_RIGHT_OR_BOTTOM:
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
						RectF rectF( pArea->GetPosiX() + pTmp->GetPosiX(), pArea->GetPosiY()+pTmp->GetPosiY(), pTmp->GetWidth(), pTmp->GetHeight() );
						SolidBrush solidBrush( Color( pTmp->GetAlpha() * ( (pTmp->GetColor()>>24) & 0xFF ), (pTmp->GetColor()>>16) & 0xFF, (pTmp->GetColor()>>8) & 0xFF, (pTmp->GetColor()) & 0xFF ) );
						mpGraphics->DrawString( pTmp->GetText(), -1, &font, rectF, &strFormat, &solidBrush );
						delete [] pFontFamily;
					}else{
						FontFamily fontFamily( L"Arial" );
						Gdiplus::Font font( &fontFamily, pTmp->GetSize(), FontStyleRegular, UnitPixel );
						RectF rectF(	pArea->GetPosiX() + pTmp->GetPosiX(), pArea->GetPosiY() + pTmp->GetPosiY(), pTmp->GetWidth(), pTmp->GetHeight() );
						SolidBrush solidBrush( Color( pTmp->GetAlpha() * ( (pTmp->GetColor()>>24) & 0xFF ), (pTmp->GetColor()>>16) & 0xFF, (pTmp->GetColor()>>8) & 0xFF, (pTmp->GetColor()) & 0xFF ) );
						mpGraphics->DrawString( pTmp->GetText(), -1, &font, rectF, &strFormat, &solidBrush );
					}break;
				}
			case APPEAR_ITEM_PATH:
				{
					CHE_WDM_AppearPathPtr pTmp = tmp.GetPathPtr();
					unsigned int fillColorVal = pTmp->GetFillColor();
					unsigned int strokeColorVal = pTmp->GetStrokeColor();
					Color fillColor( pTmp->GetAlpha() * ( (fillColorVal>>24) & 0xFF ), (fillColorVal>>16) & 0xFF, (fillColorVal>>8) & 0xFF, fillColorVal & 0xFF );
					Color strokeColor( pTmp->GetAlpha() * ( (strokeColorVal>>24) & 0xFF ), (strokeColorVal>>16) & 0xFF, (strokeColorVal>>8) & 0xFF, strokeColorVal & 0xFF );
					SolidBrush brush( fillColor );
					Pen pen( strokeColor, pTmp->GetLineWidth() );

					GraphicsPath path;
					CHE_WDM_AppearPathItem pathItem;
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
								a = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
								pTmp->GetItem( ++i, pathItem );
								c = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								d = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
								path.AddLine( a, b, c, d );
								break;
							}
						case APPEAR_PATH_ITEM_RECT:
							{
								pTmp->GetItem( ++i, pathItem );
								a = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
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
								a = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								b = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
								pTmp->GetItem( ++i, pathItem );
								c = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								d = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
								pTmp->GetItem( ++i, pathItem );
								e = pathItem.GetValue() + pTmp->GetPosiX() + pArea->GetPosiX();
								pTmp->GetItem( ++i, pathItem );
								f = pathItem.GetValue() + pTmp->GetPosiY() + pArea->GetPosiY();
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
			case APPEAR_ITEM_IMAGE:
				{
					CHE_WDM_AppearImagePtr pTmp = tmp.GetImagePtr();
					Image * tmpImage = NULL;
					if ( pTmp->GetExtData() == NULL )
					{
						tmpImage = ::new Image( pTmp->GetImageFile() );
						mImageCache.push_back( tmpImage );
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
								Rect( pArea->GetPosiX() + pTmp->GetPosiX(), pArea->GetPosiY() + pTmp->GetPosiY(), tmpImage->GetWidth(), tmpImage->GetHeight() ),
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
							mpGraphics->FillRectangle( &brush, pArea->GetPosiX(), pArea->GetPosiY(), pArea->GetWidth(), pArea->GetHeight() );
							break;
						}
					case APPEAR_IMAGE_STYLE_FIT:
						{
							mpGraphics->DrawImage( tmpImage, pArea->GetPosiX(), pArea->GetPosiY(), pArea->GetWidth(), pArea->GetHeight() );
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