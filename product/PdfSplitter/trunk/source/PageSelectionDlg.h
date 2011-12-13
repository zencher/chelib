#pragma once


// CPageSelectionDlg 对话框

class CPageSelectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPageSelectionDlg)

public:
	CPageSelectionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageSelectionDlg();

// 对话框数据
	enum { IDD = IDD_PAGESELECTIONDLG };

	unsigned int mCurPage;
	unsigned int mPageCount;

	UINT mClientWidth;
	UINT mClientHeight;
	MyIHE_WD_InterActive * mpInterActive;
	CHE_WD_Area *	mpMainArea;
	CHE_WD_Area *	mpNumberBox;
	CHE_WD_Area *	mpScrollBar;
	CHE_WD_Button * mpOkBtn;
	CHE_WD_Button * mpCancelBtn;

private:
	void DrawMainArea(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
