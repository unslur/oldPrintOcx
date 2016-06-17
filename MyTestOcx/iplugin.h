#ifndef PLUGIN_INTF_H
#define PLUGIN_INTF_H

/*IPlugin.h*/

#include "portable.h"

#ifdef __cplusplus
extern "C"{
#endif

/*identify media result for demux*/
typedef enum IDENTIFY_RESULT
{
	IDENTIFY_REJECTED	= 0,/*the media is unacceptable*/
	IDENTIFY_ACCEPTED	= 1,/*the media is acceptable*/
	IDENTIFY_NEEDDATA	= 2,/*I can't sure up to now, actual media data needed*/
	IDENTIFY_NEEDFILE	= 3,/*I can't sure up to now, whole media file needed*/
}IDENTIFY_RESULT;

/*media sample*/
struct tagSAMPLE;
typedef struct tagSAMPLE *HSAMPLE;

typedef struct tagSAMPLE
{
	void (STDCALL *ReleaseProc)(HSAMPLE);/*the only parameter is the sample itself*/
	uint32_t	total;
	uint32_t	used;
	uint8_t		*data[4];
	uint32_t	flags;/*attributes for the sample*/
	uint32_t	timestamp; /* unit: millisecond */
}SAMPLE;

/*
flags: 低8位(bit 0-7)即第一个字节， 表示类型，
用文本字符(注意是小写)的方式表示数据类型，'v'-Video,'a'-Audio,'t'-Text(Subtile),'u'-UserData(如报警数据等)
*/

#define SAMPLE_SET_TYPE(h, t) h->flags = (h->flags & 0xffffff00) | (t & 0xff)
#define SAMPLE_GET_TYPE(h)  (h->flags & 0xff)

/* 第二个字节(从低至高)，表示数据的子类型，如视频的I,P,B帧等, 对于音频和其它类型的数据，暂时未用 */
#define SAMPLE_I_FRAME	0x0100
#define SAMPLE_P_FRAME	0x0200
#define SAMPLE_B_FRAME  0x0300
#define SAMPLE_O_FRAME  0x0400   // other unknown video frame type

#define V_SET_FRAMETYPE(h, t)  h->flags = (h->flags & 0xffff00ff) | (t)
#define V_GET_FRAMETYPE(h)     (h->flags & 0xff00)

/* 第三个字节(从低至高)，表示该SAMPLE的引用计数，当其为0时即可销毁(释放相关内存) */
#define SET_REFERENCE(h, r)  h->flags = (h->flags & 0xff00ffff) | ((r & 0xff) << 16)
#define GET_REFERENCE(h)     ((h->flags >> 16) & 0xff)
#define INCREASE_REFERENCE(h)  h->flags = (h->flags & 0xff00ffff) | ((((h->flags >> 16) & 0xff)+1) << 16)
#define DECREASE_REFERENCE(h)  h->flags = (h->flags & 0xff00ffff) | ((((h->flags >> 16) & 0xff)-1) << 16)


/* 最高字节用来表示一些特性，如最高位表示本Sample跟前面的Sample是否是断码的 */
#define SAMPLE_INCONSECUTIVE  0x80000000   // 表示是跟前面的samples不连续的，即断码sample，
                                           // 用于rtp丢包而引起播放器收到Seq不连续的数据
#define SAMPLE_INCONSECUTIVE_BYTE  0x80


typedef HSAMPLE (STDCALL *NewSampleProc)(void *hInst, int32_t nBytes);

typedef int32_t (STDCALL *PushFrameProc)(HPLAYER hPlayer, HPLUGIN hPlugin, HSAMPLE hSample, int32_t nType);
//通常返回1，数据错误返回-1，缓冲已满(表示喂养者要暂停喂养)则返回0

#define  CALL_REASON_REGISTER         1
#define  CALL_REASON_UNREGISTER       2
#define  CALL_REASON_GETSPECIAL       3   // get module special interface

typedef int32_t (STDCALL *PluginMainProc)(int32_t nReason, uint32_t nContext, uint32_t nContext2, void* lpvReserved);

/*prototype for create plugin instance*/
typedef HPLUGIN (STDCALL *CreateIntfProc)(HPLAYER hPlayer, NewSampleProc pfnNewSample);

/*prototype for identify media type
	nType: 0 - URL(int8_t *), 1 - FILE *, 2 - data block(nLength is meaningfull now), 3 - file handle(int32_t),  4 - WIN32 file HANDLE
	nLength: only if nType==2
*/
typedef IDENTIFY_RESULT (STDCALL *IdentifyProc)(int32_t nType, const void *pvMedia, int32_t nLength);

/*plugin exported structure*/
typedef struct tagPLUGIN_INTF
{
	int32_t			nSignature;			/*'MODL'*/
	PLUGIN_CATEGORY nCategory;			/*plugin category*/
	const int8_t	*szName;			/*plugin name*/
	int32_t			nVersion;			/*plugin version*/
    int32_t         nPriority;          /*plugin priority, 0~31, default: 8*/
	const void		*pvFnIntf;			/*functional interface*/
	IdentifyProc	pfnIdentifyProc;	/*procedure for identify media type(demux plugin only)*/
	CreateIntfProc	pfnCreateProc;		/*procedure for create plugin interface*/
	int32_t			nNumFormats;		/*all known formats, (for CODEC and RENDERER)*/
	const AV_FMT	*pMediaFmts;		/*acceptable media formats if any*/
	/*for demux: all media types is output
	for codec:	media types are both input and output, the first is for input, the next is for output, and so on
	for renderer: all media types are input
	*/
}PLUGIN_INTF;

/*=====================player interface for plugins==========================*/
/*整个结构是：
                                 /  first list - video process plugins
root - demu/input -- +
                                 \  second/last list - audio process plugins
*/

/*iterate plugins
	hPlayer		- the player instance
	hFrom		- related to this plugin, refer to notes
	bForward	- TRUE - get next plugin in the same pipeline, FALSE - get the prev plugin in the same pipeline
Note:
	two special scene:
	HPLUGIN hDemux = PlayerGetPlugin(hPlayer, NULL, TRUE);
	HPLUGIN hRenderer = PlayerGetPlugin(hPlayer, NULL, FALSE);
*/
HPLUGIN STDCALL PlayerGetPlugin(HPLAYER hPlayer, HPLUGIN hFrom, bool_t bForward);

/*register non-auto-find plugins, i.e. the plugin isn't in the "plugin" folder
	hPlayer		- the player instance
	pIntf		- plugin for register
*/
bool_t STDCALL PlayerRegisterPlugin(HPLAYER hPlayer, PLUGIN_INTF *pIntf);
/*=====================player interface for plugins==========================*/

/*=====================general plugin interface==========================*/
/*release plugin instance*/
void STDCALL PluginRelease(HPLUGIN hPlugin);

/*get extension interface exported by the plugin*/
const void* STDCALL PluginQueryIntf(HPLUGIN hPlugin, const int8_t *szIntfName);

/*get format used in streaming
	nStream:	>=0 CONTENT_TYPE, <0 negative stream index(-1 stand for stream 0, -2 for stream 1, and so on)
*/
AV_FMT* STDCALL PluginGetStreamFormat(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);

/*functional interface*/
typedef struct tagIntfPlugin
{
	void (STDCALL *ReleaseProc)(HPLUGIN hPlugin);

	const void* (STDCALL *QueryIntfProc)(HPLUGIN hPlugin, const int8_t *szIntfName);

	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);

}IntfPlugin;
/*=====================general plugin interface==========================*/

