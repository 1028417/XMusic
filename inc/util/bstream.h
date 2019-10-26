
#pragma once

class __UtilExt bstream
{
protected:
    virtual ~bstream()
    {
        close();
    }

	bstream() {}
	
	bstream(const wstring& strFile, const string& strMode)
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

		fseek(m_pf, 0, SEEK_END);
		m_uSize = (UINT)ftell(m_pf);
		fseek(m_pf, 0, SEEK_SET);

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

class __UtilExt ibstream : public bstream
{
public:
	ibstream() {}

    ibstream(const wstring& strFile)
    {
        (void)open(strFile);
    }
    ibstream(const string& strFile)
    {
        (void)open(strFile);
    }

public:
    bool open(const wstring& strFile)
    {
        return _open(strFile, "rb");
    }
    bool open(const string& strFile)
    {
        return _open(strFile, "rb");
    }
	
    size_t read(void *buff, size_t count, size_t size=1) const
    {
        return fread(buff, size, count, m_pf);
    }
};

class __UtilExt obstream : public bstream
{
public:
	obstream() {}

    obstream(const wstring& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }
    obstream(const string& strFile, bool bTrunc)
    {
        (void)open(strFile, bTrunc);
    }

public:
    bool open(const wstring& strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }
    bool open(const string& strFile, bool bTrunc)
    {
        return _open(strFile, bTrunc?"wb":"ab");
    }

    bool write(const void *buff, size_t buffSize) const
    {
        return fwrite(buff, buffSize, 1, m_pf) == 1;
    }
    size_t write(const void *buff, size_t size, size_t count) const
    {
        return fwrite(buff, size, count, m_pf);
    }

    void flush() const
    {
        (void)fflush(m_pf);
    }
};
