
#pragma once

class COptionMgr
{
public:
    COptionMgr() = default;

private:
	tagOption m_opt;

	bool m_bInited = false;
	
public:
	tagOption& getOption()
	{
		return m_opt;
	}
	const tagOption& getOption() const
	{
		return m_opt;
	}

#if __winvc
	tagPlaySpiritOption& getPlaySpiritOption()
	{
		return m_opt.PlaySpiritOption;
	}
	const tagPlaySpiritOption& getPlaySpiritOption() const
	{
		return m_opt.PlaySpiritOption;
	}

	tagTimerOperateOpt& getTimerOption()
	{
		return m_opt.TimerOption;
	}
	const tagTimerOperateOpt& getTimerOption() const
	{
		return m_opt.TimerOption;
	}

	E_TimerOperate checkTimerOperate();

    bool checkAlarm();
#endif

    void init();

	void saveOption();
};