/*=====================source/demux plugin interface==========================*/
/*handle of source plugin*/
typedef void *HSOURCE;

/*open source media*/
bool_t STDCALL SourceOpen(HSOURCE hSource, const int8_t *szUrl, PushFrameProc pfnPushFrame);

void STDCALL SourceClose(HSOURCE hSource);

void STDCALL SourceSetNotify(HSOURCE hSource, NOTIFYPROC pfnNotify);

/*get input position, in milliseconds*/
uint32_t STDCALL SourceGetCurPos(HSOURCE hSource);

/*set input position, in milliseconds*/
bool_t STDCALL SourceSetCurPos(HSOURCE hSource, uint32_t nPosInMs, uint32_t* pnResultInMs);

/*get media duration, in milliseconds
	hSource		- the source plugin instance
	bActual		- TRUE - return actual media length, FALSE - return selected media length
*/
uint32_t STDCALL SourceGetDuration(HSOURCE hSource, bool_t bActual);

/*is the source accept seek command?*/
bool_t STDCALL SourceIsSeekable(HSOURCE hSource);

/*get stream count*/
int32_t STDCALL SourceGetStreamCount(HSOURCE hSource);

/*fetch sample*/
HSAMPLE STDCALL SourcePullSample(HSOURCE hSource, int32_t nMoveKeyFrames, int32_t nStream, bool_t *pbEndOfStream);

