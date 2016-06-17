#include "StdAfx.h"
#include ".\pskprinter.h"

/////////////////////////////////
typedef int (__stdcall *fOpenPort)(char* PortFlag);
typedef int (__stdcall *fPTK_ClearBuffer)();
typedef int (__stdcall *fPTK_SetLabelHeight)
(unsigned int lheight, unsigned int gapH,int gapOffset,BOOL bBlag);
typedef int (__stdcall *fPTK_SetLabelWidth)(unsigned int lwidth);
typedef int (__stdcall *fPTK_SetDarkness)(unsigned  int id);
typedef int (__stdcall *fPTK_SetPrintSpeed)(unsigned int px);
typedef int (__stdcall *fPTK_DrawBar2D_QR)(unsigned int x, unsigned int y,
										   unsigned int w, unsigned int v,
										   unsigned int o, unsigned int r,
										   unsigned int m, unsigned int g,
										   unsigned int s, char* pstr);
typedef int (__stdcall *fPTK_DrawBar2D_QREx) ( unsigned int x, unsigned int y, unsigned int o,unsigned int r, unsigned int g,unsigned int v, unsigned int s,LPTSTR id_name,LPTSTR pstr );

typedef int (__stdcall *fPTK_DrawText)(unsigned int px, unsigned int py,
									   unsigned int pdirec, unsigned int pFont,
									   unsigned int pHorizontal, unsigned int pVertical,
									   char ptext, char* pstr);


typedef int (__stdcall *fPTK_DrawTextTrueTypeW)
		                            (int x,int y,int FHeight,
									int FWidth,LPCTSTR FType,
                                    int Fspin,int FWeight,
									BOOL FItalic,BOOL FUnline,
                                    BOOL FStrikeOut,
									LPCTSTR id_name,
									LPCTSTR data);

typedef int (__stdcall *fClosePort)(void);
typedef int (__stdcall *fPTK_PrintLabel)(unsigned int number,
										 unsigned int cpnumber);

////////////////////////////////////////////////////
HINSTANCE gt1Psk;
fOpenPort OpenPort = NULL;
fPTK_ClearBuffer PTK_ClearBuffer = NULL;
fPTK_SetLabelHeight PTK_SetLabelHeight = NULL;
fPTK_SetLabelWidth PTK_SetLabelWidth = NULL;
fPTK_SetDarkness PTK_SetDarkness = NULL;
fPTK_SetPrintSpeed PTK_SetPrintSpeed = NULL;
fPTK_DrawBar2D_QR PTK_DrawBar2D_QR = NULL;
fPTK_DrawText PTK_DrawText = NULL;
fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW = NULL;
fClosePort ClosePort = NULL;
fPTK_PrintLabel PTK_PrintLabel = NULL;
fPTK_DrawBar2D_QREx PTK_DrawBar2D_QREx=NULL;

OSVERSIONINFO os;
 int sizeOffset3=0;
 int sizeOffset=0;
 int sizeOffset_1=0;
///////////////////////////////

CPskPrinter::CPskPrinter(void)
{
}

CPskPrinter::~CPskPrinter(void)
{
}

