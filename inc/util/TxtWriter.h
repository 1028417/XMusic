
#pragma once

#if __windows
#define __DefEOL E_EOLFlag::eol_rn
#else
#define __DefEOL E_EOLFlag::eol_n
#endif

enum class E_EOLFlag
{
	eol_n = 0,
	eol_rn
};

enum class E_TxtEncodeType
{
	TET_Asc = 0,

	TET_UTF8_NoBom,
	TET_UTF8_WithBom,

	TET_UCS2_LittleEndian,
	TET_UCS2_BigEndian
};

class __UtilExt ITxtWriter
{
public:
	virtual ~ITxtWriter() {}

    ITxtWriter(E_TxtEncodeType eEncodeType)
        : m_eEncodeType(eEncodeType)
    {
    }

protected:
    E_TxtEncodeType m_eEncodeType;

public:
    virtual bool write(const wstring& strText) = 0;

    virtual bool writeln(const wstring& strText) = 0;

    virtual bool write(const string& strText) = 0;

    virtual bool writeln(const string& strText) = 0;

    virtual bool write(const wchar_t *pStr) = 0;

    virtual bool writeln(const wchar_t *pStr) = 0;

    virtual bool write(const char *pStr) = 0;

    virtual bool writeln(const char *pStr) = 0;

    template <typename T>
    bool write(const T& num)
    {
            if (E_TxtEncodeType::TET_Asc == m_eEncodeType)
            {
                    return write(to_string(num));
            }
            else
    {
                    return write(to_wstring(num));
            }
    }

    template <typename T>
    bool writeln(const T& num)
    {
            if (E_TxtEncodeType::TET_Asc == m_eEncodeType)
            {
                    return writeln(to_string(num));
            }
            else
            {
                    return writeln(to_wstring(num));
            }
    }

    template <typename T>
    ITxtWriter& operator<<(const T& t)
    {
            (void)write(t);
            return *this;
    }

    template <typename T>
    ITxtWriter& operator>>(const T& t)
    {
            (void)writeln(t);
            return *this;
    }

#if !__winvc
    bool write(const QString& qstr)
    {
        return write(qstr.toStdWString());
    }

    bool writeln(const QString& qstr)
    {
        return writeln(qstr.toStdWString());
    }
#endif
};

class __UtilExt CTxtWriter : public ITxtWriter
{
public:
	virtual ~CTxtWriter() {}

	CTxtWriter(E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
		ITxtWriter(eEncodeType),
		m_eEOLFlag(eEOLFlag)
	{
	}

    CTxtWriter(const wstring& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLFlag)
    {
        (void)open(strFile, bTrunc);
    }
    CTxtWriter(const string& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLFlag)
    {
        (void)open(strFile, bTrunc);
    }

private:
	E_EOLFlag m_eEOLFlag;

	OFStream m_ofs;

private:
	inline bool _isUtf8() const
	{
		return E_TxtEncodeType::TET_UTF8_NoBom == m_eEncodeType
			|| E_TxtEncodeType::TET_UTF8_WithBom == m_eEncodeType;
	}

	inline bool _isUnicode() const
	{
		return E_TxtEncodeType::TET_UCS2_LittleEndian == m_eEncodeType
			|| E_TxtEncodeType::TET_UCS2_BigEndian == m_eEncodeType;
	}

    bool _writeHead();

    bool _write(const char *pStr, size_t len, bool bEndLine = false);
    bool _write(const wchar_t *pStr, size_t len, bool bEndLine = false);

    inline bool _writeEndLine();

public:
	bool is_open() const
	{
		return m_ofs.is_open();
	}

	bool open(const wstring& strFile, bool bTrunc);
	bool open(const string& strFile, bool bTrunc);

    bool write(const wstring& strText) override
	{
		return _write(strText.c_str(), strText.size());
	}

    bool writeln(const wstring& strText) override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    bool write(const string& strText) override
	{
		return _write(strText.c_str(), strText.size());
	}

    bool writeln(const string& strText) override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    bool write(const wchar_t *pStr) override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr));
	}

    bool writeln(const wchar_t *pStr) override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr), true);
	}

    bool write(const char *pStr) override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, strlen(pStr));
    }

    bool writeln(const char *pStr) override
    {
        return _write(pStr, pStr?strlen(pStr):0, true);
    }

    void close()
    {
		m_ofs.close();
    }
};