/*run source*/
bool_t STDCALL SourceRun(HSOURCE hSource);

/*pause source*/
bool_t STDCALL SourcePause(HSOURCE hSource);

/*stop source*/
bool_t STDCALL SourceStop(HSOURCE hSource);

/*Action control*/
int32_t STDCALL SourceExtProc(HSOURCE hSource, int32_t nType, uint32_t nParam1, uint32_t nParam2);

/*source/demux functional interface*/
typedef struct tagIntfSource
{
	void (STDCALL *ReleaseProc)(HSOURCE hSource);
	
	const void* (STDCALL *QueryIntfProc)(HSOURCE hSource, const int8_t *szIntfName);
	
	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);

	bool_t (STDCALL *OpenProc)(HSOURCE hSource, const int8_t *szUrl, PushFrameProc pfnPushFrame); //PushFrameProc = NULL 可在SourceExtProc中设置

	void (STDCALL *CloseProc)(HSOURCE hSource);
	
	void (STDCALL *SetNotifyProc)(HSOURCE hSource, NOTIFYPROC pfnNotify);

	uint32_t (STDCALL *GetCurPosProc)(HSOURCE hSource);

	bool_t (STDCALL *SetCurPosProc)(HSOURCE hSource, uint32_t nPosInMs, uint32_t* pnResultInMs);

	uint32_t (STDCALL *GetDurationProc)(HSOURCE hSource, bool_t bActual);

	bool_t (STDCALL *IsSeekableProc)(HSOURCE hSource);

	int32_t (STDCALL *GetStreamCountProc)(HSOURCE hSource);

	HSAMPLE (STDCALL *PullSampleProc)(HSOURCE hSource, int32_t nMoveKeyFrames, int32_t nStream, bool_t *pbEndOfStream);

	bool_t (STDCALL *RunProc)(HSOURCE hSource);

	bool_t (STDCALL *PauseProc)(HSOURCE hSource);

	bool_t (STDCALL *StopProc)(HSOURCE hSource);

    int32_t (STDCALL *SourceExtProc)(HSOURCE hSource, int32_t nType, uint32_t nParam1, uint32_t nParam2); //
}IntfSource;


/*Note

  1) some transfer source may be non-seek-able, e.g. HTTP/FTP, they know nothing about media time
  2) some transfer source may redirect the seek command to the server

*/

/*=====================source/demux plugin interface==========================*/

/*=====================CODEC plugin==========================*/
typedef void *HCODEC;

/*initialize with optional parameters*/
bool_t STDCALL CodecInit(HCODEC hCodec, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);

/*handle sample
Note:
	hOutSample is optional, when NULL, the CODEC should call PlayerNewSample() to allocate sample
*/
HSAMPLE STDCALL CodecHandleSample(HCODEC hCodec, HSAMPLE hSample);

/*functional interface*/
typedef struct tagIntfCodec
{
	void (STDCALL *ReleaseProc)(HCODEC hCodec);
	
	const void* (STDCALL *QueryIntfProc)(HCODEC hCodec, int8_t *szIntfName);

	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);

	bool_t (STDCALL *InitProc)(HCODEC hCodec, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);
	
	HSAMPLE (STDCALL *HandleSampleProc)(HCODEC hCodec, HSAMPLE hSample);
}IntfCodec;

/*all typical CODECs only support some media formats statically.
if not, call QueryIntfProc(hCodec,_T("IFormatFetcher"))
*/

/*=====================CODEC plugin==========================*/

