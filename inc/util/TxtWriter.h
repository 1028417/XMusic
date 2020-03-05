
#pragma once

#if __windows
#define __DefEOL E_EOLType::eol_rn
#else
#define __DefEOL E_EOLType::eol_n
#endif

enum class E_EOLType
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
    ITxtWriter(E_TxtEncodeType eEncodeType) : m_eEncodeType(eEncodeType)
    {
    }

	virtual ~ITxtWriter() {}

private:
	E_TxtEncodeType m_eEncodeType;

protected:
	inline bool _isAsc() const
	{
		return E_TxtEncodeType::TET_Asc == m_eEncodeType;
	}

	inline bool _isUtf8() const
	{
		return E_TxtEncodeType::TET_UTF8_NoBom == m_eEncodeType
			|| E_TxtEncodeType::TET_UTF8_WithBom == m_eEncodeType;
	}
	inline bool _isUtf8WithBom() const
	{
		return E_TxtEncodeType::TET_UTF8_WithBom == m_eEncodeType;
	}

	inline bool _isUcsLittleEndian() const
	{
		return E_TxtEncodeType::TET_UCS2_LittleEndian == m_eEncodeType;
	}
	inline bool _isUcsBigEndian() const
	{
		return E_TxtEncodeType::TET_UCS2_BigEndian == m_eEncodeType;
	}

private:
	virtual bool _write(const void *ptr, size_t len, bool bEndLine) = 0;

	inline bool _write(const char *pStr, size_t len, bool bEndLine = false)
	{
		if (_isAsc() || _isUtf8())
		{
			return _write((const void*)pStr, len, bEndLine);
		}
		else
		{
			wstring str = strutil::toWstr(pStr);
			if (_isUcsBigEndian())
			{
				strutil::transEndian(str);
			}

			return _write((const void*)str.c_str(), str.size() * 2, bEndLine);
		}
	}

	inline bool _write(const wchar_t *pStr, size_t len, bool bEndLine = false)
	{
		if (_isAsc())
		{
			cauto str = strutil::toStr(pStr, len);
			return _write((const void*)str.c_str(), str.size(), bEndLine);
		}
		else if (_isUtf8())
		{
			cauto str = strutil::toUtf8(pStr, len);
			return _write((const void*)str.c_str(), str.size(), bEndLine);
		}
		else if (_isUcsBigEndian())
		{
			cauto str = strutil::transEndian(pStr, len);
			return _write((const void*)str.c_str(), str.size()*2, bEndLine);
		}
		else
		{
			return _write((const void*)pStr, len*2, bEndLine);
		}
	}

#if !__winvc
	bool _write(const QString& qstr, bool bEndLine = false)
	{
		if (_isAsc())
		{
			cauto str = qstr.toStdString();
			return _write((const void*)str.c_str(), str.size(), bEndLine);
		}
		else if (_isUtf8())
		{
			cauto str = qstr.toUtf8().toStdString();
			return _write((const void*)str.c_str(), str.size(), bEndLine);
		}
		else
		{
			wstring str = qstr.toStdWString();
			if (_isUcsBigEndian())
			{
				strutil::transEndian(str);
			}
			return _write((const void*)str.c_str(), str.size() * 2, bEndLine);
		}
	}
#endif

public:
#if !__winvc
	inline bool write(const QString& qstr)
	{
		return _write(qstr);
	}
	inline bool writeln(const QString& qstr)
	{
		return _write(qstr, true);
	}
#endif

	inline bool write(const char *pStr, size_t len)
	{
		return _write(pStr, len);
	}
	inline bool writeln(const char *pStr, size_t len)
	{
		return _write(pStr, len, true);
	}

	inline bool write(const char *pStr)
	{
		return _write(pStr, strlen(pStr));
	}
	inline bool writeln(const char *pStr)
	{
		return _write(pStr, strlen(pStr), true);
	}

	inline bool write(const wchar_t *pStr, size_t len)
	{
		return _write(pStr, len);
	}
	inline bool writeln(const wchar_t *pStr, size_t len)
	{
		return _write(pStr, len, true);
	}

	inline bool write(const wchar_t *pStr)
	{
		return _write(pStr, wcslen(pStr));
	}
	inline bool writeln(const wchar_t *pStr)
	{
		return _write(pStr, wcslen(pStr), true);
	}

	inline bool write(const string& strText)
	{
		return _write(strText.c_str(), strText.size());
	}
	inline bool writeln(const string& strText)
	{
		return _write(strText.c_str(), strText.size(), true);
	}

	inline bool write(const wstring& strText)
	{
		return _write(strText.c_str(), strText.size());
	}
	inline bool writeln(const wstring& strText)
	{
		return _write(strText.c_str(), strText.size(), true);
	}

    template <typename T>
    bool write(const T& num)
    {
            if (_isAsc())
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
            if (_isAsc())
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
};

