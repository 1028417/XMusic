
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

	size_t m_uSize = 0;

protected:
    template <typename T>
    bool _open(const T& file, const string& strMode)
    {
        m_pf = fsutil::fopen(file, strMode);
		if (NULL == m_pf)
		{
			return false;
		}

		fsutil::seekEnd(m_pf);
		m_uSize = (UINT)ftell(m_pf);
		fsutil::seekHead(m_pf);

		return true;
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

	size_t size() const
	{
		return m_uSize;
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

		m_uSize = 0;
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

public:
    virtual bool open(const wstring& strFile)
    {
        return _open(strFile, "rb");
    }
    virtual bool open(const string& strFile)
    {
        return _open(strFile, "rb");
    }

    virtual size_t read(void *buff, size_t size, size_t count) override
    {
        return fread(buff, size, count, m_pf);
    }

    virtual bool seek(long offset, int origin) override
    {
        return fsutil::seek(m_pf, offset, origin);
    }

	virtual long pos() override
	{
		return ftell(m_pf);
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
