#pragma once

using CB_WinTimer = function<bool()>;

class __UtilExt wintimer
{
public:
	static UINT_PTR setTimer(UINT uElapse, const CB_WinTimer& cb);

	static void killTimer(UINT_PTR idEvent);
};

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

	UINT_PTR m_idTimer = 0;
	
	CB_WinTimer m_cb;

private:
	bool _onTimer();
		
	void _set(const CB_WinTimer& cb, UINT uElapse=0);

public:
	void set(UINT uElapse, const CB_WinTimer& cb);

	void set(const CB_WinTimer& cb);

	void kill();
};
