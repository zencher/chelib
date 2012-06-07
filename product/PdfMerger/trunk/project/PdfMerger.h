
// PdfMerger.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include <GdiPlus.h>
using namespace Gdiplus;

#include "../project/resource.h"

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

	void Draw( CHE_WDM_Area * pArea, WDM_AREA_APPEAR_TYPE type );

	void Invalidate();

	void InvalidateRect( int left, int top, int right, int bottom );

	void SetTimer( HE_DWORD );

	void KillTimer();

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


// CPdfMergerApp:
// See PdfMerger.cpp for the implementation of this class
//

class CPDFFileInfo
{
public:
	CPDFFileInfo() : mpFileRead( NULL ), mpPDFFile( NULL ), mpDocument( NULL ), mpPageTree( NULL ) {}

	~CPDFFileInfo()
	{
		int x = 0;
	}

	std::wstring		mFileName;
	std::wstring		mFilePath;
	std::string			mPassword;
	IHE_Read *			mpFileRead;
	CHE_PDF_File *		mpPDFFile;
	CHE_PDF_Document *	mpDocument;
	CHE_PDF_PageTree *	mpPageTree;
};

enum CListItemType
{
	ALL_PAGES,
	SINGLE_PAGE,
	PAGE_RANGE,
	EVEN_PAGES,
	ODD_PAGES
};

class CListItem
{
public:
	CListItem() : type( ALL_PAGES ), pageIndex( 0 ), pageCount( 0 ), filePageCount(0), bytes( 0 ), index( 0 ) {}

	CListItemType type;
	unsigned int pageIndex;
	unsigned int pageCount;
	std::wstring fileName;
	size_t filePageCount;
	size_t bytes;
	size_t index;
};

class CPdfMergerDlg;
class CFileLoadDlg;
class CProcessDlg;


class CPdfMergerApp : public CWinApp
{
public:
	CPdfMergerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	std::vector<std::wstring> fileList;

	DECLARE_MESSAGE_MAP()

public:
	void							LoadDocument();
	void							CloseDocument();
	bool							IsExistInFileList( const std::wstring & filePaht, size_t & indexRet );
	void							ClearPageListItem();
	void							DelCurPageListItem();
	void							UpCurPageListItem();
	void							DownCurPagaListItem();
	bool							GetCurItem( CListItem & item );

	CPdfMergerDlg *					mpMainDlg;
	CFileLoadDlg *					mpLoadDlg;
	CProcessDlg *					mpProcessDlg;

	bool							mbLoadOver;
	bool							mbWork;
	bool							mbLoadError;
	bool							mbRegister;

	std::vector<std::wstring>		mFileNameToLoad;
	std::vector<std::wstring>		mFilePathToLoad;

	std::string						mCurPassword;

	std::vector<CPDFFileInfo>		mFileCache;
	std::string						mOutPutFile;

	std::vector<CListItem>			mList;
	size_t							mCurItem;
	CHE_WDM_Area *					mpMouseOverItem;
	float							mfViewPoint;
};

extern CPdfMergerApp theApp;