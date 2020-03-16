
#pragma once

class __UtilExt WString
{
private:
	wstring m_str;

public:
	WString() = default;

	WString(const wstring& str)
		: m_str(str)
	{
	}

	WString(const wchar_t *pStr)
		: m_str(pStr ? pStr : L"")
	{
	}

	WString(WString&& other)
	{
		m_str.swap(other.m_str);
	}

	WString(wstring&& str)
	{
		m_str.swap(str);
	}

	WString& operator=(WString&& other)
	{
		m_str.swap(other.m_str);
		return *this;
	}

	WString& operator=(wstring&& str)
	{
		m_str.swap(str);
		return *this;
	}

	WString& operator=(const wstring& str)
	{
		m_str.assign(str);
		return *this;
	}

	WString& operator=(const wchar_t *pStr)
	{
		m_str.clear();
		if (pStr)
		{
			m_str.append(pStr);
		}
		return *this;
	}

public:
#if !__winvc
    operator QString() const
    {
        return strutil::toQstr(m_str);
    }

    QString qstr() const
    {
        return strutil::toQstr(m_str);
    }
#endif

	operator const wstring&() const
	{
		return m_str;
	}
	operator wstring&()
	{
		return m_str;
	}

	const wstring* operator->() const
	{
		return &m_str;
	}
	wstring* operator->()
	{
		return &m_str;
	}

    WString& operator<<(const WString& other)
    {
		m_str.append(other.m_str);
        return *this;
    }

    WString& operator<<(const wchar_t *pStr)
    {
        if (pStr)
        {
			m_str.append(pStr);
        }
        return *this;
    }

	WString& operator<<(wchar_t wc)
	{
		m_str.push_back(wc);
		return *this;
	}

	WString& operator<<(char c)
	{
		m_str.push_back(c);
		return *this;
	}

    WString& operator<<(const wstring& str)
    {
		m_str.append(str);
        return *this;
    }

    template <typename T>
    WString& operator<<(const T& t)
    {
		m_str.append(to_wstring(t));
        return *this;
    }

    template <typename T>
    WString& append(const T& t)
    {
        *this << t;
        return *this;
    }

	template <typename T>
	WString operator +(const T& t) const
	{
		WString ret(*this);
		ret << t;
		return ret;
	}

	friend WString operator +(const wstring& lhs, const WString& rhs)
	{
		WString ret(lhs);
		ret << rhs;
		return ret;
	}

	friend WString operator +(const wchar_t *lhs, const WString& rhs)
	{
		WString ret(lhs);
		ret << rhs;
		return ret;
	}

	bool operator ==(const wstring& str) const
	{
		return m_str == str;
	}

	bool operator ==(const wchar_t *pStr) const
	{
		if (NULL == pStr)
		{
			return m_str.empty();
		}

		return m_str.compare(pStr);
	}

	friend bool operator ==(const wstring& lhs, const WString& rhs)
	{
		return rhs == lhs;
	}

	friend bool operator ==(const wchar_t *lhs, const WString& rhs)
	{
		return rhs == lhs;
	}
};
