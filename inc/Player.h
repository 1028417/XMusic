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

class __PlaySDKExt IFileOpaque
{
public:
	virtual int64_t size() const = 0;

	virtual int64_t open() = 0;

	virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) const = 0;

	virtual size_t read(uint8_t *buf, int buf_size) const = 0;

	virtual void close() = 0;
};

class __PlaySDKExt CFileOpaque : public IFileOpaque
{
public:
	CFileOpaque() {}

	CFileOpaque(const wstring& strFile) : m_strFile(strFile)
	{
	}

	CFileOpaque(FILE *pf) : m_pf(pf)
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

public:
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

#ifdef __ANDROID__
        m_pf = fopen(wsutil::toStr(m_strFile).c_str(), "rb");
#else
		if (_wfopen_s(&m_pf, m_strFile.c_str(), L"rb"))
		{
			return -1;
		}
#endif
		if (NULL == m_pf)
		{
			return -1;
		}

		tagFileStat32_64 stat;
		memset(&stat, 0, sizeof stat);
		if (!fsutil::fileStat32_64(m_pf, stat))
		{
			return -1;
		}

		m_size = stat.st_size;
		return m_size;
	}

	virtual int64_t seek(int64_t offset, E_SeekFileFlag eFlag = E_SeekFileFlag::SFF_Set) const override
	{
		return fsutil::seekFile(m_pf, offset, eFlag);
	}

	virtual size_t read(uint8_t *buf, int buf_size) const override
	{
		return fread(buf, 1, buf_size, m_pf);
	}

	virtual void close() override
	{
		if (NULL != m_pf)
		{
			(void)fclose(m_pf);
		}

		detach();
	}

	void detach()
	{
		m_strFile.clear();

		m_pf = NULL;

		m_size = -1;
	}

	void attach(const wstring& strFile)
	{
		detach();

		m_strFile = strFile;
	}

	void attach(FILE *pf)
	{
		detach();

		m_pf = pf;
	}

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
	
	int m_nDuration = 0;

	CSignal m_stopedSignal;

private:
	template <typename T>
	int _Play(T& input, uint64_t uStartPos, bool bForce48000);

public:
    static int InitSDK();
    static void QuitSDK();

	static int CheckDuration(const wstring& strFile, bool bLock = true);
	static int CheckDuration(IFileOpaque& FileOpaque, bool bLock = true);

    int GetDuration() const
	{
		return m_nDuration;
	}

    int Play(const wstring& strFile, uint64_t uStartPos = 0, bool bForce48000 = false);
    int Play(IFileOpaque& FileOpaque, uint64_t uStartPos = 0, bool bForce48000 = false);

	E_PlayStatus GetPlayStatus();

    void SetVolume(UINT uVolume);

	uint64_t getClock() const;
    void Seek(UINT uPos);

    void Pause();

	void Resume();

    void Stop();
};
