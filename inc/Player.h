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

class __PlaySDKExt IAudioOpaque
{
public:
	virtual wstring getFile() const = 0;

    virtual bool open() = 0;

    virtual bool seekable() const = 0;
    virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) = 0;

    virtual int read(uint8_t *buf, int buf_size) = 0;

    virtual void close() = 0;
};

class __PlaySDKExt CAudioOpaque : public IAudioOpaque
{
public:
	CAudioOpaque();

	~CAudioOpaque();
	
protected:
	void *m_pDecoder = NULL;

	wstring m_strFile;

	FILE *m_pf = NULL;

	int32_t m_size = -1;

	uint64_t m_uPos = 0;

public:
	int32_t fileSize() const
	{
		return m_size;
	}

	void setFile(const wstring& strFile);

	int checkDuration();

protected:
	virtual wstring getFile() const override
	{
		return m_strFile;
	}

	virtual bool open() override;

	virtual bool seekable() const override
	{
		return !m_strFile.empty();
	}

	virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) override;
	
    virtual int read(uint8_t *buf, int buf_size) override;
	
	virtual void close() override;
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
    template <typename T> bool _Play(T& input, uint64_t uStartPos, bool bForce48000);
	
public:
    static int InitSDK();
    static void QuitSDK();

    int GetDuration() const;

    bool Play(IAudioOpaque& AudioOpaque, uint64_t uStartPos, bool bForce48000);

	E_PlayStatus GetPlayStatus();

    void SetVolume(UINT uVolume);

	uint64_t getClock() const;
    void Seek(UINT uPos);

    void Pause();

	void Resume();

    void Stop();
};
