
// PdfMergerDlg.h : header file
//

#pragma once

#include "PdfMerger.h"


// CPdfMergerDlg dialog
class CPdfMergerDlg : public CDialogEx
{
// Construction
public:
	CPdfMergerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PDFMERGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
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
	CHE_WDM_Button * mpAddFileBtn;
	CHE_WDM_Button * mpOptionsBtn;
	CHE_WDM_Button * mpEditBtn;
	CHE_WDM_Button * mpClearBtn;
	CHE_WDM_Button * mpUpBtn;
	CHE_WDM_Button * mpDownBtn;
	CHE_WDM_Area * mpItemList;
	CHE_WDM_Area * mpScrollBar;
	CHE_WDM_Area * mpFilePath;
	CHE_WDM_Button * mpBrowseBtn;
	CHE_WDM_Button * mpStartBtn;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
