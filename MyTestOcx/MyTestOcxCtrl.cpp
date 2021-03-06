// MyTestOcxCtrl.cpp : Implementation of the CMyTestOcxCtrl ActiveX Control class.
/**
2015-8-19 增加判断windows自带驱动Generic
2015-8-27 将数据库路径从C盘改为D盘




**/

#include "stdafx.h"
#include "MyTestOcx.h"
#include "MyTestOcxCtrl.h"
#include "MyTestOcxPropPage.h"
#include ".\mytestocxctrl.h"
#include "BarCodeDlg.h"
#include "..\..\lib\Socket.h"
#include "..\..\lib\png.h"
#include "TscPrinter.h"
#include "PskPrinter.h"
#include "ZbraPrinter.h"
#include "Reader_Cmrf801u.h"
#include "Reader_D3.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//MP300二维码读头定义
//解码结果
typedef struct
{
	int iErrCode;
	int iCodeType;
	int iLenContent;
	unsigned char *pContent;
}MPDECODERET, *PMPDECODERET;

//设置摄像设备参数
typedef struct
{
	HWND hWnd;      //父窗体句柄
	BYTE bImgSize;  //图像大小类型 1:160*120  2:320*240 
	BYTE bImgType;  //图像格式  2:灰度图  3:RGB24
	BYTE bRes1;    
	BYTE bRes2;
	PVOID pFun;
}MPREADERPARAMS, *PMPREADERPARAMS;

typedef int (__stdcall *FP_MPReader_Init)(PMPREADERPARAMS pMPReaderParams);
typedef int (__stdcall *FP_MPReader_UnInit)();
typedef int (__stdcall *FP_MPReader_Begin)();
typedef int (__stdcall *FP_MPReader_End)();
typedef int (__stdcall *FP_MPReader_AssistSense)(bool blOpen);
typedef int (__stdcall *FP_MPReader_GetDeviceSN)(char * pDeviceSN, int iLen);

HWND hWndMain = NULL;
HMODULE hDllMod = NULL;
CBarCodeDlg *pDlg = new CBarCodeDlg();

FP_MPReader_Init MPReader_Init = NULL;
FP_MPReader_UnInit MPReader_UnInit = NULL;
FP_MPReader_Begin MPReader_Begin = NULL;
FP_MPReader_End MPReader_End = NULL;

static int WINAPI MPDecodeRetProc(PMPDECODERET pMPDecoeRet);

bool blInit = false;
bool bIsOK = false;
//bool blAssistSenseOn = false;
CString strResult;
////////////////////////////////////

IMPLEMENT_DYNCREATE(CMyTestOcxCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CMyTestOcxCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CMyTestOcxCtrl, COleControl)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "MyTest", dispidMyTest, MyTest, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "print2", dispidprint2, print2, VT_I4, VTS_BSTR)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "ReadQR", dispidReadQR, ReadQR, VT_BSTR, VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "printNew", dispidprintNew, printNew, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "printNew_1", dispidprintNew_1, printNew_1, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "ReadQRNew", dispidReadQRNew, ReadQRNew, VT_BSTR, NULL  )
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "printNew3", dispidprintNew3, printNew3, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "DownloadData", dispidDownloadData, DownloadData, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "GetNumber", dispidGetNumber, GetNumber, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "ReadRFID2", dispidReadRFID2, ReadRFID2, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "WriteRFID2", dispidWriteRFID2, WriteRFID2, VT_BSTR, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "ULogin", dispidULogin, ULogin, VT_BSTR, VTS_NONE)
	//DISP_FUNCTION_ID(CMyTestOcxCtrl, "printNewWine", dispidprintNewWine, printNewWine, VT_I4, VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	//DISP_FUNCTION_ID(CMyTestOcxCtrl, "printWine3", dispidprintWine3, printWine3, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "printWine3", dispidprintWine3, printWine3, VT_I4, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMyTestOcxCtrl, "printtag2", dispidprinttag2, printtag2, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CMyTestOcxCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMyTestOcxCtrl, 1)
	PROPPAGEID(CMyTestOcxPropPage::guid)
END_PROPPAGEIDS(CMyTestOcxCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMyTestOcxCtrl, "MYTESTOCX.MyTestOcxCtrl.1",
	0x880401b, 0x3338, 0x475f, 0xbb, 0x19, 0x88, 0x97, 0xe2, 0xb4, 0xa, 0x10)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CMyTestOcxCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DMyTestOcx =
		{ 0x9A500F3E, 0xE282, 0x4A61, { 0x87, 0x1B, 0xC4, 0x24, 0x57, 0x86, 0xEA, 0xC8 } };
const IID BASED_CODE IID_DMyTestOcxEvents =
		{ 0xEB82703, 0xA740, 0x40DA, { 0x9C, 0x40, 0x4E, 0xC6, 0x7, 0x73, 0xA6, 0x13 } };



// Control type information

static const DWORD BASED_CODE _dwMyTestOcxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMyTestOcxCtrl, IDS_MYTESTOCX, _dwMyTestOcxOleMisc)


// CMyTestOcxCtrl::CMyTestOcxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CMyTestOcxCtrl

