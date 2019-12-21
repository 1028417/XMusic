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
	DS_Decoding,
	DS_Paused,
	DS_Cancel,
	DS_Finished,
};

class IAudioOpaque
{
public:
    virtual bool isOnline() const = 0;

    virtual wstring localFilePath() const = 0;

    virtual bool seekable() const = 0;

    virtual int64_t seek(int64_t offset, int origin) = 0;

    virtual size_t read(byte_p buf, size_t size) = 0;

    virtual E_DecodeStatus decodeStatus() const = 0;
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

private:
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

protected:
	virtual int64_t seek(int64_t offset, int origin) override;

        virtual size_t read(byte_p buf, size_t size) override;

        UINT byteRate() const;
};

using CB_PlayStop = cfn_void_t<bool>;

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
    bool Play(uint64_t uStartPos, bool bForce48KHz, CB_PlayStop cbStop = NULL);
	
	uint32_t GetDuration();
	uint64_t GetClock();

	void Seek(UINT uPos);

    void Pause();
	void Resume();
	
	void SetVolume(UINT uVolume);
};
