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

// MainFrm.h : interface of the CMainFrame class
//


#define IDC_RIBBON_STATUS_ZOOM_SLIDER							0x30000001
#define IDC_RIBBON_STATUS_ZOOM_BTN								0x30000002
#define IDC_RIBBON_STATUS_PAGENAVI_FIRSTPAGE					0x30000003
#define IDC_RIBBON_STATUS_PAGENAVI_PREVIOUSPAGE					0x30000004
#define IDC_RIBBON_STATUS_PAGENAVI_NEXTPAGE						0x30000005
#define IDC_RIBBON_STATUS_PAGENAVI_LASTPAGE						0x30000006
#define IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE				0x30000007
#define IDC_RIBBON_STATUS_PAGEVIEWMODE_SINGLEPAGE_CONTINUOUS	0x30000008
#define IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE				0x30000009
#define IDC_RIBBON_STATUS_PAGEVIEWMODE_DOUBLEPAGE_CONTINUOUS	0x3000000A
#define IDC_RIBBON_STATUS_PAGENAVI_GROUP						0x3000000B
#define IDC_RIBBON_STATUS_PAGEVIEWMODE_GROUP					0x3000000C
#define IDC_RIBBON_STATUS_PAGENAVI_COMBOBOX						0x3000000D

#pragma once
#include "CalendarBar.h"
#include "Resource.h"

class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); if (!bNameValid) strName.Empty(); }
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnZoom();

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	COutlookBar       m_wndNavigationBar;
	CMFCShellTreeCtrl m_wndTree;
	CCalendarBar      m_wndCalendar;
	CMFCCaptionBar    m_wndCaptionBar;

	CMFCRibbonButton*		mpZoomBtn;
	CMFCRibbonSlider*		mpZoomSlider;
	CMFCRibbonButtonsGroup* mpPageNaviGroup;
	CMFCRibbonButtonsGroup* mpPageViewModeGroup;
	CMFCRibbonComboBox*		mpPageNaviComboBox;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnZoomBtn(void);
	afx_msg void OnFirstPageBtn(void);
	afx_msg void OnPreviousPageBtn(void);
	afx_msg void OnNextPageBtn(void);
	afx_msg void OnLastPageBtn(void);
	afx_msg void OnSinglePageBtn(void);
	afx_msg void OnSinglePageContinuousBtn(void);
	afx_msg void OnDoublePageBtn(void);
	afx_msg void OnDoublePageContinuousBtn(void);
	afx_msg void OnMenuScale6400();
	afx_msg void OnMenuScale3200();
	afx_msg void OnMenuScale1600();
	afx_msg void OnMenuScale800();
	afx_msg void OnMenuScale400();
	afx_msg void OnMenuScale200();
	afx_msg void OnMenuScale150();
	afx_msg void OnMenuScale125();
	afx_msg void OnMenuScale100();
	afx_msg void OnMenuScale75();
	afx_msg void OnMenuScale50();
	afx_msg void OnMenuScale25();
	afx_msg void OnMenuScale12();
	afx_msg void OnUpdateToolsHand(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CCalendarBar& calendar, int nInitialWidth);
	BOOL CreateCaptionBar();

	int FindFocusedOutlookWnd(CMFCOutlookBarTabCtrl** ppOutlookWnd);

	CMFCOutlookBarTabCtrl* FindOutlookParent(CWnd* pWnd);
	CMFCOutlookBarTabCtrl* m_pCurrOutlookWnd;
	CMFCOutlookBarPane*    m_pCurrOutlookPage;
public:


	void OnScaleChangeTo( float scale );


};


