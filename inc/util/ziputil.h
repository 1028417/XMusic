
#pragma once

struct __UtilExt tagUnzFile
{
    wstring strPath;

    unsigned long compression_method = 0;
    uint64_t compressed_size = 0;
    uint64_t uncompressed_size = 0;

    uint64_t pos_in_zip_directory = 0;
    uint64_t num_of_file = 0;

    uint64_t data_pos = 0;
};

class __UtilExt CUnzDir
{
friend class CZipFile;
public:
    CUnzDir() = default;

public:
    map<wstring, CUnzDir> mapSubDir;
    map<wstring, tagUnzFile> mapSubFile;
    //PtrArray<tagUnzFile*> paSubFile;

private:
    inline const CUnzDir* _subDir(cwstr strName) const;

    void clear()
    {
        mapSubDir.clear();
        mapSubFile.clear();
        //paSubFile.clear();
    }

protected:
    CUnzDir& addSubDir(wstring strSubDir);
    tagUnzFile& addSubFile(wstring strSubFile);

public:
    const CUnzDir* subDir(wstring strSubDir) const;
    //未实现不区分大小写
    const tagUnzFile* subFile(wstring strSubFile) const;

    void enumSubFile(cfn_void_t<const tagUnzFile&> cb) const
    {
        for (cauto pr : mapSubFile)
        {
            cb(pr.second);
        }

        for (cauto pr : mapSubDir)
        {
            pr.second.enumSubFile(cb);
        }
    }
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

    CUnzDir m_root;

    list<pair<wstring, CUnzDir*>> m_lstSubDir;
    list<pair<wstring, tagUnzFile*>> m_lstSubFile;

private:
    bool _open(const char *szFile, void *pzlib_filefunc_def, const string& strPwd);
    bool _open(void *opaque, void *zread, void *ztell, void *zseek, void *zclose, const string& strPwd);

    bool _unzOpen() const;

    long _read(const tagUnzFile& unzFile, void *buf, size_t len) const;

public:
    operator bool() const
    {
        return m_pfile != NULL;
    }

    const CUnzDir& root() const
    {
        return m_root;
    }

    const CUnzDir* subDir(wstring strSubDir) const
    {
        return m_root.subDir(strSubDir);
    }
    //未实现不区分大小写
    const tagUnzFile* subFile(wstring strSubFile) const
    {
        return m_root.subFile(strSubFile);
    }

    const list<pair<wstring, CUnzDir*>>& subDirList() const
    {
        return m_lstSubDir;
    }
    const list<pair<wstring, tagUnzFile*>>& subFileList() const
    {
        return m_lstSubFile;
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

    bool open(const string& strFile, const string& strPwd = "") // 原生，windows gbk，其他utf8
    {
        return _open(strFile.c_str(), NULL, strPwd);
    }

    bool open(Instream& ins, const string& strPwd = "");
    bool open(IFStream& ifs, const string& strPwd = "");

    void close();

    bool unzOpen(const tagUnzFile& unzFile) const;
    long unzRead(void *buf, size_t len) const;
    void unzClose();

    long read(const tagUnzFile& unzFile, CByteBuffer& bbfBuff) const
    {
        if (0 == unzFile.uncompressed_size)
        {
            return 0;
        }

        auto ptr = bbfBuff.resizeMore(unzFile.uncompressed_size);
        return _read(unzFile, ptr, unzFile.uncompressed_size);
    }
    long read(const tagUnzFile& unzFile, CCharBuffer& cbfBuff) const
    {
        if (0 == unzFile.uncompressed_size)
        {
            return 0;
        }

        auto ptr = cbfBuff.resizeMore(unzFile.uncompressed_size);
        return _read(unzFile, ptr, unzFile.uncompressed_size);
    }
    template <class T>
    long read(cwstr strSubFile, T& buff) const
    {
        auto pUnzFile = this->subFile(strSubFile);
        if (NULL == pUnzFile)
        {
            return -1;
        }
        return read(*pUnzFile, buff);
    }

    void readDir(const CUnzDir& unzDir, const function<void(const tagUnzFile&, CByteBuffer&)>& cb) const
    {
        unzDir.enumSubFile([&](const tagUnzFile& unzFile){
            CByteBuffer bbfData;
            if (read(unzFile, bbfData) > 0)
            {
                cb(unzFile, bbfData);
            }
        });
    }
    void readDir(const CUnzDir& unzDir, const function<void(const tagUnzFile&, IFBuffer&)>& cb) const
    {
        unzDir.enumSubFile([&](const tagUnzFile& unzFile){
            CByteBuffer bbfData;
            if (read(unzFile, bbfData) > 0)
            {
                IFBuffer ifbData(bbfData);
                cb(unzFile, ifbData);
            }
        });
    }

    void readSubDir(cwstr strSubDir, const function<void(const tagUnzFile&, CByteBuffer&)>& cb) const
    {
        auto pUnzDir = this->subDir(strSubDir);
        if (pUnzDir)
        {
            readDir(*pUnzDir, cb);
        }
    }
    void readSubDir(cwstr strSubDir, const function<void(const tagUnzFile&, IFBuffer&)>& cb) const
    {
        auto pUnzDir = this->subDir(strSubDir);
        if (pUnzDir)
        {
            readDir(*pUnzDir, cb);
        }
    }

    template <class T>
    void readAllFile(const T& cb) const
    {
        readDir(m_root, cb);
    }

    bool unzipAll(cwstr strDstDir) const;

    long unzip(const tagUnzFile& unzFile, cwstr strDstFile) const;
};

enum E_ZMethod
{
	ZM_None = 0,
	ZM_Deflated,
	ZM_BZip2ed
};

class __UtilExt ziputil
{
public:
	static bool zipDir(const string& strSrcDir, const string& strDstFile, E_ZMethod method = E_ZMethod::ZM_Deflated, int level = 0);
    // windows gbk, 其他utf8
    static bool unzFile(const string& strZipFile, cwstr strDstDir, const string& strPwd = "")
    {
            return CZipFile(strZipFile, strPwd).unzipAll(strDstDir);
    }
    static bool unzFile(Instream& ins, cwstr strDstDir, const string& strPwd = "")
    {
            return CZipFile(ins, strPwd).unzipAll(strDstDir);
    }

    static int zCompress(const void *pData, size_t len, CByteBuffer& bbfBuff, int level = 0);
    static int zUncompress(const void *pData, size_t len, CByteBuffer& bbfBuff);

    static long zCompressFile(cwstr strSrcFile, cwstr strDstFile, int level=0);
    static long zUncompressFile(cwstr strSrcFile, cwstr strDstFile);

#if !__winvc
    static long qCompressFile(cwstr strSrcFile, cwstr strDstFile, int nCompressLecvel = -1);
    static long qUncompressFile(cwstr strSrcFile, cwstr strDstFile);
#endif
};
