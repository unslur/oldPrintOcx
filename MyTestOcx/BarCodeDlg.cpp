// BarCodeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyTestOcx.h"
#include "BarCodeDlg.h"


// CBarCodeDlg �Ի���

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


// CBarCodeDlg ��Ϣ�������
