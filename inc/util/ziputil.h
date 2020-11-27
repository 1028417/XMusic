
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

    prlist<bool, tagUnzfile*> m_lstUnzfile;

    const tagUnzfile *m_pCurrent = NULL;

private:
    bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);

    bool _unzOpen(const tagUnzfile& unzFile) const;
    bool _unzOpen() const;

    long _read(const tagUnzfile& unzFile, void *buf, size_t len) const;

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

    const prlist<bool, tagUnzfile*>& unzfileList() const
    {
        return m_lstUnzfile;
    }

    bool open(const string& strFile, const string& strPwd = "")
    {
        m_strPwd = strPwd;

        return _open(strFile.c_str());
    }

    bool open(Instream& ins, const string& strPwd = "");

    void close();

    const tagUnzfile* unzCurrent() const
    {
        return m_pCurrent;
    }
    bool unzOpen(const tagUnzfile& unzFile);
    long unzRead(void *buf, size_t len) const;
    void unzClose();

    long read(const tagUnzfile& unzFile, CByteBuffer& bbfBuff) const
    {
        if (0 == unzFile.uFileSize)
        {
            return 0;
        }

        auto ptr = bbfBuff.resizeMore(unzFile.uFileSize);
        return _read(unzFile, ptr, unzFile.uFileSize);
    }
    long read(const string& strSubFilePath, CByteBuffer& bbfBuff) const
    {
        auto itr = m_mapUnzfile.find(strSubFilePath);
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
    long read(const string& strSubFilePath, CCharBuffer& cbfBuff) const
    {
        auto itr = m_mapUnzfile.find(strSubFilePath);
        if (itr == m_mapUnzfile.end())
        {
            return -1;
        }

        return read(itr->second, cbfBuff);
    }

    bool unzip(const string& strDstDir) const;
    long unzip(const tagUnzfile& unzFile, const string& strDstFile) const;
    long unzip(const string& strSubFilePath, const string& strDstFile) const
    {
        auto itr = m_mapUnzfile.find(strSubFilePath);
        if (itr == m_mapUnzfile.end())
        {
            return -1;
        }

        return unzip(itr->second, strDstFile);
    }
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
    static long zCompressFile(cwstr strSrcFile, cwstr strDstFile, int level=0);
    static long zUncompressFile(cwstr strSrcFile, cwstr strDstFile);

#if !__winvc
    static long qCompressFile(cwstr strSrcFile, cwstr strDstFile, int nCompressLecvel = -1);
    static long qUncompressFile(cwstr strSrcFile, cwstr strDstFile);
#endif
};
