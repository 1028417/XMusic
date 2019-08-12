
#pragma once

#ifdef _MSC_VER
#include <stddef.h>
#include <stdint.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

#ifdef __ANDROID__
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
using fn_voidvoid = function<void()>;

#include "../sstl/sstl.h"
using namespace NS_SSTL;

#include "wsutil.h"

#include "tmutil.h"

#include "fsutil.h"

#include "fsutil.h"

#include "TxtWriter.h"

#include "Path.h"

#include "SQLiteDB.h"

#ifndef __ANDROID__
#include "winfsutil.h"

#include "fsdlg.h"

#include "wintimer.h"

#include "ProFile.h"
#endif

#include "mtutil.h"

#undef min
#undef max
