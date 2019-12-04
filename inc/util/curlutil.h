
#pragma once

struct __UtilExt tagCurlOpt
{
    tagCurlOpt(bool t_bShare)//, long t_dnsCacheTimeout)
        : bShare(t_bShare)
        //, dnsCacheTimeout(t_dnsCacheTimeout)
    {
    }

    bool bShare;
    //long dnsCacheTimeout;

    unsigned long maxRedirect = 0;

    unsigned long timeout = 0;
    unsigned long connectTimeout = 0;

    unsigned long lowSpeedLimit = 0;
    unsigned long lowSpeedLimitTime = 0;

    unsigned long maxSpeedLimit = 0;

    //unsigned long keepAliveIdl = 0;
    //unsigned long keepAliveInterval = 0;

    string strUserAgent;
};

using CB_CURLWrite = const function<size_t(char *ptr, size_t size, size_t nmemb)>;
using CB_CURLProgress = const function<int(int64_t dltotal, int64_t dlnow)>;

class __UtilExt curlutil
{
public:
    static int initCurl(string& strVerInfo);
    static void freeCurl();

    static int curlDownload(const tagCurlOpt& curlOpt, const string& strURL, CB_CURLWrite& cbWrite, CB_CURLProgress& cbProgress = NULL);

    static int curlToolPerform(const list<string>& lstParams);
    static int curlToolDownload(const string& strURL, CB_CURLWrite& cbWrite);

    static string getCurlErrMsg(UINT uCurlCode);
};

class __UtilExt CDownloader
{
public:
    virtual ~CDownloader()
    {
        _clear();
    }

    CDownloader(bool bShare = false, unsigned long timeout = 0, unsigned long connectTimeout = 3
            , unsigned long lowSpeedLimit = 0, unsigned long lowSpeedLimitTime = 0
            , unsigned long maxSpeedLimit = 0) : m_curlOpt(bShare)
    {
        m_curlOpt.timeout = timeout;
        m_curlOpt.connectTimeout = connectTimeout;

        m_curlOpt.lowSpeedLimit = lowSpeedLimit; // 单位字节
        m_curlOpt.lowSpeedLimitTime = lowSpeedLimitTime;

        m_curlOpt.maxSpeedLimit = maxSpeedLimit;

        m_curlOpt.strUserAgent = "curl/7.66.0";
    }

    CDownloader(const tagCurlOpt& curlOpt) : m_curlOpt(curlOpt)
    {
    }

protected:
    tagCurlOpt m_curlOpt;

    time_t m_beginTime = 0;

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

    virtual bool _onProgress(int64_t dltotal, int64_t dlnow)
    {
        (void)dltotal;
        (void)dlnow;
        return true;
    }

public:
    bool status() const
    {
        return m_bStatus;
    }

    using CB_Downloader = function<bool(byte_t *ptr, size_t size)>;
    int syncDownload(const string& strUrl, UINT uRetryTime = 0, const CB_Downloader& cb = NULL);
    int syncDownload(const string& strUrl, CByteBuffer& bbfData, UINT uRetryTime = 0);
    int syncDownload(const string& strUrl, CCharBuffer& cbfData, UINT uRetryTime = 0);

    void asyncDownload(const string& strUrl, UINT uRetryTime = 0, const CB_Downloader& cb = NULL);

    size_t dataSize() const
    {
        return m_uDataSize;
    }
    uint64_t sumSize() const
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
