#pragma once

#include "Player.h"

#include <deque>

enum class E_DownloadStatus
{
    DS_None = 0,
    DS_Downloading,
    DS_Cancel,
    DS_Fail
};

class __ModelExt CFileDownload
{
public:
    CFileDownload()
        : m_sgn(true)
    {
    }

private:
    void *m_curl = NULL;

    CSignal m_sgn;

    E_DownloadStatus m_eStatus = E_DownloadStatus::DS_None;

    std::mutex m_mtx;
    std::condition_variable m_condvar;

    list<pair<char*, size_t>> m_lstData;
    int m_nDataSize = 0;
	
private:
    static size_t _curlRecv(void *buffer, size_t size, size_t nmemb, void *context)
    {
        return ((CFileDownload*)context)->_onRecv(buffer, size, nmemb);
    }

    virtual size_t _onRecv(void *buffer, size_t size, size_t nmemb);

    /*static size_t _curlProgress(void *context, double total, double now, double, double)
    {
        return ((CFileDownload*)context)->_onProgress(total, now);
    }

    virtual size_t _onProgress(double total, double now) = 0;*/

    void _init();

    void _pause();

    void _resume();

public:
    E_DownloadStatus status() const
    {
        return m_eStatus;
    }

    bool download(const string& strUrl);

    size_t getData(char *pBuff, size_t buffSize);

    void cancel(bool bWait = false)
    {
		m_eStatus = E_DownloadStatus::DS_Cancel;

        if (bWait)
        {
            m_sgn.wait();
        }
    }

    static void cleanupCurl();

    static void initCurl();
};

class __ModelExt CFileOpaque : public IAudioOpaque
{
public:
    CFileOpaque() {}

    CFileOpaque(const wstring& strFile)
		: m_strFile(strFile)
    {
    }

    ~CFileOpaque()
    {
        close();
    }

    void set(const wstring& strFile)
    {
        close();

        m_strFile = strFile;
        m_size = -1;
    }

protected:
    wstring m_strFile;

    FILE *m_pf = NULL;

    int64_t m_size = -1;

    uint64_t m_uPos = 0;

public:
	int64_t fileSize() const
	{
		return m_size;
	}

    /*virtual int64_t size() const override
    {
        return m_size;
    }

	virtual uint64_t pos() const override
	{
		return m_uPos;
	}*/

    virtual bool open() override
    {
        if (m_pf)
        {
            return m_size;
        }

        m_size = -1;        
        m_uPos = 0;

#if __android
        m_pf = fopen(wsutil::toStr(m_strFile).c_str(), "rb");
#else
        (void)_wfopen_s(&m_pf, m_strFile.c_str(), L"rb");
#endif
        if (NULL == m_pf)
        {
            return false;
        }

        tagFileStat32_64 stat;
        memset(&stat, 0, sizeof stat);
        if (!fsutil::fileStat32_64(m_pf, stat))
        {
            close();
            return false;
        }

        m_size = stat.st_size;
        if (0 == m_size)
        {
            close();
            return false;
        }

        return true;
    }

    virtual bool seekable() override
    {
        return true;
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
};

class __ModelExt CAudioOpaque : public CFileOpaque
{
public:
    CAudioOpaque() {}

	CAudioOpaque(const wstring& strFile, bool bURL = false)
		: CFileOpaque(strFile)
	{
		_set(strFile, bURL);
	}

	void set(const wstring& strFile, bool bURL = false)
	{
		CFileOpaque::set(strFile);

		_set(strFile, bURL);
	}

private:
    bool m_bURL = false;
    CFileDownload m_fileDownload;

	void *m_pXmsc = NULL;

public:
    static int checkDuration(const wstring& strFile, bool bLock = true)
    {
        CAudioOpaque FileOpaque(strFile);
        return FileOpaque.checkDuration(bLock);
    }

    static int checkDuration(IMedia& media, bool bLock = true)
    {
        return checkDuration(media.GetAbsPath(), bLock);
    }

	int checkDuration(bool bLock = true)
	{
		return CPlayer::CheckDuration(*this, bLock);
	}

private:
	void _set(const wstring& strFile, bool bURL);

	wstring getFile() override
	{
		if (m_bURL || m_pXmsc)
		{
			return L"";
		}
		
		return m_strFile;
	}

    bool open() override;

	void close() override;

	size_t read(uint8_t *buf, int buf_size) override;

    bool seekable() override
    {
		return !m_bURL;
    }
};
