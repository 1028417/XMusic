
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

	TET_Unicode_LittleEndian,
	TET_Unicode_BigEndian,

	TET_Utf8_NoBom,
	TET_Utf8_WithBom
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
	static const string __UnicodeHead_LittleEndian;
	static const string __UnicodeHead_BigEndian;
	
	static const string __UTF8Bom;

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

	OBStream m_obs;

private:
	inline bool _isUnicode() const
	{
		return E_TxtEncodeType::TET_Unicode_LittleEndian == m_eEncodeType
			|| E_TxtEncodeType::TET_Unicode_BigEndian == m_eEncodeType;
	}

	inline bool _isUtf8() const
	{
		return E_TxtEncodeType::TET_Utf8_NoBom == m_eEncodeType
			|| E_TxtEncodeType::TET_Utf8_WithBom == m_eEncodeType;
	}

    bool _writeHead();

    bool _write(const char *pStr, size_t len, bool bEndLine = false);
    bool _write(const wchar_t *pStr, size_t len, bool bEndLine = false);

    inline bool _writeEndLine();

public:
	bool is_open() const
	{
		return m_obs.is_open();
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
		m_obs.close();
    }
};

enum class E_UnicodeHeadOpt
{
	UHO_LittleEndian = 0,
	UHO_BigEndian,
};

class __UtilExt CUnicodeWriter : public CTxtWriter
{
public:
	virtual ~CUnicodeWriter() {}

	CUnicodeWriter(E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_Unicode_LittleEndian
			: E_TxtEncodeType::TET_Unicode_BigEndian, eEOLFlag)
	{
	}

	CUnicodeWriter(const wstring& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_Unicode_LittleEndian
			: E_TxtEncodeType::TET_Unicode_BigEndian, eEOLFlag)
	{
	}

    CUnicodeWriter(const string& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_Unicode_LittleEndian
			: E_TxtEncodeType::TET_Unicode_BigEndian, eEOLFlag)
	{
	}
};

/*#if __winvc
#define __DefUTF8BomOpt false
#else
#define __DefUTF8BomOpt false
#endif*/

class __UtilExt CUTF8Writer : public CTxtWriter
{
public:
	virtual ~CUTF8Writer() {}

	CUTF8Writer(bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(bWithBom ? E_TxtEncodeType::TET_Utf8_WithBom : E_TxtEncodeType::TET_Utf8_NoBom, eEOLFlag)
	{
	}

	CUTF8Writer(const wstring& strFile, bool bTrunc, bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_Utf8_WithBom : E_TxtEncodeType::TET_Utf8_NoBom, eEOLFlag)
	{
	}

	CUTF8Writer(const string& strFile, bool bTrunc, bool bWithBom = false, E_EOLFlag eEOLFlag = __DefEOL) :
		CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_Utf8_WithBom : E_TxtEncodeType::TET_Utf8_NoBom, eEOLFlag)
	{
	}
};
