
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

    unsigned long maxRedirect = 1;

    unsigned long connectTimeout = 0;
    unsigned long timeout = 0;

    unsigned long lowSpeedLimit = 0;
    unsigned long lowSpeedLimitTime = 0;

    unsigned long maxSpeedLimit = 0;

    //unsigned long keepAliveIdl = 0;
    //unsigned long keepAliveInterval = 0;

    string strUserAgent = "curl/7.66.0";
};

using CB_CURL = const function<size_t(char *ptr, size_t size, size_t nmemb)>;
using CB_CURLProgress = const function<int(int64_t dltotal, int64_t dlnow)>;

class tagCURlToolHook
{
protected:
    tagCURlToolHook(CB_CURLProgress& fnProgress, CB_CURL& fnWrite, CB_CURL& fnRead)
        : fnProgress(fnProgress)
        , fnWrite(fnWrite)
        , fnRead(fnRead)
    {
    }

public:
    CB_CURLProgress fnProgress;
    CB_CURL fnWrite;
    CB_CURL fnRead;
};

class __UtilExt tagCURlToolDownloadHook : public tagCURlToolHook
{
public:
    tagCURlToolDownloadHook(CB_CURLProgress& fnProgress, CB_CURL& fnWrite=NULL)
        : tagCURlToolHook(fnProgress, fnWrite, NULL)
    {
    }

    tagCURlToolDownloadHook(signal_t bRunSignal, CB_CURL& fnWrite=NULL)
        : tagCURlToolHook([&](int64_t, int64_t){
        if (!bRunSignal)
        {
            return -1;
        }
        return 0;
    }, fnWrite, NULL)
    {
    }
};

class __UtilExt tagCURlToolUploadHook : public tagCURlToolHook
{
public:
    tagCURlToolUploadHook(CB_CURLProgress& fnProgress, CB_CURL& fnRead=NULL)
        : tagCURlToolHook(fnProgress, NULL, fnRead)
    {
    }

    tagCURlToolUploadHook(signal_t bRunSignal, CB_CURL& fnRead=NULL)
        : tagCURlToolHook([&](int64_t, int64_t){
        if (!bRunSignal)
        {
            return -1;
        }
        return 0;
    }, NULL, fnRead)
    {
    }
};

class __UtilExt curlutil
{
public:
    static int initCurl(string& strVerInfo);
    static void freeCurl();

    static int curlToolPerform(const list<string>& lstParams, tagCURlToolHook *pCURLToolHook = NULL);

    static string getCurlErrMsg(UINT uCurlCode);

    template <class T>
    static int curlDownload(const tagCurlOpt& curlOpt, const string& strURL, T t, CB_CURLProgress& cbProgress = NULL)
    {
        return _curlDownload(curlOpt, strURL, t, cbProgress);
    }

    template <class T>
    static int curlDownload(signal_t bRunSignal, const tagCurlOpt& curlOpt, const string& strUrl, T t)
    {
        return _curlDownload(curlOpt, strUrl, t, [&](int64_t, int64_t){
            if (!bRunSignal)
            {
                return -1;
            }
            return 0;
        });
    }

    template <class T>
    static int curlDownload(CB_CURLProgress& cbProgress, const string& strUrl, T t
                     , unsigned long timeout, unsigned long connectTimeout = 3)
    {
        tagCurlOpt curlOpt(false);
        curlOpt.connectTimeout = connectTimeout;
        curlOpt.timeout = timeout;
        return curlDownload(curlOpt, strUrl, t, cbProgress);
    }

    template <class T>
    static int curlDownload(signal_t bRunSignal, const string& strUrl, T t
                     , unsigned long timeout, unsigned long connectTimeout = 3)
    {
        tagCurlOpt curlOpt(false);
        curlOpt.connectTimeout = connectTimeout;
        curlOpt.timeout = timeout;
        return curlDownload(bRunSignal, curlOpt, strUrl, t);
    }

private:
    static int _curlDownload(const tagCurlOpt& curlOpt, const string& strURL, CB_CURL& cbWrite
                             , CB_CURLProgress& cbProgress);

    // 适合下载短文件，可以忽略cbfProfile->append的开销
    static int _curlDownload(const tagCurlOpt& curlOpt, const string& strUrl, CCharBuffer& cbfProfile
                     , CB_CURLProgress& cbProgress);

    inline static int _curlDownload(const tagCurlOpt& curlOpt, const string& strUrl, FILE *pf
                            , CB_CURLProgress& cbProgress)
    {
        if (NULL == pf)
        {
            return -1;
        }

        return _curlDownload(curlOpt, strUrl, [&](char *ptr, size_t size, size_t nmemb){
            return fwrite(ptr, size, nmemb, pf);
        }, cbProgress);
    }

    static int _curlDownload(const tagCurlOpt& curlOpt, const string& strUrl, cwstr strFile
                            , CB_CURLProgress& cbProgress)
    {
        auto pf = fsutil::fopen(strFile, "wb");
        return _curlDownload(curlOpt, strUrl, pf, cbProgress);
    }
    static int _curlDownload(const tagCurlOpt& curlOpt, const string& strUrl, const string& strFile
                            , CB_CURLProgress& cbProgress)
    {
        auto pf = fsutil::fopen(strFile, "wb");
        return _curlDownload(curlOpt, strUrl, pf, cbProgress);
    }
};

//using CB_DownloadRecv = const function<bool(char *ptr, size_t size)>&;
using CB_DownloadProgress = const function<bool(time_t beginTime, int64_t dltotal, int64_t dlnow)>&;

