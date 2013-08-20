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

// PDFReaderView.cpp : implementation of the CPDFReaderView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PDFReader.h"
#endif

#include "PDFReaderDoc.h"
#include "PDFReaderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDFReaderView

IMPLEMENT_DYNCREATE(CPDFReaderView, CView)

BEGIN_MESSAGE_MAP(CPDFReaderView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPDFReaderView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CPDFReaderView construction/destruction

CPDFReaderView::CPDFReaderView()
{
	// TODO: add construction code here

}

CPDFReaderView::~CPDFReaderView()
{
}

BOOL CPDFReaderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPDFReaderView drawing

void CPDFReaderView::OnDraw(CDC* /*pDC*/)
{
	CPDFReaderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CPDFReaderView printing


void CPDFReaderView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPDFReaderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPDFReaderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPDFReaderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPDFReaderView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPDFReaderView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPDFReaderView diagnostics

#ifdef _DEBUG
void CPDFReaderView::AssertValid() const
{
	CView::AssertValid();
}

void CPDFReaderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPDFReaderDoc* CPDFReaderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFReaderDoc)));
	return (CPDFReaderDoc*)m_pDocument;
}
#endif //_DEBUG


// CPDFReaderView message handlers
