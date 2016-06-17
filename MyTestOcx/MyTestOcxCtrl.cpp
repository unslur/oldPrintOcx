// MyTestOcxCtrl.cpp : Implementation of the CMyTestOcxCtrl ActiveX Control class.
/**
2015-8-19 �����ж�windows�Դ�����Generic
2015-8-27 �����ݿ�·����C�̸�ΪD��




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

//MP300��ά���ͷ����
//������
typedef struct
{
	int iErrCode;
	int iCodeType;
	int iLenContent;
	unsigned char *pContent;
}MPDECODERET, *PMPDECODERET;

//���������豸����
typedef struct
{
	HWND hWnd;      //��������
	BYTE bImgSize;  //ͼ���С���� 1:160*120  2:320*240 
	BYTE bImgType;  //ͼ���ʽ  2:�Ҷ�ͼ  3:RGB24
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
	string rtn = FindAndCreatDB(DBPathName); //����
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
	unsigned char* zip_buffer = (unsigned char*)malloc(0x400*0x400*10);// ������������
	size_t zip_length; // �������
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
	
	filePath();//�������ݿ�Ŀ¼�ļ�

	string rtn = FindAndCreatDB(DBPathName); //����
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
	sqlite3_exec(db, sqlBuff, NULL, 0, &zErrMsg);//����ǰ�����Ч����
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
		retstring = "����Ƿ����IC����";
	}
	#endif
    strResult = (char *)retstring.c_str();
	return strResult.AllocSysString();
}

BSTR CMyTestOcxCtrl::WriteRFID2(LPCTSTR codeStr, LONG mode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//�ж��Ƿ�Ϊ��ɫ������
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
		retstring = "����Ƿ����IC����";
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
	int codenum = nPrintType; //nPrintType�Ǵ������
	char buff[32]={0};
	for (int i=1;i<=codenum;i++)
	{
		//���������
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
			AfxMessageBox("�����Ƿ���ȷ����Ĭ�ϴ�ӡ����");
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
//nPrintType 1��ʾ��ӡ��ߣ�2��ʾ��ӡ�м䣬3��ʾ��ӡ�ұߣ�4��ʾһ�д�ӡ3��
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
		AfxMessageBox("�����Ƿ���ȷ����Ĭ�ϴ�ӡ����");
	}
	count++;
	return rtn;
}//*/

//nPrintType 1��ʾ��ӡ��ߣ�2��ʾ��ӡ�ұߣ�3��ʾһ�д�ӡ2��
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
	//��˹��  POSTEK G2000
	//tsc  Bar Code Printer LP-2000A
	//����  ZDesigner ZT210-200dpi ZPL
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
		AfxMessageBox("�����Ƿ���ȷ����Ĭ�ϴ�ӡ����");
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
	//��˹��  POSTEK G2000
	//tsc  Bar Code Printer LP-2000A
	//����  ZDesigner ZT210-200dpi ZPL
		
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
		AfxMessageBox("�����Ƿ���ȷ����Ĭ�ϴ�ӡ����");
	}

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return rtn;

}
////////////////////////////////////////////////////////////////////////////////////////


