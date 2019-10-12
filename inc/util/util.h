
#pragma once

#if defined(_MSC_VER)
    #define __android false
    #define __ios false
    #define __mac false
    #define __windows true
    #define __winvc true

#else
    #define __winvc false

    #include <qsystemdetection.h>
    #if defined(Q_OS_ANDROID)
        #define __android true
        #define __ios false
        #define __mac false
		#define __windows false

    #elif defined(Q_OS_IOS)
        #define __android false
        #define __ios true
        #define __mac false
		#define __windows false

    #elif defined(Q_OS_MACOS)
        #define __android false
        #define __ios false
        #define __mac true
		#define __windows false

    #else
        #define __android false
        #define __ios false
        #define __mac false
		#define __windows true
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

    #include <QtCore/qglobal.h>
    #define __dllexport Q_DECL_EXPORT
    #define __dllimport Q_DECL_IMPORT

    //#define __dllexport
    //#define __dllimport

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

#ifdef NULL
#undef NULL
#endif
#define NULL nullptr

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

#include "jsonutil.h"

#if __windows
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "winfsutil.h"

#include "winfsdlg.h"

#include "wintimer.h"

#include "ProFile.h"

__UtilExt float getDPIRate();
//__UtilExt BOOL EnablePerMonitorDialogScaling();

__UtilExt const RECT& getWorkArea(bool bFullScreen);
#endif

#include "mtutil.h"

enum class E_Platform
{
    PF_Windows,
    PF_Android,
    PF_IOS,
    PF_Mac
};

E_Platform platform();

#if !__winvc
wstring platformType();
wstring platformVersion();
#endif

#undef min
#undef max
