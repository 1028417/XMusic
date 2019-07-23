#pragma once

#include "util.h"

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#ifdef __PlaySDKPrj
	#define __PlaySDKExt __dllexport
#else
	#define __PlaySDKExt __dllimport
#endif

using CB_PlayFinish = fn_voidvoid;

enum class E_PlayStatus
{
	PS_Stop
	, PS_Play
	, PS_Pause
};

class __PlaySDKExt CPlayer
{
public:
	CPlayer(const CB_PlayFinish& cbFinish = NULL)
		: m_cbFinish(cbFinish)
		, m_stopedSignal(true)
	{
	}
	
private:
	CB_PlayFinish m_cbFinish;
	
	int m_nDuration = 0;

	CSignal m_stopedSignal;

public:
    static int InitSDK();
    static void QuitSDK();

    static int CheckDuration(const wstring& strFile, bool bLock = true);

	E_PlayStatus GetPlayStatus();
	
    int GetDuration() const
	{
		return m_nDuration;
	}

    int Play(const wstring& strFile, uint64_t uStartPos = 0, bool bForce48000 = false);
	
    void SetVolume(UINT uVolume);

	uint64_t getClock() const;
    void Seek(UINT uPos);

    void Pause();

	void Resume();

    void Stop();
};
