
#pragma once

struct __ModelExt tagPlaySpiritOption
{
	wstring strSkinName;

	int nPosX = 10000;
	int nPosY = 10000;

    UINT uVolume = 100;
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

    bool bFullScreen = false;

#if __winvc
    bool bHideMenuBar = false;

#else
    bool bUseBkgColor = false;
    int64_t crBkg = -1;
    int64_t crFore = -1;

    wstring strHBkg;
    wstring strVBkg;
    wstring strAddBkgDir;
#endif

    tagPlaySpiritOption PlaySpiritOption;

    int nAlarmHour = 0;
    int nAlarmMinute = 0;

    tagTimerOperateOpt TimerOption;
};

class __ModelExt COptionMgr
{
public:
    COptionMgr() = default;

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

	tagTimerOperateOpt& getTimerOption()
	{
		return m_Option.TimerOption;
	}
	const tagTimerOperateOpt& getTimerOption() const
	{
		return m_Option.TimerOption;
	}

	tagOption& init();

	E_TimerOperate checkTimerOperate();

    bool checkAlarm();

	void saveOption();
};
