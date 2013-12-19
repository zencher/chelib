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

// PDFReaderView.h : interface of the CPDFReaderView class
//

#pragma once

enum PAGE_VIEW_MODE
{
	VIEW_MODE_SINGLEPAGE,
	VIEW_MODE_SINGLEPAGE_CONTINUOUS,
	VIEW_MODE_DOUBLEPAGES,
	VIEW_MODE_DOUBLEPAGES_CONTINUOUS
};

enum PAGE_VIEW_ROTATE
{
	VIEW_ROTATE_0 = 0,
	VIEW_ROTATE_90 = 90,
	VIEW_ROTATE_180 = 180,
	VIEW_ROTATE_270 = 360
};


class CPDFReaderView : public CScrollView
{
protected: // create from serialization only
	CPDFReaderView();
	DECLARE_DYNCREATE(CPDFReaderView)

// Attributes
public:
	CPDFReaderDoc*		GetDocument() const;

	PAGE_VIEW_MODE		GetPageViewMode() const { return mViewMode; }
	unsigned int		GetPageStartIndex() const { return mPageStartIndex; }
	unsigned int		GetPageEndIndex() const { return mPageEndIndex; }
	PAGE_VIEW_ROTATE	GetPageRotate() const { return mRotate; }
	float				GetPageScale() const { return mScale; }

	void				SetPageViewMode( PAGE_VIEW_MODE mode ) { mViewMode = mode; }
	void				SetPageRotate( PAGE_VIEW_ROTATE rotate ) { mRotate = rotate; }
	void				SetPageScale( float scale ) { mScale = scale; }

	void				UpdataScrollViewSize();

private:
	CHE_Rect			PageRectToViewRect( CHE_Rect & rect );
	bool				GetPageInViewPosition( unsigned int pageindex, CHE_Rect & rect );

	

private:
	PAGE_VIEW_MODE		mViewMode;
	PAGE_VIEW_ROTATE	mRotate;
	unsigned int		mPageStartIndex;
	unsigned int		mPageEndIndex;
	float				mScale;	//8.25% - 6400%, 适合高度，适合宽度，100%

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPDFReaderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

#ifndef _DEBUG  // debug version in PDFReaderView.cpp
inline CPDFReaderDoc* CPDFReaderView::GetDocument() const
   { return reinterpret_cast<CPDFReaderDoc*>(m_pDocument); }
#endif

