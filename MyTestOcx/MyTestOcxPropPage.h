#pragma once

// MyTestOcxPropPage.h : Declaration of the CMyTestOcxPropPage property page class.


// CMyTestOcxPropPage : See MyTestOcxPropPage.cpp for implementation.

class CMyTestOcxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMyTestOcxPropPage)
	DECLARE_OLECREATE_EX(CMyTestOcxPropPage)

// Constructor
public:
	CMyTestOcxPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MYTESTOCX };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

