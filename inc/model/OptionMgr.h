
#pragma once

#if !__winvc
//#include <QRgb>
#define QRGB(r, g, b) ((0xffu << 24) | ((r & 0xffu) << 16) | ((g & 0xffu) << 8) | (b & 0xffu))
#define __defThemeColor QRGB(160, 210, 255)
#define __defTextColor QRGB(255, 255, 210)
#endif

struct __ModelExt tagPlaySpiritOption
{
	wstring strSkinName;

	int nPosX = 10000;
	int nPosY = 10000;

    UINT uVolume = 100;
};

struct __ModelExt tagAlarmOption
{
	tagAlarmOption()
	{
#if __winvc
		tmutil::getCurrentTime(nHour, nMinute);
#endif
	}

	int nHour = 0;
	int nMinute = 0;

	vector<wstring> vctAlarmmedia;
};

enum E_TimerOperate
{
	TO_Null = 0
	, TO_StopPlay
	, TO_Shutdown
	, TO_Hibernal
};

struct __ModelExt tagTimerOperateOpt
{
	tagTimerOperateOpt()
	{
#if __winvc
		tmutil::getCurrentTime(nHour, nMinute);
#endif
	}

	E_TimerOperate eTimerOperate = TO_Null;

	int nHour = 0;
	int nMinute = 0;
};

struct __ModelExt tagOption
{
	wstring strRootDir;
    PairList<wstring, E_AttachDirType> plAttachDir;

	UINT uPlayingItem = 0;

    bool bRandomPlay = false;
	bool bForce48KHz = false;

    bool bFullScreen = !__winvc;

#if __winvc
    bool bHideMenuBar = false;

#else
    wstring strHBkg;
    wstring strVBkg;

    bool bUseThemeColor = false;
    UINT crTheme = __defThemeColor;
    UINT crText = __defTextColor;
#endif

    tagPlaySpiritOption PlaySpiritOption;

    tagAlarmOption AlarmOption;

    tagTimerOperateOpt TimerOption;
};

class __ModelExt COptionMgr
{
public:
	COptionMgr() {}

private:
	tagOption m_Option;

	bool m_bInited = false;
	
public:
	tagOption& getOption()
	{
		return m_Option;
	}
	const tagOption& getOption() const
	{
		return m_Option;
	}

	tagPlaySpiritOption& getPlaySpiritOption()
	{
		return m_Option.PlaySpiritOption;
	}
	const tagPlaySpiritOption& getPlaySpiritOption() const
	{
		return m_Option.PlaySpiritOption;
	}

	tagAlarmOption& getAlarmOption()
	{
		return m_Option.AlarmOption;
	}
	const tagAlarmOption& getAlarmOption() const
	{
		return m_Option.AlarmOption;
	}

	tagTimerOperateOpt& getTimerOption()
	{
		return m_Option.TimerOption;
	}
	const tagTimerOperateOpt& getTimerOption() const
	{
		return m_Option.TimerOption;
	}

	tagOption& init();

	wstring checkAlarm();

	E_TimerOperate checkTimerOperate();

	void saveOption();
};
