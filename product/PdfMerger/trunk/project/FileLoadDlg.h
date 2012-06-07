#pragma once

#include "PdfMerger.h"

class CFileLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileLoadDlg)

public:
	CFileLoadDlg(CWnd* pParent = NULL); 
	virtual ~CFileLoadDlg();

	enum { IDD = IDD_LOADING };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()

private:
	UINT						mClientWidth;
	UINT						mClientHeight;
	CDC							mMemdc;
	CBitmap						mBitmap;
	CBitmap *					mpOldBitmap;
	Gdiplus::Graphics *			mGraphics;
	MyIHE_WDM_InterActive *		mpInterActive;
	CHE_WDM_Area *				mpMainArea;
	CHE_WDM_AnimationMgr		mAnimations;
	unsigned int				mTimerId;
	CHE_WDM_AppearTextPtr		mText;

	void						DrawMainArea(void);

public:
	void						ShowText( std::wstring & str ); 

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
};
