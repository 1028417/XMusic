
#pragma once

#ifdef _MSC_VER
	#define __winvc true
	#define __winqt false
	#define __android false

#else
	#ifdef __ANDROID__
		#define __winvc false
		#define __winqt false
		#define __android true
	#else
		#define __winvc false
		#define __winqt true
		#define __android false
	#endif
#endif

#if __winvc
    #pragma warning(disable: 4251)
    #pragma warning(disable: 4275)

    #include <stddef.h>
    #include <stdint.h>
#else
    #include <sys/types.h>
    #include <unistd.h>

    #ifndef QT_NO_DEBUG
        #define _DEBUG
    #endif
#endif

#if __android
	#define __dllexport
	#define __dllimport
#else
	#define __dllexport __declspec(dllexport)
	#define __dllimport __declspec(dllimport)
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

using namespace std;

#include <sstream>

#include <list>
#include <vector>
#include <set>
#include <map>

#include <algorithm>

#include <functional>
using fn_void = function<void()>;

#include "../sstl/sstl.h"
using namespace NS_SSTL;

#include "wsutil.h"

#include "tmutil.h"

#include "fsutil.h"

#include "fsutil.h"

#include "TxtWriter.h"

#include "Path.h"

#include "SQLiteDB.h"

#include "mtutil.h"

#if !__android
#include "winfsutil.h"

#include "fsdlg.h"

#include "wintimer.h"

#include "ProFile.h"
#endif

#undef min
#undef max