bool printNew2_psk(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QR PTK_DrawBar2D_QR,fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW){
	char buff[0x100];
	
	list<string> printv = splitPrintStr(printStr);
	int x = 0, y = 0, z = 0, ret = 0,XX = X + 135;
	char numBuff[10]={"dda"};
	static int num=1;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码
			sprintf(numBuff,"484%d",num++);
			
			if (strstr(iter->c_str(),"http") != NULL)
			{
				//ret = PTK_DrawBar2D_QR(X+134, Y, 0, 0, 0, 3, 4, 3, 3, (char*)iter->c_str());
				ret = PTK_DrawBar2D_QREx(X+134,Y,0,4,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num=num%39;
			}
			else if(strlen(iter->c_str())<5){
				break;}
			else
			{
				//ret = PTK_DrawBar2D_QR(X+134, Y, 0, 0, 0, 4, 4, 3, 3, (char*)iter->c_str());
				ret = PTK_DrawBar2D_QREx(X+120,Y,0,5,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num%=3;
			}
		
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}
			
			y = Y + 170;
		}else if( 1 == z ){//打数字码
		
			list<string> printCodes = splitPrintv(string(*iter),28);
			int x = X;
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeW(x, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				x += 64;
				y += 22;
			}
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			//DelRecordByCode(citer->c_str(),DBPathName);
			DelRecordByCode(iter->c_str(),DBPathName);
		}else if( 2 == z || 4 == z || 6 == z || 7 == z||3 == z || 5 == z ){ //品名 产地 批号 厂家
			if( 3 == z){
				delchar((char *)iter->c_str(),' ');
				// *iter="重"+iter->substr(5,50);
			
			}//
			if (iter->length()==0)
			{
				z++;
				continue;
			}
			
			//else
			ret = PTK_DrawTextTrueTypeW(X, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			
		//	if( 2 != z && 4 != z )
				y += 20;
		}
		#if 0
else if(  5 == z ){ //重量  /*生产日期*/ 2015-6-23 取消药材打印日期
			if (z == 5)
			{
				//y+=22;
				z++;
				continue;
			}

#if 0
			ret = PTK_DrawTextTrueTypeW(XX, y, 17,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}y += 22;
			#endif
			
}
#endif
		
		z++;
	}
	
	return true;

}
bool printNew2_psk_1(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QR PTK_DrawBar2D_QR,fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW)

{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int x = 0, y = 0, z = 0, ret = 0,XX = X + 135;
	int numTextX=X,numTextY=120,numX=X+170,numY=216;
	char numBuff[10]={"sps"};
	static int num=0;
	
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码
			sprintf(numBuff,"rfdf%d",num++);
			if (strstr(iter->c_str(),"http") != NULL)
			{
				ret = PTK_DrawBar2D_QREx(X+180,Y+92,0,3,0,0,3,(char*)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				num%=2;
			}
			else if(strlen(iter->c_str())<5){
				break;}
			else
			{
				
				ret = PTK_DrawBar2D_QREx(X+180,Y+92,0,4,0,0,3,(char*)numBuff,(char*)iter->c_str());//"psk+1"字符串：调用一次打印机，同时（init与uninit之间）打印多张二维码（此处为两张）且与其他参数不一样时，每次需要该字符串唯一，
				num%=3;
			}
		
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
				throw(buff);
			}
			
			y = Y;
		}else if( 1 == z ){//打数字码
			string num=iter->substr(7,50);
			list<string> printCodes = splitPrintv(num,16);
			
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeW(numX, numY, 12,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				
				numY += 10;
			}
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			DelRecordByCode(iter->c_str(),DBPathName);
		}else if( 2 == z || 4 == z || 6 == z || 7 == z ||3 == z || 5 == z){ //品名 产地 批号 厂家
			if( 7 == z){
				
				//if(strstr((char*)iter->c_str(),"厂家")!=NULL)
				//{
				//	*iter=iter->substr(5,50);
				//}
				*iter=iter->substr((iter->find( "厂家:")!=-1?iter->find("厂家:")+5:0),iter->length());
			}
			
				ret = PTK_DrawTextTrueTypeW(numTextX, numTextY, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
			
					numTextY += 20;
		}
#if 0
		else if( 3 == z || 5 == z ){ //重量  /*生产日期*/ 2015-6-23 取消药材打印日期
			if (z == 5)
			{
				y+=22;
				z++;
				continue;
			}
			ret = PTK_DrawTextTrueTypeW(XX, y, 20,0,"微软雅黑", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			y += 22;
		}
#endif
		z++;
	}
	return true;
}

