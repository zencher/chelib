#pragma once
#include "PdfSplitter.h"

class MyIHE_WDM_InterActive;

class CListItem
{
public:
	enum CListItemType type;
	unsigned int pageIndex;
	unsigned int pageCount;
};

// CPdfSpliterDlg dialog
class CPdfSpliterDlg : public CDialogEx
{
public:
	CPdfSpliterDlg(CWnd* pParent = NULL);
	~CPdfSpliterDlg();

	enum { IDD = IDD_PDFSPLITER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void DrawMainArea();

private:
	UINT		mClientWidth;
	UINT		mClientHeight;

	CDC			mMemdc;
	CBitmap		mBitmap;
	CBitmap *	mpOldBitmap;
	Gdiplus::Graphics *	mGraphics;

public:
	MyIHE_WDM_InterActive * mpInterActive;

	CHE_WDM_Area *	mpMainArea;
	CHE_WDM_Area *	mpTextBarTitle;
	CHE_WDM_Area *	mpTextBar;
	CHE_WDM_Area *	mpFilePathText;
	CHE_WDM_Area *	mpFileSizeInfo;
	CHE_WDM_Area *	mpFilePageCountInfo;
	CHE_WDM_Area *	mpListBoxItems;
	CHE_WDM_Area *	mpListScrollBar;
	CHE_WDM_Area *	mpTextBar2;
	CHE_WDM_Button * mpBrowseBtn;
	CHE_WDM_Button * mpToolBtnAdd;
	CHE_WDM_Button * mpToolBtnClear;
	CHE_WDM_Button * mpToolBtnUp;
	CHE_WDM_Button * mpToolBtnDown;
	CHE_WDM_Button * mpStartBtn;
	CHE_WDM_Button * mpTextBar2Btn;
	

	unsigned int	mTimerId;

public:
	void AppendListItem( const CListItem & item );
	void DeleteListItem( unsigned int index );
	void CancelSelection();
	void UpdateSelection(void);
	void UpdateToolBtn();
	void UpdateList();
	void UpdateTargetFileArea();
	void UpdateFileInfoArea();
	void UpdateNewFileArea();

	void SetWindowsTitleNormal();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnOpenCmd();
	afx_msg void OnCloseCmd();
	afx_msg void OnQuitCmd();
	afx_msg void OnWebCmd();
	afx_msg void OnAboutCmd();
	afx_msg void OnBuy();
	afx_msg void OnUserGuide();
	afx_msg void OnRegister();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};