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

#include "../ReaderDocument/readerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDFReaderView

IMPLEMENT_DYNCREATE(CPDFReaderView, CScrollView)

BEGIN_MESSAGE_MAP(CPDFReaderView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPDFReaderView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// CPDFReaderView construction/destruction


CPDFReaderView::CPDFReaderView()
	: mViewMode(0), mPageStartIndex(0), mPageEndIndex(0), mScale(1.0f), mRotate(0)
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

	return CScrollView::PreCreateWindow(cs);
}

// CPDFReaderView drawing

void CPDFReaderView::OnDraw(CDC* pDC)
{
	CPDFReaderDoc* pDoc = GetDocument();
	if ( pDoc == NULL )
	{
		return;
	}

	int x = 0;

	CPoint point = GetScrollPosition();
	CRect clientRect;
	GetClientRect( &clientRect );
	clientRect.left += point.x;
	clientRect.top += point.y;
	clientRect.bottom += point.y;
	clientRect.right += point.x;

 	//wchar_t str[128];
 	//wsprintfW( str, L"%i-%i", mPageStartIndex, mPageEndIndex );
 	//pDC->TextOut( clientRect.left + 50, clientRect.top + 100, str );

	CHE_Rect dcRect;
	dcRect.left = clientRect.left;
	dcRect.bottom = clientRect.bottom - clientRect.Height();
	dcRect.width = clientRect.Width();
	dcRect.height = clientRect.Height();

	CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();
	CRenderManager * pRenderManager = pDoc->GetRenderManager();
	CRenderCache * pCache = pDoc->GetRenderCache();
	CHE_Rect pageRect;
	for ( HE_ULONG i = mPageStartIndex; i <= mPageEndIndex; ++i )
	{
		pageRect = pReaderDoc->GetPageRect( i );
		pageRect.left = pageRect.left * 96.0f / 72;
		pageRect.bottom = pageRect.bottom * 96.0f / 72;
		pageRect.height = pageRect.height * 96.0f / 72;
		pageRect.width = pageRect.width * 96.0f / 72;

		if ( dcRect.width > pageRect.width )
		{
			x = ( dcRect.width - pageRect.width ) / 2.0f;
		}
		
		if ( pageRect.IsUnion( dcRect ) )
		{
			RECT pageOutLine;
			pageOutLine.left = x-1;
			pageOutLine.top = pageRect.bottom-1;
			pageOutLine.right = x + pageRect.width+1;
			pageOutLine.bottom = pageRect.bottom + pageRect.height+1;
			pDC->Rectangle( &pageOutLine );
			pDC->StrokePath();
			CHE_Bitmap * pBitmap = pCache->GetRenderBitmap( i, mRotate, mScale );
			if ( pBitmap )
			{
				CBitmap bitmap;
				bitmap.CreateBitmap( pBitmap->Width(), pBitmap->Height(), 1, pBitmap->Depth(), pBitmap->GetBuffer() );
				pDC->DrawState( CPoint(x, pageRect.bottom), CSize(pBitmap->Width(), pBitmap->Height()), &bitmap, 0 );
			}
		}
	}
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
	CScrollView::AssertValid();
}

void CPDFReaderView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPDFReaderDoc* CPDFReaderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFReaderDoc)));
	return (CPDFReaderDoc*)m_pDocument;
}
#endif //_DEBUG


// CPDFReaderView message handlers


void CPDFReaderView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CPDFReaderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();

	CHE_Rect rect = pReaderDoc->GetAllPageRect();

	SetScrollSizes( MM_TEXT, CSize( rect.width * 96.0 / 72, rect.height * 96.0 / 72 ), CSize( rect.width * 96.0 / 72, rect.height * 96.0 / ( 72 * pReaderDoc->GetPageCount() ) ), CSize( rect.width * 96.0 / 72, 10 ) );
}


void CPDFReaderView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	CScrollView::OnPrepareDC(pDC, pInfo);
}


BOOL CPDFReaderView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return TRUE;
	return CScrollView::OnEraseBkgnd(pDC);
}


BOOL CPDFReaderView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// TODO: 在此添加专用代码和/或调用基类

	CPDFReaderDoc* pDoc = GetDocument();
	if ( pDoc )
	{
		CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();
		CRenderManager * pRenderManager = pDoc->GetRenderManager();
		CRenderCache * pCache = pDoc->GetRenderCache();
		//获得滚动条即将到达的位置
		CPoint point = GetScrollPosition();
		point.x += sizeScroll.cx;
		point.y += sizeScroll.cy;

		//获得滚动条即将到达的位置对应的视图客户区矩形范围
		CRect clientRect;
		GetClientRect( &clientRect );
		clientRect.left += point.x;
		clientRect.top += point.y;
		clientRect.bottom += point.y;
		clientRect.right += point.x;

		//将视图客户区的矩形范围转成和页面矩形相同的坐标系统
		CHE_Rect dcRect;
		CHE_Rect pageRect;
		dcRect.left = clientRect.left;
		dcRect.bottom = clientRect.bottom - clientRect.Height();
		dcRect.width = clientRect.Width();
		dcRect.height = clientRect.Height();
		
		bool bFoundMatch = false;
		for ( HE_ULONG i = 0; i < pReaderDoc->GetPageCount(); ++i )
		{
			pageRect = pReaderDoc->GetPageRect( i );
			pageRect.left = pageRect.left * 96.0f / 72;
			pageRect.bottom = pageRect.bottom * 96.0f / 72;
			pageRect.height = pageRect.height * 96.0f / 72;
			pageRect.width = pageRect.width * 96.0f / 72;

			CHE_Rect renderSize;
			renderSize.left = 0;
			renderSize.bottom = 0;
			renderSize.width = pageRect.width * 72.0 / 96;
			renderSize.height = pageRect.height * 72.0 / 96;

			if ( pageRect.IsUnion( dcRect ) )
			{
				if ( bFoundMatch == false )
				{
					bFoundMatch = true;
					mPageStartIndex = i;
					mPageEndIndex = i;
				}else{
					mPageEndIndex = i;
				}
				CHE_Bitmap * pBitmap = pCache->GetRenderBitmap( i, mRotate, mScale );
				if ( pBitmap == NULL )
				{
					pRenderManager->NewWork( i, mScale, mRotate, renderSize, pReaderDoc->GetPageConent( i ) );
				}
			}else{
				if ( bFoundMatch == true )
				{
					mPageEndIndex = i-1;
					break;
				}
			}
		}

		pCache->Update( mPageStartIndex, mPageEndIndex, mRotate, mScale );
	}

	
	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}


void CPDFReaderView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 	switch ( nSBCode )
 	{
 	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		{
			SCROLLINFO lpScrollInfo;
			GetScrollInfo( SB_VERT, &lpScrollInfo, SIF_TRACKPOS );
			nPos = lpScrollInfo.nTrackPos;
			break;
		}
	default:
		break;
 	}
	CScrollView::OnVScroll( nSBCode, nPos, pScrollBar);
}
