#pragma once

#if __windows
#define __TimerID UINT_PTR
#else
#define __TimerID void
#endif

class __UtilExt timerutil
{
private:
    static __TimerID _setTimer(UINT uElapse, const fn_bool& cb);

public:
	static void singleShot(UINT uElapse, const fn_void& cb);
    static __TimerID setTimer(UINT uElapse, const fn_void& cb);
    static __TimerID setTimerEx(UINT uElapse, const fn_bool& cb);

#if __windows
    static void killTimer(UINT_PTR idEvent);
#endif
};

#if __windows
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
		
	void _set(const fn_bool& cb, UINT uElapse=0);

public:
	void set(UINT uElapse, const fn_bool& cb);

	void set(const fn_bool& cb);

	void kill();
};
#endif
