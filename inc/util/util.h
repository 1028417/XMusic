
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

using byte_t = uint8_t;

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
