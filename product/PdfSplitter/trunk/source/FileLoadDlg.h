#pragma once

#include "CWNDMD.h"
#include "PdfSplitter.h"

class CFileLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileLoadDlg)

public:
	CFileLoadDlg(CWnd* pParent = NULL); 
	virtual ~CFileLoadDlg();

	enum { IDD = IDD_LOADFILE };

	UINT mClientWidth;
	UINT mClientHeight;
	unsigned int mTimerId;
	MyIHE_WD_InterActive * mpInterActive;
	CHE_WD_Area *	mpMainArea;
	CHE_WD_Animation * mpAnimation;

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()

private:
	void DrawMainArea(void);

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
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
