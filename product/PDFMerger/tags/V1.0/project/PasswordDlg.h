#pragma once


// CPasswordDlg 对话框

class CPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordDlg();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void SetErrorFlag() { mbErrorFlag = true; }

// 对话框数据
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	bool mbErrorFlag;
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};
