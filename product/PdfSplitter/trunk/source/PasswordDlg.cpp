// PasswordDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PdfSplitter.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"


// CPasswordDlg �Ի���

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
		pWnd->SetWindowText( L"����һ���ܱ������ĵ����������ĵ��򿪿��" );
	}else
	{
		pWnd->SetWindowText( L"�ĵ��򿪿���������������롣" );
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasswordDlg ��Ϣ��������