class __UtilExt CCurlDownload
{
public:
    virtual ~CCurlDownload() = default;

protected:
    CCurlDownload(bool bShare = false, unsigned long connectTimeout = 3, unsigned long timeout = 0
            , unsigned long lowSpeedLimit = 0, unsigned long lowSpeedLimitTime = 0, unsigned long maxSpeedLimit = 0)
        : m_curlOpt(bShare)
    {
        m_curlOpt.connectTimeout = connectTimeout;
        m_curlOpt.timeout = timeout;

        m_curlOpt.lowSpeedLimit = lowSpeedLimit; // 单位字节
        m_curlOpt.lowSpeedLimitTime = lowSpeedLimitTime;

        m_curlOpt.maxSpeedLimit = maxSpeedLimit;
    }

    CCurlDownload(const tagCurlOpt& curlOpt) : m_curlOpt(curlOpt)
    {
    }

protected:
    bool m_bStatus = false;

private:
    tagCurlOpt m_curlOpt;

    XThread m_thread;

    time_t m_beginTime = 0;

    uint64_t m_uRecvSize = 0;
    uint64_t m_uTotalSize = 0;

private:
    virtual bool _onRecv(char *ptr, size_t size) = 0;

    virtual void clear() {}

    int _syncDownload(const string& strUrl, UINT uRetryTime, CB_DownloadProgress cbProgress);

public:
    const bool& status() const
    {
        return m_bStatus;
    }

    uint64_t recvSize() const
    {
        return m_uRecvSize;
    }

    uint64_t totalSize() const
    {
        return m_uTotalSize;
    }

    int syncDownload(const string& strUrl, UINT uRetryTime = 0, CB_DownloadProgress cbProgress = NULL);

    void asyncDownload(const string& strUrl, UINT uRetryTime, cfn_void_t<int> cbFinish
            , CB_DownloadProgress cbProgress = NULL);

    uint64_t cancel();
};

class __UtilExt CDownloader : public CCurlDownload
{
public:
    CDownloader() : CCurlDownload(false)
    {
    }

    CDownloader(const tagCurlOpt& curlOpt) : CCurlDownload(curlOpt)
    {
    }

    CDownloader(unsigned long connectTimeout, unsigned long timeout = 0
            , unsigned long lowSpeedLimit = 0, unsigned long lowSpeedLimitTime = 0, unsigned long maxSpeedLimit = 0)
        : CCurlDownload(false, connectTimeout, timeout, lowSpeedLimit, lowSpeedLimitTime, maxSpeedLimit)
    {
    }

private:
    mutex m_mtxDataLock;
    list<string> m_lstData;
    size_t m_uDataSize = 0;

private:
    virtual bool _onRecv(char *ptr, size_t size) override;

    template <class T>
    inline int _syncDownload(const string& strUrl, T& retData, UINT uRetryTime, CB_DownloadProgress cbProgress)
    {
        int nRet = CCurlDownload::syncDownload(strUrl, uRetryTime, cbProgress);
        if (0 == nRet)
        {
            (void)_getAllData(retData);
        }
        return nRet;
    }

    template <class T>
    size_t _getAllData(T& buff)
    {
        size_t uReadSize = m_uDataSize;
        if (0 == uReadSize)
        {
            return 0;
        }

        auto ptr = buff.resizeMore(uReadSize);
        auto size = getData((byte_p)ptr, uReadSize);
        if (size < 0)
        {
            size = 0;
        }
        if ((size_t)size < uReadSize)
        {
            buff.resizeLess(uReadSize - (size_t)size);
        }

        return (size_t)size;
    }

public:
    int syncDownload(const string& strUrl, CByteBuffer& bbfData, UINT uRetryTime = 0
            , CB_DownloadProgress cbProgress = NULL)
    {
        return _syncDownload(strUrl, bbfData, uRetryTime, cbProgress);
    }

    int syncDownload(signal_t bRunSiganl, const string& strUrl, CByteBuffer& bbfData, UINT uRetryTime = 0)
    {
        return _syncDownload(strUrl, bbfData, uRetryTime, [&](time_t, int64_t, int64_t){
            return bRunSiganl;
        });
    }

    int syncDownload(const string& strUrl, CCharBuffer& cbfData, UINT uRetryTime = 0
            , CB_DownloadProgress cbProgress = NULL)
    {
        return _syncDownload(strUrl, cbfData, uRetryTime, cbProgress);
    }

    int syncDownload(signal_t bRunSiganl, const string& strUrl, CCharBuffer& cbfData, UINT uRetryTime = 0)
    {
        return _syncDownload(strUrl, cbfData, uRetryTime, [&](time_t, int64_t, int64_t){
            return bRunSiganl;
        });
    }

    void asyncDownload(const string& strUrl, UINT uRetryTime, cfn_void_t<int> cbFinish
            , CB_DownloadProgress cbProgress = NULL)
    {
        CCurlDownload::asyncDownload(strUrl, uRetryTime, cbFinish, cbProgress);
    }

    void asyncDownload(signal_t bRunSiganl, const string& strUrl, UINT uRetryTime
            , cfn_void_t<int> cbFinish)
    {
        CCurlDownload::asyncDownload(strUrl, uRetryTime, cbFinish, [&](time_t, int64_t, int64_t){
            return bRunSiganl;
        });
    }

    size_t dataSize() const
    {
        return m_uDataSize;
    }
//    uint64_t readPos() const
//    {
//        return m_uRecvSize - m_uDataSize;
//    }

    int getData(byte_p pBuff, size_t buffSize);

    int getData(TD_ByteBuffer& buff)
    {
        return getData(buff, buff.size());
    }

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

    void clear() override;
};
