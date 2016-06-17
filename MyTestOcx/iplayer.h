#ifndef PLAYER_INTF_H
#define PLAYER_INTF_H

/*IPlayer.h*/

#include "portable.h"
#ifdef __cplusplus
extern "C"{
#endif

typedef void *HPLAYER;

/*create a player instance
	szConfigFile - optional global settings for the player instance
	pvParentWnd	 - initial parent window handle
	x,y			 - initial position in client or screen coordinate
*/
HPLAYER STDCALL PlayerCreate(const int8_t *szConfigFile, void * pvParentWnd, int32_t x, int32_t y);

/*destroy a player instance*/
void STDCALL PlayerRelease(HPLAYER hPlayer);

/*prototype for the client to receive notify from the player
	pvContext	- an arbitrary context for the client
	nMsgID		- notify type indicator, see playmsg.h
	szDescription - extra description, it's optional
*/
typedef void (STDCALL *NOTIFYPROC)( void *pvContext, uint32_t nMsgID, const int8_t* szDescription);

/*install a callback for notify
	hPlayer		- the player instance
	pfnNotify	- the callback procedure
	pvContext	- arbitrary context for the client
Note:
	possible events are: search/opening/opened media, start buffering, start playing, paused, ..
*/
void STDCALL PlayerSetNotify(HPLAYER hPlayer, NOTIFYPROC pfnNotify, void *pvContext);

/*specify video output position
	hPlayer		- the player instance
	pvParentWnd	- parent window handle if non-NULL
	x,y			- coordinate in screen or parent window
*/
bool_t STDCALL PlayerSetVideoPos(HPLAYER hPlayer, void * pvParentWnd, int32_t x, int32_t y);

/*open media file
	hPlayer		- the player instance
	szUrl		- local file, network file, live stream, ...
*/
bool_t STDCALL PlayerOpen(HPLAYER hPlayer, const int8_t *szUrl);

/*turn into play state
	hPlayer		- the player instance
Note:
	Init -> Buffering <-> Paused <-> playing -> stopped
	according to the state transform diagram (STD), only paused state can be changed into play state directly
*/
bool_t STDCALL PlayerPlay(HPLAYER hPlayer);

/*turn into paused state
	hPlayer		- the player instance
Note:
	Init -> Buffering <-> Paused <-> playing -> stopped
	according to the state transform diagram (STD), only buffering/paying state can be changed into paused state directly
*/
bool_t STDCALL PlayerPause(HPLAYER hPlayer);

/*turn into stopped state
	hPlayer		- the player instance
Note:
	Init -> Buffering <-> Paused <-> playing -> stopped
	according to the state transform diagram (STD), only buffering/paying/paused state can be changed into paused state directly
*/
bool_t STDCALL PlayerStop(HPLAYER hPlayer);

/*turn into stopped state
	hPlayer		- the player instance
	nPosInMs	- media position in milliseconds
Note:
	the player will turn into paused state if in either buffering or playing state
*/
bool_t STDCALL PlayerSeek(HPLAYER hPlayer, uint32_t nPosInMs);

/*get current play position
	hPlayer		- the player instance
Note:
	return value is media position in milliseconds
*/
uint32_t STDCALL PlayerGetCurPos(HPLAYER hPlayer);

/*player state*/
typedef enum enPLAYER_STATE
{
	STATE_READY	= 0,	    /*not any file opened*/
	STATE_STOPPED,          /*have ever opened some file successfully*/
	STATE_OPENING,			/*opening file, i.e. connecting to server for the file*/
	STATE_PAUSED,			/*ready for play*/
	STATE_BUFFERING,		/*buffering data, it's not ready for play*/
	STATE_PLAYING,			/*playing*/
}PLAYER_STATE;

/*get playing state
	hPlayer		- the player instance
*/
PLAYER_STATE STDCALL PlayerGetState(HPLAYER hPlayer);

/*change play speed
	hPlayer			- the player instance
	nNumerator
	nDenominator	- nNumerator/nDenominator is the speed
*/
bool_t STDCALL PlayerSetSpeed(HPLAYER hPlayer, uint32_t nNumerator, uint32_t nDenominator);

/*get play speed
	hPlayer		- the player instance
*/
void STDCALL PlayerGetSpeed(HPLAYER hPlayer, uint32_t *pnNumerator, uint32_t *pnDenominator);

/*get the count of audio track
	hPlayer		- the player instance
*/
int32_t STDCALL PlayerGetAudioTrackCount(HPLAYER hPlayer);

/*change the audio track being played
	hPlayer		- the player instance
	nTrack		- zero-indexed audio track identify
Note:
	the first audio track is always selected by default if you do not specify other else.
*/
bool_t STDCALL PlayerChooseAudioTrack(HPLAYER hPlayer, int32_t nTrack);

/*get the audio track being played
	hPlayer		- the player instance
Note:
	it's zero-indexed
*/
int32_t STDCALL PlayerGetCurAudioTrack(HPLAYER hPlayer);

/*get the audio channel being played
	hPlayer		- the player instance
Note:
	refer to the comment of PlayerChooseAudioChannel()
*/
int32_t STDCALL PlayerGetCurAudioChannel(HPLAYER hPlayer);

/*change the audio channel being played
	hPlayer		- the player instance
	nChannel	- -1: reverse left and right channel, 0 - original stereo channels, 1 - left channel,
				  2 - right channel
Note:
	1) original stereo channels are used by default
	2) no harm to call it if no audio or the audio is mono
*/
bool_t STDCALL PlayerChooseAudioChannel(HPLAYER hPlayer, int32_t nChannel);

/*get the volume of audio
	hPlayer		- the player instance
	bAudioGain	- FALSE - playback/sound-card volume, TRUE - audio gain volume on data
Note:
	1) playback volume is in range of [0,100], stand for silent to full loud
	2) playback volume is present as left in low 16-bit, right in high 16-bit
	3) audio gain volume is in range of [-200,+200], stand for -20.0dB to +20.0dB
	4) relative loudness is expressed in:

		Ratio = 10 ^ (Gain / 20.0)

		Ratio is in [1/10, 10], Gain is in [-20.0, +20.0]
*/
uint32_t STDCALL PlayerGetVolume(HPLAYER hPlayer);

bool_t STDCALL PlayerSetGain(HPLAYER hPlayer, int32_t nGain);

/*set the volume for audio
	hPlayer		- the player instance
	bAudioGain	- FALSE - playback/sound-card volume, TRUE - audio gain volume on data
	nVolume		- volume. for playback is in range of [0,100], for audio gain is in range of [-200, +200]
Note:
	refer to PlayerGetVolume()
*/
bool_t STDCALL PlayerSetVolume(HPLAYER hPlayer, uint32_t nVolume);

/*get the state of audio mute
	hPlayer		- the player instance
*/
bool_t STDCALL PlayerGetMute(HPLAYER hPlayer);

/*set state of audio mute
	hPlayer		- the player instance
	bMute		- TRUE - mute, FALSE - non-muted
Note:
	mute audio don't change the volume of audio, or record it down, and restore when cancel mute
*/
bool_t STDCALL PlayerSetMute(HPLAYER hPlayer, bool_t bMute);

/*get the degree of rotate
	hPlayer		- the player instance
Note:
	the return value is either 0, 90, 180 or 270
*/
int32_t STDCALL PlayerGetVideoRotateAngle(HPLAYER hPlayer);

/*rotate the video in 0, 90, 180 or 270 degrees
	hPlayer		- the player instance
	nAngle		- angle in degree, up to now ONLY support 90
Note:
	by default, it's 0 degree(no rotate)
*/
bool_t STDCALL PlayerRotateVideo(HPLAYER hPlayer, int32_t nAngle);

/*get video zoom rate
	hPlayer		- the player instance
Note:
	the return value is in unit of 1/100, i.e. 200 if double-sized
*/
int32_t STDCALL PlayerGetVideoZoomRate(HPLAYER hPlayer);

/*set video zoom rate
	hPlayer		- the player instance
	nRate		- zoom rate in unit of 1/100, i.e. 100 for normal size, 200 for double size
                  range: 0==arbitrary, that is fit to window size; OR 25~800, that is 1/4~8x
*/
bool_t STDCALL PlayerZoomVideo(HPLAYER hPlayer, int32_t nRate);

/*resize video output
	hPlayer		- the player instance
	nWidth		- new width in pixels
	nHeight		- new height in pixels
Note:
	1) usually, the width and height should be even value (0,2,4,6,8,...)
	2) if both width and height is zero(0), restore to original size
	3) either width or height is zero(0), resize in the same aspect ratio to original image size
*/
bool_t STDCALL PlayerResizeVideo(HPLAYER hPlayer, int32_t nWidth, int32_t nHeight);

/* get current video show position
	hPlayer		- the player instance
	RECT*		- Pointer to the buffer that will receive the video show position
*/

typedef struct tagVPOS
{
    int32_t  left;
    int32_t  top;
    uint32_t width;
    uint32_t height;
}VPOS;

bool_t STDCALL PlayerGetVideoShowPosition(HPLAYER hPlayer, VPOS* pVposShow);


/*stop rendering video
	hPlayer		- the player instance
    nMediaType  - one of CONTENT_AUDIO, CONTENT_VIDEO, CONTENT_USRDATA
	bDiscard	- TRUE - disable FALSE - re-enable(if present)
*/
bool_t STDCALL PlayerDiscardMedia(HPLAYER hPlayer, int32_t nMediaType, bool_t bDiscard);


/*take a snapshot of current displayed video
	hPlayer		- the player instance
Note:
	1) snapshots are named as vs<nnn>.ext, nnn are 3 numbers, ext is file extension, see below
	1) both format and path for the saved images are configured by PlayerCreate()
	2) yes, we have default settings: .jpg save in
		  Linux: /home/<current user>/my snapshots
		Windows: my documents\my snapshots

    nFormat: 0--bmp, 1--jpg, 2--yuv420
*/
bool_t STDCALL PlayerSnapshotVideo(HPLAYER hPlayer, const int8_t *szFullFileName, int32_t nFormat);

/* Start Media Record */
bool_t STDCALL PlayerRecord(HPLAYER hPlayer, const int8_t *szFullFileName);

/* Start Media Record with multiple file, in specific path,  auto create date sub path */
#define REC_TYPE_SERIAL 1   //'S' Record by timer.
#define REC_TYPE_ALARM  2   //'A' Record raised by motion detection or warning device.
#define REC_TYPE_MANUAL 3   //'M' Record by manual

bool_t STDCALL PlayerRecordEx(HPLAYER hPlayer, const char *pszName, int32_t nEventType, uint32_t nTotalSeconds, uint32_t nFlags);

/* Stop Media Record with specific File Name OR specific type, 两者只取其一，另一个参数必须为空 */
bool_t STDCALL PlayerStopRecord(HPLAYER hPlayer, const int8_t *szFileName, int32_t nEventType);

/*media content type*/
typedef enum enCONTENT_TYPE
{
	CONTENT_INVALID		= 0,	/*invalid type, unknown type*/
	CONTENT_AUDIO		= 1,	/*audio*/
	CONTENT_VIDEO		= 2,	/*video*/
    CONTENT_USRDATA		= 3,	/*user data*/
	CONTENT_SUBTITLE	= 4,	/*text subtitle*/
}CONTENT_TYPE;

/*audio format*/
typedef struct tagAV_FMT
{
	int32_t			nType;			/*always be CONTENT_AUDIO (0x01) or CONTENT_VIDEO (0x02)  */
	uint32_t		nDuration;		/*duration in milliseconds*/
	int8_t			szCodecName[16];/*CODEC name*/

    union 
    {
        struct  // for audio  
        {
            int32_t			nChannels;		/*channel count, 1(mono),2(stereo) or 5*/
            int32_t			nSamplerate;	/*sample rate, 8000, 16000, ..*/
            int32_t			nBitsPerSample;	/*bits per sample, 8, 16, 32, ..*/
        };
        
        struct  // for video
        {
            int32_t			nWidth;			/*width in pixels*/
            int32_t			nHeight;		/*height in pixels*/
            int32_t			nFps;			/*in unit of 1/1000 frame per second (FPS), i.e. 10000 stand for 10fps*/
        };
    };

	void			*pvCodecConfig;	/*extra CODEC config data*/
	uint16_t		uConfigSize;	/*pvCodecConfig size in bytes*/
	uint16_t		uStride;
	uint32_t		uImageSize;

	/*recommendation: allocate the whole block if dynamically allocate it*/
}AV_FMT;

int32_t STDCALL PlayerGetStreamCount(HPLAYER hPlayer);

/*get media formats of source(not renderer!)
	hPlayer		- the player instance
Note:
	return value is either MEDIA_FMT or MEDIA_INFO, depends on MEDIA_FMT.bExtraInfo
*/
AV_FMT* STDCALL PlayerGetStreamFormat(HPLAYER hPlayer, int32_t nStream);

/*statistics information*/
typedef struct tagSTAT_INFO
{
	uint32_t uPlayedAudioFrames;		/*audio frames been played*/
	uint32_t uBufferedAudioFrames;		/*audio frames remain in buffer*/
	uint32_t uAudioAvgBitrate;			/*average bitrate of audio stream (after decode?)*/
    uint32_t uAudioFps;                 /* fps * 100 */
	uint32_t uPlayedVideoFrames;		/*video frames been played*/
	uint32_t uBufferedVideoFrames;		/*video frames remain in buffer*/
	uint32_t uVideoAvgBitrate;			/*average video bitrate of audio stream (after decode?)*/
	uint32_t uVideoDropFrames;			/*video frames been dropped*/
    uint32_t uVideoFps;                 /* fps * 100 */
    uint32_t uBufferTimes;              /* enter buffering state times */
	uint32_t nReceivedPackets;	        /* received packets count */
	uint32_t nTreatedPackets;	        /* treated packets count */
    uint32_t nLostPackets;	            /* lost packets count */
}STAT_INFO;

/*get statistics information
	hPlayer		- the player instance
	pStatInfo	- structure to receive the result
*/
bool_t STDCALL PlayerGetStatistics(HPLAYER hPlayer, STAT_INFO *pStatInfo);


/* e.g. set ptz action, ect..., parameters see PlayerExtDef.h
   most cases it return TRUE(if succeed) or FALSE(if failed), 
   but a few cases it return error number, e.g. EXT_GETCFG_DIRECT, EXT_SETCFG_DIRECT
*/
int32_t STDCALL PlayerExtProc(HPLAYER hPlayer, uint32_t nType, uint32_t nParam1, uint32_t nParam2);

/*plugin category*/
typedef enum enPLUGIN_CATEGORY
{
	PLUGCAT_GENERAL			= 0,/*general or unknown*/
	PLUGCAT_DEMUX			= 1,/*demux, as as stream source*/
	PLUGCAT_EFFECTS			= 2,/*effect enhancement plugin, such as de-noise, gain, de-interlace, color balance, ..*/
	PLUGCAT_RENDERER		= 3,/*output plugin*/
	PLUGCAT_TRANS			= 4,/*transform plugin, such as audio channel switcher, color space converter, ..*/
	PLUGCAT_CODEC			= 5,/*encoder and/or decoder*/
}PLUGIN_CATEGORY;

/*general plugin handle*/
typedef void *HPLUGIN;

/*search for specified plugin
	hPlayer		- the player instance
	nCategory	- plugin category
	szName		- plugin name
Note:
	either nCategory or szName is required at lest
*/
HPLUGIN STDCALL PlayerFindPlugin(HPLAYER hPlayer, PLUGIN_CATEGORY nCategory, const int8_t *szName);

/*insert extra plugin after successfully opened the media, but before process
	hPlayer		- the player instance
	szName		- plugin name, wich is searched in plguin registry(NOT Windows(TM) registry!)
	hRefer		- the referer plugin, which is search by PlayerFindPlugin() or loaded by PlayerLoadPlugin()
	bBefore		- TRUE - insert before the refer plugin, FALSE - insert behind the refer plugin
*/
HPLUGIN STDCALL PlayerLoadPlugin(HPLAYER hPlayer, const int8_t *szName, HPLUGIN hRefer, bool_t bBefore);

/*the playlist is not a part of the player core, this make live simpler*/
#define  DEVCFG_MAX_BYTES   4096

/*detailed plugin system are described in iplugin.h*/
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


/*
pszRecordPaths      - Pointer to a buffer containing several of null-terminated filter strings. 
                      The last string in the buffer must be terminated by two NULL characters. 
nReserveSpaceInMB   - 
bAutoRemoveOldFiles - 

*/
bool_t STDCALL RecorderInit(const char *pszRecordPaths, uint32_t nReserveSpaceInMB, bool_t bAutoRemoveOldFiles, uint32_t nRemoveNumFilesEverytime, 
                            uint32_t nSecondsPerFile, uint32_t nMegaBytesPerFile, uint16_t nAppDataBytes);
void   STDCALL RecorderSetParameters(uint32_t nReserveSpaceInMB, bool_t bAutoRemoveOldFiles, uint32_t nFileSize);
bool_t STDCALL RecorderModifyPath(const char *pszRecordPaths, bool_t bAddOrRemove);
void   STDCALL RecorderUninit();

int32_t STDCALL GetServerPlayList(const char *pszServer, uint16_t nPort, const char *pszUser, const char *pszPwd, 
                                  char *szBuff, uint32_t nBuffLen);

#ifdef __cplusplus
}
#endif

#endif//PLAYER_INTF_H
