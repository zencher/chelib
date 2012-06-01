#pragma once

#include "PdfMerger.h"

// CEditDlg �Ի���

class CEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDlg)

public:
	CEditDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEditDlg();

// �Ի�������
	enum { IDD = IDD_EDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

	CHE_WDM_AppearTextPtr mDropListText;
	CHE_WDM_Area * mpDropList;
	CHE_WDM_Area * mpDropListItems;
	CHE_WDM_Area * mpSinglePageArea;
	CHE_WDM_Area * mpSingleNumberBox;


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
};
