// PasswordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"


// CPasswordDlg 对话框

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialogEx)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasswordDlg::IDD, pParent)
{
	mbErrorFlag = false;
}

CPasswordDlg::~CPasswordDlg()
{
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CPasswordDlg::OnOK()
{
	CWnd * pWnd = GetDlgItem( IDC_PASSWORD );
	wchar_t tmpWStr[1024];
	memset( tmpWStr, 0, 1024 * sizeof(wchar_t) );
	pWnd->GetWindowText( tmpWStr, 1024 );
	if ( wcslen( tmpWStr ) > 0 )
	{
		char tmpStr[1024];
		memset( tmpStr, 0, 1024 );
		WideCharToMultiByte( CP_ACP, 0, tmpWStr, -1, tmpStr, 1024, NULL, NULL );
		theApp.mPasswrod = tmpStr;
	}else{
		theApp.mPasswrod.clear();
	}
	EndDialog( 0 );
}

void CPasswordDlg::OnCancel()
{
	EndDialog( 1 );
}

BOOL CPasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd * pWnd = GetDlgItem( IDC_INFO );
	if ( !mbErrorFlag )
	{
		pWnd->SetWindowText( L"这是一个受保护的文档，请输入文档打开口令。" );
	}else
	{
		pWnd->SetWindowText( L"文档打开口令错误，请重新输入。" );
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasswordDlg 消息处理程序
