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

    virtual int64_t size() const = 0;

	virtual bool seekable() const
	{
		return true;
	}

    virtual int64_t seek(int64_t offset, int origin) = 0;

    virtual int read(byte_p buf, UINT size) = 0;
};

class __PlaySDKExt CAudioOpaque : private IAudioOpaque
{
public:
    CAudioOpaque();

    virtual ~CAudioOpaque();

private:
    void *m_pDecoder = NULL;

    FILE *m_pf = NULL;

protected:
    long long m_nFileSize = -1;
	
public:
	void* decoder()
	{
		return m_pDecoder;
	}

    virtual void close();

    long long open(cwstr strFile);

	uint32_t checkDuration();

    const E_DecodeStatus& decodeStatus() const;

	virtual bool isOnline() const override
	{
		return false;
	}

	uint32_t duration() const;
	uint64_t clock() const;

	int sampleRate() const;

private:
	virtual wstring localFilePath() const override
	{
		return L"";
	}

	virtual int64_t size() const override
	{
	    return m_nFileSize;
	}

protected:
        virtual int64_t seek(int64_t offset, int origin) override;

        virtual int read(byte_p buf, UINT size) override;

        bool seekingFlag() const;
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

    thread m_thread;
	
public:
    static int InitSDK();
    static void QuitSDK();

	void Stop();
    bool Play(uint64_t uStartPos, bool bForce48KHz, CB_PlayStop cbStop);
	
    bool Pause();
    bool Resume();

    bool Seek(UINT uPos);

	void SetVolume(UINT uVolume);

    bool packetQueueEmpty() const;
};