BOOL CMyTestOcxCtrl::CMyTestOcxCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MYTESTOCX,
			IDB_MYTESTOCX,
			afxRegApartmentThreading,
			_dwMyTestOcxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

FILE *fp=NULL;
int num=0;




// CMyTestOcxCtrl::CMyTestOcxCtrl - Constructor
CMyTestOcxCtrl::CMyTestOcxCtrl()
{
	InitializeIIDs(&IID_DMyTestOcx, &IID_DMyTestOcxEvents);
	
	

	// TODO: Initialize your control's instance data here.
}
// CMyTestOcxCtrl::~CMyTestOcxCtrl - Destructor
CMyTestOcxCtrl::~CMyTestOcxCtrl()
{

	// TODO: Cleanup your control's instance data here.
}

// CMyTestOcxCtrl::OnDraw - Drawing function
void CMyTestOcxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CMyTestOcxCtrl::DoPropExchange - Persistence support
void CMyTestOcxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}
// CMyTestOcxCtrl::OnResetState - Reset control to default state
void CMyTestOcxCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}

// CMyTestOcxCtrl::AboutBox - Display an "About" box to the user
void CMyTestOcxCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_MYTESTOCX);
	dlgAbout.DoModal();
}



// CMyTestOcxCtrl message handlers
LONG CMyTestOcxCtrl::MyTest(LPCTSTR bstrTest)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	//AfxMessageBox(bstrTest);
	AfxMessageBox("cy");
	filePath();
	string rtn = FindAndCreatDB(DBPathName); //检测库
	AfxMessageBox(rtn.c_str());
	return 0;
}

string codes;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	//int i;
	char buff[0x50];
	sprintf(buff,"%s,%s|",argv[0],argv[1]);
	codes += buff;
	/*for(int i=0; i<argc; i++){
		codes += argv[i];
		codes += ",";
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	codes += "|";*/
	return 0;
}

