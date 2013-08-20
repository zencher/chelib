#pragma once


// COptionSecurity 对话框

class COptionSecurity : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionSecurity)

public:
	COptionSecurity();
	virtual ~COptionSecurity();

// 对话框数据
	enum { IDD = IDD_OPTIONS_SECURITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