enum class E_UnicodeHeadOpt
{
	UHO_LittleEndian = 0,
	UHO_BigEndian,
};

/*#if __winvc
#define __DefUTF8BomOpt false
#else
#define __DefUTF8BomOpt false
#endif*/

class __UtilExt CUTF8TxtWriter : public CTxtWriter
{
public:
	virtual ~CUTF8TxtWriter() {}

	CUTF8TxtWriter(bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLFlag)
	{
	}

	CUTF8TxtWriter(const wstring& strFile, bool bTrunc, bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLFlag)
	{
	}

	CUTF8TxtWriter(const string& strFile, bool bTrunc, bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLFlag)
	{
	}
};

class __UtilExt CUCS2TextWriter : public CTxtWriter
{
public:
	virtual ~CUCS2TextWriter() {}

	CUCS2TextWriter(E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
			: E_TxtEncodeType::TET_UCS2_BigEndian, eEOLFlag)
	{
	}

	CUCS2TextWriter(const wstring& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
			: E_TxtEncodeType::TET_UCS2_BigEndian, eEOLFlag)
	{
	}

	CUCS2TextWriter(const string& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
			: E_TxtEncodeType::TET_UCS2_BigEndian, eEOLFlag)
	{
	}
};

enum class E_TxtHeadType
{
	THT_None = 0,
	THT_UTF8Bom,
	THT_UCS2Head_LittleEndian,
	THT_UCS2Head_BigEndian,
};

class __UtilExt CTxtReader
{
public:
	CTxtReader() {}

private:
	E_TxtHeadType m_eHeadType = E_TxtHeadType::THT_None;

public:
	E_TxtHeadType headType() const
	{
		return m_eHeadType;
	}

	bool hasUTF8Bom() const
	{
		return E_TxtHeadType::THT_UTF8Bom == m_eHeadType;
	}

	void read(Instream& ins, string& strText)
	{
		string t_strText;
        _read(ins, t_strText);
        strText.append(t_strText);
	}
    void read(Instream& ins, wstring& strText)
	{
		wstring t_strText;
		_read(ins, t_strText);
		strText.append(t_strText);
	}
	
	void read(Instream& ins, cfn_bool_t<const string&> cb)
	{
		_read(ins, cb);
	}
	void read(Instream& ins, cfn_bool_t<const wstring&> cb)
	{
		_read(ins, cb);
	}

private:
	void _readData(const char *lpData, size_t len, string& strText);
	void _readData(const char *lpData, size_t len, wstring& strText);

	template <class T>
    void _read(Instream& ins, T& strText)
    {
            CCharBuffer cbfData;
            (void)ins.readex(cbfData, (size_t)ins.size());
            __Ensure(cbfData);

            _readData(cbfData, cbfData->size(), strText);
	}

	template <class T>
        void _read(Instream& ins, cfn_bool_t<const T&> cb)
	{
		T strText;
                _read(ins, strText);

		size_t prePos = 0;
		size_t pos = 0;
		while (true)
		{
			pos = strText.find('\n', prePos);
			if (T::npos == pos)
			{
				break;
			}

			auto strSub = strText.substr(prePos, pos - prePos);
			if (!strSub.empty())
			{
				if ('\r' == *strText.rbegin())
				{
					strSub.pop_back();
				}
			}

			if (!cb(strSub))
			{
                                return;
			}

			prePos = pos + 1;
		}

		if (prePos < strText.size())
		{
			cb(strText.substr(prePos));
                }
	}
};