string GetNumbers(const char* orgID,const char* taskID,int num,const char* dbPathName){
	char buff[0x200];
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open(dbPathName, &db);

	
	sprintf(buff,"select code,flag from number where orgid='%s' and taskid='%s' limit %d",orgID,taskID,num);
	
	codes = "";
	rc = sqlite3_exec(db, buff, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		sprintf(buff, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return buff;
	}
	
	if ( codes.length() > 0 )
		codes = codes.substr(0,codes.length()-1);
	//AfxMessageBox(codes.c_str());
	return codes;
}


string GetDownData(const char* userID,const char* password,const char* dt,const char* taskID,const char* taskType,const char* ip,int port){
	char buff[0x100];
	sprintf(buff,"%s %s %s %s %s",userID,password,dt,taskID,taskType);
	string sendStr = buff;
	Socket so;
	//so.CInit("192.168.0.215",5555);
	so.CInit(ip,port);
	so.CCreateSocket();
	if(!so.CConnect()){
		so.CCloseSocket();
		return "net error";
	}
	so.Send(sendStr);


	int recvInt = 0,allRecvLen = 0;
	char* recvBuff = (char*)malloc(0x400*0x400*10);
	while(true){
		if ( !so.Recv(recvBuff+allRecvLen,int(0x400),recvInt) ) break;
		allRecvLen += recvInt;
	}
	so.CCloseSocket();
	//AfxMessageBox(recvBuff);

	if (strstr(recvBuff,"error") != NULL){
		return recvBuff;
	}

	LodePNG_DecompressSettings setting;
	setting.ignoreAdler32 = 0;
	/*char stringsss[32];
	itoa(allRecvLen, stringsss, 10);
	AfxMessageBox(stringsss);*/
	unsigned char* zip_buffer = (unsigned char*)malloc(0x400*0x400*10);// 这个是输出缓冲
	size_t zip_length; // 输出长度
	int ret = LodePNG_zlib_decompress(&zip_buffer, &zip_length, (unsigned char*)recvBuff, allRecvLen, &setting);
	free(recvBuff);
	recvBuff = NULL;
	if (ret != 0) {
		char buff[0x50];
		sprintf(buff,"unzip error %d",ret);
		return buff;
	}
	*(zip_buffer+zip_length) = 0;
	char* p = (char*)zip_buffer;

	char* q = strstr((char*)zip_buffer," ");
	*q = 0;
	string orgID = p;
	p = q + 1;

	char sqlBuff[0x200];
	
	filePath();//设置数据库目录文件

	string rtn = FindAndCreatDB(DBPathName); //检测库
	if(rtn != "OK"){
		return rtn;
	}
	//AfxMessageBox("1");
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open(DBPathName, &db);
	if( rc ){
		sqlite3_close(db);
		sprintf(buff,"Can't open database: %s",sqlite3_errmsg(db));
		return buff;
	}
	//AfxMessageBox("2");
	sprintf(sqlBuff,"delete from number where orgid='%s' and taskid='%s'",orgID.c_str(),taskID);
	sqlite3_exec(db, sqlBuff, NULL, 0, &zErrMsg);//插入前清除无效数据
	rc = sqlite3_exec(db, "BEGIN;", NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		sprintf(buff, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return buff;
	}	
//<<<<<<< .mine
//	fp=fopen("d://zyczs/num.txt","a+");
//	num=0;
//	//AfxMessageBox(p);
//||||||| .r813
//
//=======
//	//AfxMessageBox(p);
//>>>>>>> .r894
	while(true){
		q = strstr(p,"|");
		if( NULL == q ) break;
		*q = 0;
		string record = p;
		p = q+1;
		num++;
	//	AfxMessageBox("4");
		string code = record.substr(0,record.find(","));
		record = record.substr(record.find(",")+1);
		string flag = record.substr(record.find(",")+1);
		sprintf(sqlBuff,"insert into number(orgid,taskid,flag,code) values('%s','%s',%s,'%s')",orgID.c_str(),taskID,flag.c_str(),code.c_str());
		rc = sqlite3_exec(db, sqlBuff, NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
		sprintf(buff, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return buff;
		}
	}
	//AfxMessageBox("3");
	free(zip_buffer);
	zip_buffer = NULL;
	/*char cnum[10]={0};
	itoa(num,cnum,10);
	sprintf(cnum,"%s\n",cnum);
	fwrite(cnum,sizeof(cnum),1,fp);*/
	//AfxMessageBox("5");
	sqlite3_exec(db, "COMMIT;", NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		sprintf(buff, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return buff;
	}
	//fclose(fp);
	sqlite3_close(db);
	//AfxMessageBox("5");
	return "OK";
}

BSTR CMyTestOcxCtrl::ReadRFID2(LONG overTime)//function.dll    USB.dll
{
	CString strResult;
	string retstring="";
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReader_Cmrf801u reader;
	if (reader.QuestReader801InUse())
	{
		retstring = reader.ReadRFID_CMRF801U(overTime);
		
	}
	else
	{
		CReader_D3 reader;
		if (reader.QuestReaderD3InUse())
		{
			retstring = reader.ReadRFID_D3(overTime);
			//AfxMessageBox("d3");
		}
	}
	#if 0
	if (retstring == "")
	{
		retstring = "检查是否放入IC卡！";
	}
	#endif
    strResult = (char *)retstring.c_str();
	return strResult.AllocSysString();
}

BSTR CMyTestOcxCtrl::WriteRFID2(LPCTSTR codeStr, LONG mode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//判断是否为黑色读卡器
    CString strResult;
	string retstring="";
	CReader_Cmrf801u reader;
	if (reader.QuestReader801InUse())
	{
		retstring = reader.WriteRFID_CMRF801U(codeStr,mode);
	}
	else
	{
		CReader_D3 reader;
		if (reader.QuestReaderD3InUse())
		{
			retstring = reader.WriteRFID_D3(codeStr,mode);
			//AfxMessageBox("d3");
		}
	}
	#if 0
if (retstring == "")
	{
		retstring = "检查是否放入IC卡！";
	}
#endif
	strResult = (char *)retstring.c_str();
    return strResult.AllocSysString();
}

LONG CMyTestOcxCtrl::printWine3(LPCTSTR http, LPCTSTR code, LPCTSTR pp, LONG nPrintType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// TODO: Add your dispatch handler code here
	//string printStr = (const char*)pp;
	bool bIsPrint = false;
	int id_name = 0,rtn = 0;
	string printStr="";

	srand(time(NULL));
	char printerName[0x64] = {0};
	strcpy(printerName,GetDefaultPrinter().c_str());
	int codenum = nPrintType; //nPrintType是打码个数
	char buff[32]={0};
	for (int i=1;i<=codenum;i++)
	{
		//产生随机数
		int num = rand()%9999;
		sprintf(buff,"%04d",num);
		//
		printStr+= http;
		printStr+="\n";
		printStr+=code;
		printStr+=buff;
		printStr+="\n";
		printStr+=pp;

		if (i != codenum && i%3 != 0)
		{
			printStr += "|";
			continue;
		}
		
		if (strstr(printerName,"Printer LP") != NULL)
		{
			CTscPrinter printer;
			printer.TscInit(printerName);
			printer.wineFlag = true;
			rtn = printer.PrintTsc3(printerName,printStr,4);
			printer.TscUnInit();
		}
		else if (strstr(printerName,"POSTEK") != NULL)
		{
			CPskPrinter printer;
			printer.PskInit(printerName);
			rtn = printer.PrintPsk3(printerName,printStr,4);
			printer.PskUnInit();
		}
		else if (strstr(printerName,"ZDesigner") != NULL || strstr(printerName,"Generic") != NULL)
		{
			CZbraPrinter printer;
			rtn = printer.PrintZbra3(printerName,printStr,4);
		}
		else
		{
			AfxMessageBox("请检查是否正确设置默认打印机！");
		}
        printStr="";       
	}

	return rtn;
}

const string CMyTestOcxCtrl::GetDefaultPrinter() const{
	CPrintDialog printDlg(FALSE); 
	printDlg.GetDefaults();
	return printDlg.GetDeviceName().GetBuffer(0);
}
int count=0;
//nPrintType 1表示打印左边，2表示打印中间，3表示打印右边，4表示一行打印3个
LONG CMyTestOcxCtrl::printNew3(LPCTSTR pp, LPCTSTR printerNamet, LONG nPrintType){
	string printStr = (const char*)pp;
	string printStr1 = "", printStr2 = "", printStr3 = "";
	char buff[0x100]={0};
	bool bIsPrint = false;
	int id_name = 0,rtn = 0;

	char printerName[0x64] = {0};
	if ( strlen(printerNamet) == 0 )
		strcpy(printerName,GetDefaultPrinter().c_str());
	else
		strcpy(printerName,printerNamet);
	if (strstr(printerName,"Printer LP") != NULL
		|| strstr(printerName,"Printer B") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
		rtn = printer.PrintTsc3(printerName,printStr,nPrintType);
		printer.TscUnInit();
	}
	else if (strstr(printerName,"POSTEK") != NULL)
	{
		CPskPrinter printer;
		printer.PskInit(printerName);
		rtn = printer.PrintPsk3(printerName,printStr,nPrintType);
		printer.PskUnInit();
	}
	else if (strstr(printerName,"ZDesigner") != NULL || strstr(printerName,"Generic") != NULL)
	{
		CZbraPrinter printer;
		
		rtn = printer.PrintZbra3(printerName,printStr,nPrintType);
	}
	else
	{
		AfxMessageBox("请检查是否正确设置默认打印机！");
	}
	count++;
	return rtn;
}//*/

//nPrintType 1表示打印左边，2表示打印右边，3表示一行打印2个
LONG CMyTestOcxCtrl::printNew_1(LPCTSTR pp, LPCTSTR printerNamet, LONG nPrintType){
	
	string printStr = (const char*)pp;
	//string printStr1, printStr2;
	
	bool bIsPrint = false;
	int id_name = 0,rtn = 0;
	
	
    char printerName[0x64] = {0};
	if ( strlen(printerNamet) == 0 )
		strcpy(printerName,GetDefaultPrinter().c_str());
	else
		strcpy(printerName,printerNamet);
	//波斯得  POSTEK G2000
	//tsc  Bar Code Printer LP-2000A
	//斑马  ZDesigner ZT210-200dpi ZPL
	if (strstr(printerName,"Printer LP") != NULL
		|| strstr(printerName,"Printer B") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
        rtn = printer.PrintTsc2(printerName,printStr,nPrintType);
		printer.TscUnInit();
	}
	else if (strstr(printerName,"POSTEK") != NULL)
	{
		CPskPrinter printer;
		printer.PskInit(printerName);
		rtn = printer.PrintPsk2(printerName,printStr,nPrintType);
		printer.PskUnInit();
	}
	else if (strstr(printerName,"ZDesigner") != NULL || strstr(printerName,"Microsoft") != NULL)
	{
		CZbraPrinter printer;
		rtn = printer.PrintZbra2(printerName,printStr,nPrintType);
	}
	else
	{
		AfxMessageBox("请检查是否正确设置默认打印机！");
	}

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return rtn;
}
LONG CMyTestOcxCtrl::printNew(LPCTSTR pp, LPCTSTR printerNamet, LONG nPrintType){
	string printStr = (const char*)pp;
	//string printStr1, printStr2;
	
	bool bIsPrint = false;
	int id_name = 0,rtn = 0;
	
	
    char printerName[0x64] = {0};
	if ( strlen(printerNamet) == 0 )
		strcpy(printerName,GetDefaultPrinter().c_str());
	else
		strcpy(printerName,printerNamet);
	//波斯得  POSTEK G2000
	//tsc  Bar Code Printer LP-2000A
	//斑马  ZDesigner ZT210-200dpi ZPL
		
	if (strstr(printerName,"Printer LP") != NULL
		|| strstr(printerName,"Printer B") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
        rtn = printer.PrintTsc2_1(printerName,printStr,nPrintType);
		printer.TscUnInit();
	}
	else if (strstr(printerName,"POSTEK") != NULL)
	{
		CPskPrinter printer;
		printer.PskInit(printerName);
		rtn = printer.PrintPsk2_1(printerName,printStr,nPrintType);
		printer.PskUnInit();
	}
	else if (strstr(printerName,"ZDesigner") != NULL || strstr(printerName,"Generic") != NULL)
	{
		CZbraPrinter printer;
		rtn = printer.PrintZbra2_1(printerName,printStr,nPrintType);
	}
	else
	{
		AfxMessageBox("请检查是否正确设置默认打印机！");
	}

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return rtn;

}
////////////////////////////////////////////////////////////////////////////////////////


/*
#define PRINTER_STATUS_PAUSED             0x00000001    暂停
#define PRINTER_STATUS_ERROR              0x00000002    错误
#define PRINTER_STATUS_PENDING_DELETION   0x00000004    正在删除
#define PRINTER_STATUS_PAPER_JAM          0x00000008    卡纸
#define PRINTER_STATUS_PAPER_OUT          0x00000010    缺纸
#define PRINTER_STATUS_MANUAL_FEED        0x00000020    人工进纸
#define PRINTER_STATUS_PAPER_PROBLEM      0x00000040    纸张问题
#define PRINTER_STATUS_OFFLINE            0x00000080    掉线
#define PRINTER_STATUS_IO_ACTIVE          0x00000100    端口已打开
#define PRINTER_STATUS_BUSY               0x00000200    忙碌
#define PRINTER_STATUS_PRINTING           0x00000400    打印中
#define PRINTER_STATUS_OUTPUT_BIN_FULL    0x00000800    输出满
#define PRINTER_STATUS_NOT_AVAILABLE      0x00001000    无可用
#define PRINTER_STATUS_WAITING            0x00002000    等待
#define PRINTER_STATUS_PROCESSING         0x00004000    处理中
#define PRINTER_STATUS_INITIALIZING       0x00008000    初始化中
#define PRINTER_STATUS_WARMING_UP         0x00010000    正在预热
#define PRINTER_STATUS_TONER_LOW          0x00020000    调色剂少
#define PRINTER_STATUS_NO_TONER           0x00040000    调色剂无
#define PRINTER_STATUS_PAGE_PUNT          0x00080000    页面悬空
#define PRINTER_STATUS_USER_INTERVENTION  0x00100000    用户干预
#define PRINTER_STATUS_OUT_OF_MEMORY      0x00200000    超出内存
#define PRINTER_STATUS_DOOR_OPEN          0x00400000    盖子打开
#define PRINTER_STATUS_SERVER_UNKNOWN     0x00800000    未知服务
#define PRINTER_STATUS_POWER_SAVE         0x01000000    节电

#define JOB_STATUS_PAUSED               0x00000001    //暂停
#define JOB_STATUS_ERROR                0x00000002    //错误
#define JOB_STATUS_DELETING             0x00000004    //正在删除
#define JOB_STATUS_SPOOLING             0x00000008    //正在缓冲
#define JOB_STATUS_PRINTING             0x00000010    //正在打印
#define JOB_STATUS_OFFLINE              0x00000020    //离线
#define JOB_STATUS_PAPEROUT             0x00000040    //缺纸
#define JOB_STATUS_PRINTED              0x00000080    //打印完成
#define JOB_STATUS_DELETED              0x00000100    //已经删除
#define JOB_STATUS_BLOCKED_DEVQ         0x00000200
#define JOB_STATUS_USER_INTERVENTION    0x00000400    //用户干预
#define JOB_STATUS_RESTART              0x00000800    //重启
//*/

LONG CMyTestOcxCtrl::print2(LPCTSTR pp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const int DYJC = 34;
	int resultI = 1;

	// 增加如下代码
	char szprinter[80];
	char *szDevice,*szDriver,*szOutput;
	HDC hdcprint; // 定义一个设备环境句柄
	//定义一个打印作业
	static DOCINFO di={sizeof(DOCINFO),"printer",NULL}; 
	// 得到设备字符串存入数组szprinter中
	GetProfileString("windows","device",",,,",szprinter,80); 
	// 将设备字符串分解
	if(NULL!=(szDevice=strtok(szprinter,","))&&		
		NULL!=(szDriver=strtok(NULL,","))&&		
		NULL!=(szOutput=strtok(NULL,",")))		
		// 创建一个打印机设备句柄 

		if((hdcprint=CreateDC(szDriver,szDevice,szOutput,NULL))!=0){	
			if(StartDoc(hdcprint,&di)>0){ //开始执行一个打印作业
				StartPage(hdcprint); //打印机走纸,开始打印				
				SaveDC(hdcprint); //保存打印机设备句柄				
				// 输出一行文字
				char buff[DYJC*2+1];
				string printStr = pp; //"上面是在MSDN中的TextOut的\n定义,第一个参数是文字输出的开始点的横坐标,第二参数是纵坐标,第三个参数是输\n出文字的内容,第四个参数是要输出几个字";
				list<string> printv = splitPrintv(splitPrintStr(printStr),DYJC);
				int l = 0;
				for ( list<string>::const_iterator iter = printv.begin();iter != printv.end(); ++iter ){
					memset(buff,0,sizeof(buff));
					strcpy(buff,iter->c_str());
					TextOut(hdcprint,1,l*41+1,buff,DYJC);
					l++;
				}

				RestoreDC(hdcprint,-1); //恢复打印机设备句柄
				EndPage(hdcprint); //打印机停纸,停止打印
				EndDoc(hdcprint); //结束一个打印作业
				//MessageBox("打印完毕!","提示",MB_ICONINFORMATION);				
			}
			// 用API函数DeleteDC销毁一个打印机设备句柄 			
			DeleteDC(hdcprint);			
		}		
		else{
			MessageBox("没有默认打印机,或者没有安装打印机!");
			resultI = 0;			
		}
	return resultI;
}

bool isTrueCode(const char* str){
	char* p = (char*)str;
	while(*p){
		if( !isalnum(*p) ) return false;
		p++;
	}
	return true;
}

BSTR CMyTestOcxCtrl::ReadQR(LPCTSTR com, LONG baudRate, LONG overTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;

	HANDLE hCom;  //全局变量，串口句柄
	hCom=CreateFile(com,//COM7口
		GENERIC_READ|GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if(hCom==(HANDLE)-1)
	{
		AfxMessageBox("打开COM失败!");
		strResult = "";
		return strResult.AllocSysString();
	}

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;
	//在读一次输入缓冲区的内容后读操作就立即返回，
	//而不管是否读入了要求的字符。
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom,&TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom,&dcb);
	dcb.BaudRate=baudRate; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	dcb.StopBits=TWOSTOPBITS; //两个停止位
	SetCommState(hCom,&dcb);

	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	char buffer[2];
	buffer[0] = 0x1b;
	buffer[1] = 0x32;
	unsigned long lpNumberOfBytesWritten = 0;
	WriteFile(hCom,buffer,2,&lpNumberOfBytesWritten,NULL);



	char str[100];
	memset(str,0,sizeof(str));
	DWORD wCount;//读取的字节数
	BOOL bReadStat;
	int timeF = 0;
	while(true){
		bReadStat=ReadFile(hCom,str,100,&wCount,NULL);
		if(!bReadStat){
			AfxMessageBox("读串口失败!");
			timeF *= 10;
		}
		if(strlen(str)>0){
			if( isTrueCode(str)){
				break;
			}else{
				memset(str,0,sizeof(str));
			}
		}
		if( timeF++ > overTime*10){ //超时
			break;
		}
		Sleep(100);
	}



	buffer[2];
	buffer[0] = 0x1b;
	buffer[1] = 0x30;
	lpNumberOfBytesWritten = 0;
	WriteFile(hCom,buffer,2,&lpNumberOfBytesWritten,NULL);


	if(!CloseHandle(hCom)){
		AfxMessageBox("关闭COM失败!");
	}
	strResult = str;
	//MessageBox(str);
	return strResult.AllocSysString();
}


BSTR CMyTestOcxCtrl::ReadQRNew()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nCount = 0;
	bIsOK = false;
	blInit = false;

	hDllMod = LoadLibrary("MoponReader.dll");
	if(hDllMod)
	{
		MPReader_Init = (FP_MPReader_Init)GetProcAddress(hDllMod, "MPReader_Init");
		MPReader_UnInit = (FP_MPReader_UnInit)GetProcAddress(hDllMod, "MPReader_UnInit");
		MPReader_Begin = (FP_MPReader_Begin)GetProcAddress(hDllMod, "MPReader_Begin");
		MPReader_End = (FP_MPReader_End)GetProcAddress(hDllMod, "MPReader_End");
		//MPReader_AssistSense = (FP_MPReader_AssistSense)GetProcAddress(hDllMod, "MPReader_AssistSense");
		//MPReader_GetDeviceSN = (FP_MPReader_GetDeviceSN)GetProcAddress(hDllMod, "MPReader_GetDeviceSN");
	}
	else AfxMessageBox("驱动不完整!");

	//pDlg->Create(IDD_BARCODEDLG);
	//pDlg->ShowWindow(SW_SHOW);


	//初始化
	if(MPReader_Init!=NULL) 
	{

		MPREADERPARAMS rMPReaderParams;
		memset(&rMPReaderParams, 0, sizeof(MPREADERPARAMS));
		rMPReaderParams.hWnd = this->m_hWnd;		
		rMPReaderParams.bImgSize = 3;

		//iSel = ::SendDlgItemMessage(m_hWnd, IDC_COMBO2, CB_GETCURSEL, 0, 0);
		//if(iSel<=0)  rMPReaderParams.bImgType = 2;
		//else rMPReaderParams.bImgType = 3;
		rMPReaderParams.bImgType = 3;
		rMPReaderParams.pFun = &MPDecodeRetProc;
		int iRet = MPReader_Init(&rMPReaderParams);
		if (iRet == 0) 
		{
			AfxMessageBox("初始化设备成功！");
			blInit = true;
		}
		else {
			strResult = "设备初始化失败，请检查后重试 ！";
			return strResult.AllocSysString();
		}
	}	

	//开始读取
	if((MPReader_Begin!=NULL) && blInit)
	{		
		int iRet = MPReader_Begin();
		//if (iRet == 0 ) AfxMessageBox("已开流!");
	}

	while(true){
		
		nCount++;
		
		if (bIsOK)
			break;	//读取成功返回

		if (nCount > 150)
			break;

		Sleep(100);
	}

	//结束读取
	if((MPReader_End!=NULL) && blInit)
	{
		MPReader_End();
			//AfxMessageBox("设备已关闭!");
	}	

	//释放资源
	if((MPReader_UnInit!=NULL) && blInit)
	{
		MPReader_UnInit() ;
			//AfxMessageBox("设备资源已释放!");

		blInit = false;
	}
	
	//pDlg->CloseWindow();
	//pDlg->DestroyWindow();
	//pDlg = NULL;

	if (bIsOK) {
		return strResult.AllocSysString();
	}
	else {

		strResult = "规定时间内识读失败，请检查后重试！";
		return strResult.AllocSysString();
	}
}

int WINAPI MPDecodeRetProc(PMPDECODERET pMPDecoeRet)
{
	AfxMessageBox("MPDecodeRetProc");

	char szShow[100] = {0};
	if(pMPDecoeRet->iErrCode!=0)
	{
		sprintf(szShow, "解码失败, 错误代码为: %d", pMPDecoeRet->iErrCode);
		bIsOK = false;
		
		return -1;
	}
	else 
	{
		if(pMPDecoeRet->iCodeType==100) sprintf(szShow, "解码成功  GM码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==200) sprintf(szShow, "解码成功  QR码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==201) sprintf(szShow, "解码成功  DM码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==202) sprintf(szShow, "解码成功  EZ码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==300) sprintf(szShow, "解码成功  UPC-A码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==301) sprintf(szShow, "解码成功  UPC-E码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==302) sprintf(szShow, "解码成功  EAN-13码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==303) sprintf(szShow, "解码成功  EAN-8码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==304) sprintf(szShow, "解码成功  JAN-13码  码图内容如下\r\n");
		else if(pMPDecoeRet->iCodeType==305) sprintf(szShow, "解码成功  JAN-8码  码图内容如下\r\n");
		
		if(pMPDecoeRet->iLenContent<=2062)
		{
			strResult = (LPCTSTR)pMPDecoeRet->pContent;
			AfxMessageBox(strResult);
			bIsOK = true;
		}
	}	
	
	return 0;
}
BSTR CMyTestOcxCtrl::DownloadData(LPCTSTR inVar)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
//<<<<<<< .mine
//	/*AfxMessageBox(inVar);
//	return 0;*/
//||||||| .r813
//=======
//	//AfxMessageBox(inVar);
//>>>>>>> .r894
	string InVar = inVar;
	
	string userID,password,dt,taskID,taskType,ip,port;
	userID = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);

	password = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);

	dt = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);

	taskID = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);

	taskType = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);

	ip = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);
	port = InVar;
	
	string rtnStr = GetDownData(userID.c_str(),password.c_str(),dt.c_str(),taskID.c_str(),taskType.c_str(),ip.c_str(),atoi(port.c_str()));
	strResult = rtnStr.c_str();
	return strResult.AllocSysString();
}

