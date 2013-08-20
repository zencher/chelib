#pragma once


// COptionPropertyPage 对话框

class COptionPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionPropertyPage)

public:
	COptionPropertyPage();
	virtual ~COptionPropertyPage();

// 对话框数据
	enum { IDD = IDD_OPTIONS_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	virtual void OnFinalRelease();
	virtual BOOL OnInitDialog();
	void DoSave(void);
};
