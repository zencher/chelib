#pragma once


// CPasswordDlg �Ի���

class CPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordDlg();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	void SetErrorFlag() { mbErrorFlag = true; }

// �Ի�������
	enum { IDD = IDD_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	bool mbErrorFlag;
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};
