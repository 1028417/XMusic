#pragma once

class __ModelExt CFileDownload
{
public:
    CFileDownload() {}

private:
    bool m_bStatus = false;

    list<pair<char*, size_t>> m_lstData;
    size_t m_uDataSize = 0;
    //size_t m_uTotalSize = 0;

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
    bool download(const string& strUrl);

    int getData(char *pBuff, size_t buffSize);

    void cancel();
};
