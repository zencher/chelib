#pragma once

#include "../../../../trunk/include/wdm/che_wdm.h"

class MyIHE_WDM_InterActive;

// CProcessDlg 对话框

class CProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessDlg();

// 对话框数据
	enum { IDD = IDD_PROCESS };

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	MyIHE_WDM_InterActive * mpInterActive;
	unsigned int mProcessBarValue;

public:
	void SetProcessBarValue( unsigned int val );
	void UpdateProcessBar();
	void ShowTips();

	unsigned int	mTimerId;
	CHE_WDM_Area *	mpMainArea;
	CHE_WDM_Area *	mpProcess;
	CHE_WDM_Area *	mpTipArea;
	CHE_WDM_AnimationMgr mAnimationMgr;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