BSTR CMyTestOcxCtrl::GetNumber(LPCTSTR inVar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	//AfxMessageBox(inVar);
	string InVar = inVar;
	string userID,taskID;
	int num;
	userID = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);
	taskID = InVar.substr(0,InVar.find(" "));
	InVar = InVar.substr(InVar.find(" ")+1);
	num = atoi(InVar.c_str());
	filePath();
	string rtnData = GetNumbers(userID.c_str(),taskID.c_str(),num,DBPathName);
	strResult = rtnData.c_str(); 
	return strResult.AllocSysString();
}

BOOL CMyTestOcxCtrl::initUsb()
{
	//读取加密锁ID  如果失败 就弹提示框 并退出去  
	hInstMaster = LoadLibrary("USBKeyDLL.dll");
	if(!hInstMaster){	
		MessageBox("加载USBKeyDLL.dll失败！","提示：",MB_SYSTEMMODAL); 
		return FALSE;
	}
	KeyOpen = (USBKeyOpen_T*)GetProcAddress(hInstMaster, "USBKeyOpen");
	KeyClose = (USBKeyClose_T*)GetProcAddress(hInstMaster, "USBKeyClose");
	KeyID = (USBKeyID_T*)GetProcAddress(hInstMaster, "USBKeyID");
	KeyAuth = (USBKeyAuth_T*)GetProcAddress(hInstMaster, "USBKeyAuth");
	if (NULL == KeyOpen || NULL == KeyClose || NULL == KeyID || NULL == KeyAuth)
	{
		FreeLibrary(hInstMaster);
		hInstMaster = NULL;
		return FALSE;
	}
	bool openFlag = false;
	for(int i=1;i<255;i++)
	{
		if (KeyOpen(i,115200)) {
			memset(usbId,0,sizeof(usbId));
			if (KeyID(usbId)){
				openFlag = true;
				break;
			}else{
				KeyClose();
			}
		}
	}
	if (!openFlag)
	{
		KeyClose();
		FreeLibrary(hInstMaster);
		hInstMaster = NULL;
	}

	return openFlag;
}

