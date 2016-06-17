#ifndef __GLOBAL_H__
#define __GLOBAL_H__


typedef enum
{
    vmode_1_win=1,
    vmode_4_win=4,
    vmode_6_win=6,
    vmode_8_win=8,
    vmode_9_win=9,
    vmode_16_win=16,
    vmode_25_win=25,
    vmode_36_win=36,
} view_mode_t;

typedef enum
{
    ptz_left=0,
    ptz_up=1,
    ptz_right=2,
    ptz_down=3,
} ptz_cmd_t;


// Node type definition
#define NTYPE_ROOT           0
#define NTYPE_CAM_DIR        2
#define NTYPE_CAM_ITEM       3
#define NTYPE_DVS_ITEM       4  //�����ITEM,����Ŀ¼����
#define NTYPE_EMAP_DIR       5
#define NTYPE_EMAP_ITEM      6

#define TIMAGE_ROOT           0
#define TIMAGE_CAM_DIR        1 
#define TIMAGE_CAM_ITEM       2
#define TIMAGE_CAM_DISABLED   3
#define TIMAGE_DVS            4
#define TIMAGE_DVS_DISABLED   5
#define TIMAGE_EMAP_DIR       6
#define TIMAGE_EMAP_ITEM      7
#define TIMAGE_EMAP_DISABLED  8

#define DOC_VERSION   110

//�ƻ�����
#define  SCDU_TYPE_ONCE         0   //һ����
#define  SCDU_TYPE_EVERYDAY     1   //ÿ��
#define  SCDU_TYPE_WEEKLY       2   //ÿ��


//�ܼ�������
#define  SUNDAY_MASK    0x01
#define  MONDAY_MASK    0x02
#define  TUESEDAY_MASK  0x04
#define  WEDNESDAY_MASK 0x08
#define  THURSDAY_MASK  0x10
#define  FRIDAY_MASK    0x20
#define  SATURDAY_MASK  0x40

#define  GETDAY(val,mask)   ((val & mask) ? TRUE : FALSE)


//�ƻ���ִ�й��ܶ���
#define  SCDU_ACT_UNKNOWN  0  //δ֪���ܣ����ڻ�ȡ���еļƻ�
#define  SCDU_ACT_RECORD   1  //ִ��¼��

#define  MAX_SCHEDULES     8

typedef struct tagSchedule
{
    WORD  nType;        //0--һ���ԣ�1--ÿ�죬2--ÿ��
    WORD  nAction;      //0--һ¼��1,2,3-����Ԥ������
    DWORD dwWeekDate;   //��nTypeΪ2(��ÿ��ʱ)�������λ��ʼ��ÿλ����һ�켴1,2,4,8,16,32,64�ֱ������һ������
                        //��nTypeΪ0(��һ����)�����ʾָ�������ڣ����ʽΪyear*10000+month*100+date
    DWORD dwBeginTime;  //��ʼʱ�䣬�賿0�㿪ʼ������
    DWORD dwEndTime;    //����ʱ�䣬��ʽͬ��
    char  szName[64];
    
    tagSchedule()
    {
        nType = 0;
        nAction = SCDU_ACT_RECORD;
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        dwWeekDate = sysTime.wYear * 10000 + sysTime.wMonth*100 + sysTime.wDay;
        dwBeginTime = 0;
        dwEndTime = 24*3600 - 1;
        strcpy(szName, "�¼ƻ�");
    }
    
}SCHEDULE;

//typedef CTypedPtrArray<CPtrArray, SCHEDULE*>  CScheduleArray;

time_t  MyGetLocalTime();
time_t  GetScheduleBeginTime(SCHEDULE *pScedu);
time_t  GetScheduleEndTime(SCHEDULE *pScedu);

BOOL  SetControlTime(CDateTimeCtrl& timeCtrl, DWORD dwSecondTime); //dwSecondTime: �賿0�㿪ʼ������
DWORD GetControlTime(CDateTimeCtrl& timeCtrl); //����: �賿0�㿪ʼ������

//��־��Ϣ����
#define  INFO_TYPE_ALARM        1   //����
#define  INFO_TYPE_USER         2   //�û�
#define  INFO_TYPE_SERVER       3   //������
#define  INFO_TYPE_DEVICE       4   //�豸״̬

typedef struct tagLogInfo
{
    DWORD   nType;          //���ͣ��������û������������豸״̬
    DWORD   nIndex;         //���
    DWORD   dwDate;         //���ڣ���ʽΪyear*10000+month*100+date����
    DWORD   dwTime;         //ʱ�䣬��ʽΪhour*10000+minute*100+seconds
    char    szDevName[64];  //�����������豸����
    char    szContent[128]; //����
    char    *pszRecFile;    //¼���ļ�����Ա���¼��

    tagLogInfo()
    {
        nType = 0;
        nIndex = 0;
        dwDate = 0;
        dwTime = 0;
        memset(szDevName, 0, sizeof(szDevName));
        memset(szContent, 0, sizeof(dwTime));
        pszRecFile = NULL;
    }
    
}LOG_INFO;

/*
#include "Node.h"
#include "FolderNode.h"
#include "CamNode.h"
#include "DvsNode.h"
#include "EMapNode.h"

CNode* LoadNodeFromStorage(LPSTORAGE lpStorage, CNode *pParentNode, WORD wVersion);
CNode* CloneNode(CNode *pSrcNode);
*/
#define URL_FORMAT   ("ltsp://%s:%d/%d.sdp")

typedef struct tagMdRgn
{
	int     nIndex;
	RECT    rcRgn;
}MD_RGN;

#define   MAX_MD_RGNS   6

#define   DRAG_DELAY   60  //ms, ���ڴ���������ҷ

//void AppendLine2LogFile(const char *szFormat, ... );

#endif // __GLOBAL_H__