class __UtilExt CTxtWriter : public ITxtWriter
{
public:
	virtual ~CTxtWriter() {}

    CTxtWriter(E_TxtEncodeType eEncodeType, E_EOLType eEOLType = __DefEOL) :
		ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLType)
	{
	}

    CTxtWriter(const wstring& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLType eEOLType = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLType)
    {
        (void)open(strFile, bTrunc);
    }
    CTxtWriter(const string& strFile, bool bTrunc, E_TxtEncodeType eEncodeType, E_EOLType eEOLType = __DefEOL) :
        ITxtWriter(eEncodeType),
        m_eEOLFlag(eEOLType)
    {
        (void)open(strFile, bTrunc);
    }

private:
    E_EOLType m_eEOLFlag;

	OFStream m_ofs;

private:
    bool _writeHead();

	bool _write(const void *ptr, size_t len, bool bEndLine = false) override;

    inline bool _writeEndLine();

public:
	bool is_open() const
	{
		return m_ofs.is_open();
	}

	bool open(const wstring& strFile, bool bTrunc);
	bool open(const string& strFile, bool bTrunc);

    void close()
    {
		m_ofs.close();
    }
};

#if __windows
#define __DefUTF8BomOpt true
#else
#define __DefUTF8BomOpt false
#endif

class __UtilExt CUTF8TxtWriter : public CTxtWriter
{
public:
	virtual ~CUTF8TxtWriter() {}

    CUTF8TxtWriter(bool bWithBom = __DefUTF8BomOpt, E_EOLType eEOLType = __DefEOL) :
        CTxtWriter(bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLType)
	{
	}

    CUTF8TxtWriter(const wstring& strFile, bool bTrunc, bool bWithBom = __DefUTF8BomOpt, E_EOLType eEOLType = __DefEOL) :
        CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLType)
	{
	}

    CUTF8TxtWriter(const string& strFile, bool bTrunc, bool bWithBom = __DefUTF8BomOpt, E_EOLType eEOLType = __DefEOL) :
        CTxtWriter(strFile, bTrunc, bWithBom ? E_TxtEncodeType::TET_UTF8_WithBom : E_TxtEncodeType::TET_UTF8_NoBom, eEOLType)
	{
	}
};

enum class E_UnicodeHeadOpt
{
	UHO_LittleEndian = 0,
	UHO_BigEndian,
};

class __UtilExt CUCS2TextWriter : public CTxtWriter
{
public:
	virtual ~CUCS2TextWriter() {}

    CUCS2TextWriter(E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLType eEOLType = __DefEOL) :
		CTxtWriter(E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
            : E_TxtEncodeType::TET_UCS2_BigEndian, eEOLType)
	{
	}

    CUCS2TextWriter(const wstring& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLType eEOLType = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
            : E_TxtEncodeType::TET_UCS2_BigEndian, eEOLType)
	{
	}

    CUCS2TextWriter(const string& strFile, bool bTrunc, E_UnicodeHeadOpt eHeadType = E_UnicodeHeadOpt::UHO_LittleEndian, E_EOLType eEOLType = __DefEOL) :
		CTxtWriter(strFile, bTrunc, E_UnicodeHeadOpt::UHO_LittleEndian == eHeadType ? E_TxtEncodeType::TET_UCS2_LittleEndian
            : E_TxtEncodeType::TET_UCS2_BigEndian, eEOLType)
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

    E_TxtHeadType read(Instream& ins, string& strText)
	{
		string t_strText;
        _read(ins, t_strText);
        strText.append(t_strText);
        return m_eHeadType;
	}
    E_TxtHeadType read(Instream& ins, wstring& strText)
	{
		wstring t_strText;
		_read(ins, t_strText);
		strText.append(t_strText);
        return m_eHeadType;
	}
	
    E_TxtHeadType read(Instream& ins, cfn_bool_t<const string&> cb)
	{
		_read(ins, cb);
        return m_eHeadType;
	}
    E_TxtHeadType read(Instream& ins, cfn_bool_t<const wstring&> cb)
	{
		_read(ins, cb);
        return m_eHeadType;
	}

private:
	void _readData(const char *lpData, size_t len, string& strText);
	void _readData(const char *lpData, size_t len, wstring& strText);

	template <class T>
    void _read(Instream& ins, T& strText)
    {
		CCharBuffer cbfData;
		if (ins.read(cbfData)>0)
		{
			_readData(cbfData, cbfData->size(), strText);
		}
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
				if ('\r' == strText.back())
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
