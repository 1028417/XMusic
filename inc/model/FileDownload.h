#pragma once

enum class E_DownloadStatus
{
    DS_OK = 0,
    DS_Cancel,
    DS_Fail
};

class __ModelExt CFileDownload
{
public:
    CFileDownload() {}

private:
    E_DownloadStatus m_eStatus = E_DownloadStatus::DS_OK;

    std::condition_variable m_condvar;

    size_t m_uTotalSize = 0;
    size_t m_uDataSize = 0;
    list<pair<char*, size_t>> m_lstData;

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

    void _clear();

public:
    int download(const string& strUrl);

    size_t getData(char *pBuff, size_t buffSize);

	void setPause(bool bPause);

    void cancel();
};