/*
#define PRINTER_STATUS_PAUSED             0x00000001    ��ͣ
#define PRINTER_STATUS_ERROR              0x00000002    ����
#define PRINTER_STATUS_PENDING_DELETION   0x00000004    ����ɾ��
#define PRINTER_STATUS_PAPER_JAM          0x00000008    ��ֽ
#define PRINTER_STATUS_PAPER_OUT          0x00000010    ȱֽ
#define PRINTER_STATUS_MANUAL_FEED        0x00000020    �˹���ֽ
#define PRINTER_STATUS_PAPER_PROBLEM      0x00000040    ֽ������
#define PRINTER_STATUS_OFFLINE            0x00000080    ����
#define PRINTER_STATUS_IO_ACTIVE          0x00000100    �˿��Ѵ�
#define PRINTER_STATUS_BUSY               0x00000200    æµ
#define PRINTER_STATUS_PRINTING           0x00000400    ��ӡ��
#define PRINTER_STATUS_OUTPUT_BIN_FULL    0x00000800    �����
#define PRINTER_STATUS_NOT_AVAILABLE      0x00001000    �޿���
#define PRINTER_STATUS_WAITING            0x00002000    �ȴ�
#define PRINTER_STATUS_PROCESSING         0x00004000    ������
#define PRINTER_STATUS_INITIALIZING       0x00008000    ��ʼ����
#define PRINTER_STATUS_WARMING_UP         0x00010000    ����Ԥ��
#define PRINTER_STATUS_TONER_LOW          0x00020000    ��ɫ����
#define PRINTER_STATUS_NO_TONER           0x00040000    ��ɫ����
#define PRINTER_STATUS_PAGE_PUNT          0x00080000    ҳ������
#define PRINTER_STATUS_USER_INTERVENTION  0x00100000    �û���Ԥ
#define PRINTER_STATUS_OUT_OF_MEMORY      0x00200000    �����ڴ�
#define PRINTER_STATUS_DOOR_OPEN          0x00400000    ���Ӵ�
#define PRINTER_STATUS_SERVER_UNKNOWN     0x00800000    δ֪����
#define PRINTER_STATUS_POWER_SAVE         0x01000000    �ڵ�

#define JOB_STATUS_PAUSED               0x00000001    //��ͣ
#define JOB_STATUS_ERROR                0x00000002    //����
#define JOB_STATUS_DELETING             0x00000004    //����ɾ��
#define JOB_STATUS_SPOOLING             0x00000008    //���ڻ���
#define JOB_STATUS_PRINTING             0x00000010    //���ڴ�ӡ
#define JOB_STATUS_OFFLINE              0x00000020    //����
#define JOB_STATUS_PAPEROUT             0x00000040    //ȱֽ
#define JOB_STATUS_PRINTED              0x00000080    //��ӡ���
#define JOB_STATUS_DELETED              0x00000100    //�Ѿ�ɾ��
#define JOB_STATUS_BLOCKED_DEVQ         0x00000200
#define JOB_STATUS_USER_INTERVENTION    0x00000400    //�û���Ԥ
#define JOB_STATUS_RESTART              0x00000800    //����
//*/

LONG CMyTestOcxCtrl::print2(LPCTSTR pp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const int DYJC = 34;
	int resultI = 1;

	// �������´���
	char szprinter[80];
	char *szDevice,*szDriver,*szOutput;
	HDC hdcprint; // ����һ���豸�������
	//����һ����ӡ��ҵ
	static DOCINFO di={sizeof(DOCINFO),"printer",NULL}; 
	// �õ��豸�ַ�����������szprinter��
	GetProfileString("windows","device",",,,",szprinter,80); 
	// ���豸�ַ����ֽ�
	if(NULL!=(szDevice=strtok(szprinter,","))&&		
		NULL!=(szDriver=strtok(NULL,","))&&		
		NULL!=(szOutput=strtok(NULL,",")))		
		// ����һ����ӡ���豸��� 

		if((hdcprint=CreateDC(szDriver,szDevice,szOutput,NULL))!=0){	
			if(StartDoc(hdcprint,&di)>0){ //��ʼִ��һ����ӡ��ҵ
				StartPage(hdcprint); //��ӡ����ֽ,��ʼ��ӡ				
				SaveDC(hdcprint); //�����ӡ���豸���				
				// ���һ������
				char buff[DYJC*2+1];
				string printStr = pp; //"��������MSDN�е�TextOut��\n����,��һ����������������Ŀ�ʼ��ĺ�����,�ڶ�������������,��������������\n�����ֵ�����,���ĸ�������Ҫ���������";
				list<string> printv = splitPrintv(splitPrintStr(printStr),DYJC);
				int l = 0;
				for ( list<string>::const_iterator iter = printv.begin();iter != printv.end(); ++iter ){
					memset(buff,0,sizeof(buff));
					strcpy(buff,iter->c_str());
					TextOut(hdcprint,1,l*41+1,buff,DYJC);
					l++;
				}

				RestoreDC(hdcprint,-1); //�ָ���ӡ���豸���
				EndPage(hdcprint); //��ӡ��ֽͣ,ֹͣ��ӡ
				EndDoc(hdcprint); //����һ����ӡ��ҵ
				//MessageBox("��ӡ���!","��ʾ",MB_ICONINFORMATION);				
			}
			// ��API����DeleteDC����һ����ӡ���豸��� 			
			DeleteDC(hdcprint);			
		}		
		else{
			MessageBox("û��Ĭ�ϴ�ӡ��,����û�а�װ��ӡ��!");
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

	HANDLE hCom;  //ȫ�ֱ��������ھ��
	hCom=CreateFile(com,//COM7��
		GENERIC_READ|GENERIC_WRITE, //��������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);
	if(hCom==(HANDLE)-1)
	{
		AfxMessageBox("��COMʧ��!");
		strResult = "";
		return strResult.AllocSysString();
	}

	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;
	//�ڶ�һ�����뻺���������ݺ���������������أ�
	//�������Ƿ������Ҫ����ַ���
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom,&TimeOuts); //���ó�ʱ

	DCB dcb;
	GetCommState(hCom,&dcb);
	dcb.BaudRate=baudRate; //������Ϊ9600
	dcb.ByteSize=8; //ÿ���ֽ���8λ
	dcb.Parity=NOPARITY; //����żУ��λ
	dcb.StopBits=TWOSTOPBITS; //����ֹͣλ
	SetCommState(hCom,&dcb);

	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);

	char buffer[2];
	buffer[0] = 0x1b;
	buffer[1] = 0x32;
	unsigned long lpNumberOfBytesWritten = 0;
	WriteFile(hCom,buffer,2,&lpNumberOfBytesWritten,NULL);



	char str[100];
	memset(str,0,sizeof(str));
	DWORD wCount;//��ȡ���ֽ���
	BOOL bReadStat;
	int timeF = 0;
	while(true){
		bReadStat=ReadFile(hCom,str,100,&wCount,NULL);
		if(!bReadStat){
			AfxMessageBox("������ʧ��!");
			timeF *= 10;
		}
		if(strlen(str)>0){
			if( isTrueCode(str)){
				break;
			}else{
				memset(str,0,sizeof(str));
			}
		}
		if( timeF++ > overTime*10){ //��ʱ
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
		AfxMessageBox("�ر�COMʧ��!");
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
	else AfxMessageBox("����������!");

	//pDlg->Create(IDD_BARCODEDLG);
	//pDlg->ShowWindow(SW_SHOW);


	//��ʼ��
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
			AfxMessageBox("��ʼ���豸�ɹ���");
			blInit = true;
		}
		else {
			strResult = "�豸��ʼ��ʧ�ܣ���������� ��";
			return strResult.AllocSysString();
		}
	}	

	//��ʼ��ȡ
	if((MPReader_Begin!=NULL) && blInit)
	{		
		int iRet = MPReader_Begin();
		//if (iRet == 0 ) AfxMessageBox("�ѿ���!");
	}

	while(true){
		
		nCount++;
		
		if (bIsOK)
			break;	//��ȡ�ɹ�����

		if (nCount > 150)
			break;

		Sleep(100);
	}

	//������ȡ
	if((MPReader_End!=NULL) && blInit)
	{
		MPReader_End();
			//AfxMessageBox("�豸�ѹر�!");
	}	

	//�ͷ���Դ
	if((MPReader_UnInit!=NULL) && blInit)
	{
		MPReader_UnInit() ;
			//AfxMessageBox("�豸��Դ���ͷ�!");

		blInit = false;
	}
	
	//pDlg->CloseWindow();
	//pDlg->DestroyWindow();
	//pDlg = NULL;

	if (bIsOK) {
		return strResult.AllocSysString();
	}
	else {

		strResult = "�涨ʱ����ʶ��ʧ�ܣ���������ԣ�";
		return strResult.AllocSysString();
	}
}

