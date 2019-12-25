
#pragma once

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

	bool readex(void *buff, size_t size)
	{
		return read(buff, size, 1) == 1;
	}

	template <typename T>
	size_t readex(TBuffer<T>& buff)
	{
        return read(buff, sizeof(T), buff.count());
	}

	size_t readex(CByteBuffer& bbfBuff, size_t uReadSize)
	{
		return _readex(bbfBuff, uReadSize);
	}
	size_t readex(CCharBuffer& cbfBuff, size_t uReadSize)
	{
		return _readex(cbfBuff, uReadSize);
	}

    virtual bool seek(long long offset, int origin) = 0;

    virtual long long pos() = 0;

    virtual uint64_t size() = 0;

private:
	template <class T>
	size_t _readex(T& buff, size_t uReadSize)
	{
		uint64_t max = this->size();
		if (max < (uint64_t)uReadSize)
		{
			uReadSize = (size_t)max;
		}
		if (0 == uReadSize)
		{
			return 0;
		}

		auto ptr = buff.resizeMore(uReadSize);
		size_t size = read(ptr, 1, uReadSize);
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

    bool writex(const void *buff, size_t size)
	{
		return write(buff, size, 1) == 1;
	}

    template <typename T>
    size_t writex(TBuffer<T>& buff)
    {
        return read(buff, sizeof(T), buff.count());
    }

    size_t writex(CByteBuffer& bbfBuff)
    {
        return write(bbfBuff, 1, bbfBuff->size());
    }

    size_t writex(CCharBuffer& cbfBuff)
    {
        return write(cbfBuff, 1, cbfBuff->size());
    }

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

	virtual size_t write(const void *buff, size_t size, size_t count) override
	{
		return fwrite(buff, size, count, m_pf);
	}

    virtual void flush() override
    {
        (void)fflush(m_pf);
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
		uint64_t t_count = (m_size - m_pos) / size;
		if (t_count < (uint64_t)count)
		{
			count = (size_t)t_count;
		}
        if (0 != count)
        {
            size *= count;
            memcpy(buff, m_ptr+m_pos, size);
            m_pos += size;
        }

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