bool printNew3_psk(const string& printStr,int X, int Y,int& id_name,fPTK_DrawBar2D_QR PTK_DrawBar2D_QR,fPTK_DrawTextTrueTypeW PTK_DrawTextTrueTypeW)
{
	char buff[0x100];
	list<string> printv = splitPrintStr(printStr);
	int y = 0,z = 0,ret = 0;
	bool flag = false;
	char numBuff[10]={"pskss"};
	static int num=1;
	int division=12;
	int offset_X=0,offset_Y=0;
	for ( list<string>::iterator iter = printv.begin();iter != printv.end(); ++iter ){
		if( 0 == z ){ //打二维码

			sprintf(numBuff,"hhkh%d",num++);
			if (strstr(iter->c_str(),"http") != NULL)
			{	
				ret = PTK_DrawBar2D_QREx(X+104,Y,0,3,0,0,3,(char *)numBuff,(char*)iter->c_str());//字符串长度限制导致无法正确扫描二维码，故选取该API，
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
					throw(buff);
				}
				y = Y + 100;
				flag = true;					
				num=num%39;
			}
			else if(strlen(iter->c_str())<5){
				break;
			}
			else
			{
				
				ret = PTK_DrawBar2D_QREx(X+104,Y,0,4,0,0,3,numBuff,(char*)iter->c_str());
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawBar2D_QR:%d",ret);
					throw(buff);
				}
				y = Y + 100;
				num=num%3;
			}
		}else if( 1 == z ){//打数字码
			*iter=iter->substr((iter->find( "溯源码:")!=-1?iter->find("溯源码:")+7:0),iter->length());
			if (iter->c_str()[0]=='8')
			{
				division=20;
				offset_X=-102;offset_Y=30;
			}
			list<string> printCodes = splitPrintv(string(*iter),division);
			for ( list<string>::const_iterator citer = printCodes.begin(); citer != printCodes.end(); ++citer ){
				ret = PTK_DrawTextTrueTypeW(X+106+offset_X, y, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)citer->c_str()); //x-100
				if ( ret != 0 ){
					sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
					throw(buff);
				}
				y += 15;
			}
			if (iter->c_str()[0]=='6')
			{
				offset_Y=-15;

			}
		
			 y = Y + 100+offset_Y;
			DelRecordByCode(iter->c_str(),DBPathName);
		}
		else{//其它文字
			if (2 == z)
			{
				delchar((char *)iter->c_str(),' ');
				
			}
			
			if (iter->length()==0)
			{
				z++;
				continue;
			}
			ret = PTK_DrawTextTrueTypeW(X, y, 14,0,"宋体", 1, 600, 0, 0, 0, itoa(id_name++,buff,10), (char*)iter->c_str()); //x-100
			if ( ret != 0 ){
				sprintf(buff,"PTK_DrawTextTrueTypeW:%d",ret);
				throw(buff);
			}
			y += 15;
		}
		z++;
	}
	return true;
}

