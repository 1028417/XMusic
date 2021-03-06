
#pragma once

#include <string>

using cwstr = const wstring&;
using cwchr_p = const wchar_t*;

#define __size_max size_t(-1)

#if __windows
#define CP_GBK 936u
#endif

#if !__winvc
#include <QString>
using cqstr = const QString&;

#define __W2Q(pStr, len) QString::fromWCharArray(pStr, len)
#define __WS2Q(wstr) QString::fromStdWString(wstr)

#define to_string(x) QString::number(x).toStdString()
#define to_wstring(x) QString::number(x).toStdWString()

#define sscanf_s sscanf
#endif

#define __npos string::npos
#define __wnpos wstring::npos

#define __substr strutil::substr

class __UtilExt strutil
{
private:
	template <typename T>
	inline static size_t _checkLen(const T *pStr)
	{
		size_t len = 0;
		while (*pStr++)
		{
			len++;
		}
		return len;
	}

	inline static size_t _checkLen(const wchar_t *pStr, size_t& len)
	{
		if (__size_max == len)
		{
			len = _checkLen(pStr);
		}
		return len;
	}

	inline static size_t _checkLen(const char *pStr, size_t& len)
	{
		if (size_t (-1) == len)
		{
			len = _checkLen(pStr);
		}
		return len;
	}

	template <typename T, class C = basic_string<T, char_traits<T>, allocator<T>>, class RET>
	static void _split(const C& strText, T wcSplitor, RET& vecRet)
	{
		/*auto fn = [&](const C& strSub) {
			if (bTrim && wcSplitor != L' ')
			{
				cauto str = strutil::trim_r(strSub);
				if (!str.empty())
				{
					vecRet.push_back(str);
				}
			}
			else
			{
				vecRet.push_back(strSub);
			}
		};*/

		size_t pos = 0;
		while ((pos = strText.find_first_not_of(wcSplitor, pos)) != C::npos)
		{
			auto nextPos = strText.find(wcSplitor, pos);
			if (C::npos == nextPos)
			{
				cauto strSub = strText.substr(pos);
				vecRet.push_back(strSub);  //fn(strSub);
				break;
			}

			cauto strSub = strText.substr(pos, nextPos - pos);
			vecRet.push_back(strSub);  //fn(strSub);

			pos = nextPos + 1;
		}
	}

public:
	static int collate(cwstr lhs, cwstr rhs);
	static int collate_cn(cwstr lhs, cwstr rhs);

	static UINT checkWordCount(cwstr str);

	template <class S>
	static S substr(const S& str, size_t pos, size_t len = __size_max)
	{
		if (str.length() < pos)
		{
			return S();
		}

		return str.substr(pos, len);
	}

	static bool endWith(cwstr str, cwstr strEnd);
	static bool endWith(const string& str, const string& strEnd);

	template <class S, typename T = decltype(*S().c_str())>
	inline static void ltrim(S& str, T t = ' ')
	{
		auto pos = str.find_first_not_of(t);
		str.erase(0, pos);
	}

	template <class S, typename T = decltype(*S().c_str())>
	static S ltrim_r(const S& str, T t = ' ')
	{
		auto pos = str.find_first_not_of(t);
		if (S::npos == pos)
		{
			return S();
		}

		if (pos > 0)
		{
			return str.substr(pos);
		}

		return str;
	}

	template <class S, typename T = decltype(*S().c_str())>
	inline static void rtrim(S& str, T t = ' ')
	{
		auto pos = str.find_last_not_of(t);
		str.erase(pos + 1);
	}
	template <class S, typename T = decltype(*S().c_str())>
	static S rtrim_r(const S& str, T t = ' ')
	{
		auto pos = str.find_last_not_of(t);
		return str.substr(0, pos + 1);
	}

	template <class S, typename T = decltype(*S().c_str())>
	static void trim(S& str, T t = ' ')
	{
		ltrim(str, t);
		rtrim(str, t);
	}

	template <class S, typename T = decltype(*S().c_str())>
	static S trim_r(const S& str, T t = ' ')
	{
		auto strRet = ltrim_r(str, t);
		rtrim(strRet, t);
		return strRet;
	}

	template <class T>
	static void split(cwstr strText, wchar_t wcSplitor, T& vecRet)
	{
		_split(strText, wcSplitor, vecRet);
	}

