#pragma once
#ifndef TSC_PRINTER_H
#define TSC_PRINTER_H
#include "Funcs.h"

typedef int (__stdcall *fTSC_Openport)(char*);
typedef int (__stdcall *fTSC_Closeport)();
typedef int (__stdcall *fTSC_Sendcommand)(char*);
typedef int (__stdcall *fTSC_Setup)(char *,char *,char *,char *,char *,char *,char *);
typedef int (__stdcall *fTSC_Clearbuffer)();
typedef int (__stdcall *fTSC_Printlabel)(char *, char *);
typedef int (__stdcall *fTSC_Windowsfont)(int x, int y, int fontheight, int rotation, int fontstyle, int fontunderline, char *szFaceName, char *content);


class CTscPrinter
{
public:
	CTscPrinter(void);
	~CTscPrinter(void);
	bool wineFlag;
    bool TscInit(char* printerName);
	bool TscUnInit();
	bool printNew2_tsc(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText);
	bool printNew2_tsc_tag(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText);
	bool printNew2_tsc_1(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText);
	bool printNew3_tsc(const string& printStr,int X, int Y,fTSC_Sendcommand TSC_DrawQR,fTSC_Windowsfont TSC_DrawText);
	int PrintTsc2(char* printerName,const string& printStr,LONG nPrintType);
	int PrintTscTag2(char* printerName,const string& printStr,LONG nPrintType);
	int PrintTsc2_1(char* printerName,const string& printStr,LONG nPrintType);
	int PrintTsc3(char* printerName,const string& printStr,LONG nPrintType);
};

#endif
