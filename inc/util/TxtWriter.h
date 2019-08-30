
#pragma once

#if __android
#define __DefEOL E_EOLFlag::eol_n
#else
#define __DefEOL E_EOLFlag::eol_rn
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
    virtual size_t write(const wstring& strText) const = 0;

    virtual size_t writeln(const wstring& strText) const = 0;

    virtual size_t write(const string& strText) const = 0;

    virtual size_t writeln(const string& strText) const = 0;

    virtual size_t write(const wchar_t *pStr) const = 0;

    virtual size_t writeln(const wchar_t *pStr) const = 0;

    virtual size_t write(const char *pStr) const = 0;

    virtual size_t writeln(const char *pStr) const = 0;

    template <typename T>
    size_t write(const T& num) const
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
    size_t writeln(const T& num) const
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
    size_t write(const QString& qstr) const
    {
        return write(qstr.toStdWString());
    }

    size_t writeln(const QString& qstr) const
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

	~CTxtWriter()
	{
		close();
	}

private:
	E_EOLFlag m_eEOLFlag;

	FILE *m_lpFile = NULL;

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

	bool _open(const wstring& strFile, bool bTrunc);
	bool _open(const string& strFile, bool bTrunc);

	void _writeHead();

	inline size_t _fwrite(const void *pData, size_t size) const;

	size_t _write(const char *pStr, size_t len, bool bEndLine = false) const;

	size_t _write(const wchar_t *pStr, size_t len, bool bEndLine = false) const;

	inline size_t _writeEndLine() const;

public:
	bool isOpened() const
	{
		return NULL != m_lpFile;
	}

	bool open(const wstring& strFile, bool bTrunc);
	bool open(const string& strFile, bool bTrunc);

    size_t write(const wstring& strText) const override
	{
		return _write(strText.c_str(), strText.size());
	}

    size_t writeln(const wstring& strText) const override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    size_t write(const string& strText) const override
	{
		return _write(strText.c_str(), strText.size());
	}

    size_t writeln(const string& strText) const override
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    size_t write(const wchar_t *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr));
	}

    size_t writeln(const wchar_t *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, wcslen(pStr), true);
	}

    size_t write(const char *pStr) const override
	{
		if (NULL == pStr)
		{
			return 0;
		}

		return _write(pStr, strlen(pStr));
	}

    size_t writeln(const char *pStr) const override
    {
        return _write(pStr, pStr?strlen(pStr):0, true);
    }

    bool close();
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
