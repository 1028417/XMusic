
#pragma once

struct __UtilExt tagUnzfile
{
    string strPath;

    size_t uFileSize = 0;

    size_t pos_in_zip_directory = 0;
    size_t num_of_file = 0;
};

class __UtilExt CZipFile
{
public:
    CZipFile() = default;

    CZipFile(const string& strFile, const string& strPwd = "")
    {
        (void)open(strFile, strPwd);
    }

    CZipFile(Instream& ins, const string& strPwd = "")
    {
        (void)open(ins, strPwd);
    }

    virtual ~CZipFile()
    {
        close();
    }

private:
    string m_strPwd;

    void* m_unzfile = NULL;

    list<tagUnzfile> m_lstUnzdirInfo;

    map<string, tagUnzfile> m_mapUnzfile;

    list<pair<bool, tagUnzfile*>> m_lstUnzfile;

private:
    bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);
	
    long _readCurrent(void *buf, size_t len) const;

public:
    operator bool() const
    {
        return m_unzfile != NULL;
    }

    const map<string, tagUnzfile>& unzfileMap() const
    {
        return m_mapUnzfile;
    }

    const list<tagUnzfile>& unzdirList() const
    {
        return m_lstUnzdirInfo;
    }

    const list<pair<bool, tagUnzfile*>>& unzfileList() const
    {
        return m_lstUnzfile;
    }

    bool open(const string& strFile, const string& strPwd = "")
    {
        m_strPwd = strPwd;

        return _open(strFile.c_str());
    }

    bool open(Instream& ins, const string& strPwd = "");

    long read(const tagUnzfile& unzFile, CByteBuffer& bbfBuff) const
    {
        if (0 == unzFile.uFileSize)
        {
            return 0;
        }

        auto ptr = bbfBuff.resizeMore(unzFile.uFileSize);
        return _read(unzFile, ptr, unzFile.uFileSize);
    }
    long read(const string& strUnzfile, CByteBuffer& bbfBuff) const
    {
        auto itr = m_mapUnzfile.find(strUnzfile);
        if (itr == m_mapUnzfile.end())
        {
            return -1;
        }

        return read(itr->second, bbfBuff);
    }

    long read(const tagUnzfile& unzFile, CCharBuffer& cbfBuff) const
    {
        if (0 == unzFile.uFileSize)
        {
            return 0;
        }

        auto ptr = cbfBuff.resizeMore(unzFile.uFileSize);
        return _read(unzFile, ptr, unzFile.uFileSize);
    }
    long read(const string& strUnzfile, CCharBuffer& cbfBuff) const
    {
        auto itr = m_mapUnzfile.find(strUnzfile);
        if (itr == m_mapUnzfile.end())
        {
            return -1;
        }

        return read(itr->second, cbfBuff);
    }

    long unzip(const tagUnzfile& unzFile, const string& strDstFile);
    long unzip(const string& strUnzfile, const string& strDstFile)
    {
        auto itr = m_mapUnzfile.find(strUnzfile);
        if (itr == m_mapUnzfile.end())
        {
            return -1;
        }

        return unzip(itr->second, strDstFile);
    }

    bool unzip(const string& strDstDir) const;

    void close();
	
private:
    long _read(const tagUnzfile& unzFile, void *buf, size_t len) const;
};

class __UtilExt ziputil
{
public:
    static bool unzFile(const string& strZipFile, const string& strDstDir, const string& strPwd = "")
    {
            return CZipFile(strZipFile, strPwd).unzip(strDstDir);
    }
    static bool unzFile(Instream& ins, const string& strDstDir, const string& strPwd = "")
    {
            return CZipFile(ins, strPwd).unzip(strDstDir);
    }

    static int zCompress(const void* pData, size_t len, CByteBuffer& bbfBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
    static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
    static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
