// MyTestOcx.cpp : Implementation of CMyTestOcxApp and DLL registration.

#include "stdafx.h"
#include "MyTestOcx.h"
#include "comcat.h"
#include "Objsafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMyTestOcxApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xC83D8C79, 0x1C2A, 0x4ABF, { 0xB4, 0x47, 0x7A, 0x47, 0xDA, 0x6E, 0x10, 0x8F } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
//add by mwj 2011/1/19
const GUID CDECL CLSID_SafeItem ={ 0x0880401B, 0x3338, 0x475F,
{ 0xBB,0x19,0x88,0x97,0xE2,0xB4,0x0A,0x10 } };

// CMyTestOcxApp::InitInstance - DLL initialization

BOOL CMyTestOcxApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CMyTestOcxApp::ExitInstance - DLL termination

int CMyTestOcxApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}

//add by mwj 2011/1/19
// 创建组件种类
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	// Make sure the HKCR\Component Categories\{..catid...}
	// key is registered.
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english

	// Make sure the provided description is not too long.
	// Only copy the first 127 characters if it is.
	int len = wcslen(catDescription);
	if (len>127)
		len = 127;
	wcsncpy(catinfo.szDescription, catDescription, len);
	// Make sure the description is null terminated.
	catinfo.szDescription[len] = '\0';

	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}

// 注册组件种类
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categoDllUnregisterServerries information.
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Register this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}
	if (pcr != NULL)
		pcr->Release();
	return hr;
}

// 卸载组件种类
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	HRESULT hr;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	// 标记控件初始化安全.
	// 创建初始化安全组件种类
	hr = CreateComponentCategory(CATID_SafeForInitializing,
		L"Controls safely initializable from persistent data!");
	if (FAILED(hr))
		return hr;
	// 注册初始化安全
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	// 标记控件脚本安全
	// 创建脚本安全组件种类 
	hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");
	if (FAILED(hr))
		return hr;
	// 注册脚本安全组件种类
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	return NOERROR;
}

//////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hr;

	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	// 删除控件初始化安全入口.
	hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;
	// 删除控件脚本安全入口
	hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	//////////////////////////
	return NOERROR;
} 


// DllRegisterServer - Adds entries to the system registry
/*
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
*/