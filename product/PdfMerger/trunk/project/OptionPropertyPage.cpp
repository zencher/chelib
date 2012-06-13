// OptionPropertyPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "OptionPropertyPage.h"
#include "afxdialogex.h"


// COptionPropertyPage 对话框

IMPLEMENT_DYNAMIC(COptionPropertyPage, CPropertyPage)

COptionPropertyPage::COptionPropertyPage()
	: CPropertyPage(COptionPropertyPage::IDD)
{

}

COptionPropertyPage::~COptionPropertyPage()
{
}

void COptionPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionPropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// COptionPropertyPage 消息处理程序



BOOL COptionPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CWnd * pWnd = NULL;
	if ( theApp.mFileInfoTitle.size() > 0 )
	{
		pWnd = GetDlgItem( IDC_TITLE );
		if ( pWnd )
		{
			pWnd->SetWindowText( theApp.mFileInfoTitle.c_str() );
		}
	}
	if ( theApp.mFileInfoSubject.size() > 0 )
	{
		pWnd = GetDlgItem( IDC_SUBJECT );
		if ( pWnd )
		{
			pWnd->SetWindowText( theApp.mFileInfoSubject.c_str() );
		}
	}
	if ( theApp.mFileInfoAuthor.size() > 0 )
	{
		pWnd = GetDlgItem( IDC_AUTHOR );
		if ( pWnd )
		{
			pWnd->SetWindowText( theApp.mFileInfoAuthor.c_str() );
		}
	}
	if ( theApp.mFileInfoKeywords.size() )
	{
		pWnd = GetDlgItem( IDC_KEYWORD );
		if ( pWnd )
		{
			pWnd->SetWindowText( theApp.mFileInfoKeywords.c_str() );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void COptionPropertyPage::DoSave(void)
{
	CWnd * pWnd = NULL;
	wchar_t tmpStr[1024];
	

	pWnd = GetDlgItem( IDC_TITLE );
	if ( pWnd )
	{
		memset( tmpStr, sizeof(wchar_t), sizeof(wchar_t) * 1024 );
		pWnd->GetWindowText( tmpStr, 1024 );
		theApp.mFileInfoTitle = tmpStr;
	}

	pWnd = GetDlgItem( IDC_SUBJECT );
	if ( pWnd )
	{
		memset( tmpStr, sizeof(wchar_t), sizeof(wchar_t) * 1024 );
		pWnd->GetWindowText( tmpStr, 1024 );
		theApp.mFileInfoSubject = tmpStr;
	}

	pWnd = GetDlgItem( IDC_AUTHOR );
	if ( pWnd )
	{
		memset( tmpStr, sizeof(wchar_t), sizeof(wchar_t) * 1024 );
		pWnd->GetWindowText( tmpStr, 1024 );
		theApp.mFileInfoAuthor = tmpStr;
	}

	pWnd = GetDlgItem( IDC_KEYWORD );
	if ( pWnd )
	{
		memset( tmpStr, sizeof(wchar_t), sizeof(wchar_t) * 1024 );
		pWnd->GetWindowText( tmpStr, 1024 );
		theApp.mFileInfoKeywords = tmpStr;
	}
}
