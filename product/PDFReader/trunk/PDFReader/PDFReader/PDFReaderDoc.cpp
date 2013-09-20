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

// PDFReaderDoc.cpp : implementation of the CPDFReaderDoc class
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

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DWORD WINAPI renderMenegerThread( LPVOID pParam/*CPDFReaderDoc * pDoc*/ )
{
	CPDFReaderDoc * pDoc = (CPDFReaderDoc*)( pParam );
	if ( pDoc == NULL  )
	{
		return 1;
	}
	CRenderManager * pMgr = pDoc->GetRenderManager();
	CRenderCache * pCache = pDoc->GetRenderCache();
	//CReaderDocument * pReader = NULL;
	WORKITEM item;
	if ( pMgr == NULL  )
	{
		return 2;
	}
	while ( pMgr )
	{
		WaitForSingleObject( pMgr->GetResultEvent(), INFINITE );
		while ( pMgr->GetResult( item ) )
		{
			//pReader = pDoc->GetReaderDoc();
			//if ( pReader && item.pBitmapRet )
			//{
			//	pReader->SetBitmap( item.param, item.pBitmapRet );
			//}
			if ( item.pBitmapRet )
			{
				pCache->PushItem( item.param, item.rotate % 360, item.scale, item.pageRect, item.pBitmapRet );
			}
			if ( pDoc )
			{
				CPDFReaderView * pView = NULL;
				POSITION posi = pDoc->GetFirstViewPosition();
				pView = (CPDFReaderView *)( pDoc->GetNextView( posi ) );
				if ( item.param >= pView->GetPageStartIndex() && item.param <= pView->GetPageEndIndex() )
				{
					pView->Invalidate(FALSE);
				}
			}
		}
	}
	return 0;
}


// CPDFReaderDoc

IMPLEMENT_DYNCREATE(CPDFReaderDoc, CDocument)

BEGIN_MESSAGE_MAP(CPDFReaderDoc, CDocument)
END_MESSAGE_MAP()


// CPDFReaderDoc construction/destruction

CPDFReaderDoc::CPDFReaderDoc()
	: mpReaderDoc(NULL), mRenderManager(4), mRenderCache(100)
{
	// TODO: add one-time construction code here
	DWORD threadId = 0;
	CreateThread( NULL, 0, renderMenegerThread, this, 0, &threadId );
}

CPDFReaderDoc::~CPDFReaderDoc()
{
	if ( mpReaderDoc )
	{
		mpReaderDoc->GetAllocator()->Delete( mpReaderDoc );
	}
}

BOOL CPDFReaderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CPDFReaderDoc serialization

void CPDFReaderDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CPDFReaderDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CPDFReaderDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CPDFReaderDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPDFReaderDoc diagnostics

#ifdef _DEBUG
void CPDFReaderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPDFReaderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPDFReaderDoc commands


BOOL CPDFReaderDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CHE_Allocator * pAllocator = GetDefaultAllocator();
	if ( pAllocator == NULL )
	{
		return FALSE;
	}

	mpReaderDoc = pAllocator->New<CReaderDocument>( pAllocator );
	if ( mpReaderDoc == NULL )
	{
		return FALSE;
	}

	int		iLen = 0;
	char *	pFilePath = NULL;
	
	iLen = WideCharToMultiByte( CP_ACP, 0, lpszPathName, -1, NULL, 0, NULL, NULL );
	pFilePath = new char[iLen + 1];
	memset( pFilePath, 0, sizeof(char)*(iLen+1) );
	WideCharToMultiByte( CP_ACP, 0, lpszPathName, -1, pFilePath, iLen, NULL, NULL );
	
	mpReaderDoc->OpenFile( pFilePath );

	delete [] pFilePath;

	return TRUE;
}
