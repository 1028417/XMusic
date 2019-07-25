#pragma once

struct tagPlaySpiritOption
{
	wstring strSkinName;

	int iPosX = 500;
	int iPosY = 500;

	int iVolume = 50;
};

struct tagAlarmOption
{
	tagAlarmOption()
	{
		tmutil::getCurrentTime(nHour, nMinute);
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

		tmutil::getCurrentTime(nHour, nMinute);
	}

	E_TimerOperate eTimerOperate;

	int nHour = 0;
	int nMinute = 0;
};

struct tagOption
{
#ifdef __ANDROID__
	tagOption()
		: strRootDir(L"/XMusic")
		, vecAttachDir{ L"/XMusic" }
	{
	}
#endif

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

class __ModelExt CDataMgr
{
public:
	CDataMgr(CDao& dao)
		: m_dao(dao)
	{
	}

private:
	CDao& m_dao;

	tagOption m_Option;
	
public:
	tagOption& getOption()
	{
		return m_Option;
	}
	
	tagPlaySpiritOption& getPlaySpiritOption()
	{
		return m_Option.PlaySpiritOption;
	}

	tagAlarmOption& getAlarmOption()
	{
		return m_Option.AlarmOption;
	}

	tagTimerOperateOpt& getTimerOption()
	{
		return m_Option.TimerOption;
	}

	bool init();
	
	bool clearData();

	bool updateMediaSetName(CMediaSet& MediaSet, const wstring& strName);
	bool updateMediaSetProperty(CMediaSet& MediaSet, const CMediasetProperty& property);
	
	bool initAlarmmedia();
	
	bool addAlarmmedia(const TD_IMediaList& lstMedias);
	
	bool removeAlarmmedia(UINT uIndex);

	bool clearAlarmmedia();

	bool queryPlayRecordMaxTime(dbtime_t& tTime);
	bool queryPlayRecord(dbtime_t tTime, vector<pair<wstring, int>>& vecPlayRecord);
	bool clearPlayRecord();

	wstring checkAlarm();

	E_TimerOperate checkTimerOperate();
	
	void saveOption();
};
