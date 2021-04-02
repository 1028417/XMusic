
#pragma once

struct __UtilExt tagUnzSubFile
{
    wstring strPath;

    unsigned long compression_method = 0;
    unsigned long compressed_size = 0;
    unsigned long uncompressed_size = 0;

    unsigned long pos_in_zip_directory = 0;
    unsigned long num_of_file = 0;

    uint64_t data_pos = 0;
};

struct __UtilExt tagUnzDir
{
    /*tagUnzDir() = default;

    tagUnzDir(const tagUnzDir& parent, cwstr strName)
        : parent(&parent)
        , strName(strName)
    {
    }

    wstring path() const
    {
        if (NULL == parent)
        {
            return L"";
        }

        return parent.path() + L"/" + strName;
    }

    const tagUnzDir *parent = NULL;
    wstring strName;*/

    map<wstring, tagUnzDir> mapSubDir;
    map<wstring, tagUnzSubFile> mapSubFile;
    //PtrArray<tagUnzSubFile*> paSubFile;

    inline tagUnzDir& _addDir(cwstr strName)
    {
        auto& unzSubDir = mapSubDir[strName];
        //unzSubDir.parent = this;
        //unzSubDir.strName = strName;
        return unzSubDir;
    }

    tagUnzDir& addDir(wstring strSubDir);
    tagUnzSubFile& addFile(wstring strSubFile);

    const tagUnzDir* subDir(wstring strSubDir) const;
    const tagUnzSubFile* subFile(wstring strSubFile) const;

    void enumSubFile(cfn_void_t<const tagUnzSubFile&> cb) const
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

    void clear()
    {
        mapSubDir.clear();
        mapSubFile.clear();
        //paSubFile.clear();
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

    tagUnzDir m_root;

    map<wstring, tagUnzDir*> m_mapSubDir;
    map<wstring, tagUnzSubFile*> m_mapSubfile;

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

    const tagUnzDir& root() const
    {
        return m_root;
    }

    const tagUnzDir* subDir(wstring strSubDir) const
    {
        return m_root.subDir(strSubDir);
    }

    const tagUnzSubFile* subFile(wstring strSubFile) const
    {
        return m_root.subFile(strSubFile);
    }

    const map<wstring, tagUnzDir*>& subDirMap() const
    {
        return m_mapSubDir;
    }

    const map<wstring, tagUnzSubFile*>& subFileMap() const
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

    void readDir(const tagUnzDir& unzDir, const function<void(const tagUnzSubFile&, CByteBuffer&)>& cb) const
    {
        unzDir.enumSubFile([&](const tagUnzSubFile& unzSubFile){
            CByteBuffer bbfData;
            if (read(unzSubFile, bbfData) > 0)
            {
                cb(unzSubFile, bbfData);
            }
        });
    }
    void readDir(const tagUnzDir& unzDir, const function<void(const tagUnzSubFile&, IFBuffer&)>& cb) const
    {
        unzDir.enumSubFile([&](const tagUnzSubFile& unzSubFile){
            CByteBuffer bbfData;
            if (read(unzSubFile, bbfData) > 0)
            {
                IFBuffer ifbData(bbfData);
                cb(unzSubFile, ifbData);
            }
        });
    }

    void readSubDir(cwstr strSubDir, const function<void(const tagUnzSubFile&, CByteBuffer&)>& cb) const
    {
        auto pUnzDir = this->subDir(strSubDir);
        if (pUnzDir)
        {
            readDir(*pUnzDir, cb);
        }
    }
    void readSubDir(cwstr strSubDir, const function<void(const tagUnzSubFile&, IFBuffer&)>& cb) const
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

    long unzip(const tagUnzSubFile& unzSubFile, cwstr strDstFile) const;
    long unzip(cwstr strSubFile, cwstr strDstFile) const
    {
        auto itr = m_mapSubfile.find(strSubFile);
        if (itr == m_mapSubfile.end())
        {
            return false;
        }

        return unzip(*itr->second, strDstFile);
    }
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
