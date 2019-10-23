#pragma once

#include "util/util.h"

#if __winvc
#pragma warning(disable: 4251)
#endif

#ifdef __PlaySDKPrj
	#define __PlaySDKExt __dllexport
#else
	#define __PlaySDKExt __dllimport
#endif

enum class E_DecodeStatus
{
	DS_Opening,
	DS_Decoding,
	DS_Paused,
	DS_Cancel,
	DS_Finished,
};

class __PlaySDKExt CAudioOpaque
{
public:
    CAudioOpaque(FILE *pf = NULL);

	~CAudioOpaque();

protected:
	FILE *m_pf = NULL;

    uint64_t m_uPos = 0;

private:
    void *m_pDecoder = NULL;

public:
	void* decoder()
	{
		return m_pDecoder;
	}

	UINT checkDuration();

	E_DecodeStatus decodeStatus();

public:
    virtual wstring file() const {return L"";}

    virtual uint64_t pos() const
    {
        return m_uPos;
    }

    virtual bool seekable() const {return NULL!=m_pf;}
    virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set);
	
    virtual int read(uint8_t *buf, size_t size);

    virtual void close();
};

using CB_PlayFinish = fn_void_t<E_DecodeStatus>;

class __PlaySDKExt CPlayer
{
public:
	CPlayer(CAudioOpaque& AudioOpaque)
		: m_AudioOpaque(AudioOpaque)
	{
	}

	~CPlayer()
	{
		Stop();
	}
	
private:
	CAudioOpaque& m_AudioOpaque;
	
    mutex m_mutex;

	XThread m_thread;
	
private:
	virtual void _onFinish(E_DecodeStatus) {}

public:
    static int InitSDK();
    static void QuitSDK();

	void Stop();
    bool Play(uint64_t uStartPos, bool bForce48000, const CB_PlayFinish& cbFinish = NULL);
	
	uint32_t GetDuration();
	uint64_t GetClock();
    void Seek(UINT uPos);

    void Pause();
	void Resume();
	
	void SetVolume(UINT uVolume);
};
