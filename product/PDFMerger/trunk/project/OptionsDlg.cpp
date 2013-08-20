// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "OptionsDlg.h"


// COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CPropertySheet)

COptionsDlg::COptionsDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage( &mPropertyPage );
	//AddPage( &mSecurityPage );
}

COptionsDlg::COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage( &mPropertyPage );
	//AddPage( &mSecurityPage );
}

COptionsDlg::~COptionsDlg()
{
}


BEGIN_MESSAGE_MAP(COptionsDlg, CPropertySheet)
//	ON_COMMAND(IDYES, &COptionsDlg::OnIdyes)
	ON_COMMAND(IDOK, &COptionsDlg::OnIdok)
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CRect rectWnd;
	CRect rectBtn;

	GetWindowRect( rectWnd );

	CWnd * pWnd = GetDlgItem( ID_APPLY_NOW );
	if ( pWnd )
	{
		pWnd->GetWindowRect ( rectBtn );
		pWnd->ShowWindow( SW_HIDE );

		pWnd = GetDlgItem( IDOK );
		if ( pWnd )
		{
			pWnd->MoveWindow( rectBtn.left-rectWnd.left, rectBtn.top-rectWnd.top-rectBtn.Height(), rectBtn.Width(), rectBtn.Height(), TRUE );
		}
	}

	pWnd = GetDlgItem( IDHELP );
	if ( pWnd )
	{
		pWnd->GetWindowRect ( rectBtn );
		pWnd->ShowWindow( SW_HIDE );

		pWnd = GetDlgItem( IDCANCEL );
		if ( pWnd )
		{
			pWnd->MoveWindow( rectBtn.left-rectWnd.left, rectBtn.top-rectWnd.top-rectBtn.Height(), rectBtn.Width(), rectBtn.Height(), TRUE );
		}
	}

	
	return bResult;
}

void COptionsDlg::OnIdok()
{
	mPropertyPage.DoSave();

	EndDialog( 0 );
}
