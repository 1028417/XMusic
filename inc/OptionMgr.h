
#pragma once

struct tagPlaySpiritOption
{
	wstring strSkinName;

	int iPosX = -1000;
	int iPosY = -1000;

    UINT uVolume = 100;
};

struct tagAlarmOption
{
	tagAlarmOption()
	{
		//tmutil::getCurrentTime(nHour, nMinute);
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

struct tagTimerOperateOpt
{
	tagTimerOperateOpt()
	{
		eTimerOperate = TO_Null;

		//tmutil::getCurrentTime(nHour, nMinute);
	}

	E_TimerOperate eTimerOperate;

	int nHour = 0;
	int nMinute = 0;
};

struct tagOption
{
	wstring strRootDir;
	vector<wstring> vecAttachDir;

	UINT uPlayingItem = 0;

    bool bRandomPlay = false;
	bool bForce48000 = false;

    bool bHideMenuBar = false;
    bool bFullScreen = false;

    wstring strHBkg;
    wstring strVBkg;

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

	void init();

	wstring checkAlarm();

	E_TimerOperate checkTimerOperate();

	void saveOption();
};
