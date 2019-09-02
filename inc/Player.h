#pragma once

#include "util.h"

#if __winvc
#pragma warning(disable: 4251)
#endif

#ifdef __PlaySDKPrj
	#define __PlaySDKExt __dllexport
#else
	#define __PlaySDKExt __dllimport
#endif

class __PlaySDKExt IAudioOpaque
{
public:
    virtual int64_t size() const = 0;

    virtual int64_t open() = 0;

    virtual bool seekable() = 0;
    virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) = 0;

    virtual size_t read(uint8_t *buf, int buf_size) = 0;

    virtual void close() = 0;
};

using CB_PlayFinish = fn_void;

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
	
	CSignal m_stopedSignal;

private:
	template <typename T>
	bool _Play(T& input, uint64_t uStartPos, bool bForce48000);

public:
    static int InitSDK();
    static void QuitSDK();

	static int CheckDuration(const wstring& strFile, bool bLock = true);
    static int CheckDuration(IAudioOpaque& AudioOpaque, bool bLock = true);

	UINT GetDuration() const;
	
    bool Play(const wstring& strFile, uint64_t uStartPos = 0, bool bForce48000 = false);
    bool Play(IAudioOpaque& AudioOpaque, uint64_t uStartPos = 0, bool bForce48000 = false);

	E_PlayStatus GetPlayStatus();

    void SetVolume(UINT uVolume);

	uint64_t getClock() const;
    void Seek(UINT uPos);

    void Pause();

	void Resume();

    void Stop();
};
