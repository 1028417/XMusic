
#pragma once

class COptionMgr
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

#if __winvc
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

	E_TimerOperate checkTimerOperate();

    bool checkAlarm();
#endif

    void init();

	void saveOption();
};
