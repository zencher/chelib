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

// PDFReaderDoc.h : interface of the CPDFReaderDoc class
//


#pragma once

#include "RenderWork.h"
#include "RenderCache.h"

class CReaderDocument;

class CPDFReaderDoc : public CDocument
{
protected: // create from serialization only
	CPDFReaderDoc();
	DECLARE_DYNCREATE(CPDFReaderDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CPDFReaderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CReaderDocument * GetReaderDoc() { return mpReaderDoc; }
	
	CRenderManager * GetRenderManager() { return &mRenderManager; }

	CRenderCache * GetRenderCache() { return &mRenderCache; }

protected:
	CReaderDocument *	mpReaderDoc;
	CRenderManager		mRenderManager;
	CRenderCache		mRenderCache;	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
