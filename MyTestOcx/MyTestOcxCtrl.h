#pragma once

#include <string>
#include <string.h>
#include <vector>
using namespace std;



typedef int (__stdcall *RfOpenAndConnectT)(HANDLE &hCom, 
										   char* cPort, 
										   UCHAR flagCrc);

typedef int (__stdcall *RfCloseAndDisconnectT)(HANDLE &hCom, 
											   UCHAR flagCrc);

typedef int (__stdcall *RfReadDataSingleT)(HANDLE hCom, 
										   UCHAR* uAccessPwd,//密码 
										   UCHAR uBank, //区号 0,1-epc,2,3   Reserved-Mem Bank0  Epc-Mem Bank1 Tid-Mem Bank2 User-Mem Bank3
										   UCHAR* uPtr, //起始位置 按字算 必须从第二个2字后操作
										   UCHAR uCnt, //个数(字长)
										   UCHAR* uReadData, //存储区
										   UCHAR* uUii, 
										   UCHAR* uLenUii, 
										   UCHAR* uErrorCode, 
										   UCHAR flagCrc);

typedef int (__stdcall *RfWriteDataSingleT)(HANDLE hCom, 
											UCHAR* uAccessPwd, 
											UCHAR uBank, 
											UCHAR* uPtr, 
											UCHAR uCnt, 
											UCHAR* uWriteData, 
											UCHAR* uUii, 
											UCHAR* uLenUii, 
											UCHAR* uErrorCode, 
											UCHAR flagCrc);
RfOpenAndConnectT RfOpenAndConnect = NULL;
RfCloseAndDisconnectT RfCloseAndDisconnect = NULL;
RfReadDataSingleT RfReadDataSingle = NULL;
RfWriteDataSingleT RfWriteDataSingle = NULL;
////////////////////////////////////


typedef int (USBKeyOpen_T)(int port, int baudrate);
typedef int (USBKeyClose_T)();
typedef int (USBKeyID_T)(char* id);
typedef int (USBKeyAuth_T)(char* dst, const char* src);


// MyTestOcxCtrl.h : Declaration of the CMyTestOcxCtrl ActiveX Control class.


// CMyTestOcxCtrl : See MyTestOcxCtrl.cpp for implementation.

class CMyTestOcxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMyTestOcxCtrl)

// Constructor
public:
	CMyTestOcxCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CMyTestOcxCtrl();

	DECLARE_OLECREATE_EX(CMyTestOcxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CMyTestOcxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMyTestOcxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CMyTestOcxCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidprinttag2=17L,
		dispidprintNew_1=16L,
		dispidprintWine3 = 15L,		dispidULogin = 14L,		dispidWriteRFID2 = 13L,		dispidReadRFID2 = 12L,		dispidGetNumber = 11L,		dispidDownloadData = 10L,		
		dispidprintNew3 = 9L,
		dispidReadQRNew = 8L,	
		dispidprintNew = 7L,		
		dispidReadQR = 6L,		
		dispidprint2 = 5L,		
		dispidWriteRFID = 4L,		
		dispidReadRFID = 3L,		
		dispidprint = 2L,		
		dispidMyTest = 1L
	};
private:
	const string GetDefaultPrinter() const;
	USBKeyOpen_T* KeyOpen;
	USBKeyClose_T* KeyClose;
	USBKeyID_T* KeyID;
	USBKeyAuth_T* KeyAuth;
	HINSTANCE hInstMaster;
	char usbId[64],usbAuth[64];
	
protected:
	LONG MyTest(LPCTSTR bstrTest);
	LONG printNew(LPCTSTR pp, LPCTSTR printerName, LONG nPrintType);
	LONG printNew_1(LPCTSTR pp, LPCTSTR printerName, LONG nPrintType);
	LONG printNew3(LPCTSTR pp, LPCTSTR printerName, LONG nPrintType);
	LONG print2(LPCTSTR pp);
	BSTR ReadQR(LPCTSTR com, LONG baudRate, LONG overTime);
	BSTR ReadQRNew();
	BSTR DownloadData(LPCTSTR inVar);
	BSTR GetNumber(LPCTSTR inVar);
	BSTR ReadRFID2(LONG overTime);
	BSTR WriteRFID2(LPCTSTR codeStr, LONG mode);
	bool JudgeDefaultType(void);
	bool JudgeOldType(void);
	bool ModifyPass(void);
	string EncryptNoUseBlock();
	BSTR ULogin(void);
	BOOL initUsb();
	string UkeyGetDownData(IN const char* sendCommand, IN const char* ip, IN int port, OUT unsigned char** buffer, OUT size_t& bufferLen) const;
	string SentUsbIdToServer(string& taskIP,int taskPort,char* usbId);
	LONG printWine3(LPCTSTR http, LPCTSTR code, LPCTSTR pp, LONG nPrintType);
	LONG printtag2(LPCTSTR pp, LPCTSTR printerName, LONG nPrintType);
};

