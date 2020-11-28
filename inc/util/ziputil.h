
#pragma once

struct __UtilExt tagUnzSubFile
{
    string strPath;

    unsigned long compressed_size = 0;
    unsigned long uncompressed_size = 0;

    unsigned long pos_in_zip_directory = 0;
    unsigned long num_of_file = 0;
};

struct tagUnzDir
{
    tagUnzDir() = default;

    tagUnzDir(const string& strPath)
        : strPath(strPath)
    {
    }

    string strPath;

    list<tagUnzDir> lstSubDir;

    PtrArray<tagUnzSubFile> paSubFile;
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

    void* m_pfile = NULL;

    tagUnzDir m_unzRoot;
    list<string> m_lstSubDir;
    map<string, tagUnzSubFile> m_mapSubfile;

    const tagUnzSubFile *m_pCurrent = NULL;

private:
    bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);

    bool _unzOpen(const tagUnzSubFile& unzSubFile) const;
    bool _unzOpen() const;

    long _read(const tagUnzSubFile& unzSubFile, void *buf, size_t len) const;

public:
    operator bool() const
    {
        return m_pfile != NULL;
    }

    const list<string>& subDirList() const
    {
        return m_lstSubDir;
    }

    const map<string, tagUnzSubFile>& subFileMap() const
    {
        return m_mapSubfile;
    }

    bool open(const string& strFile, const string& strPwd = "")
    {
        m_strPwd = strPwd;

        return _open(strFile.c_str());
    }

    bool open(Instream& ins, const string& strPwd = "");

    void close();

    const tagUnzSubFile* unzCurrent() const
    {
        return m_pCurrent;
    }
    bool unzOpen(const tagUnzSubFile& unzSubFile);
    long unzRead(void *buf, size_t len) const;
    void unzClose();

    long read(const tagUnzSubFile& unzSubFile, CByteBuffer& bbfBuff) const
    {
        if (0 == unzSubFile.uncompressed_size)
        {
            return 0;
        }

        auto ptr = bbfBuff.resizeMore(unzSubFile.uncompressed_size);
        return _read(unzSubFile, ptr, unzSubFile.uncompressed_size);
    }
    long read(const tagUnzSubFile& unzSubFile, CCharBuffer& cbfBuff) const
    {
        if (0 == unzSubFile.uncompressed_size)
        {
            return 0;
        }

        auto ptr = cbfBuff.resizeMore(unzSubFile.uncompressed_size);
        return _read(unzSubFile, ptr, unzSubFile.uncompressed_size);
    }

    bool unzip(const string& strDstDir) const;
    long unzip(const tagUnzSubFile& unzSubFile, const string& strDstFile) const;
    long unzip(const string& strSubFilePath, const string& strDstFile) const
    {
        auto itr = m_mapSubfile.find(strSubFilePath);
        if (itr == m_mapSubfile.end())
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