/*=====================effect enhancement plugin==========================*/
typedef void *HEFFECT;

/*initialize with optional parameters*/
bool_t STDCALL EffectInit(HEFFECT hEffect, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);

/*handle sample
Note:
	hOutSample is optional, when NULL, the CODEC should call PlayerNewSample() to allocate sample
*/
HSAMPLE STDCALL EffectHandleSample(HEFFECT hEffect, HSAMPLE hSample);

/*functional interface*/
typedef struct tagIntfEffect
{
	void (STDCALL *ReleaseProc)(HEFFECT hEffect);
	
	const void* (STDCALL *QueryIntfProc)(HEFFECT hEffect, int8_t *szIntfName);
	
	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);
	
	bool_t (STDCALL *InitProc)(HEFFECT hEffect, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);
	
	HSAMPLE (STDCALL *HandleSampleProc)(HEFFECT hEffect, HSAMPLE hSample);
	
}IntfEffect;
/*=====================effect enhancement plugin==========================*/

/*=====================transform plugin==========================*/
typedef void *HTRANS;

/*initialize with optional parameters*/
bool_t STDCALL TransInit(HTRANS hTrans, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);

/*handle sample
Note:
	hOutSample is optional, when NULL, the CODEC should call PlayerNewSample() to allocate sample
*/
HSAMPLE STDCALL TransHandleSample(HTRANS hTrans, HSAMPLE hSample);

/*functional interface*/
typedef struct tagIntfTrans
{
	void (STDCALL *ReleaseProc)(HTRANS hTrans);
	
	const void* (STDCALL *QueryIntfProc)(HTRANS hTrans, const int8_t *szIntfName);
	
	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);
	
	bool_t (STDCALL *InitProc)(HTRANS hTrans, AV_FMT *pInFmt, AV_FMT *pOutFmt, int8_t *szConfigText);

	HSAMPLE (STDCALL *HandleSampleProc)(HTRANS hTrans, HSAMPLE hSample);
	
}IntfTrans;
/*=====================transform plugin==========================*/

/*=====================renderer plugin==========================*/
typedef void *HRENDERER;

/*initialize with optional parameters*/
bool_t STDCALL RendererInit(HRENDERER hRenderer, AV_FMT *pFmt, const int8_t *szConfigText);

/*render sample*/
HSAMPLE STDCALL RendererHandleSample(HRENDERER hRenderer, HSAMPLE hSample);

/*update final input media format*/
bool_t STDCALL RendererUpdateInFormat(HRENDERER hRenderer, AV_FMT *pFmt);

/*functional interface*/
typedef struct tagIntfRenderer
{
	void (STDCALL *ReleaseProc)(HRENDERER hRenderer);
	
	const void* (STDCALL *QueryIntfProc)(HRENDERER hRenderer, int8_t *szIntfName);
	
	AV_FMT* (STDCALL *GetStreamFormatProc)(HPLUGIN hPlugin, int32_t nStream, bool_t bInput);
	
	bool_t (STDCALL *InitProc)(HRENDERER hRenderer, AV_FMT *pFmt, const int8_t *szConfigText);

	HSAMPLE (STDCALL *HandleSampleProc)(HRENDERER hCodec, HSAMPLE hSample);

	bool_t (STDCALL *UpdateInFormatProc)(HRENDERER hRenderer, AV_FMT *pFmt);
}IntfRenderer;
/*=====================renderer plugin==========================*/

/*=====================video renderer interface==========================*/
bool_t STDCALL RendererSetHandle(HRENDERER hRenderer, void *pvWindow);
bool_t STDCALL RendererRepos(HRENDERER hRenderer, int32_t x, int32_t y);
bool_t STDCALL RendererReshape(HRENDERER hRenderer, int32_t w, int32_t h);
bool_t STDCALL RendererDiscard(HRENDERER hRenderer, bool_t bDiscard);
bool_t STDCALL RendererRotate(HRENDERER hRenderer, int32_t nAngle);
bool_t STDCALL RendererCaptureFrame(HRENDERER hRenderer, const int8_t* szSaveFileName, int32_t nFormat); //0--bmp, 1--jpg, 2--yuv

