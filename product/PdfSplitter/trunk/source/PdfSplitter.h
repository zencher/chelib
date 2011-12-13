#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include <GdiPlus.h>

#include "../project/resource.h"
#include "CWNDMD.h"
#include "PdfSplitterDlg.h"


#include "../../../../trunk/include/pdf/che_pdf_parser.h"

using namespace Gdiplus;

class MyIHE_WD_InterActive : public IHE_WD_InterActive
{
public:
	MyIHE_WD_InterActive( CDialogEx * pDlg, HINSTANCE hInstance )
	{
		mpDlg = pDlg;
		mhInstance = hInstance;
		mpGraphics = NULL;
	}

	void SetGraphics( Graphics * pGraphics ) { mpGraphics = pGraphics; }

	void SetClip( CHE_WD_Area * pArea );

	void ResetClip();

	void Draw( CHE_WD_Area * pArea, CHE_WD_Appearance * pAppear );

	void Invalidate();

	void SetTimer( unsigned int );

	void KillTimer();

private:
	HINSTANCE mhInstance;
	CDialogEx * mpDlg;
	Graphics * mpGraphics;
};

enum CListItemType
{
	SINGLE_PAGE,
	PAGE_RANGE
};

class CPdfSpliterApp : public CWinApp
{
public:
	CPdfSpliterApp();

public:
	virtual BOOL InitInstance();

public:
	void AddPageListItem( const CListItem & item );

	void DelCurPageListItem();

	bool					mbLoadOver;
	std::wstring			mTargetFile;
	IHE_Read *				mpFileRead;
	CHE_PDF_Parser			mParser;
	float					mfViewPoint;
	std::list< CListItem >	mPageList;
	unsigned int			mItemCount;
	unsigned int			mCurItem;

	CPdfSpliterDlg *		mpMainDlg;

	DECLARE_MESSAGE_MAP()
};

extern CPdfSpliterApp theApp;