//从服务器端下载数据
string CMyTestOcxCtrl::UkeyGetDownData(IN const char* sendCommand, IN const char* ip, IN int port, OUT unsigned char** buffer, OUT size_t& bufferLen) const{
	Socket so;
	so.CInit(ip,port);
	so.CCreateSocket();
	try{
		if(!so.CConnect()) throw exception("网络不通");
		if(!so.Send(sendCommand)) throw exception("请求失败");

		char recvBuff[0x400*0x200];
		int recvInt = 0,allRecvLen = 0;
		while(true){
			if ( !so.Recv(recvBuff+allRecvLen,0x400,recvInt) ) break;
			allRecvLen += recvInt;
		}
		recvBuff[allRecvLen]=0x00;
		if (allRecvLen <= 36){
			if ( strcmp(recvBuff,"1") == 0){
				throw exception("已经下载");
			}else if (strcmp(recvBuff,"0") == 0){
				throw exception("任务非法");
			}else if (36 == allRecvLen){
				memcpy(*buffer,recvBuff,allRecvLen);
				bufferLen = allRecvLen;
				throw exception("OK");
			}else if (9 == allRecvLen){
				throw exception("请先检查设备列表是否更新或者加密锁是否配置正确,如有疑问,请联系服务商!");
			}
			else if (0 == allRecvLen){
				throw exception("程序调整中，请联系服务商！");
			}
			else if (strcmp("password ok",recvBuff) == 0)
			{
				throw exception("OK");
			}
			else if (strcmp("password error",recvBuff) == 0)
			{
				throw exception("Ukey验证失败！");
			}else throw exception("下载失败");
		}
		LodePNG_DecompressSettings setting;
		setting.ignoreAdler32 = 0;
		int ret = LodePNG_zlib_decompress(buffer, &bufferLen, (unsigned char*)recvBuff, allRecvLen, &setting);
		if( ret != 0 ) throw exception("解压失败");
		*(*buffer+bufferLen) = 0;

		so.CShutdownSocket();
		so.CCloseSocket();
	}catch(const exception& e){
		so.CShutdownSocket();
		so.CCloseSocket();
		return e.what();
	}
	return "OK";
}

