
#pragma once

struct __UtilExt tagUnzSubFile
{
    string strPath;

    unsigned long compression_method = 0;
    unsigned long compressed_size = 0;
    unsigned long uncompressed_size = 0;

    unsigned long pos_in_zip_directory = 0;
    unsigned long num_of_file = 0;

    uint64_t data_pos = 0;
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

    CZipFile(IFStream& ifs, const string& strPwd = "")
    {
        (void)open(ifs, strPwd);
    }

    virtual ~CZipFile()
    {
        close();
    }

private:
    string m_strPwd;

    void* m_pfile = NULL;

    list<string> m_lstSubDir;
    map<string, tagUnzSubFile> m_mapSubfile;

private:
    bool _open(const char *szFile, void *pzlib_filefunc_def, const string& strPwd);
    bool _open(void *opaque, void *zread, void *ztell, void *zseek, void *zclose, const string& strPwd);

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

    bool open(FILE *pf, const string& strPwd = "");

    bool open(cwstr strFile, const string& strPwd = "")
    {
        auto pf = fsutil::fopen(strFile, "rb");
        if (NULL == pf)
        {
            return false;
        }

        return open(pf, strPwd);
    }

    bool open(const string& strFile, const string& strPwd = "") // 原生
    {
        return _open(strFile.c_str(), NULL, strPwd);
    }

    bool open(Instream& ins, const string& strPwd = "");
    bool open(IFStream& ifs, const string& strPwd = "");

    void close();

    bool unzOpen(const tagUnzSubFile& unzSubFile) const;
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

	static int zCompress(const void* pData, size_t len, CByteBuffer& bbfBuff, int level = 0);
	static int zUncompress(const void* pData, size_t len, CByteBuffer& bbfBuff);

    static long zCompressFile(cwstr strSrcFile, cwstr strDstFile, int level=0);
    static long zUncompressFile(cwstr strSrcFile, cwstr strDstFile);

#if !__winvc
    static long qCompressFile(cwstr strSrcFile, cwstr strDstFile, int nCompressLecvel = -1);
    static long qUncompressFile(cwstr strSrcFile, cwstr strDstFile);
#endif
};
