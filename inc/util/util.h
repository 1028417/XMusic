
#pragma once

#include <math.h>

#include "../sstl/sstl.h"
using namespace NS_SSTL;

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
#else
    #include <sys/types.h>
    #include <unistd.h>

#ifdef QT_NO_DEBUG
    #define __isdebug false
#else
    #define __isdebug true
#endif

    #include <QtGlobal>
    #define __dllexport Q_DECL_EXPORT
    #define __dllimport Q_DECL_IMPORT
#endif

#ifdef __UtilPrj
    #define __UtilExt __dllexport
#else
    #define __UtilExt __dllimport
#endif

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

#define memzero(x) memset(&(x), 0, sizeof(x))
#define zeroset(x) memzero(x)

#define cauto const auto&

using byte_t = unsigned char;
using byte_p = byte_t*;
using cbyte_p = const byte_t*;

#ifndef BOOL
using BOOL = int;

#define TRUE 1
#define FALSE 0
#endif

#ifndef UINT
using UINT = unsigned int;
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define __1e6 ((UINT)1e6)

#define __srand srand((UINT)time(0));
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

#include "buffer.h"

#include "strutil.h"

#include "tmutil.h"

#include "fsutil.h"

#if __windows
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "winfsutil.h"
#include "winfsdlg.h"
//#include "ProFile.h"

__UtilExt float getDPIRate();
//__UtilExt BOOL EnablePerMonitorDialogScaling();

__UtilExt const RECT& getWorkArea(bool bFullScreen);
#endif

#include "timerutil.h"

#include "mtutil.h"

#include "mtutil.h"

#if !__winvc
#include "curlutil.h"
#endif

#undef min
#undef max

#if !__winvc
//#include <QRgb>
#define QRGB(r, g, b) ((0xffu << 24) | (((r) & 0xffu) << 16) | (((g) & 0xffu) << 8) | ((b) & 0xffu))
#endif
