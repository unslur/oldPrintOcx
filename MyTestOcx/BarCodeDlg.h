#pragma once


// CBarCodeDlg �Ի���

class CBarCodeDlg : public CDialog
{
	DECLARE_DYNAMIC(CBarCodeDlg)

public:
	CBarCodeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBarCodeDlg();

// �Ի�������
	enum { IDD = IDD_BARCODEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
