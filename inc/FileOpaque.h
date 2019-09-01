#pragma once

#include "Player.h"

#if __android
class CFileDownload
{
public:
    CFileDownload(){}

    ~CFileDownload()
    {
        cleanup();
    }

private:
    void *m_curl = NULL;

private:
    static size_t _curlRecv(void *buffer, size_t size, size_t nmemb, void *context)
    {
        return ((CFileDownload*)context)->_onRecv(buffer, size, nmemb);
    }

    virtual size_t _onRecv(void *buffer, size_t size, size_t nmemb) = 0;
        //uRet = fwrite(buffer, size, nmemb, (FILE *)stream);

    /*static size_t _curlProgress(void *context, double total, double now, double, double)
    {
        return ((CFileDownload*)context)->_onProgress(total, now);
    }

    virtual size_t _onProgress(double total, double now) = 0;*/

public:
    bool download(const string& strUrl);

    void init();

    void cleanup();

    static void cleanupCurl();

    static void initCurl();
};
#endif

class __ModelExt CFileOpaque : public IAudioOpaque
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

class __ModelExt CFileOpaqueEx : public CFileOpaque
{
public:
	static UINT checkDuration(const wstring& strFile, bool bLock = true)
	{
		CFileOpaqueEx FileOpaque(strFile);
		return FileOpaque.checkDuration(bLock);
	}

	static UINT checkDuration(IMedia& media, bool bLock = true)
	{
		return checkDuration(media.GetAbsPath(), bLock);
	}

public:
    CFileOpaqueEx(const wstring& strFile, bool bURL = false)
		: CFileOpaque(strFile)
	{
        m_bURL = bURL;
	}

	UINT checkDuration(bool bLock = true)
	{
		return CFileOpaque::checkDuration(bLock);
	}

private:
    bool m_bURL = false;

	void *m_pXmsc = NULL;

private:
	int64_t open() override;

	void close() override;

	size_t read(uint8_t *buf, int buf_size) override;
};
