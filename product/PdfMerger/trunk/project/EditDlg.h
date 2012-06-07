#pragma once

#include "PdfMerger.h"

// CEditDlg 对话框

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEditDlg();

// 对话框数据
	enum { IDD = IDD_EDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	void DrawMainArea();

	UINT		mClientWidth;
	UINT		mClientHeight;

	CDC			mMemdc;
	CBitmap		mBitmap;
	CBitmap *	mpOldBitmap;
	Gdiplus::Graphics *	mGraphics;

	MyIHE_WDM_InterActive * mpInterActive;
	CHE_WDM_Area *	mpMainArea;

public:
	CHE_WDM_AnimationMgr mAnimationMgr;

	CListItem *	mpCurItem;

	size_t mNumber1;
	size_t mNumber2;

	CListItemType mItemType;
	size_t mPageCount;
	size_t mPageBegin;
	size_t mPageEnd;
	size_t mCurPage;

	CHE_WDM_Area * mpFileInfo;
	CHE_WDM_Button * mpModeBtn;

	bool mbBG2;
	CHE_WDM_Button * mpBG2;
	CHE_WDM_Area * mpBG3;

	CHE_WDM_Area * mpBtnArea;
	CHE_WDM_Button * mpSMode1;
	CHE_WDM_Button * mpSMode2;
	CHE_WDM_Button * mpSMode3;
	CHE_WDM_Button * mpSMode4;
	CHE_WDM_Button * mpSMode5;
	CHE_WDM_AppearImagePtr mSModeImage1;
	CHE_WDM_AppearImagePtr mSModeImage2;
	CHE_WDM_AppearImagePtr mSModeImage3;
	CHE_WDM_AppearImagePtr mSModeImage4;
	CHE_WDM_AppearImagePtr mSModeImage5;

	CHE_WDM_Area * mpNumberBox1;
	CHE_WDM_Area * mpNumberBox2;

	CHE_WDM_Area * mpSinglePageArea;
	CHE_WDM_Area * mpSinglePageScrollBar;
	CHE_WDM_DragArea * mpSingleDragBox;

	CHE_WDM_Area * mpPageRangeArea;
	CHE_WDM_Area * mpPageRangeScrollBar;
	CHE_WDM_Area * mpPageRangeScrollFill;
	CHE_WDM_DragArea * mpPageRangeBeginDragBox;
	CHE_WDM_DragArea * mpPageRangeEndDragBox;

public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void HideAllWeight();
	void ShowSinglePageWeight();
	void ShowPageRangeWeight();
	void ShowAllPagesWeight();
	void ShowEvenPagesWeight();
	void ShowOddPageWeight();
};
