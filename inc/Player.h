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
    DS_OpenFail,
	DS_Decoding,
	DS_Paused,
	DS_Cancel,
	DS_Finished,
};

class IAudioOpaque
{
public:
    virtual long checkPreserveDataSize() const
    {
        return -1;
	}
	virtual bool isOnline() const = 0;

	virtual wstring localFilePath() const = 0;

	virtual bool seekable() const = 0;

	virtual int64_t seek(int64_t offset, int origin) = 0;

	virtual int read(uint8_t *buf, size_t size) = 0;

	virtual E_DecodeStatus decodeStatus() const = 0;

    virtual UINT byteRate() const = 0;
};

class __PlaySDKExt CAudioOpaque : public IAudioOpaque
{
public:
    CAudioOpaque();

    virtual ~CAudioOpaque();

private:
    void *m_pDecoder = NULL;

	FILE *m_pf = NULL;

    //uint64_t m_uPos = 0;

private:
    friend class CPlayer;
	void* decoder()
	{
        return m_pDecoder;
	}

public:
    virtual void close();
	long long open(const wstring& strFile);

	UINT checkDuration();

	E_DecodeStatus decodeStatus() const override;

protected:
	virtual bool isOnline() const override
	{
		return false;
	}

	virtual wstring localFilePath() const override
	{
		return L"";
	}

    virtual bool seekable() const override
	{
		return NULL!=m_pf;
	}
    
	virtual int64_t seek(int64_t offset, int origin) override;

    virtual int read(uint8_t *buf, size_t size) override;

    UINT byteRate() const override;
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
