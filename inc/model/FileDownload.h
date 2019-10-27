#pragma once

class __ModelExt CFileDownload
{
public:
    static CFileDownload& inst()
    {
        static CFileDownload inst;
        return inst;
    }

private:
    CFileDownload() {}

	~CFileDownload()
	{
		_clear();
	}

private:
    XThread m_thread;

    bool m_bStatus = false;

	mutex m_mtxDataLock;
    list<pair<uint8_t*, size_t>> m_lstData;
    size_t m_uDataSize = 0;
    size_t m_uSumSize = 0;

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

    bool _download(const string& strUrl);

public:
    void download(const string& strUrl);
    bool download(const string& strUrl, CByteVector& vecBuff);

    size_t dataSize() const
    {
        return m_uDataSize;
    }
    size_t sumSize() const
    {
        return m_uSumSize;
    }

    int getData(uint8_t *pBuff, size_t buffSize);
    void getAllData(CByteVector& vecBuff);

    void cancel();
};
