// MyTestOcxPropPage.cpp : Implementation of the CMyTestOcxPropPage property page class.

#include "stdafx.h"
#include "MyTestOcx.h"
#include "MyTestOcxPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMyTestOcxPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CMyTestOcxPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMyTestOcxPropPage, "MYTESTOCX.MyTestOcxPropPage.1",
	0x32fcba77, 0xc9c6, 0x4440, 0x96, 0x1c, 0x8d, 0xa7, 0x7a, 0x33, 0x4a, 0xd3)



// CMyTestOcxPropPage::CMyTestOcxPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CMyTestOcxPropPage

BOOL CMyTestOcxPropPage::CMyTestOcxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MYTESTOCX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CMyTestOcxPropPage::CMyTestOcxPropPage - Constructor

CMyTestOcxPropPage::CMyTestOcxPropPage() :
	COlePropertyPage(IDD, IDS_MYTESTOCX_PPG_CAPTION)
{
}



// CMyTestOcxPropPage::DoDataExchange - Moves data between page and properties

void CMyTestOcxPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CMyTestOcxPropPage message handlers
