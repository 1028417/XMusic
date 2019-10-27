
#pragma once

#if defined(_MSC_VER)
    #define __winvc true
    #define __windows true

    #define __android false
    #define __ios false
    #define __mac false

#else
    #define __winvc false

    #include <qsystemdetection.h>
    #if defined(Q_OS_ANDROID)
        #define __windows false
        #define __android true
        #define __ios false
        #define __mac false

    #elif defined(Q_OS_IOS)
        #define __windows false
        #define __android false
        #define __ios true
        #define __mac false

    #elif defined(Q_OS_MACOS)
        #define __windows false
        #define __android false
        #define __ios false
        #define __mac true

    #else
        #define __windows true
        #define __android false
        #define __ios false
        #define __mac false
    #endif
#endif

#if __winvc
    #ifdef _DEBUG
        #define __isdebug true
    #else
        #define __isdebug false
    #endif

    #pragma warning(disable: 4251)
    #pragma warning(disable: 4275)

    #define __dllexport __declspec(dllexport)
    #define __dllimport __declspec(dllimport)

    #include <stddef.h>
    #include <stdint.h>
#else
    #include <sys/types.h>
    #include <unistd.h>

#ifdef NULL
    #undef NULL
#endif
    #define NULL nullptr

#ifdef QT_NO_DEBUG
    #define __isdebug false
#else
    #define __isdebug true
#endif

    #include <QtCore/qglobal.h>
    #define __dllexport Q_DECL_EXPORT
    #define __dllimport Q_DECL_IMPORT
#endif

#ifdef __UtilPrj
    #define __UtilExt __dllexport
#else
    #define __UtilExt __dllimport
#endif

#define memzero(x) memset(&(x), 0, sizeof(x))
#define zeroset(x) memzero(x)

#define cauto const auto&

#ifndef UINT
using UINT = unsigned int;
#endif

#ifndef BOOL
using BOOL = int;

#define TRUE 1
#define FALSE 0
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define __1e6 ((UINT)1e6)

#define __srand srand((UINT)_time32(NULL));
#define __rand(uMax) ((UINT)(rand()%(uMax+1)))

#define __Ensure(x) \
	if (!(x)) \
	{ \
		return; \
	}

#define __EnsureReturn(x, y) \
	if (!(x)) \
	{ \
		return y; \
	}

#define __EnsureContinue(x) \
	if (!(x)) \
	{ \
		continue; \
	}

#define __EnsureBreak(x) \
	if (!(x)) \
	{ \
		break; \
	}

#include <math.h>

#include "../sstl/sstl.h"
using namespace NS_SSTL;

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

using byte_t = uint8_t;

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
        int size = (int)m_vecBuff.size() - less;
        if (size >= 0)
        {
			m_vecBuff.resize(size);
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

	const TD_ByteVector& vector() const
	{
		return m_vecBuff;
	}
	TD_ByteVector& vector()
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
        int size = (int)m_strBuff.size() - less;
        if (size >= 0)
        {
			m_strBuff.resize(size);
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

#include "strutil.h"

#include "tmutil.h"

#include "fsutil.h"

#if __windows
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "winfsutil.h"

#include "winfsdlg.h"

#include "ProFile.h"

__UtilExt float getDPIRate();
//__UtilExt BOOL EnablePerMonitorDialogScaling();

__UtilExt const RECT& getWorkArea(bool bFullScreen);
#endif

#include "timerutil.h"

#include "mtutil.h"

enum class E_Platform
{
    PF_Windows,
    PF_Android,
    PF_IOS,
    PF_Mac
};

__UtilExt E_Platform platform();

#if !__winvc
__UtilExt wstring platformType();
__UtilExt wstring platformVersion();
#endif

#undef min
#undef max
