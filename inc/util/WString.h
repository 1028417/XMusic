
#pragma once

class __UtilExt WString
{
private:
	wstring m_str;

public:
	WString() = default;

	WString(const WString& ws)
		: m_str(ws.m_str)
	{
	}

	WString(cwstr str)
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

	WString& operator=(const WString& ws)
	{
		m_str.assign(ws.m_str);
		return *this;
	}

	WString& operator=(cwstr str)
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

        bool operator ==(const WString& other) const
        {
                return m_str == other.m_str;
        }
        bool operator !=(const WString& other) const
        {
                return m_str != other.m_str;
        }

        inline bool operator ==(cwstr str) const
        {
                return m_str == str;
        }
        inline bool operator !=(cwstr str) const
        {
                return m_str != str;
        }

        inline bool operator ==(const wchar_t *pStr) const
        {
                if (NULL == pStr)
                {
                        return m_str.empty();
                }

                return 0==m_str.compare(pStr);
        }
        inline bool operator !=(const wchar_t *pStr) const
        {
                if (NULL == pStr)
                {
                        return !m_str.empty();
                }

                return 0!=m_str.compare(pStr);
        }

        friend bool operator ==(cwstr lhs, const WString& rhs)
        {
                return rhs == lhs;
        }
        friend bool operator !=(cwstr lhs, const WString& rhs)
        {
                return rhs != lhs;
        }

        friend bool operator ==(const wchar_t *lhs, const WString& rhs)
        {
                return rhs == lhs;
        }
        friend bool operator !=(const wchar_t *lhs, const WString& rhs)
        {
                return rhs != lhs;
        }

public:
#if !__winvc
    operator QString() const
    {
        return __WS2Q(m_str);
    }

    QString qstr() const
    {
        return __WS2Q(m_str);
    }
#endif

	operator cwstr () const
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

    WString& operator<<(cwstr str)
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

	friend WString operator +(cwstr lhs, const WString& rhs)
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
};
