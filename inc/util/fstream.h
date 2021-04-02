
#pragma once

#define fsize_t_max ((size_t)-1)
#if __windows || __android
#define fsize_t(fsize64) (size_t(MIN((uint64_t)fsize_t_max, fsize64)))
#else
#define fsize_t(fsize64) fsize64
#endif

class __UtilExt FStream
{
public:
	virtual ~FStream()
	{
		close();
	}

protected:
    FStream() = default;
	
	FStream(cwstr strFile, const string& strMode)
	{
		(void)_open(strFile, strMode);
	}
    FStream(const string& strFile, const string& strMode)
    {
        (void)_open(strFile, strMode);
    }

	FStream(const FStream& other) = delete;
	FStream(FStream&& other) = delete;
	FStream& operator=(const FStream& other) = delete;
	FStream& operator=(FStream&& other) = delete;

protected:
    FILE *m_pf = NULL;

protected:
    template <typename T>
    bool _open(const T& file, const string& strMode)
    {
		close();

        m_pf = fsutil::fopen(file, strMode);
		return NULL != m_pf;
    }
	
public:
    operator bool() const
    {
        return m_pf != NULL;
    }

    bool is_open() const
    {
        return m_pf != NULL;
    }

    bool eof() const
    {
        if (NULL == m_pf)
        {
            return true;
        }

        return feof(m_pf);
    }

    void close()
    {
        if (m_pf)
        {
            fclose(m_pf);
            m_pf = NULL;
        }
    }
};

class __UtilExt Instream
{
public:
    Instream() = default;
    virtual ~Instream() = default;

private:
    virtual size_t _read(void *buff, size_t size, size_t count) = 0;

public:
    size_t read(void *buff, size_t size, size_t count)
    {
        return _read(buff, size, count);
    }

    inline size_t read(void *buff, size_t size)
    {
        return _read(buff, 1, size);
    }

    inline bool readex(void *buff, size_t size)
    {
        return _read(buff, size, 1) == 1;
    }

    template <typename T>
    size_t read(TBuffer<T>& buff)
    {
        return read(buff, sizeof(T), buff.count());
    }
    template <typename T>
    bool readex(TBuffer<T>& buff)
    {
        return readex(buff, buff.size());
    }

    size_t read(CByteBuffer& bbfBuff, size_t uReadSize = 0)
    {
        return _read(bbfBuff, uReadSize);
    }

    size_t read(CCharBuffer& cbfBuff, size_t uReadSize = 0)
    {
        return _read(cbfBuff, uReadSize);
    }

    virtual bool seek(long long offset, int origin) = 0;

    virtual long long pos() = 0;

    virtual uint64_t size() = 0;

private:
    template <class T>
    size_t _read(T& buff, size_t uReadSize = 0)
    {
        long long pos = this->pos();
        if (pos < 0)
        {
            return 0;
        }

        uint64_t max64 = this->size() - (uint64_t)pos;
        size_t max = fsize_t(max64);
        if (0 == uReadSize)
        {
            uReadSize = max;
        }
        else
        {
            uReadSize = MIN(uReadSize, max);
        }

        auto ptr = buff.resizeMore(uReadSize);
        size_t size = read(ptr, uReadSize);
        if (size < uReadSize)
        {
            buff.resizeLess(uReadSize - size);
        }

        return size;
    }
};

class __UtilExt IFBuffer : public Instream
{
protected:
    IFBuffer() = default;

public:
    virtual ~IFBuffer() = default;

    IFBuffer(cbyte_p ptr, size_t size)
        : m_ptr(ptr)
        , m_size(size)
    {
    }

    IFBuffer(const CByteBuffer& bbfBuff)
        : m_ptr(bbfBuff)
        , m_size(bbfBuff->size())
    {
    }

    IFBuffer(const IFBuffer& other)
    {
        m_ptr = other.m_ptr;
        m_size = other.m_size;
        m_pos = 0;
    }

    IFBuffer& operator=(const IFBuffer& other)
    {
        m_ptr = other.m_ptr;
        m_size = other.m_size;
        m_pos = 0;
        return *this;
    }

protected:
    cbyte_p m_ptr;
    uint64_t m_size;

    uint64_t m_pos = 0;

private:
    virtual size_t _read(void *buff, size_t size, size_t count) override
    {
        if (NULL == buff || 0 == size || 0 == count)
        {
                return 0;
        }

        uint64_t max = (m_size - m_pos) / size;
        if (max < (uint64_t)count)
        {
                count = (size_t)max;
        }

        size *= count;
        memcpy(buff, m_ptr+m_pos, size);
        m_pos += size;

        return count;
    }

public:
    operator bool() const
    {
        return m_ptr && 0!=m_size;
    }

    virtual bool seek(long long offset, int origin) override
    {
        long long pos = (long long)m_pos;
        if (SEEK_SET == origin)
        {
            pos = offset;
        }
        else if (SEEK_CUR == origin)
        {
            pos += offset;
        }
        else if (SEEK_END == origin)
        {
            pos = (long long)m_size-1+offset;
        }
        else
        {
            return false;
        }

        if (pos < 0 || pos >= (long long)m_size)
        {
            return false;
        }
        m_pos = (uint64_t)pos;

        return true;
    }

