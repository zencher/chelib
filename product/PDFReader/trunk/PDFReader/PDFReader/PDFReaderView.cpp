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
	: mViewMode(VIEW_MODE_SINGLEPAGE_CONTINUOUS), mPageStartIndex(0), 
	mPageEndIndex(0), mScale(1.0f), mRotate(VIEW_ROTATE_0)
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
	
	int x = 10;

	CPoint point = GetScrollPosition();
	CRect clientRect;
	GetClientRect( &clientRect );
	clientRect.left += point.x;
	clientRect.top += point.y;
	clientRect.bottom += point.y;
	clientRect.right += point.x;
	clientRect.left--;
	clientRect.top--;
	clientRect.bottom++;
	clientRect.right++;
	CBrush * pOld = NULL;
	CBrush bgbrush( RGB(220,220,220) );
	pOld = pDC->SelectObject( &bgbrush );
	pDC->Rectangle( clientRect );

	pDC->SelectObject( pOld );

	CHE_Rect dcRect;
	dcRect.left = clientRect.left;
	dcRect.bottom = clientRect.bottom - clientRect.Height();
	dcRect.width = clientRect.Width();
	dcRect.height = clientRect.Height();

	CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();
	CRenderManager * pRenderManager = pDoc->GetRenderManager();
	CRenderCache * pCache = pDoc->GetRenderCache();
	CHE_Rect pageRect;
	HE_ULONG xOffset = 0;
	HE_ULONG yOffset = 0;
	for ( HE_ULONG i = mPageStartIndex; i <= mPageEndIndex; ++i )
	{		
		GetPageInViewPosition( i, pageRect );
		
		if ( dcRect.width > pageRect.width )
		{
			x = ( dcRect.width - pageRect.width ) / 2.0f;
		}
		
		if ( pageRect.IsUnion( dcRect ) )
		{
			RECT pageOutLine;
			pageOutLine.left = x - 1;
			pageOutLine.top = pageRect.bottom-1;
			pageOutLine.right = x + pageRect.width+1;
			pageOutLine.bottom = pageRect.bottom + pageRect.height+1-yOffset;

			CPen pen( 0, 1, RGB( 170, 170, 170 ) );
			pDC->SelectObject( &pen );
			pDC->Rectangle( &pageOutLine );
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen1( 0, 1, RGB( 175, 175, 175 ) );
			pDC->SelectObject( &pen1 );
			pageOutLine.left--;
  			pageOutLine.top--;
  			pageOutLine.right++;
  			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen2( 0, 1, RGB( 180, 180, 180 ) );
			pDC->SelectObject( &pen2 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen3( 0, 1, RGB( 185, 185, 185 ) );
			pDC->SelectObject( &pen3 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

 			CPen pen4( 0, 1, RGB( 190, 190, 190 ) );
 			pDC->SelectObject( &pen4 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen5( 0, 1, RGB( 195, 195, 195 ) );
			pDC->SelectObject( &pen5 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen6( 0, 1, RGB( 200, 200, 200 ) );
			pDC->SelectObject( &pen6 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen7( 0, 1, RGB( 205, 205, 205 ) );
			pDC->SelectObject( &pen7 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen8( 0, 1, RGB( 210, 210, 210 ) );
			pDC->SelectObject( &pen8 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
			pDC->StrokePath();

			CPen pen9( 0, 1, RGB( 215, 215, 215 ) );
			pDC->SelectObject( &pen9 );
			pageOutLine.left--;
			pageOutLine.top--;
			pageOutLine.right++;
			pageOutLine.bottom++;
			pDC->BeginPath();
			pDC->MoveTo( pageOutLine.left, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.top );
			pDC->LineTo( pageOutLine.right, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.bottom );
			pDC->LineTo( pageOutLine.left, pageOutLine.top );
			pDC->EndPath();
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
	if ( pReaderDoc )
	{
		UpdataScrollViewSize();
		CRenderManager * pRenderManager = pDoc->GetRenderManager();
		CHE_Rect pageSize = pReaderDoc->GetPageRect( 0 );
		pRenderManager->NewWork( 0, mScale, mRotate, pageSize, pReaderDoc->GetPageConent( 0 ) );
	}
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
	//return CScrollView::OnEraseBkgnd(pDC);

	return TRUE;
}


BOOL CPDFReaderView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
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
			GetPageInViewPosition( i, pageRect );

			pageRect.left -= 10;
			pageRect.bottom -= 10;
			pageRect.width += 10;
			pageRect.height += 10;

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
					CHE_Rect pageSize = pReaderDoc->GetPageRect( i );
					pageSize.left = 0;
					pageSize.bottom = 0;
					pRenderManager->NewWork( i, mScale, mRotate, pageSize, pReaderDoc->GetPageConent( i ) );
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

CHE_Rect CPDFReaderView::PageRectToViewRect( CHE_Rect & rect )
{
	CHE_Rect rectRet;
	float scale = GetPageScale();
	CHE_Matrix rmatrix;
	CHE_Matrix smatrix = CHE_Matrix::ScaleMatrix( scale * 96.0 / 72.0, scale * 96.0 / 72.0 );
	switch ( GetPageRotate() )
	{
	case VIEW_ROTATE_0:
		rmatrix = CHE_Matrix::RotateMatrix( 0 );
		break;
	case VIEW_ROTATE_90:
		rmatrix = CHE_Matrix::RotateMatrix( 90 );
		break;
	case VIEW_ROTATE_180:
		rmatrix = CHE_Matrix::RotateMatrix( 180 );
		break;
	case VIEW_ROTATE_270:
		rmatrix = CHE_Matrix::RotateMatrix( 270 );
		break;
	default:
		break;
	}
	smatrix.Concat( rmatrix );
	rectRet = smatrix.Transform( rect );
	return rectRet;
}

bool CPDFReaderView::GetPageInViewPosition( unsigned int pageindex, CHE_Rect & rect )
{
	CPDFReaderDoc* pDoc = GetDocument();
	if ( pDoc )
	{
		CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();
		if ( pReaderDoc )
		{
			if ( pageindex >= pReaderDoc->GetPageCount() )
			{
				return false;
			}
			CHE_Rect pageRect = pReaderDoc->GetPageRect( pageindex );
			switch ( GetPageViewMode() )
			{
			case VIEW_MODE_SINGLEPAGE:
				{
					pageRect.left = 0;
					pageRect.bottom = 0;
					rect = PageRectToViewRect( pageRect );
					rect.bottom = 0 + 10 * 2 * pageindex + 10;
					rect.left = 0;
					return true;
				}
			case VIEW_MODE_SINGLEPAGE_CONTINUOUS:
				{
					rect = PageRectToViewRect( pageRect );
					rect.bottom += 10 * 2 * pageindex + 10;
					return true;
				}
			case VIEW_MODE_DOUBLEPAGES:
				break;
			case VIEW_MODE_DOUBLEPAGES_CONTINUOUS:
				break;
			default:
				break;
			}
		}
	}
	return false;
}

void CPDFReaderView::UpdataScrollViewSize()
{
	CPDFReaderDoc* pDoc = GetDocument();
	if ( pDoc )
	{
		CReaderDocument * pReaderDoc = pDoc->GetReaderDoc();
		if ( pReaderDoc )
		{
			CHE_Rect viewRect( 0.0f, 0.0f, 0.0f, 0.0f );
			for ( unsigned int i = 0; i < pReaderDoc->GetPageCount(); ++i )
			{
				CHE_Rect rect = pReaderDoc->GetPageRect( i );
				rect.left = 0;
				rect.bottom = 0;
				rect = PageRectToViewRect( rect );
				rect.width += 20;
				rect.height += 20;

				if ( rect.width > viewRect.width )
				{
					viewRect.width = rect.width;
				}
				viewRect.height += rect.height;
			}
			SetScrollSizes( MM_TEXT, CSize( viewRect.width, viewRect.height ), CSize( viewRect.width, viewRect.height / pReaderDoc->GetPageCount() ), CSize( viewRect.width, 10 ) );
		}
	}
}