	template <class T>
	static void split(const string& strText, char wcSplitor, T& vecRet)
	{
		_split(strText, wcSplitor, vecRet);
	}

	static bool matchIgnoreCase(cwstr str1, cwstr str2, size_t maxlen = __size_max);
	static bool matchIgnoreCase(cwchr_p pstr1, cwchr_p pstr2, size_t maxlen = __size_max);

	template <typename C>
	inline static C lowerCase(C chr)
	{
		if (chr >= 'A' && chr <= 'Z')
		{
			return chr - 'A' + 'a';
		}
		return chr;
	}
	template <typename C>
	inline static C upperCase(C chr)
	{
		if (chr >= 'a' && chr <= 'z')
		{
			return chr - 'a' + 'A';
		}
		return chr;
	}

	static void lowerCase(wstring& str)
	{
		for (auto& chr : str)
		{
			chr = lowerCase(chr);
		}
	}
	static void lowerCase(string& str)
	{
		for (auto& chr : str)
		{
			chr = lowerCase(chr);
		}
	}
	template <class S>
	static S lowerCase_r(const S& str)
	{
		auto strRet = str;
		lowerCase(strRet);
		return strRet;
	}

	static void upperCase(wstring& str)
	{
		for (auto& chr : str)
		{
			chr = upperCase(chr);
		}
	}
	static void upperCase(string& str)
	{
		for (auto& chr : str)
		{
			chr = upperCase(chr);
		}
	}
	template <class S>
	static S upperCase_r(const S& str)
	{
		auto strRet = str;
		upperCase(strRet);
		return strRet;
	}

	static UINT replace(wstring& str, cwstr strFind, const wchar_t *pszReplace = NULL);
	static UINT replace(string& str, const string& strFind, const char *pszReplace = NULL);

	static UINT replace(wstring& str, cwstr strFind, cwstr strReplace);
	static UINT replace(string& str, const string& strFind, const string& strReplace);

	template <class S, typename T = decltype(S().c_str())>
	static S replace_r(const S& str, const S& strFind, T pszReplace = NULL)
	{
		auto strRet = str;
		replace(strRet, strFind, pszReplace);
		return strRet;
	}

	template <class S>
	static S replace_r(const S& str, const S& strFind, const S& strReplace)
	{
		auto strRet = str;
		replace(strRet, strFind, strReplace);
		return strRet;
	}

	template <class S, typename T = decltype(S()[0])>
	static UINT replaceChar(S& str, T chrFind, T chrReplace)
	{
		UINT uRet = 0;
		for (auto& chr : str)
		{
			if (chrFind == chr)
			{
				chr = chrReplace;
				uRet++;
			}
		}

		return uRet;
	}
	template <class S, typename T = decltype(S()[0])>
	static S replaceChar_r(const S& str, T chrFind, T chrReplace)
	{
		auto strRet = str;
		replaceChar(strRet, chrFind, chrReplace);
		return strRet;
	}

	template <class S, typename T = decltype(S()[0])>
	static UINT replaceChars(S& str, const S& strFindChars, T chrReplace)
	{
		UINT uRet = 0;
		for (auto& chr : str)
		{
			if (S::npos != strFindChars.find(chr))
			{
				chr = chrReplace;
				uRet++;
			}
		}

		return uRet;
	}
	template <class S, typename T = decltype(S()[0])>
	static S replaceChars_r(const S& str, const S& strFindChars, T chrReplace)
	{
		auto strRet = str;
		replaceChars(strRet, strFindChars, chrReplace);
		return strRet;
	}

	template <class S, typename T = decltype(S()[0])>
	static UINT eraseChar(S& str, T chrFind)
	{
		UINT uRet = 0;
		for (auto itr = str.begin(); itr != str.end(); )
		{
			if (*itr == chrFind)
			{
				itr = str.erase(itr);
				uRet++;
			}
			else
			{
				++itr;
			}
		}

		return uRet;
	}
	template <class S, typename T = decltype(S()[0])>
	static S eraseChar_r(const S& str, T chrFind)
	{
		auto strRet = str;
		eraseChar(strRet, chrFind);
		return strRet;
	}