    virtual long long pos() override
    {
        return m_pos;
    }

    virtual uint64_t size() override
    {
        return m_size;
    }
};

#if !__winvc
class __UtilExt CFileMap
{
public:
    CFileMap() = default;

    CFileMap(cwstr strFile)
        : m_qf(__WS2Q(strFile))
    {
    }

    ~CFileMap()
    {
        close();
    }

private:
    QFile m_qf;
    uchar *m_ptr = NULL;

private:
    uchar* _resize(size_t size)
    {
        if (!m_qf.resize(size))
        {
            return NULL;
        }

        if (0 == size)
        {
            return NULL;
        }

        return m_qf.map(0, size);
    }

public:
    bool isOpen() const
    {
        return m_qf.isOpen();
    }
    size_t size() const
    {
        return m_qf.size();
    }

    inline uchar* open()
    {
        if (!m_qf.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
        {
            return NULL;
        }

        auto size = m_qf.size();
        if (0 == size)
        {
            return NULL;
        }

        m_ptr = m_qf.map(0, size);
        if (NULL == m_ptr)
        {
            close(false);
        }
        return m_ptr;
    }

    uchar* open(cwstr strFile)
    {
        m_qf.setFileName(__WS2Q(strFile));
        return open();
    }

    inline uchar* create(size_t size)
    {
        if (!m_qf.open(QIODevice::ReadWrite | QIODevice::Unbuffered | QIODevice::Truncate))
        {
            return NULL;
        }

        if (0 == size)
        {
            return NULL;
        }

        m_ptr = _resize(size);
        if (NULL == m_ptr)
        {
            close(true);
        }
        return m_ptr;
    }

    uchar* create(cwstr strFile, size_t size)
    {
        m_qf.setFileName(__WS2Q(strFile));
        return create(size);
    }

    uchar* resize(size_t size)
    {
        if (m_ptr)
        {
            (void)m_qf.unmap(m_ptr);
            m_ptr = NULL;
        }

        m_ptr = _resize(size);
        return m_ptr;
    }

    void close(bool bRemove = false)
    {
        if (m_ptr)
        {
            (void)m_qf.unmap(m_ptr);
            m_ptr = NULL;
        }

        if (m_qf.isOpen())
        {
            m_qf.close();
            if (bRemove)
            {
                m_qf.remove();
            }
        }
    }
};

class __UtilExt IFBufferMap : public IFBuffer
{
public:
    IFBufferMap() = default;

    IFBufferMap(cwstr strFile)
    {
        (void)open(strFile);
    }

    ~IFBufferMap()
    {
        close();
    }

private:
    CFileMap m_fileMap;

public:
    bool open(cwstr strFile)
    {
        m_ptr = m_fileMap.open(strFile);
        if (NULL == m_ptr)
        {
            return false;
        }

        m_size = m_fileMap.size();

        return true;
    }

    void close()
    {
        m_fileMap.close(false);
        m_ptr = NULL;
        m_size = 0;
    }
};
#endif

class __UtilExt IFStream : public FStream, public Instream
{
public:
    virtual ~IFStream() = default;

    IFStream() = default;

    IFStream(cwstr strFile)
    {
        (void)open(strFile);
    }
    IFStream(const string& strFile)
    {
        (void)open(strFile);
    }

protected:
    uint64_t m_size = 0;

private:
	template <class T>
	bool _open(const T& strFile)
	{
        m_size = 0;
		if (!FStream::_open(strFile, "rb"))
		{
			return false;
		}
		
        (void)fseek64(m_pf, 0, SEEK_END);
        m_size = (uint64_t)ftell64(m_pf);
        (void)fseek64(m_pf, 0, SEEK_SET);

		return true;
	}

    virtual size_t _read(void *buff, size_t size, size_t count) override
    {
        return fread(buff, size, count, m_pf);
    }

public:
    virtual uint64_t size() override
	{
		return m_size;
	}

	virtual bool open(cwstr strFile)
	{
		return _open(strFile);
	}
	virtual bool open(const string& strFile)
	{
		return _open(strFile);
	}

    virtual bool seek(long long offset, int origin) override
    {
        return fseek64(m_pf, offset, origin) == 0;
    }

	virtual long long pos() override
	{
        return ftell64(m_pf);
	}

    void close()
	{
		FStream::close();
		m_size = 0;
	}

	template <class S>
	inline static size_t readfile(const S& strFile, void *buff, size_t size, size_t count)
	{
		IFStream ifs(strFile);
		__EnsureReturn(ifs, 0);

		return ifs.read(buff, size, count);
	}

	template <class S>
	inline static size_t readfile(const S& strFile, void *buff, size_t size)
	{
		return readfile(strFile, buff, 1, size);
	}

