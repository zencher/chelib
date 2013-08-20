#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include <GdiPlus.h>
using namespace Gdiplus;

#include "../project/resource.h"
#include "ProcessDlg.h"
#include "PdfSplitterDlg.h"

#include "../../../../trunk/include/wdm/che_wdm.h"
#include "../../../../trunk/include/pdf/che_pdf_file.h"
#include "../../../../trunk/include/pdf/che_pdf_document.h"

class MyIHE_WDM_InterActive : public IHE_WDM_InterActive
{
public:
	MyIHE_WDM_InterActive( CDialogEx * pDlg, HINSTANCE hInstance )
	{
		mpDlg = pDlg;
		mhInstance = hInstance;
		mpGraphics = NULL;
		mLeft = 0;
		mTop = 0;
		mRight = 0;
		mBottom = 0;
		mbRefleshAll = true;
		mbDirtyRect = false;
	}

	~MyIHE_WDM_InterActive();

	void SetGraphics( Graphics * pGraphics ) { mpGraphics = pGraphics; }

	void SetClip( int left, int top, int right, int bottom );

	void SetClip( CHE_WDM_Area * pArea );

	void ResetClip();

	void Draw( CHE_WDM_Area * pArea, CHE_WDM_AppearItemPtr ptr );

	void Draw( CHE_WDM_Area * pArea, WDM_AREA_APPEAR_TYPE type );

	void Invalidate();

	void InvalidateRect( int left, int top, int right, int bottom );

	void SetTimer( CHE_WDM_Area * pArea, HE_DWORD elapse );

	void KillTimer( CHE_WDM_Area * pArea );

	HE_BOOL	MeasureString( CHE_WDM_AppearTextPtr ptr, HE_DWORD & width, HE_DWORD & height );

	HE_BOOL MeasureChars( CHE_WDM_AppearTextPtr ptr, HE_DWORD count, HE_DWORD & width, HE_DWORD & height );

	HE_FLOAT GetFontHeight( CHE_WDM_AppearTextPtr ptr );

	int GetLeft() { return mLeft; }

	int GetTop() { return mTop; }

	int GetRight() { return mRight; }

	int GetBottom() { return mBottom; }

	bool IsReflesh() { return mbRefleshAll; }

	bool IsDirtyRect() { return mbDirtyRect; }

	void Reset() { mbRefleshAll = false; mbDirtyRect = false; }

private:
	HINSTANCE mhInstance;
	CDialogEx * mpDlg;
	Graphics * mpGraphics;
	std::vector<GraphicsContainer> mContainerStack;
	bool mbRefleshAll;
	bool mbDirtyRect;
	int mLeft;
	int mTop;
	int mRight;
	int mBottom;
	std::vector<Image *> mImageCache;
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

	void ClearPageListItem();

	void DelCurPageListItem();

	void UpCurPageListItem();

	void DownCurPagaListItem();

	void LoadDocument();

	void CloseDocument();

	void CreateRegEntry();

	void WriteRegInfo();

	bool CheckRefInfo();

	bool					mbLoadOver;
	bool					mbWork;
	bool					mbLoadError;
	bool					mbRegister;
	std::wstring			mTargetFile;
	std::wstring			mNewFile;
	IHE_Read *				mpFileRead;
	CHE_PDF_File			mFile;
	CHE_PDF_Document *		mpDocument;	
	CHE_PDF_PageTree *		mpPageTree;
	std::string				mPasswrod;
	std::string				mName;
	std::string				mKey;

	float					mfViewPoint;
	std::vector<CListItem>	mPageList;
	unsigned int			mItemCount;
	unsigned int			mCurItem;

	CPdfSpliterDlg *		mpMainDlg;
	CProcessDlg *			mpProcessDlg;

	CHE_WDM_Area *			mpMouseOverItem;


	DECLARE_MESSAGE_MAP()
};

extern CPdfSpliterApp theApp;