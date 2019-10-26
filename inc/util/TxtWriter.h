
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
    ITxtWriter(E_TxtEncodeType eEncodeType)
        : m_eEncodeType(eEncodeType)
    {
    }

protected:
    E_TxtEncodeType m_eEncodeType;

public:
    virtual bool write(const wstring& strText) const = 0;

    virtual bool writeln(const wstring& strText) const = 0;

    virtual bool write(const string& strText) const = 0;

    virtual bool writeln(const string& strText) const = 0;

    virtual bool write(const wchar_t *pStr) const = 0;

    virtual bool writeln(const wchar_t *pStr) const = 0;

    virtual bool write(const char *pStr) const = 0;

    virtual bool writeln(const char *pStr) const = 0;

    template <typename T>
    bool write(const T& num) const
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
    bool writeln(const T& num) const
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
    const ITxtWriter& operator<<(const T& t) const
    {
            (void)write(t);
            return *this;
    }

    template <typename T>
    const ITxtWriter& operator>>(const T& t) const
    {
            (void)writeln(t);
            return *this;
    }

#if !__winvc
    bool write(const QString& qstr) const
    {
        return write(qstr.toStdWString());
    }

    bool writeln(const QString& qstr) const
    {
        return writeln(qstr.toStdWString());
    }
#endif
};

class __UtilExt CTxtWriter : public ITxtWriter, private obstream
{
public:
	static const string __UnicodeHead_LittleEndian;
	static const string __UnicodeHead_BigEndian;
	
	static const string __UTF8Bom;

public:
	CTxtWriter(E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
		ITxtWriter(eEncodeType),
		m_eEOLFlag(eEOLFlag)
	{
	}

    CTxtWriter(const wstring& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLFlag)
    {
        (void)_open(strFile, bTrunc);
    }
    CTxtWriter(const string& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLFlag eEOLFlag = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLFlag)
    {
        (void)_open(strFile, bTrunc);
    }

private:
	E_EOLFlag m_eEOLFlag;

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

    bool _open(const wstring& strFile, bool bTrunc)
    {
        return obstream::open(strFile, bTrunc ? "wb" : "ab");
    }
    bool _open(const string& strFile, bool bTrunc)
    {
        return obstream::open(strFile, bTrunc ? "wb" : "ab");
    }

    bool _writeHead();

    bool _write(const char *pStr, size_t len, bool bEndLine = false) const;
    bool _write(const wchar_t *pStr, size_t len, bool bEndLine = false) const;

    inline bool _writeEndLine() const;

public:
	bool isOpened() const
	{
        return NULL != m_pf;
	}

	bool open(const wstring& strFile, bool bTrunc);
	bool open(const string& strFile, bool bTrunc);

    bool write(const wstring& strText) const override
	{
		return _write(strText.c_str(), strText.size());
	}

    bool writeln(const wstring& strText) const override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    bool write(const string& strText) const override
	{
		return _write(strText.c_str(), strText.size());
	}

    bool writeln(const string& strText) const override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    bool write(const wchar_t *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr));
	}

    bool writeln(const wchar_t *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr), true);
	}

    bool write(const char *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, strlen(pStr));
    }

    bool writeln(const char *pStr) const override
    {
        return _write(pStr, pStr?strlen(pStr):0, true);
    }

    void close()
    {
        obstream::close();
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
