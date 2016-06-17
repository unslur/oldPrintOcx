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
#define NTYPE_DVS_ITEM       4  //特殊的ITEM,兼有目录属性
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

//计划定义
#define  SCDU_TYPE_ONCE         0   //一次性
#define  SCDU_TYPE_EVERYDAY     1   //每天
#define  SCDU_TYPE_WEEKLY       2   //每周


//周几的掩码
#define  SUNDAY_MASK    0x01
#define  MONDAY_MASK    0x02
#define  TUESEDAY_MASK  0x04
#define  WEDNESDAY_MASK 0x08
#define  THURSDAY_MASK  0x10
#define  FRIDAY_MASK    0x20
#define  SATURDAY_MASK  0x40

#define  GETDAY(val,mask)   ((val & mask) ? TRUE : FALSE)


//计划的执行功能定义
#define  SCDU_ACT_UNKNOWN  0  //未知功能，用于获取所有的计划
#define  SCDU_ACT_RECORD   1  //执行录像

#define  MAX_SCHEDULES     8

typedef struct tagSchedule
{
    WORD  nType;        //0--一次性，1--每天，2--每周
    WORD  nAction;      //0--一录像，1,2,3-……预留后用
    DWORD dwWeekDate;   //当nType为2(即每周时)，从最低位开始，每位代表一天即1,2,4,8,16,32,64分别代表周一到周日
                        //当nType为0(即一次性)，则表示指定的日期，其格式为year*10000+month*100+date
    DWORD dwBeginTime;  //开始时间，凌晨0点开始的秒数
    DWORD dwEndTime;    //结束时间，格式同上
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
        strcpy(szName, "新计划");
    }
    
}SCHEDULE;

//typedef CTypedPtrArray<CPtrArray, SCHEDULE*>  CScheduleArray;

time_t  MyGetLocalTime();
time_t  GetScheduleBeginTime(SCHEDULE *pScedu);
time_t  GetScheduleEndTime(SCHEDULE *pScedu);

BOOL  SetControlTime(CDateTimeCtrl& timeCtrl, DWORD dwSecondTime); //dwSecondTime: 凌晨0点开始的秒数
DWORD GetControlTime(CDateTimeCtrl& timeCtrl); //返回: 凌晨0点开始的秒数

//日志信息定义
#define  INFO_TYPE_ALARM        1   //报警
#define  INFO_TYPE_USER         2   //用户
#define  INFO_TYPE_SERVER       3   //服务器
#define  INFO_TYPE_DEVICE       4   //设备状态

typedef struct tagLogInfo
{
    DWORD   nType;          //类型，报警、用户、服务器、设备状态
    DWORD   nIndex;         //序号
    DWORD   dwDate;         //日期，格式为year*10000+month*100+date　　
    DWORD   dwTime;         //时间，格式为hour*10000+minute*100+seconds
    char    szDevName[64];  //发生报警的设备名称
    char    szContent[128]; //描述
    char    *pszRecFile;    //录制文件，针对报警录像

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

#define   DRAG_DELAY   60  //ms, 用于处理无意拖曳

//void AppendLine2LogFile(const char *szFormat, ... );

#endif // __GLOBAL_H__