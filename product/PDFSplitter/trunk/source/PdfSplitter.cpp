
// PdfSpliter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "Welcome.h"
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
	mbLoadError = false;
	mbRegister = false;
	mpMainDlg = NULL;
	mpProcessDlg = NULL;
	mpFileRead = NULL;
	mfViewPoint = 0.0;
	mItemCount = 0;
	mCurItem = 0;
	mpDocument = NULL;
	mpPageTree = NULL;
}


// The one and only CPdfSpliterApp object

CPdfSpliterApp theApp;


// CPdfSpliterApp initialization

BOOL CPdfSpliterApp::InitInstance()
{
	//_CrtSetBreakAlloc( 1340 );

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

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
	SetRegistryKey(_T("Peroit PDF Splitter"));

// 	check reigster information
	long retVal = 0;
	HKEY hRegKey;
	char tmpStr[1024];
	
	retVal = RegOpenKeyEx( HKEY_CURRENT_USER, L"SOFTWARE\\Peroit Studio\\PDF Splitter", 0, KEY_READ, &hRegKey );
	if ( ERROR_SUCCESS == retVal )
	{
		DWORD valueType = REG_SZ;
		DWORD valueSize = 1023;

		retVal = RegQueryValueExA( hRegKey, "User", 0, &valueType, (BYTE*)&tmpStr, &valueSize );
		if ( ERROR_SUCCESS == retVal )
		{
			theApp.mName = tmpStr;
		}

		valueType = REG_SZ;
		valueSize = 1023;

		retVal = RegQueryValueExA( hRegKey, "Key", 0, &valueType, (BYTE*)&tmpStr, &valueSize );
		if ( ERROR_SUCCESS == retVal )
		{
			theApp.mKey = tmpStr; 
		}
	}else{
		CreateRegEntry();
	}

	if ( ! theApp.CheckRefInfo() )
	{
		CWelcomeDlg dlg;
		dlg.DoModal();
	}

	CPdfSpliterDlg mainDlg;
	mpMainDlg = &mainDlg;
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
	if ( theApp.mpMainDlg->mpDelBtn->GetParent() )
	{
		theApp.mpMainDlg->mpDelBtn->GetParent()->PopChild( 0 );
	}
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
		mpMainDlg->mpItemList->ChildToLower( mCurItem-1 );
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
		mpMainDlg->mpItemList->ChildToUpper( mCurItem-1 );
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
			if ( mFile.Open( mpFileRead ) )
			{
				mpDocument = CHE_PDF_Document::CreateDocument( &mFile );
				mpPageTree = mpDocument->GetPageTree();
			}else{
				mbLoadError = true;
				CloseDocument();
			}
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
	mpMainDlg->ClearFileInfoArea();
	if ( mpFileRead )
	{
		//todo
		//释放文档对象和页面树

		mFile.Close();
		HE_DestoryIHERead( mpFileRead );
		mpFileRead = NULL;
		mbLoadOver = false;
	}
}

void CPdfSpliterApp::CreateRegEntry()
{
	HKEY hRegKey;
	DWORD dwOpt = REG_CREATED_NEW_KEY;
	long retVal = 0;

	retVal = RegCreateKeyEx( HKEY_CURRENT_USER, L"SOFTWARE\\Peroit Studio\\PDF Splitter", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwOpt );
	if ( retVal == ERROR_SUCCESS )
	{
		RegCloseKey( hRegKey );
	}
}

void CPdfSpliterApp::WriteRegInfo()
{
	HKEY hRegKey;
	long retVal = 0;

	retVal = RegOpenKeyEx( HKEY_CURRENT_USER, L"SOFTWARE\\Peroit Studio\\PDF Splitter", 0, KEY_WRITE, &hRegKey );
	if ( ERROR_SUCCESS != retVal )
	{
		CreateRegEntry();

		retVal = RegOpenKeyEx( HKEY_CURRENT_USER, L"SOFTWARE\\Peroit Studio\\PDF Splitter", 0, KEY_WRITE, &hRegKey );
	}

	if ( ERROR_SUCCESS != retVal )
	{
		return;
	}

	DWORD valueType = REG_SZ;
	DWORD valueSize = 0;

	valueSize = theApp.mName.length();
	retVal = RegSetValueExA( hRegKey, "User", 0, valueType, (BYTE*)( theApp.mName.c_str() ), valueSize );

	valueSize = theApp.mKey.length();
	retVal = RegSetValueExA( hRegKey, "Key", 0, valueType, (BYTE*)( theApp.mKey.c_str() ), valueSize );

	RegCloseKey( hRegKey );
}

//为了将代码搞乱的一下内联函数

