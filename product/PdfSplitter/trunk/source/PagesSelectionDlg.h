#pragma once


// CPagesSelectionDlg 对话框

class CPagesSelectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPagesSelectionDlg)

public:
	CPagesSelectionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPagesSelectionDlg();

// 对话框数据
	enum { IDD = IDD_PAGESSELECTIONDLG };

	unsigned int mPageBegin;
	unsigned int mPageEnd;
	unsigned int mPageCount;

	UINT mClientWidth;
	UINT mClientHeight;

	CDC			mMemdc;
	CBitmap		mBitmap;
	CBitmap *	mpOldBitmap;
	Gdiplus::Graphics *	mGraphics;

	MyIHE_WD_InterActive * mpInterActive;
	CHE_WD_Area *	mpMainArea;
	CHE_WD_Area *	mpBeginNumberBox;
	CHE_WD_Area *	mpEndNumberBox;
	CHE_WD_Area *	mpScrollBar;
	CHE_WD_Button * mpOkBtn;
	CHE_WD_Button * mpCancelBtn;

private:
	void DrawMainArea(void);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
