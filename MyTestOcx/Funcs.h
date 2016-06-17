#ifndef _FUNCS_H_
#define _FUNCS_H_

#include <string>
#include <vector>
#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

//#define DBPathName "d:\\zyczs\\zyczs.db"
extern char  DBPathName[40];
void splitStrTwo( string printStr, string &Str1, string &Str2 );
list<string> splitPrintStr( string printStr );
void splitStrThree( string printStr, string &Str1, string &Str2, string&Str3 );
list<string> splitPrintv( list<string>& printStrv, int DYJC );
list<string> splitPrintv( string& tstr, int DYJC );

string FindAndCreatDB( const char* dbPathName );//文件不存在就创建空文件
string DelRecordByCode(const char* code,const char* dbPathName);
char* Gb2312ToUtf8(const char *pcGb2312) /*const*/;
void  delchar(char * str,char CharNum);
void filePath();
#endif