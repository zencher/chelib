// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PDFReader.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CMDIFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CMDIFrameWndEx::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CMDIFrameWndEx::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CMDIFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(IDC_RIBBON_STATUS_ZOOM_SLIDER, &CMainFrame::OnZoom)
	ON_COMMAND(IDC_RIBBON_STATUS_ZOOM_BTN, &CMainFrame::OnZoomBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGENAVI_FIRSTPAGE, &CMainFrame::OnFirstPageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGENAVI_PREVIOUSPAGE, &CMainFrame::OnPreviousPageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGENAVI_NEXTPAGE, &CMainFrame::OnNextPageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGENAVI_LASTPAGE, &CMainFrame::OnLastPageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE, &CMainFrame::OnSinglePageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE_CONTINUOUS, &CMainFrame::OnSinglePageContinuousBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE, &CMainFrame::OnDoublePageBtn)
	ON_COMMAND(IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE_CONTINUOUS, &CMainFrame::OnDoublePageBtn)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	BOOL bNameValid;
	CString resStr;
	
	CMFCToolBarImages imageList;
	imageList.SetImageSize(CSize(16, 16));
	imageList.Load(IDB_PAGENAVI);

	mpPageNaviGroup = new CMFCRibbonButtonsGroup;
	mpPageNaviGroup->SetImages( &imageList, NULL, NULL );
	mpPageNaviGroup->SetID( IDC_RIBBON_STATUS_PAGENAVI_GROUP );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGENAVI_FIRSTPAGE);
	ASSERT(bNameValid);
	mpPageNaviGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGENAVI_FIRSTPAGE, resStr, 0, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGENAVI_PREVIOUSPAGE);
	ASSERT(bNameValid);
	mpPageNaviGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGENAVI_PREVIOUSPAGE, resStr, 1, -1, TRUE ) );
	
	mpPageNaviComboBox = new CMFCRibbonComboBox( IDC_RIBBON_STATUS_PAGENAVI_COMBOBOX, TRUE, 100 );
	mpPageNaviComboBox->SetEditText(_T("1/23"));
	mpPageNaviGroup->AddButton( mpPageNaviComboBox );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGENAVI_NEXTPAGE);
	ASSERT(bNameValid);
	mpPageNaviGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGENAVI_NEXTPAGE, resStr, 2, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGENAVI_LASTPAGE);
	ASSERT(bNameValid);
	mpPageNaviGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGENAVI_LASTPAGE, resStr, 3, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGENAVI_DES);
	ASSERT(bNameValid);
	m_wndStatusBar.AddExtendedElement( mpPageNaviGroup, resStr );

	mpPageViewModeGroup = new CMFCRibbonButtonsGroup;
	imageList.Load( IDB_PAGEMODE );

	mpPageViewModeGroup->SetImages( &imageList, NULL, NULL );
	mpPageViewModeGroup->SetID( IDC_RIBBON_STATUS_PAGEVIEWMODE_GROUP );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGEVIEWMODE_SINGLEPAGE);
	ASSERT(bNameValid);
	mpPageViewModeGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE, resStr, 0, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGEVIEWMODE_CONTINUOUSPAGE);
	ASSERT(bNameValid);
	mpPageViewModeGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE_CONTINUOUS, resStr, 1, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGEVIEWMODE_DOUBLEPAGES);
	ASSERT(bNameValid);
	mpPageViewModeGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE, resStr, 2, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGEVIEWMODE_DOUBLEPAGECONTINUOUS);
	ASSERT(bNameValid);
	mpPageViewModeGroup->AddButton( new CMFCRibbonButton( IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE_CONTINUOUS, resStr, 3, -1, TRUE ) );

	bNameValid = resStr.LoadString(IDS_STATUS_PAGEVIEWMODE);
	ASSERT(bNameValid);
	m_wndStatusBar.AddExtendedElement( mpPageViewModeGroup, resStr );

	mpZoomBtn = new CMFCRibbonButton( IDC_RIBBON_STATUS_ZOOM_BTN, _T("  100%") );
	bNameValid = resStr.LoadString(IDS_STATUS_PAGESCALE);
	ASSERT(bNameValid);
	m_wndStatusBar.AddExtendedElement( mpZoomBtn, resStr );
	
	mpZoomSlider = new CMFCRibbonSlider(IDC_RIBBON_STATUS_ZOOM_SLIDER);
	mpZoomSlider->SetZoomButtons( TRUE );
	mpZoomSlider->SetRange(-100, 100);
	mpZoomSlider->SetPos(0);
	bNameValid = resStr.LoadString(IDS_STATUS_PAGESCALE_SLIDER);
	ASSERT(bNameValid);
	m_wndStatusBar.AddExtendedElement(mpZoomSlider, resStr  );

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Create and setup "Outlook" navigation bar:
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 250))
	{
		TRACE0("Failed to create navigation pane\n");
		return -1;      // fail to create
	}

	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}

	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth)
{
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	tree.Create(dwTreeStyle, rectDummy, &bar, 1200);
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 2, TRUE, dwStyle);

	calendar.Create(rectDummy, &bar, 1201);
	bNameValid = strTemp.LoadString(IDS_CALENDAR);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&calendar, strTemp, 3, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);

	bar.SetButtonsFont(&afxGlobalData.fontBold);

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnZoom()
{
	wchar_t tmpstr[128];
	wsprintfW( tmpstr, L"%d", mpZoomSlider->GetPos() );
	mpZoomBtn->SetText( tmpstr );
 	mpZoomBtn->Redraw();
// 	m_wndStatusBar.RecalcLayout();
}

void CMainFrame::OnZoomBtn(void)
{
	MessageBox( _T("OnZoomBtn") );
}

void CMainFrame::OnFirstPageBtn(void)
{
	MessageBox( _T("OnFirstPageBtn") );
}

void CMainFrame::OnPreviousPageBtn(void)
{
	MessageBox( _T("OnPreviousPageBtn") );
}

void CMainFrame::OnNextPageBtn(void)
{
	MessageBox( _T("OnNextPageBtn") );
}

void CMainFrame::OnLastPageBtn(void)
{
	MessageBox( _T("OnLastPageBtn") );
}

void CMainFrame::OnSinglePageBtn(void)
{
	MessageBox( _T("OnSinglePageBtn") );
}

void CMainFrame::OnSinglePageContinuousBtn(void)
{
	MessageBox( _T("OnSinglePageContinuousBtn") );
}

void CMainFrame::OnDoublePageBtn(void)
{
	MessageBox( _T("OnDoublePage") );
}

void CMainFrame::OnDoublePageContinuousBtn(void)
{
	MessageBox( _T("OnDoublePageContinuous") );
}
