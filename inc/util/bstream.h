
#pragma once

class __UtilExt BStream
{
protected:
    virtual ~BStream()
    {
        close();
    }

	BStream() {}
	
	BStream(const wstring& strFile, const string& strMode)
	{
		(void)_open(strFile, strMode);
	}

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

	virtual bool seek(long offset, int origin) = 0;

	virtual long pos() = 0;
};

class __UtilExt Outstream
{
public:
	virtual ~Outstream() {}

	virtual size_t write(const void *buff, size_t size, size_t count) = 0;
	bool writeex(const void *buff, size_t size)
	{
		return write(buff, size, 1) == 1;
	}

    virtual void flush() = 0;
};

class __UtilExt IBStream : public BStream, public Instream
{
public:
	virtual ~IBStream() {}

	IBStream() {}

    IBStream(const wstring& strFile)
    {
        (void)open(strFile);
    }
    IBStream(const string& strFile)
    {
        (void)open(strFile);
    }

private:
	UINT m_uSize = 0;

private:
	template <class T>
	bool _open(const T& strFile)
	{
		if (!BStream::_open(strFile, "rb"))
		{
			return false;
		}

		(void)fseek(m_pf, 0, SEEK_END);
		m_uSize = (UINT)ftell(m_pf);
		(void)fseek(m_pf, 0, SEEK_SET);

		return true;
	}

public:
    UINT size() const
	{
		return m_uSize;
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

    virtual bool seek(long offset, int origin) override
    {
        return 0==fseek(m_pf, offset, origin);
    }

	virtual long pos() override
	{
		return ftell(m_pf);
	}

	virtual void close() override
	{
		BStream::close();
		m_uSize = 0;
	}
};

class __UtilExt OBStream : public BStream, public Outstream
{
public:
	virtual ~OBStream() {}

	OBStream() {}

    OBStream(const wstring& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }
    OBStream(const string& strFile, bool bTrunc)
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

class __UtilExt IFBuff : public Instream
{
public:
    IFBuff(byte_t *ptr, size_t size)
        : m_ptr(ptr)
        , m_size(size)
    {
    }

    IFBuff(const CByteBuff& btbBuff)
        : m_ptr(btbBuff)
        , m_size(btbBuff->size())
    {

    }

    virtual ~IFBuff(){}

protected:
    const byte_t *m_ptr;
    size_t m_size;

    size_t m_pos = 0;

public:
    operator bool() const
    {
        return m_ptr && 0!=m_size;
    }

    virtual size_t read(void *buff, size_t size, size_t count) override
    {
        count = MIN(count, (m_size-m_pos)/size);
        if (0 != count)
        {
            size *= count;
            memcpy(buff, m_ptr+m_pos, size);
            m_pos += size;
        }

        return count;
    }

    virtual bool seek(long offset, int origin) override
    {
        long pos = (long)m_pos;
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
            pos = (long)m_size-1+offset;
        }
        else
        {
            return false;
        }

        if (pos < 0 || pos >= (long)m_size)
        {
            return false;
        }
        m_pos = (size_t)pos;

        return true;
    }

    virtual long pos() override
    {
        return m_pos;
    }
};
