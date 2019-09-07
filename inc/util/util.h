
#pragma once

#if defined(_MSC_VER)
    #define __windows true
    #define __winvc true
    #define __winqt false
    #define __android false
    #define __ios false
    #define __mac false

#else
    #include <qsystemdetection.h>

    #if defined(Q_OS_ANDROID)
        #define __windows false
        #define __winvc false
        #define __winqt false
        #define __android true
        #define __ios false
        #define __mac false

    #elif defined(Q_OS_IOS)
    #define __windows false
        #define __winvc false
        #define __winqt false
        #define __android false
        #define __ios true
        #define __mac false

    #elif defined(Q_OS_MACOS)
        #define __windows false
        #define __winvc false
        #define __winqt false
        #define __android false
        #define __ios false
        #define __mac true

    #else
        #define __windows true
        #define __winvc false
        #define __winqt true
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

    #define __dllexport //Q_CORE_EXPORT
    #define __dllimport

#ifdef QT_NO_DEBUG
    #define __isdebug false
#else
    #define __isdebug true
#endif
#endif

#ifdef __UtilPrj
    #define __UtilExt __dllexport
#else
    #define __UtilExt __dllimport
#endif

#define zeroset(x) memset(&(x), 0, sizeof((x)))

#define cauto const auto

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

#include "../sstl/sstl.h"
using namespace NS_SSTL;

using fn_void = function<void()>;

#include "wsutil.h"

#include "tmutil.h"

#include "fsutil.h"

#include "fsutil.h"

#include "TxtWriter.h"

#include "Path.h"

#include "SQLiteDB.h"

#include "mtutil.h"

#if __windows
#include "winfsutil.h"

#include "fsdlg.h"

#include "wintimer.h"

#include "ProFile.h"
#endif

#undef min
#undef max