	template <class S>
	static UINT eraseChars(S& str, const S& strFindChars)
	{
		UINT uRet = 0;
		for (auto itr = str.begin(); itr != str.end(); )
		{
			if (S::npos != strFindChars.find(*itr))
			{
				itr = str.erase(itr);
				uRet++;
			}
			else
			{
				++itr;
			}
		}

		return uRet;
	}
	template <class S>
	static S eraseChars_r(const S& str, const S& strFindChars)
	{
		auto strRet = str;
		eraseChars(strRet, strFindChars);
		return strRet;
	}

	static string base64_encode(const char *pStr, size_t len, const char *pszBase = NULL, char chrTail = 0);
	static string base64_encode(const char *pStr, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_encode(pStr, _checkLen(pStr), pszBase, chrTail);
	}
	static string base64_encode(const string& str, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_encode(str.c_str(), str.size(), pszBase, chrTail);
	}

	static string base64_decode(const char *pStr, size_t len, const char *pszBase = NULL, char chrTail = 0);
	static string base64_decode(const char *pStr, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_decode(pStr, _checkLen(pStr), pszBase, chrTail);
	}
	static string base64_decode(const string& str, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_decode(str.c_str(), str.size(), pszBase, chrTail);
	}

#if __windows
	static string toMbs(const wchar_t *pStr, size_t len, UINT uCodePage = CP_ACP);
	static string toMbs(const wchar_t *pStr, UINT uCodePage = CP_ACP)
	{
		return toMbs(pStr, _checkLen(pStr), uCodePage);
	}
	static string toMbs(cwstr str, UINT uCodePage = CP_ACP)
	{
		return toMbs(str.c_str(), str.size(), uCodePage);
	}

	static wstring fromMbs(const char *pStr, size_t len, UINT uCodePage = CP_ACP);
	static wstring fromMbs(const char *pStr, UINT uCodePage = CP_ACP)
	{
		return fromMbs(pStr, _checkLen(pStr), uCodePage);
	}
	static wstring fromMbs(const string& str, UINT uCodePage = CP_ACP)
	{
		return fromMbs(str.c_str(), str.size(), uCodePage);
	}
#endif

	static bool checkUtf8(const char *pStr, size_t len = __size_max);
	static bool checkUtf8(const string& str)
	{
		return checkUtf8(str.c_str(), str.size());
	}

	static wstring fromUtf8(const char *pStr, size_t len = __size_max);
	static wstring fromUtf8(const string& str)
	{
		return fromUtf8(str.c_str(), str.size());
	}

	static string toUtf8(const wchar_t *pStr, size_t len = __size_max);
    static string toUtf8(cwstr str)
	{
		return toUtf8(str.c_str(), str.size());
	}
	
    static wstring fromGbk(const char *pStr, size_t len = __size_max);
    static wstring fromGbk(const string& str)
    {
        return fromGbk(str.c_str(), str.size());
    }

    static string toGbk(const wchar_t *pStr, size_t len = __size_max);
    static string toGbk(cwstr str)
    {
        return toGbk(str.c_str(), str.size());
    }

#if !__winvc
    static string toGbk(cqstr qs);
#endif

    static string toAsc(const wchar_t *pStr, size_t len = __size_max);
    static string toAsc(cwstr str)
    {
        return string(str.begin(), str.end());
    }
    static void toAsc(cwstr str, string& strRet)
    {
        strRet.assign(str.begin(), str.end());
    }

    static wstring fromAsc(const char *pStr, size_t len = __size_max);
    static wstring fromAsc(const string& str)
    {
        return wstring(str.begin(), str.end());
    }
    static void fromAsc(const string& str, wstring& strRet)
    {
        strRet.assign(str.begin(), str.end());
    }

	static wstring fromStr(const char *pStr, size_t len = __size_max);
	static wstring fromStr(const string& str)
	{
		return fromStr(str.c_str(), str.size());
	}

	template <typename T>
    static wstring ContainerToStr(const T& container, cwstr strSplitor)
	{
        wstringstream strmRet;
        for (typename T::const_iterator it = container.begin(); ; )
		{
            strmRet << *it;
			
			it++;
			__EnsureBreak(it != container.end());

            strmRet << strSplitor;
		}

        return strmRet.str();
	}
	
	struct tagCNSortor
	{
        bool operator()(cwstr lhs, cwstr rhs) const
		{
			return strutil::collate(lhs, rhs)<0;
		}
    };
};

#include "WString.h"
