
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
friend class CUnZip;
public:
    CUnzDir() = default;

private:
    wstring m_strName;

    map<wstring, CUnzDir> m_mapSubDir;
    map<wstring, tagUnzFile> m_mapSubFile;
    //PtrArray<tagUnzFile*> m_paSubFile;

private:
    inline const CUnzDir* _subDir(cwstr strDirName) const
    {
        auto itr = m_mapSubDir.find(strDirName);
        if (itr == m_mapSubDir.end())
        {
            return NULL;
        }
        return &itr->second;
    }

    inline CUnzDir& _addSubDir(cwstr strDirName)
    {
        auto& unzDir = m_mapSubDir[strDirName];
        unzDir.m_strName = strDirName;
        return unzDir;
    }

    CUnzDir& addSubDir(wstring strSubDir);
    tagUnzFile& addSubFile(wstring strSubFile);

public:
    void clear()
    {
        m_mapSubDir.clear();
        m_mapSubFile.clear();
        //m_paSubFile.clear();
    }

    cwstr name() const
    {
        return m_strName;
    }

    const map<wstring, CUnzDir>& subDirMap() const
    {
        return m_mapSubDir;
    }

    const map<wstring, tagUnzFile>& subFileMap() const
    {
        return m_mapSubFile;
    }

    const CUnzDir* subDir(wstring strSubDir) const;
    //未实现不区分大小写
    const tagUnzFile* subFile(wstring strSubFile) const;

    void enumSubFile(cfn_void_t<const tagUnzFile&> cb) const
    {
        for (cauto pr : m_mapSubFile)
        {
            cb(pr.second);
        }

        for (cauto pr : m_mapSubDir)
        {
            pr.second.enumSubFile(cb);
        }
    }
};

class __UtilExt CUnZip
{
public:
    CUnZip() = default;

    CUnZip(const string& strFile, const string& strPwd = "")
    {
        (void)open(strFile, strPwd);
    }

    CUnZip(Instream& ins, const string& strPwd = "")
    {
        (void)open(ins, strPwd);
    }

    CUnZip(IFStream& ifs, const string& strPwd = "")
    {
        (void)open(ifs, strPwd);
    }

    virtual ~CUnZip()
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

	template <class T>
	long _read(const tagUnzFile& unzFile, T& buff) const
	{
		if (0 == unzFile.uncompressed_size)
		{
			return 0;
		}
		auto size = (size_t)unzFile.uncompressed_size;
		if (unzFile.uncompressed_size > size)
		{
			return 0;
		}

		auto ptr = buff.resizeMore(size);
		return _read(unzFile, ptr, size);
	}

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

    bool open(const string& strFile, const string& strPwd = "") // windows gbk路径，其他utf8路径
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
		return _read(unzFile, bbfBuff);
	}
	long read(const tagUnzFile& unzFile, CCharBuffer& cbfBuff) const
	{
		return _read(unzFile, cbfBuff);
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

struct tagMinZipSrc
{
	tagMinZipSrc() = default;

	tagMinZipSrc(const string& strFile, const string& strInnerPath, E_ZMethod method = E_ZMethod::ZM_Deflated, int level = 0)
		: strFile(strFile)
		, strInnerPath(strInnerPath)
		, method(method)
		, level(level)
	{
	}

	string strFile;
	string strInnerPath;
	E_ZMethod method = E_ZMethod::ZM_Deflated;
	int level = 0;
};

class __UtilExt ziputil
{
public:
	static int zDir(bool bKeetRoot, const string& src, const string& dest, E_ZMethod method=E_ZMethod::ZM_Deflated, int level=0);
	static int zFile(const string& src, const string& dest, E_ZMethod method=E_ZMethod::ZM_Deflated, int level=0)
	{
		return zFiles({ tagMinZipSrc(src, fsutil::GetFileName(src), method, level) }, dest);
	}
	static int zFiles(const list<tagMinZipSrc>& lstSrc, const string& dest);
    // windows gbk路径, 其他utf8路径
    static bool unzFile(const string& strZipFile, cwstr strDstDir, const string& strPwd = "")
    {
            return CUnZip(strZipFile, strPwd).unzipAll(strDstDir);
    }
    static bool unzFile(Instream& ins, cwstr strDstDir, const string& strPwd = "")
    {
            return CUnZip(ins, strPwd).unzipAll(strDstDir);
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
