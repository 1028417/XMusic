#pragma once

#if __winvc
#define __TimerID UINT_PTR
#else
#define __TimerID void
#endif

class __UtilExt timerutil
{
private:
    static __TimerID _setTimer(UINT uElapse, cfn_bool cb);

public:
	static void async(UINT uElapse, cfn_void cb);

    static __TimerID setTimer(UINT uElapse, cfn_void cb);
    static __TimerID setTimerEx(UINT uElapse, cfn_bool cb);

#if __winvc
    static void killTimer(UINT_PTR idEvent);
#endif
};

#if __winvc
class __UtilExt CWinTimer
{
public:
	CWinTimer()
	{
	}

	~CWinTimer()
	{
		kill();
	}

private:
	UINT m_uElapse = 0;

    __TimerID m_idTimer = 0;
	
	fn_bool m_cb;

private:
	bool _onTimer();
		
	void _set(cfn_bool cb, UINT uElapse=0);

public:
	void set(UINT uElapse, cfn_bool cb);

	void set(cfn_bool cb);

	void kill();
};
#endif
