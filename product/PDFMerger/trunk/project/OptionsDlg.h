#pragma once

#include "OptionPropertyPage.h"
#include "OptionSecurity.h"

// COptionsDlg

class COptionsDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsDlg();

protected:
	DECLARE_MESSAGE_MAP()

private:
	COptionPropertyPage mPropertyPage;
	//COptionSecurity mSecurityPage;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnIdok();
};


