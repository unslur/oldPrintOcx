// BarCodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTestOcx.h"
#include "BarCodeDlg.h"


// CBarCodeDlg 对话框

IMPLEMENT_DYNAMIC(CBarCodeDlg, CDialog)
CBarCodeDlg::CBarCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBarCodeDlg::IDD, pParent)
{
}

CBarCodeDlg::~CBarCodeDlg()
{
}

void CBarCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBarCodeDlg, CDialog)
END_MESSAGE_MAP()


// CBarCodeDlg 消息处理程序
