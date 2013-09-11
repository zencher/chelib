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




class CPDFReaderView : public CScrollView
{
protected: // create from serialization only
	CPDFReaderView();
	DECLARE_DYNCREATE(CPDFReaderView)

// Attributes
public:
	CPDFReaderDoc*	GetDocument() const;

	unsigned int	GetViewMode() const { return mViewMode; }
	unsigned int	GetPageStartIndex() const { return mPageStartIndex; }
	unsigned int	GetPageEndIndex() const { return mPageEndIndex; }
	unsigned int	GetRotate() const { return mRotate; }
	float			GetScale() const { return mScale; }

	void			SetViewMode( unsigned int mode ) { mViewMode = mode; }
	void			SetRotate( unsigned int rotate ) { mRotate = rotate; }
	void			SetScale( float scale ) { mScale = scale; }

private:
	unsigned int	mViewMode;	//0：单页连续；1: 双页连续；2：单页
	unsigned int	mPageStartIndex;
	unsigned int	mPageEndIndex;
	unsigned int	mRotate;	//0：不旋转；90：顺时针90度；180：顺时针180度；270：顺时针270度
	float			mScale; //8.25% - 6400%, 适合高度，适合宽度，100%

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
};

#ifndef _DEBUG  // debug version in PDFReaderView.cpp
inline CPDFReaderDoc* CPDFReaderView::GetDocument() const
   { return reinterpret_cast<CPDFReaderDoc*>(m_pDocument); }
#endif

