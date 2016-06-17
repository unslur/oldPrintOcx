#include "stdafx.h"
#include "Funcs.h"



extern char  DBPathName[40]={0};
string FindAndCreatDB( const char* dbPathName ){//文件不存在就创建空文件
	bool creatF = false;
#if 1
	fstream   file;
	//AfxMessageBox(dbPathName);
	file.open( dbPathName,ios::in);
	
	if(file.is_open()){
		file.seekg(0,std::ios::end );
		if(file.tellg() > 0 )
			creatF= true;
	}else{//文件不存在创建文件
		file.open(dbPathName,ios::out);//只能创建已存在的目录下的文件
		if(!file.is_open()) //创建失败
			return "Create file error";
		
	}
	file.close();
#endif	
#if 0

FILE *fp=fopen(dbPathName,"a+");
if(fp==NULL){
	return strerror(errno);
}
fseek(fp,0,SEEK_END);
if(ftell(fp)){
creatF=true;
}
fclose(fp);
#endif
	if (!creatF){
		char buff[0x400];
		sqlite3 *db;
		char *zErrMsg = 0;
		int rc;
		rc = sqlite3_open(dbPathName, &db);
		if( rc ){
			sqlite3_close(db);
			sprintf(buff,"Can't open database: %s",sqlite3_errmsg(db));
			return buff;
		}
		rc = sqlite3_exec(db, "create table number(id integer primary key autoincrement,orgid varchar(30),taskid varchar(30),flag integer,code varchar(40))", NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			sprintf(buff, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return buff;
		}
		sqlite3_close(db);
	}	
	return "OK"; 
}
void filePath(){//设置数据库目录文件
	
	if(GetDriveType("D:") == DRIVE_FIXED ){
		strcpy(DBPathName,"d:\\zyczs\\zyczs.db");
	
	}	
	else if( GetDriveType("C:") == DRIVE_FIXED){
		strcpy(DBPathName,"c:\\zyczs\\zyczs.db");
	
	}

	else
	{
		GetSystemDirectory(DBPathName,strlen(DBPathName)-1);
		strcpy(DBPathName+1,":\\zyczs\\zyczs.db");
	}
	//strcpy(DBPathName,"c:\\zyczs\\zyczs.db");

}
string DelRecordByCode(const char* code,const char* dbPathName){
//string DelRecordByCode(const char* code,const char* dbPathName){
	char buff[0x400];
	//const char *dbPathName =dBPathName.c_str();
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_open(dbPathName, &db);
	if( rc ){
		sqlite3_close(db);
		sprintf(buff,"Can't open database: %s",sqlite3_errmsg(db));
		return buff;
	}
	
	sprintf(buff,"delete from number where code='%s'",code);
	rc = sqlite3_exec(db, buff, NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		sprintf(buff, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return buff;
	}
	sqlite3_close(db);
	return "OK";
}//*/

void splitStrTwo( string printStr, string &Str1, string &Str2 )
{

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		Str2 = printStr.substr(b+1,printStr.length());
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}

list<string> splitPrintStr( string printStr ){
	list<string> vstr;
	while (true){
		int b = printStr.find("\n");
		if( b != -1 ){
			vstr.push_back(printStr.substr(0,b));
			printStr = printStr.substr(b+1,printStr.length());
		}
		else{
			if(printStr.length()!=0){
				vstr.push_back(printStr);
			}
			break;
		}
	}
	return vstr;
}


void splitStrThree( string printStr, string &Str1, string &Str2, string&Str3 ){

	string strTemp;

	int b = printStr.find("|");
	if( b != -1 ){
		Str1 = printStr.substr(0,b);
		strTemp = printStr.substr(b+1,printStr.length());
		b = 0;
		b = strTemp.find("|");

		if (b != -1) {
			Str2 = strTemp.substr(0, b);
			Str3 = strTemp.substr(b+1,strTemp.length());
		}
		else {
			if(printStr.length()!=0){
				Str2 = strTemp;
			}
		}
	}
	else{
		if(printStr.length()!=0){
			Str1 = printStr;
		}
	}

	return;
}

//#define DYJC 34

list<string> splitPrintv( list<string>& printStrv, int DYJC ){
	list<string> vstr;
	for(list<string>::const_iterator iter = printStrv.begin(); iter != printStrv.end();++iter){
		string tstr = *iter; //GBK 前字81-FE 之间，尾字节在 40-FE
		while(true){
			if(tstr.length()<=DYJC){
				vstr.push_back(tstr);
				break;
			}
			bool f = false;
			for( int i = 0; i < DYJC;){
				char m = tstr.at(i);
				if( unsigned(m) < 0x80 ){
					f = true;
					i++;
				}
				else{
					i += 2;
					if(i%2==0){
						f = true;
					}else{
						f = false;
					}
				}
			}
			if(f){
				vstr.push_back(tstr.substr(0,DYJC));
				tstr = tstr.substr(DYJC,tstr.length());
			}
			else{
				vstr.push_back(tstr.substr(0,DYJC-1));
				tstr = tstr.substr(DYJC-1,tstr.length());
			}
		}
	}
	return vstr;
}


list<string> splitPrintv( string& tstr, int DYJC ){
	list<string> vstr;
	while(true){
		if(tstr.length()<=DYJC){
			vstr.push_back(tstr);
			break;
		}
		bool f = false;
		for( int i = 0; i < DYJC;){
			char m = tstr.at(i);
			if( unsigned(m) < 0x80 ){//汉字第一个字节在）0x81-0xFE之间 ，
				f = true;
				i++;
			}
			else{
				i += 2;
				if(i%2==0){
					f = true;
				}else{
					f = false;
				}
			}
		}
		if(f){
			vstr.push_back(tstr.substr(0,DYJC));
			tstr = tstr.substr(DYJC,tstr.length());
		}
		else{
			vstr.push_back(tstr.substr(0,DYJC-1));
			tstr = tstr.substr(DYJC-1,tstr.length());
		}
	}
	return vstr;
}

char* Gb2312ToUtf8(const char *pcGb2312) /*const*/
{
	int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, NULL, 0);

	wchar_t * pcUnicode = new wchar_t[nUnicodeLen+1]; 
	memset(pcUnicode, 0, nUnicodeLen * 2 + 2); 

	MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, pcUnicode, nUnicodeLen);

	int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, NULL, 0, NULL, NULL);

	char *pcUtf8=new char[nUtf8Len + 1]; 
	memset(pcUtf8, 0, nUtf8Len + 1);   

	WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, pcUtf8, nUtf8Len, NULL, NULL);

	delete[] pcUnicode; 
	return pcUtf8;
}
void delchar(char * str,char CharNum){
	char *d = str;
	while (*str != '\0') {
			
		if (*str != CharNum) {
			*d++ = *str;
		}
			
		++str;
		
	}
	*d = '\0';
	
}