bool CPdfSpliterApp::CheckRefInfo()
{
	if ( mKey.length() == 0 )
	{
		return false;
	}
	if ( mName.length() == 0 )
	{
		return false;
	}

	unsigned char name[1024];
	char key[1024];

	memset( name, 0, 1024 );
	memset( key, 0, 1024 );

	for ( size_t i = 0; i < mName.length(); ++i )
	{
		name[i] = (unsigned char)( mName[i] );
	}

	for ( size_t i = 0; i < mKey.length(); ++i )
	{
		key[i] = mKey[i];
	}

	for ( size_t i = 0; i < 1024; ++i )
	{
		name[i] = (unsigned char)( name[i] + i );
	}

	for ( size_t i = 0; i < 1023; ++i )
	{
		name[i] = (unsigned char)( name[i] * name[i] );
	}

	char tmpStr[1024];
	memset( tmpStr, 0, 1024 );
	sprintf( tmpStr, "%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X", name[0], name[1], name[2], name[3], name[4], name[5], name[6], name[7], name[8], name[9], name[10], name[11] );

	if ( strcmp( tmpStr, key ) == 0 )
	{
		mbRegister = true;
		return true;
	}
	
	return false;
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

void MyIHE_WDM_InterActive::SetTimer( CHE_WDM_Area * pArea, HE_ULONG elapse )
{
	// 	if ( mpDlg )
	// 	{
	// 		mpDlg->SetTimer( 0, elapse, NULL );
	// 	}
}

void MyIHE_WDM_InterActive::KillTimer( CHE_WDM_Area * pArea )
{
	// 	if ( mpDlg )
	// 	{
	// 		mpDlg->KillTimer( 999 );
	// 	}
}

HE_BOOL	MyIHE_WDM_InterActive::MeasureString( CHE_WDM_AppearTextPtr ptr, HE_ULONG & width, HE_ULONG & height )
{
	if ( ! ptr || ! ptr->GetText() )
	{
		return FALSE;
	}

	PointF pointF;
	RectF rectF;
	StringFormat strFormat;
	CHE_WDM_Layout layout = ptr->GetLayout();
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

	Gdiplus::Font font( L"Arial", ptr->GetSize(), FontStyleRegular, UnitPixel );
	mpGraphics->MeasureString( ptr->GetText(), wcslen( ptr->GetText() ), &font, pointF, &strFormat, &rectF );	
	width = rectF.GetRight() - rectF.GetLeft();
	height = rectF.GetBottom() - rectF.GetTop();
	return TRUE;
}

HE_BOOL MyIHE_WDM_InterActive::MeasureChars( CHE_WDM_AppearTextPtr ptr, HE_ULONG count, HE_ULONG & width, HE_ULONG & height )
{
	if ( ! ptr || ! ptr->GetText() )
	{
		return FALSE;
	}
	HE_UINT32 charCount = wcslen( ptr->GetText() );
	if ( count > charCount )
	{
		return FALSE;
	}

	RectF rectF;
	rectF.X = ptr->GetPosiX();
	rectF.Y = ptr->GetPosiY();
	rectF.Width = ptr->GetWidth();
	rectF.Height = ptr->GetHeight();

	CharacterRange charRanges;
	charRanges.First = 0;
	charRanges.Length = count;

	StringFormat strFormat;
	CHE_WDM_Layout layout = ptr->GetLayout();
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
	strFormat.SetMeasurableCharacterRanges( 1, &charRanges );

	Region charsRegion;
	Gdiplus::Font font( L"Arial", ptr->GetSize(), FontStyleRegular, UnitPixel );

	mpGraphics->MeasureCharacterRanges( ptr->GetText(), count, &font, rectF, &strFormat, 1, &charsRegion );
	charsRegion.GetBounds( &rectF, mpGraphics );
	width = rectF.GetRight() - rectF.GetLeft();
	height = rectF.GetBottom() - rectF.GetTop();
	return TRUE;
}

HE_FLOAT MyIHE_WDM_InterActive::GetFontHeight( CHE_WDM_AppearTextPtr ptr )
{
	if ( ! ptr )
	{
		return 0;
	}
	Gdiplus::Font font( L"Arial", ptr->GetSize(), FontStyleRegular, UnitPixel );
	return font.GetHeight( mpGraphics );
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

void MyIHE_WDM_InterActive::Draw( CHE_WDM_Area * pArea, CHE_WDM_AppearItemPtr ptr )
{
	if ( ! pArea || ! mpGraphics || ! ptr )
	{
		return;
	}

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

	switch( ptr->GetType() )
	{
	case APPEAR_ITEM_TEXT:
		{
			CHE_WDM_AppearTextPtr pTmp = ptr.GetTextPtr();
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
			CHE_WDM_AppearPathPtr pTmp = ptr.GetPathPtr();
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
			CHE_WDM_AppearImagePtr pTmp = ptr.GetImagePtr();
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

	mpGraphics->EndContainer(container);
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