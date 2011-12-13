#pragma once
#include "CWNDMD.h"
#include "PdfSplitter.h"

class MyIHE_WD_InterActive;

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
	UINT mClientWidth;
	UINT mClientHeight;

public:
	MyIHE_WD_InterActive * mpInterActive;

	CHE_WD_Area *	mpMainArea;
	CHE_WD_Area *	mpTextBarTitle;
	CHE_WD_Area *	mpTextBar;
	CHE_WD_Area *	mpFilePathText;
	CHE_WD_Area *	mpFileSizeInfo;
	CHE_WD_Area *	mpFilePageCountInfo;
	CHE_WD_Area *	mpListBox;
	CHE_WD_Area *	mpListBoxItems;
	CHE_WD_Area *	mpListScrollBar;
	CHE_WD_Area *	mpProcess;
	CHE_WD_Button * mpBrowseBtn;
	CHE_WD_Button * mpToolBtnAdd;
	CHE_WD_Button * mpToolBtnDel;
	CHE_WD_Button * mpToolBtnUp;
	CHE_WD_Button * mpToolBtnDown;
	CHE_WD_Button * mpStartBtn;

	unsigned int	mTimerId;

public:
	void SetProcessBarValue( unsigned int val );

	void AppendListItem( const CListItem & item );

	void DeleteListItem( unsigned int index );

	void CancelSelection();

	void UpdateToolBtn();

	void UpdataList();

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
	void UpdataSelection(void);
};