bool CPskPrinter::PskInit(char* printerName)
{
	gt1Psk = LoadLibrary("WINPSK.dll");
	OpenPort = (fOpenPort)GetProcAddress(gt1Psk,"OpenPort");
	ClosePort = (fClosePort)GetProcAddress(gt1Psk,"ClosePort");
	PTK_DrawBar2D_QR = (fPTK_DrawBar2D_QR)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QR");
	PTK_SetLabelHeight = (fPTK_SetLabelHeight)GetProcAddress(gt1Psk,"PTK_SetLabelHeight");
	PTK_PrintLabel = (fPTK_PrintLabel)GetProcAddress(gt1Psk,"PTK_PrintLabel");
	PTK_SetDarkness = (fPTK_SetDarkness)GetProcAddress(gt1Psk,"PTK_SetDarkness");
	PTK_SetPrintSpeed = (fPTK_SetPrintSpeed)GetProcAddress(gt1Psk,"PTK_SetPrintSpeed");
	PTK_SetLabelWidth = (fPTK_SetLabelWidth)GetProcAddress(gt1Psk,"PTK_SetLabelWidth");
	PTK_ClearBuffer = (fPTK_ClearBuffer)GetProcAddress(gt1Psk,"PTK_ClearBuffer");
	PTK_DrawText = (fPTK_DrawText)GetProcAddress(gt1Psk,"PTK_DrawText");
	PTK_DrawTextTrueTypeW = (fPTK_DrawTextTrueTypeW)GetProcAddress(gt1Psk,"PTK_DrawTextTrueTypeW");
	PTK_DrawBar2D_QREx=(fPTK_DrawBar2D_QREx)GetProcAddress(gt1Psk,"PTK_DrawBar2D_QREx");
	//try{
		int ret = OpenPort(printerName);	
		if ( ret != 0 ){
			//sprintf(buff,"OpenPort:%d",ret);
			//throw(buff);
			return false;
		}

		return true;
}
bool CPskPrinter::PskUnInit()
{
	ClosePort();
	FreeLibrary(gt1Psk);
	return true;
}
int CPskPrinter::PrintPsk2(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	string printStr1, printStr2;
	splitStrTwo(printStr, printStr1, printStr2);
	

	try{
		int ret = PTK_ClearBuffer();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeight(368, 40,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidth(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarkness(10);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeed(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		int X = 80,Y = 23;
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 3)){
			bIsPrint |= printNew2_psk(printStr1,X,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印右边
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 3)){
			bIsPrint |= printNew2_psk(printStr2,X+=366,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}
		//==================================
		ret = PTK_PrintLabel(1, 1);	
		if ( ret != 0 ){
			sprintf(buff,"PTK_PrintLabel:%d",ret);
			throw(buff);
		}
		//ClosePort();
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}
int CPskPrinter::PrintPsk2_1(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	string printStr1, printStr2;
	splitStrTwo(printStr, printStr1, printStr2);
	

	try{
		int ret = PTK_ClearBuffer();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeight(240, 24,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidth(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarkness(15);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeed(5);//用于设置每一个单位代表多少mm，表示每1单位0.125mm
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		int X = 80,Y = 23;
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 3)){
			bIsPrint |= printNew2_psk_1(printStr1,X,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印右边
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 3)){
			bIsPrint |= printNew2_psk_1(printStr2,X+=366,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}
		//==================================
		ret = PTK_PrintLabel(1, 1);	
		if ( ret != 0 ){
			sprintf(buff,"PTK_PrintLabel:%d",ret);
			throw(buff);
		}
		//ClosePort();
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	//FreeLibrary(gt1);
	return rtn;
}


int CPskPrinter::PrintPsk3(char* printerName,const string& printStr,LONG nPrintType)
{
	int rtn,id_name = 0;
	bool bIsPrint = false;
	char buff[0x100]={0};
	
	string printStr1, printStr2,printStr3;
	splitStrThree(printStr, printStr1, printStr2, printStr3);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	
	try{
		int ret = PTK_ClearBuffer();
		if ( ret != 0 ){
			sprintf(buff,"PTK_ClearBuffer:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelHeight(200, 24,0,false);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelHeight:%d",ret);
			throw(buff);
		}
		ret = PTK_SetLabelWidth(800);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetLabelWidth:%d",ret);
			throw(buff);
		}
		ret = PTK_SetDarkness(13);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetDarkness:%d",ret);
			throw(buff);
		}
		ret = PTK_SetPrintSpeed(5);
		if ( ret != 0 ){
			sprintf(buff,"PTK_SetPrintSpeed:%d",ret);
			throw(buff);
		}

		//int X = 42,Y = 10;//old
		int X = 37,Y = 18;//new
		//sprintf(numBuff,"%d",num++)
		//打印左边
		if (printStr1.length() > 0 && (nPrintType == 1 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr1,X,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印中间
		if (printStr2.length() > 0 && (nPrintType == 2 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr2,X+=260,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}

		//打印右边
		if (printStr3.length() > 0 && (nPrintType == 3 || nPrintType == 4)){
			bIsPrint |= printNew3_psk(printStr3,X+=260,Y,id_name,PTK_DrawBar2D_QR,PTK_DrawTextTrueTypeW);
		}
		//==================================
		if (bIsPrint){
			ret = PTK_PrintLabel(1, 1);	
			if ( ret != 0 ){
				sprintf(buff,"PTK_PrintLabel:%d",ret);
				throw(buff);
			}
		}
	
		rtn = 1;
	}
	catch (const exception& e) {
		AfxMessageBox(e.what());
		rtn = 0;
	}
	catch (...) {
		sprintf(buff,"打印机错误:%d",GetLastError());
		AfxMessageBox(buff);
		rtn = 0;
	}
	
	return rtn;
}