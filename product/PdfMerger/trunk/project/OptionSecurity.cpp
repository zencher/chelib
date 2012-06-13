// OptionSecurity.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfMerger.h"
#include "OptionSecurity.h"
#include "afxdialogex.h"


// COptionSecurity 对话框

IMPLEMENT_DYNAMIC(COptionSecurity, CPropertyPage)

COptionSecurity::COptionSecurity()
	: CPropertyPage(COptionSecurity::IDD)
{

}

COptionSecurity::~COptionSecurity()
{
}

void COptionSecurity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionSecurity, CPropertyPage)
END_MESSAGE_MAP()


// COptionSecurity 消息处理程序
