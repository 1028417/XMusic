
#pragma once

using CB_CURLWrite = const function<size_t(char *ptr, size_t size, size_t nmemb)>;
using CB_CURLProgress = const function<int(int64_t dltotal, int64_t dlnow)>;

class __UtilExt curlutil
{
public:
    static int initCurl(string& strVerInfo);

    static int curlDownload(const string& strURL, string& strErrMsg, CB_CURLWrite& cbWrite, CB_CURLProgress& cbProgress = NULL);
    static int curlDownload(const string& strURL, CB_CURLWrite& cbWrite, CB_CURLProgress& cbProgress = NULL)
    {
        string strErrMsg;
        return curlDownload(strURL, strErrMsg, cbWrite, cbProgress);
        (void)strErrMsg;
    }

    static int curlToolPerform(const list<string>& lstParams);
    static int curlToolDownload(const string& strURL, CB_CURLWrite& cbWrite);
};

class __UtilExt CDownloader
{
public:
    ~CDownloader()
    {
        _clear();
    }

    CDownloader() {}

private:
    XThread m_thread;

    bool m_bStatus = false;

    mutex m_mtxDataLock;
    list<pair<byte_t*, size_t>> m_lstData;
    size_t m_uDataSize = 0;
    size_t m_uSumSize = 0;

private:
    void _clear();

    template <class T>
    size_t _getAllData(T& buff)
    {
        size_t uReadSize = m_uDataSize;
        if (0 == uReadSize)
        {
            return 0;
        }

        auto ptr = buff.resizeMore(uReadSize);
        size_t size = (size_t)MAX(0, getData((byte_t*)ptr, uReadSize));
        if (size < uReadSize)
        {
            buff.resizeLess(uReadSize - size);
        }

        return size;
    }

public:
    bool status() const
    {
        return m_bStatus;
    }

    using CB_Downloader = function<void(byte_t *ptr, size_t size)>;
    int syncDownload(const string& strUrl, UINT uRetryTime = 0, const CB_Downloader& cb = NULL);
    int syncDownload(const string& strUrl, CByteBuffer& bbfData, UINT uRetryTime = 0);
    int syncDownload(const string& strUrl, CCharBuffer& cbfData, UINT uRetryTime = 0);

    void asyncDownload(const string& strUrl, UINT uRetryTime = 0, const CB_Downloader& cb = NULL);

    size_t dataSize() const
    {
        return m_uDataSize;
    }
    size_t sumSize() const
    {
        return m_uSumSize;
    }

    int getData(byte_t *pBuff, size_t buffSize);

    CByteBuffer getByteData()
    {
        CByteBuffer bbfData;
        (void)_getAllData(bbfData);
        return bbfData;
    }
    size_t getByteData(CByteBuffer& bbfData)
    {
        return _getAllData(bbfData);
    }

    CCharBuffer getTextData()
    {
        CCharBuffer cbfData;
        (void)_getAllData(cbfData);
        return cbfData;
    }
    size_t getTextData(CCharBuffer& cbfData)
    {
        return _getAllData(cbfData);
    }

    void cancel();
};
