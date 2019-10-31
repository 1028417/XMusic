
#pragma once

template<typename T>
struct TBuffer
{
public:
	TBuffer(size_t count)
	{
		m_count = MAX(1, count);
		m_size = m_count * sizeof(T);

		m_pBuff = (T*)malloc(m_size);
		memset(m_pBuff, 0, m_size);
	}
	
	~TBuffer()
	{
		free(m_pBuff);
	}

	TBuffer(const TBuffer& other) = delete;
	TBuffer(TBuffer&& other) = delete;
	TBuffer& operator=(const TBuffer& other) = delete;
	TBuffer& operator=(TBuffer&& other) = delete;

private:
	size_t m_count;
	size_t m_size;

	T *m_pBuff;

public:
	size_t size() const
	{
		return m_size;
	}
	size_t count() const
	{
		return m_count;
	}

	T* ptr()
	{
		return m_pBuff;
	}
	const T* ptr() const
	{
		return m_pBuff;
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

using TD_ByteBuffer = TBuffer<byte_t>;
using TD_CharBuffer = TBuffer<char>;

class __UtilExt CByteBuffer
{
private:
	using TD_ByteVector = vector<byte_t>;
	TD_ByteVector m_vecBuff;

public:
    CByteBuffer() {}

    CByteBuffer(size_t size, byte_t val = 0)
        : m_vecBuff(size, val)
	{
	}

	CByteBuffer(const CByteBuffer& other)
		: m_vecBuff(other.m_vecBuff.begin(), other.m_vecBuff.end())
	{
	}
	CByteBuffer& operator=(const CByteBuffer& other)
	{
		m_vecBuff.assign(other.m_vecBuff.begin(), other.m_vecBuff.end());
		return *this;
	}

	CByteBuffer(const TD_ByteVector& vecBuff)
		: m_vecBuff(vecBuff.begin(), vecBuff.end())
	{
	}
	CByteBuffer& operator=(const TD_ByteVector& vecBuff)
	{
		m_vecBuff.assign(vecBuff.begin(), vecBuff.end());
		return *this;
	}

	CByteBuffer(CByteBuffer&& other)
	{
		m_vecBuff.swap(other.m_vecBuff);
	}
	CByteBuffer& operator=(CByteBuffer&& other)
	{
		m_vecBuff.swap(other.m_vecBuff);
		return *this;
	}

	CByteBuffer(TD_ByteVector&& vecBuff)
	{
		m_vecBuff.swap(vecBuff);
	}
	CByteBuffer& operator=(TD_ByteVector&& vecBuff)
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
};

class __UtilExt CCharBuffer
{
private:
	string m_strBuff;

public:
    CCharBuffer() {}

    CCharBuffer(size_t size, char val = '\0')
        : m_strBuff(size, val)
    {
    }

	CCharBuffer(const CCharBuffer& other)
		: m_strBuff(other.m_strBuff.begin(), other.m_strBuff.end())
	{
	}
	CCharBuffer operator=(const CCharBuffer& other)
	{
		m_strBuff.assign(other.m_strBuff.begin(), other.m_strBuff.end());
		return *this;
	}

	CCharBuffer(const string& strBuff)
		: m_strBuff(strBuff.begin(), strBuff.end())
	{
	}
	CCharBuffer operator=(const string& strBuff)
	{
		m_strBuff.assign(strBuff.begin(), strBuff.end());
		return *this;
	}

	CCharBuffer(CCharBuffer&& other)
	{
		m_strBuff.swap(other.m_strBuff);
	}
	CCharBuffer operator=(CCharBuffer&& other)
	{
		m_strBuff.swap(other.m_strBuff);
		return *this;
	}

	CCharBuffer(string&& strBuff)
	{
		m_strBuff.swap(strBuff);
	}
	CCharBuffer operator=(string&& strBuff)
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

	const string* operator->() const
	{
		return &m_strBuff;
	}
	string* operator->()
	{
		return &m_strBuff;
	}

	const string& operator*() const
	{
		return m_strBuff;
	}
	string& operator*()
	{
		return m_strBuff;
	}

    operator const string&() const
    {
        return m_strBuff;
    }
    operator string&()
    {
        return m_strBuff;
    }

    const string& str() const
    {
        return m_strBuff;
    }
    string& str()
    {
        return m_strBuff;
    }

	const char* c_str() const
	{
		return m_strBuff.c_str();
	}
};