string CMyTestOcxCtrl::SentUsbIdToServer(string& taskIP,int taskPort,char* usbId)
{
	string retString = "";
	size_t bufferLen=0;
	unsigned char* buffer = (unsigned char*)malloc(256);//解压后的空间
	if( NULL == buffer ){
		return "";
	}  
	memset(buffer,0,256);
	//这个接口和李哥讨论  是否要和打码机的混合在一起  如果混合的话 需要每个U盾都设置打码设备

	//test
	sprintf((char *)buffer,"GETRANDOM %s",usbId);//新接口   之前是发的设备总数 现在发的1
	retString = UkeyGetDownData((char *)buffer,taskIP.c_str(),taskPort,&buffer,bufferLen);	
	if ( retString != "OK" || 0 == bufferLen){
		free(buffer);
		buffer = NULL;
		MessageBox(retString.c_str(),"提示：",MB_SYSTEMMODAL);
		return "";
	}
	buffer[bufferLen]=0x00;
	char tmpAuth[256]={0};
	strcpy(tmpAuth,(char *)buffer);
	retString = tmpAuth;
	free(buffer);
	buffer = NULL;
	return retString;
}

bool ReadConfig(char* ip,unsigned int& port)
{
	if (ip == NULL)
	{
		return false;
	}
    DWORD ret=0; 
	ret = GetPrivateProfileStringA("ukey","keyip","",ip,32,"d:\\zyczs\\ukey.ini");
	if (0 == ret || strlen(ip) == 0)
	{
		return false;
	}
    port = GetPrivateProfileInt("ukey","keyport",0,"d:\\zyczs\\ukey.ini");
	if (0 == port)
	{
		return false;
	}
	return true;
}