/* see iplayer.h
typedef struct tagDrawRegion
{
    int16_t    nIndex;
    uint16_t   nThickness;
    int32_t    left;
    int32_t    top;
    int32_t    right;
    int32_t    bottom;
    uint32_t   nRgbColor;
} DRAW_REGION;
*/

bool_t STDCALL RendererSetDrawRegion(HRENDERER hRenderer, int32_t nNumRegions, DRAW_REGION* pAryRetions);
bool_t STDCALL RendererSetDrawTimestamp(HRENDERER hRenderer, bool_t bEnable, uint32_t nRgbColor, uint32_t nBaseTime);

// nSide: 1--left, 2--top, 3--right, 4--bottom
bool_t STDCALL RendererSetCutPixels(HRENDERER hRenderer, int32_t nSide, uint32_t nCutLines);
bool_t STDCALL FillFrameBuffer(HRENDERER hRenderer, uint32_t nRGBColor);

typedef struct tagIntfVideoRenderer
{
	bool_t (STDCALL *SetHandleProc)(HRENDERER hRenderer, void *pvWindow);
	bool_t (STDCALL *ReposProc)(HRENDERER hRenderer, int32_t x, int32_t y);
	bool_t (STDCALL *ReshapeProc)(HRENDERER hRenderer, int32_t w, int32_t h);
	bool_t (STDCALL *DiscardProc)(HRENDERER hRenderer, bool_t bDiscard);
    bool_t (STDCALL *RotateProc)(HRENDERER hRenderer, int32_t nAngle);
    bool_t (STDCALL *CaptureFrameProc)(HRENDERER hRenderer, const int8_t* szSaveFileName, int32_t nFormat);
    bool_t (STDCALL *SetDrawRegionProc)(HRENDERER hRenderer, int32_t nNumRegions, DRAW_REGION* pAryRetions);
    bool_t (STDCALL *SetDrawTimestampProc)(HRENDERER hRenderer, bool_t bEnable, uint32_t nRgbColor, uint32_t nBaseTime);
    bool_t (STDCALL *SetCutPixelsProc)(HRENDERER hRenderer, int32_t nSide, uint32_t nCutLines);
    bool_t (STDCALL *FillFrameBufferProc)(HRENDERER hRenderer, uint32_t nRGBColor);
}IntfVideoRenderer;
/*=====================video renderer interface==========================*/

/*=====================audio renderer interface==========================*/
bool_t STDCALL RendererPause(HRENDERER hRenderer);
bool_t STDCALL RendererResume(HRENDERER hRenderer);
/*play pos = play offset + time base*/
uint32_t STDCALL RendererGetPos(HRENDERER hRenderer, uint32_t *pnPendingPackets);
void STDCALL RendererSetTimeBase(HRENDERER hRenderer, uint32_t nTimeBase, bool_t bReset);
uint32_t STDCALL RendererGetVolume(HRENDERER hRenderer);
bool_t STDCALL RendererSetVolume(HRENDERER hRenderer, uint32_t nVolume);
uint32_t STDCALL RenderGetRate(HRENDERER hRenderer);
bool_t STDCALL RendererSetRate(HRENDERER hRenderer, uint32_t nRate);

typedef struct tagIntfAudioRenderer
{
	bool_t (STDCALL *PauseProc)(HRENDERER hRenderer);
	bool_t (STDCALL *ResumeProc)(HRENDERER hRenderer);
	uint32_t (STDCALL *GetPosProc)(HRENDERER hRenderer, uint32_t *pnPendingAudio);
	void (STDCALL *SetTimeBaseProc)(HRENDERER hRenderer, uint32_t nTimeBase, bool_t bReset);
	uint32_t (STDCALL *GetVolumeProc)(HRENDERER hRenderer);
	bool_t (STDCALL *SetVolumeProc)(HRENDERER hRenderer, uint32_t nVolume);
	uint32_t (STDCALL *GetRateProc)(HRENDERER hRenderer);
	bool_t (STDCALL *SetRateProc)(HRENDERER hRenderer, uint32_t nRate);
}IntfAudioRenderer;
/*=====================audio renderer interface==========================*/

#ifdef __cplusplus
}
#endif

#endif//PLUGIN_INTF_H
