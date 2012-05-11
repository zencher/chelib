#pragma once


class CSelectionModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectionModeDlg)

public:
	CSelectionModeDlg(CWnd* pParent = NULL);
	virtual ~CSelectionModeDlg();

	enum { IDD = IDD_SELECTIONMODEDLG };

private:
	UINT mClientWidth;
	UINT mClientHeight;

	CDC			mMemdc;
	CBitmap		mBitmap;
	CBitmap *	mpOldBitmap;
	Gdiplus::Graphics *	mGraphics;

	MyIHE_WD_InterActive * mpInterActive;
	CHE_WD_Area *	mpMainArea;
	CHE_WD_Button * mpBtnSinglePage;
	CHE_WD_Button * mpBtnPageRange;
	CHE_WD_Button * mpBtnOddPages;
	CHE_WD_Button * mpBtnEvenPages;
	CHE_WD_Button * mpBtnCancel;

	void DrawMainArea(void);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual void OnCancel();
};