BSTR CMyTestOcxCtrl::ULogin(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult="";
    initUsb();//取得了usbId
	// TODO: Add your dispatch handler code here
	//string taskId="12345";//这个随便输入
	string onewayPass="";
	string keyIp="";
	char tmpkeyIp[32]={0};
	unsigned int keyPort=0;
	if(!ReadConfig(tmpkeyIp,keyPort))
	{
		MessageBox("读取d:\\zyczs\\ukey.ini失败！","提示：",MB_SYSTEMMODAL);
		return strResult.AllocSysString();;
	}
    keyIp = tmpkeyIp;
	onewayPass = SentUsbIdToServer(keyIp,keyPort,usbId);//通过USBID请求pass 
	if ("" == onewayPass){
		//MessageBox("Ukey连接建立失败！","提示：",MB_SYSTEMMODAL);
		return strResult.AllocSysString();;
	}
	if (!KeyAuth(usbAuth,onewayPass.c_str())){
		MessageBox("Ukey认证失败！","提示：",MB_SYSTEMMODAL); 
		return strResult.AllocSysString();
	}
	size_t bufferLen=0;
	unsigned char* unzip_buffer = (unsigned char*)malloc(100); //接收源数据
	if( NULL == unzip_buffer ){
		MessageBox("内存不足","温馨提示：",MB_SYSTEMMODAL);
		return strResult.AllocSysString();
	}
	sprintf((char*)unzip_buffer,"GETPASSWORD %s %s",usbId,usbAuth);
	string retString = UkeyGetDownData((char*)unzip_buffer,keyIp.c_str(),keyPort,&unzip_buffer,bufferLen);
	free(unzip_buffer);
	unzip_buffer = NULL;
	if ( retString == "OK")
		strResult="ok";	
	//关闭、释放加密锁
	if (NULL != hInstMaster)
	{
		KeyClose();
		FreeLibrary(hInstMaster);
		hInstMaster = NULL;
	}

	return strResult.AllocSysString();
}
LONG CMyTestOcxCtrl::printtag2(LPCTSTR pp, LPCTSTR printerNamet, LONG nPrintType){
	string printStr = (const char*)pp;
	int rtn = 0;
	char printerName[0x64] = {0};
	if ( strlen(printerNamet) == 0 )
		strcpy(printerName,GetDefaultPrinter().c_str());
	else
		strcpy(printerName,printerNamet);
	
	if (strstr(printerName,"Printer LP") != NULL
		|| strstr(printerName,"Printer B") != NULL)
	{
		CTscPrinter printer;
		printer.TscInit(printerName);
		rtn = printer.PrintTscTag2(printerName,printStr,nPrintType);
		printer.TscUnInit();
	}
	else
	{
		AfxMessageBox("请检查是否正确设置默认打印机！");
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return rtn;


}


