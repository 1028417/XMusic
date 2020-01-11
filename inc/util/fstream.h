
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
	FStream() {}
	
	FStream(const wstring& strFile, const string& strMode)
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

    virtual void close()
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
	virtual ~Instream() {}

	virtual size_t read(void *buff, size_t size, size_t count) = 0;
	inline size_t read(void *buff, size_t size)
	{
		return read(buff, 1, size);
	}

	inline bool readex(void *buff, size_t size)
	{
		return read(buff, size, 1) == 1;
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

class __UtilExt Outstream
{
public:
	virtual ~Outstream() {}

	virtual size_t write(const void *buff, size_t size, size_t count) = 0;

    virtual void flush() = 0;
};

class __UtilExt IFStream : public FStream, public Instream
{
public:
	virtual ~IFStream() {}

	IFStream() {}

    IFStream(const wstring& strFile)
    {
        (void)open(strFile);
    }
    IFStream(const string& strFile)
    {
        (void)open(strFile);
    }

private:
    uint64_t m_size = 0;

private:
	template <class T>
	bool _open(const T& strFile)
	{
		if (!FStream::_open(strFile, "rb"))
		{
			return false;
		}
		
        (void)fseek64(m_pf, 0, SEEK_END);
        m_size = (uint64_t)ftell64(m_pf);
        (void)fseek64(m_pf, 0, SEEK_SET);

		return true;
	}

public:
    virtual uint64_t size() override
	{
		return m_size;
	}

	virtual bool open(const wstring& strFile)
	{
		return _open(strFile);
	}
	virtual bool open(const string& strFile)
	{
		return _open(strFile);
	}

    virtual size_t read(void *buff, size_t size, size_t count) override
    {
        return fread(buff, size, count, m_pf);
    }

    virtual bool seek(long long offset, int origin) override
    {
        return fseek64(m_pf, offset, origin) == 0;
    }

	virtual long long pos() override
	{
        return ftell64(m_pf);
	}

	virtual void close() override
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

class __UtilExt OFStream : public FStream, public Outstream
{
public:
	virtual ~OFStream() {}

	OFStream() {}

    OFStream(const wstring& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }
    OFStream(const string& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }

public:
    virtual bool open(const wstring& strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }
    virtual bool open(const string& strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }

    virtual void flush() override
    {
        (void)fflush(m_pf);
    }

	inline virtual size_t write(const void *buff, size_t size, size_t count) override
	{
		return fwrite(buff, size, count, m_pf);
	}
	inline size_t write(const void *buff, size_t size)
	{
		return fwrite(buff, 1, size, m_pf);
	}

	inline bool writex(const void *buff, size_t size)
	{
		return fwrite(buff, size, 1, m_pf) == 1;
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

	size_t write(const CByteBuffer& bbfBuff)
	{
		return write(bbfBuff, bbfBuff->size());
	}
	bool writex(const CByteBuffer& bbfBuff)
	{
		return writex(bbfBuff, bbfBuff->size());
	}

	size_t write(const CCharBuffer& cbfBuff)
	{
		return write(cbfBuff, cbfBuff->size());
	}
	bool writex(const CCharBuffer& cbfBuff)
	{
		return writex(cbfBuff, cbfBuff->size());
	}

	template <typename S>
	inline static long writefile(const S& strFile, bool bTrunc, const void *buff, size_t size, size_t count)
	{
		OFStream ofs(strFile, bTrunc);
		__EnsureReturn(ofs, -1);

		return ofs.write(buff, size, count);
	}

	template <typename S>
	inline static long writefile(const S& strFile, bool bTrunc, const void *buff, size_t size)
	{
		return writefile(strFile, bTrunc, buff, 1, size);
	}

	template <typename S>
	inline static bool writefilex(const S& strFile, bool bTrunc, const void *buff, size_t size)
	{
		return writefile(strFile, bTrunc, buff, size, 1) == 1;
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
	static long writefile(const S& strFile, bool bTrunc, CByteBuffer& bbfBuff)
	{
		return writefile(strFile, bTrunc, bbfBuff, bbfBuff->size());
	}

	template <typename S>
	static bool writefilex(const S& strFile, bool bTrunc, CByteBuffer& bbfBuff)
	{
		return writefilex(strFile, bTrunc, bbfBuff, bbfBuff->size());
	}

	template <typename S>
	static long writefile(const S& strFile, bool bTrunc, CCharBuffer& cbfBuff)
	{
		return writefile(strFile, bTrunc, cbfBuff, cbfBuff->size());
	}

	template <typename S>
	static bool writefilex(const S& strFile, bool bTrunc, CCharBuffer& cbfBuff)
	{
		return writefilex(strFile, bTrunc, cbfBuff, cbfBuff->size());
	}
};

class __UtilExt IFBuffer : public Instream
{
public:
    virtual ~IFBuffer(){}

    IFBuffer(c_byte_p ptr, size_t size)
        : m_ptr(ptr)
        , m_size(size)
    {
    }

	IFBuffer(const CByteBuffer& bbfBuff)
		: m_ptr(bbfBuff)
		, m_size(bbfBuff->size())
	{
	}

	IFBuffer(const IFBuffer& other) = delete;
	IFBuffer(IFBuffer&& other) = delete;
	IFBuffer& operator=(const IFBuffer& other) = delete;
	IFBuffer& operator=(IFBuffer&& other) = delete;

protected:
	c_byte_p m_ptr;
	uint64_t m_size;

    uint64_t m_pos = 0;

public:
    operator bool() const
    {
        return m_ptr && 0!=m_size;
    }

    virtual size_t read(void *buff, size_t size, size_t count) override
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
