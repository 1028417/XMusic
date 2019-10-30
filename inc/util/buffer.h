
#pragma once

template<typename T>
struct TBuffer
{
public:
	TBuffer(size_t size)
	{
		m_size = MAX(1, size);
		m_pBuff = (T*)calloc(1, m_size * sizeof(T));
	}

	~TBuffer()
	{
		free(m_pBuff);
	}

	TBuffer(const TBuffer& other) = delete;
	TBuffer& operator=(const TBuffer& other) = delete;

private:
	size_t m_size;
	T *m_pBuff;

public:
	size_t size() const
	{
		return m_size;
	}

	operator T*()
	{
		return m_pBuff;
	}

	operator const T*() const
	{
		return m_pBuff;
	}
};

class __UtilExt CByteBuff
{
private:
	using TD_ByteVector = vector<byte_t>;
	TD_ByteVector m_vecBuff;

public:
    CByteBuff() {}

    CByteBuff(size_t size)
        : m_vecBuff(size)
	{
	}

	CByteBuff(const CByteBuff& other)
		: m_vecBuff(other.m_vecBuff.begin(), other.m_vecBuff.end())
	{
	}
	CByteBuff& operator=(const CByteBuff& other)
	{
		m_vecBuff.assign(other.m_vecBuff.begin(), other.m_vecBuff.end());
		return *this;
	}

	CByteBuff(const TD_ByteVector& vecBuff)
		: m_vecBuff(vecBuff.begin(), vecBuff.end())
	{
	}
	CByteBuff& operator=(const TD_ByteVector& vecBuff)
	{
		m_vecBuff.assign(vecBuff.begin(), vecBuff.end());
		return *this;
	}

	CByteBuff(CByteBuff&& other)
	{
		m_vecBuff.swap(other.m_vecBuff);
	}
	CByteBuff& operator=(CByteBuff&& other)
	{
		m_vecBuff.swap(other.m_vecBuff);
		return *this;
	}

	CByteBuff(TD_ByteVector&& vecBuff)
	{
		m_vecBuff.swap(vecBuff);
	}
	CByteBuff& operator=(TD_ByteVector&& vecBuff)
	{
		m_vecBuff.swap(vecBuff);
		return *this;
	}

public:
    byte_t* resizeMore(size_t size)
    {
        if (0 == size)
        {
            return NULL;
        }

        auto pos = m_vecBuff.size();
		m_vecBuff.resize(pos+size);
        return ptr()+pos;
    }

    void resizeLess(size_t less)
    {
        size_t size = m_vecBuff.size();
        if (less <= size)
        {
            m_vecBuff.resize(size-less);
        }
    }

	operator bool() const
	{
		return !m_vecBuff.empty();
	}
	bool operator !() const
	{
		return m_vecBuff.empty();
	}

    inline const byte_t* ptr() const
	{
        if (m_vecBuff.empty())
        {
            return NULL;
        }
        return &m_vecBuff.front();
	}
	inline byte_t* ptr()
	{
		if (m_vecBuff.empty())
		{
			return NULL;
		}
		return &m_vecBuff.front();
	}

	operator const byte_t*() const
	{
		return ptr();
	}
	operator byte_t*()
	{
		return ptr();
	}

	operator const void*() const
	{
		return ptr();
	}
	operator void*()
	{
		return ptr();
	}

	const TD_ByteVector* operator->() const
	{
		return &m_vecBuff;
	}
	TD_ByteVector* operator->()
	{
		return &m_vecBuff;
	}

	const TD_ByteVector& operator*() const
	{
		return m_vecBuff;
	}
	TD_ByteVector& operator*()
	{
		return m_vecBuff;
	}

	operator const TD_ByteVector&() const
	{
		return m_vecBuff;
	}
	operator TD_ByteVector&()
	{
		return m_vecBuff;
	}

	const TD_ByteVector& data() const
	{
		return m_vecBuff;
	}
	TD_ByteVector& data()
	{
		return m_vecBuff;
	}
	
	char* toStr()
	{
		m_vecBuff.push_back('\0');
		return (char*)ptr();
	}
};

class __UtilExt CCharBuff
{
private:
	string m_strBuff;

public:
    CCharBuff() {}

    CCharBuff(size_t size)
        : m_strBuff(size, '\0')
    {
    }

	CCharBuff(const CCharBuff& other)
		: m_strBuff(other.m_strBuff.begin(), other.m_strBuff.end())
	{
	}
	CCharBuff operator=(const CCharBuff& other)
	{
		m_strBuff.assign(other.m_strBuff.begin(), other.m_strBuff.end());
		return *this;
	}

	CCharBuff(const string& strBuff)
		: m_strBuff(strBuff.begin(), strBuff.end())
	{
	}
	CCharBuff operator=(const string& strBuff)
	{
		m_strBuff.assign(strBuff.begin(), strBuff.end());
		return *this;
	}

	CCharBuff(CCharBuff&& other)
	{
		m_strBuff.swap(other.m_strBuff);
	}
	CCharBuff operator=(CCharBuff&& other)
	{
		m_strBuff.swap(other.m_strBuff);
		return *this;
	}

	CCharBuff(string&& strBuff)
	{
		m_strBuff.swap(strBuff);
	}
	CCharBuff operator=(string&& strBuff)
	{
		m_strBuff.swap(strBuff);
		return *this;
	}

public:
    char* resizeMore(size_t more)
    {
        if (0 == more)
        {
            return NULL;
        }

        auto pos = m_strBuff.size();
		m_strBuff.resize(pos+more);
        return ptr()+pos;
    }

    void resizeLess(size_t less)
    {
        size_t size = m_strBuff.size();
        if (less <= size)
        {
            m_strBuff.resize(size-less);
        }
    }

	operator bool() const
	{
		return !m_strBuff.empty();
	}
	bool operator !() const
	{
		return m_strBuff.empty();
	}

	inline const char* ptr() const
	{
		if (m_strBuff.empty())
		{
			return NULL;
		}
		return &m_strBuff.front();
	}
	inline char* ptr()
	{
		if (m_strBuff.empty())
		{
			return NULL;
		}
		return &m_strBuff.front();
	}

	operator const char*() const
	{
		return ptr();
	}
    operator char*()
    {
        return ptr();
    }

	operator const byte_t*() const
	{
		return (const byte_t*)ptr();
	}
	operator byte_t*()
	{
		return (byte_t*)ptr();
	}

	operator const void*() const
	{
		return (const byte_t*)ptr();
	}
	operator void*()
	{
		return (byte_t*)ptr();
	}

	operator string() const
	{
		return m_strBuff.c_str();
	}

	string str() const
	{
		return m_strBuff.c_str();
	}

	const char* c_str() const
	{
		return m_strBuff.c_str();
	}
};
