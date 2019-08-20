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

class __PlaySDKExt IFileOpaque
{
public:
	virtual int64_t size() const = 0;

	virtual int64_t open() = 0;

	virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) = 0;

	virtual size_t read(uint8_t *buf, int buf_size) = 0;

	virtual void close() = 0;
};

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
	
	CSignal m_stopedSignal;

private:
	template <typename T>
	bool _Play(T& input, uint64_t uStartPos, bool bForce48000);

public:
    static int InitSDK();
    static void QuitSDK();

	static int CheckDuration(const wstring& strFile, bool bLock = true);
	static int CheckDuration(IFileOpaque& FileOpaque, bool bLock = true);

	UINT GetDuration() const;
	
    bool Play(const wstring& strFile, uint64_t uStartPos = 0, bool bForce48000 = false);
    bool Play(IFileOpaque& FileOpaque, uint64_t uStartPos = 0, bool bForce48000 = false);

	E_PlayStatus GetPlayStatus();

    void SetVolume(UINT uVolume);

	uint64_t getClock() const;
    void Seek(UINT uPos);

    void Pause();

	void Resume();

    void Stop();
};

class __PlaySDKExt CFileOpaque : public IFileOpaque
{
public:
	CFileOpaque() {}

	CFileOpaque(const wstring& strFile) : m_strFile(strFile)
	{
	}

	~CFileOpaque()
	{
		close();
	}

protected:
	wstring m_strFile;

	FILE *m_pf = NULL;

	int64_t m_size = -1;

	bool m_bExist = false;

	uint64_t m_uPos = 0;

public:
	bool exists() const
	{
		return m_bExist;
	}

	virtual int64_t size() const override
	{
		return m_size;
	}

	virtual int64_t open() override
	{
		if (NULL != m_pf)
		{
			return m_size;
		}
		m_size = -1;

		m_bExist = false;

#if __android
		m_pf = fopen(wsutil::toStr(m_strFile).c_str(), "rb");
#else
		(void)_wfopen_s(&m_pf, m_strFile.c_str(), L"rb");
#endif
		if (NULL != m_pf)
		{
			m_bExist = true;

			tagFileStat32_64 stat;
			memset(&stat, 0, sizeof stat);
			if (fsutil::fileStat32_64(m_pf, stat))
			{
				m_size = stat.st_size;
			}
		}

		m_uPos = 0;

		return m_size;
	}

	virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) override
	{
		auto nRet = fsutil::seekFile(m_pf, offset, eFlag);
		if (nRet >= 0)
		{
			m_uPos = nRet;
		}
		return m_uPos;
	}

	virtual size_t read(uint8_t *buf, int buf_size) override
	{
		size_t uRet = fread(buf, 1, buf_size, m_pf);
		m_uPos += uRet;
		return uRet;
	}

	virtual void close() override
	{
		if (NULL != m_pf)
		{
			(void)fclose(m_pf);
			m_pf = NULL;
		}

		m_uPos = 0;
	}

	UINT checkDuration(bool bLock=true)
	{
		return CPlayer::CheckDuration(*this, bLock);
	}
};
