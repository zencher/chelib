#pragma once


// COptionSecurity �Ի���

class COptionSecurity : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionSecurity)

public:
	COptionSecurity();
	virtual ~COptionSecurity();

// �Ի�������
	enum { IDD = IDD_OPTIONS_SECURITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
