#pragma once


// COptionPropertyPage �Ի���

class COptionPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionPropertyPage)

public:
	COptionPropertyPage();
	virtual ~COptionPropertyPage();

// �Ի�������
	enum { IDD = IDD_OPTIONS_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
//	virtual void OnFinalRelease();
	virtual BOOL OnInitDialog();
	void DoSave(void);
};