int WINAPI MPDecodeRetProc(PMPDECODERET pMPDecoeRet)
{
	AfxMessageBox("MPDecodeRetProc");

	char szShow[100] = {0};
	if(pMPDecoeRet->iErrCode!=0)
	{
		sprintf(szShow, "����ʧ��, �������Ϊ: %d", pMPDecoeRet->iErrCode);
		bIsOK = false;
		
		return -1;
	}
	else 
	{
		if(pMPDecoeRet->iCodeType==100) sprintf(szShow, "����ɹ�  GM��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==200) sprintf(szShow, "����ɹ�  QR��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==201) sprintf(szShow, "����ɹ�  DM��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==202) sprintf(szShow, "����ɹ�  EZ��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==300) sprintf(szShow, "����ɹ�  UPC-A��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==301) sprintf(szShow, "����ɹ�  UPC-E��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==302) sprintf(szShow, "����ɹ�  EAN-13��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==303) sprintf(szShow, "����ɹ�  EAN-8��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==304) sprintf(szShow, "����ɹ�  JAN-13��  ��ͼ��������\r\n");
		else if(pMPDecoeRet->iCodeType==305) sprintf(szShow, "����ɹ�  JAN-8��  ��ͼ��������\r\n");
		
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
	//��ȡ������ID  ���ʧ�� �͵���ʾ�� ���˳�ȥ  
	hInstMaster = LoadLibrary("USBKeyDLL.dll");
	if(!hInstMaster){	
		MessageBox("����USBKeyDLL.dllʧ�ܣ�","��ʾ��",MB_SYSTEMMODAL); 
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

//�ӷ���������������
string CMyTestOcxCtrl::UkeyGetDownData(IN const char* sendCommand, IN const char* ip, IN int port, OUT unsigned char** buffer, OUT size_t& bufferLen) const{
	Socket so;
	so.CInit(ip,port);
	so.CCreateSocket();
	try{
		if(!so.CConnect()) throw exception("���粻ͨ");
		if(!so.Send(sendCommand)) throw exception("����ʧ��");

		char recvBuff[0x400*0x200];
		int recvInt = 0,allRecvLen = 0;
		while(true){
			if ( !so.Recv(recvBuff+allRecvLen,0x400,recvInt) ) break;
			allRecvLen += recvInt;
		}
		recvBuff[allRecvLen]=0x00;
		if (allRecvLen <= 36){
			if ( strcmp(recvBuff,"1") == 0){
				throw exception("�Ѿ�����");
			}else if (strcmp(recvBuff,"0") == 0){
				throw exception("����Ƿ�");
			}else if (36 == allRecvLen){
				memcpy(*buffer,recvBuff,allRecvLen);
				bufferLen = allRecvLen;
				throw exception("OK");
			}else if (9 == allRecvLen){
				throw exception("���ȼ���豸�б��Ƿ���»��߼������Ƿ�������ȷ,��������,����ϵ������!");
			}
			else if (0 == allRecvLen){
				throw exception("��������У�����ϵ�����̣�");
			}
			else if (strcmp("password ok",recvBuff) == 0)
			{
				throw exception("OK");
			}
			else if (strcmp("password error",recvBuff) == 0)
			{
				throw exception("Ukey��֤ʧ�ܣ�");
			}else throw exception("����ʧ��");
		}
		LodePNG_DecompressSettings setting;
		setting.ignoreAdler32 = 0;
		int ret = LodePNG_zlib_decompress(buffer, &bufferLen, (unsigned char*)recvBuff, allRecvLen, &setting);
		if( ret != 0 ) throw exception("��ѹʧ��");
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
	unsigned char* buffer = (unsigned char*)malloc(256);//��ѹ��Ŀռ�
	if( NULL == buffer ){
		return "";
	}  
	memset(buffer,0,256);
	//����ӿں��������  �Ƿ�Ҫ�ʹ�����Ļ����һ��  �����ϵĻ� ��Ҫÿ��U�ܶ����ô����豸

	//test
	sprintf((char *)buffer,"GETRANDOM %s",usbId);//�½ӿ�   ֮ǰ�Ƿ����豸���� ���ڷ���1
	retString = UkeyGetDownData((char *)buffer,taskIP.c_str(),taskPort,&buffer,bufferLen);	
	if ( retString != "OK" || 0 == bufferLen){
		free(buffer);
		buffer = NULL;
		MessageBox(retString.c_str(),"��ʾ��",MB_SYSTEMMODAL);
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
    initUsb();//ȡ����usbId
	// TODO: Add your dispatch handler code here
	//string taskId="12345";//����������
	string onewayPass="";
	string keyIp="";
	char tmpkeyIp[32]={0};
	unsigned int keyPort=0;
	if(!ReadConfig(tmpkeyIp,keyPort))
	{
		MessageBox("��ȡd:\\zyczs\\ukey.iniʧ�ܣ�","��ʾ��",MB_SYSTEMMODAL);
		return strResult.AllocSysString();;
	}
    keyIp = tmpkeyIp;
	onewayPass = SentUsbIdToServer(keyIp,keyPort,usbId);//ͨ��USBID����pass 
	if ("" == onewayPass){
		//MessageBox("Ukey���ӽ���ʧ�ܣ�","��ʾ��",MB_SYSTEMMODAL);
		return strResult.AllocSysString();;
	}
	if (!KeyAuth(usbAuth,onewayPass.c_str())){
		MessageBox("Ukey��֤ʧ�ܣ�","��ʾ��",MB_SYSTEMMODAL); 
		return strResult.AllocSysString();
	}
	size_t bufferLen=0;
	unsigned char* unzip_buffer = (unsigned char*)malloc(100); //����Դ����
	if( NULL == unzip_buffer ){
		MessageBox("�ڴ治��","��ܰ��ʾ��",MB_SYSTEMMODAL);
		return strResult.AllocSysString();
	}
	sprintf((char*)unzip_buffer,"GETPASSWORD %s %s",usbId,usbAuth);
	string retString = UkeyGetDownData((char*)unzip_buffer,keyIp.c_str(),keyPort,&unzip_buffer,bufferLen);
	free(unzip_buffer);
	unzip_buffer = NULL;
	if ( retString == "OK")
		strResult="ok";	
	//�رա��ͷż�����
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
		AfxMessageBox("�����Ƿ���ȷ����Ĭ�ϴ�ӡ����");
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return rtn;


}

