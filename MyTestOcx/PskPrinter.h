#pragma once
#ifndef PSK_PRINTER_H
#define PSK_PRINTER_H
#include "Funcs.h"


class CPskPrinter
{
public:
	CPskPrinter(void);
	~CPskPrinter(void);

	bool PskInit(char* printerName);
	bool PskUnInit();
	int PrintPsk2(char* printerName,const string& printStr,LONG nPrintType);
	int PrintPsk2_1(char* printerName,const string& printStr,LONG nPrintType);
	int PrintPsk3(char* printerName,const string& printStr,LONG nPrintType);
};

#endif