	template <class S>
	inline static bool readfilex(const S& strFile, void *buff, size_t size)
	{
		return readfile(strFile, buff, size, 1) == 1;
	}

	template <class S, typename T>
	static size_t readfile(const S& strFile, TBuffer<T>& buff)
	{
		return readfile(strFile, buff, sizeof(T), buff.count());
	}

	template <class S, typename T>
	static bool readfilex(const S& strFile, TBuffer<T>& buff)
	{
		return readfilex(strFile, buff, buff.size());
	}

	template <class S>
	static size_t readfile(const S& strFile, CByteBuffer& bbfBuff, size_t uReadSize = 0)
	{
		IFStream ifs(strFile);
		__EnsureReturn(ifs, 0);

		return ((Instream&)ifs).read(bbfBuff, uReadSize);
	}

	template <class S>
	static size_t readfile(const S& strFile, CCharBuffer& cbfBuff, size_t uReadSize = 0)
	{
		IFStream ifs(strFile);
		__EnsureReturn(ifs, 0);

		return ((Instream&)ifs).read(cbfBuff, uReadSize);
	}
};

class __UtilExt Outstream
{
public:
    virtual ~Outstream() = default;

    virtual size_t write(const void *pData, size_t size, size_t count) = 0;

    virtual void flush() const = 0;
};

class __UtilExt OFStream : public FStream, public Outstream
{
public:
    virtual ~OFStream() = default;

    OFStream() = default;

    OFStream(cwstr strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }
    OFStream(const string& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }

public:
    virtual bool open(cwstr strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }
    virtual bool open(const string& strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }

    void flush() const override
    {
        (void)fflush(m_pf);
    }

    inline virtual size_t write(const void *pData, size_t size, size_t count) override
	{
        return fwrite(pData, size, count, m_pf);
	}
    inline size_t write(const void *pData, size_t size)
	{
        return write(pData, 1, size);
	}

    inline bool writex(const void *pData, size_t size)
	{
        return write(pData, size, 1) == 1;
	}

    template <typename T>
    bool writex(const T& data)
    {
        return writex(&data, sizeof(data));
    }
    template <typename T>
    size_t write(const T *pData, size_t count)
    {
        return write(pData, sizeof(T), count);
    }

	template <typename T>
	size_t write(const TBuffer<T>& buff)
	{
		return write(buff, sizeof(T), buff.count());
	}
	template <typename T>
	bool writex(const TBuffer<T>& buff)
	{
		return writex(buff, buff.size());
	}

    size_t write(const CByteBuffer& bbfData)
	{
        return write(bbfData, bbfData->size());
	}
    bool writex(const CByteBuffer& bbfData)
	{
        return writex(bbfData, bbfData->size());
	}

    size_t write(const CCharBuffer& cbfData)
	{
        return write(cbfData, cbfData->size());
	}
    bool writex(const CCharBuffer& cbfData)
	{
        return writex(cbfData, cbfData->size());
	}

	template <typename S>
    inline static long writefile(const S& strFile, bool bTrunc, const void *pData, size_t size, size_t count)
	{
		OFStream ofs(strFile, bTrunc);
		__EnsureReturn(ofs, -1);

        return ofs.write(pData, size, count);
	}

	template <typename S>
    inline static long writefile(const S& strFile, bool bTrunc, const void *pData, size_t size)
	{
        return writefile(strFile, bTrunc, pData, 1, size);
	}

	template <typename S>
    inline static bool writefilex(const S& strFile, bool bTrunc, const void *pData, size_t size)
	{
        OFStream ofs(strFile, bTrunc);
        __EnsureReturn(ofs, false);

        if (size)
        {
            if (!ofs.writex(pData, size))
            {
                ofs.close();
                fsutil::removeFile(strFile);
                return false;
            }
        }

        return true;
	}

	template <typename S, typename T>
	static long writefile(const S& strFile, bool bTrunc, const TBuffer<T>& buff)
	{
		return writefile(strFile, bTrunc, buff, sizeof(T), buff.count());
	}

	template <typename S, typename T>
	static bool writefilex(const S& strFile, bool bTrunc, const TBuffer<T>& buff)
	{
		return writefilex(strFile, bTrunc, buff, buff.size());
	}

	template <typename S>
    static long writefile(const S& strFile, bool bTrunc, const CByteBuffer& bbfData)
	{
        return writefile(strFile, bTrunc, bbfData, bbfData->size());
	}

	template <typename S>
    static bool writefilex(const S& strFile, bool bTrunc, const CByteBuffer& bbfData)
	{
        return writefilex(strFile, bTrunc, bbfData, bbfData->size());
	}

	template <typename S>
    static long writefile(const S& strFile, bool bTrunc, const CCharBuffer& cbfData)
	{
        return writefile(strFile, bTrunc, cbfData, cbfData->size());
	}

	template <typename S>
    static bool writefilex(const S& strFile, bool bTrunc, const CCharBuffer& cbfData)
	{
        return writefilex(strFile, bTrunc, cbfData, cbfData->size());
	}
};
