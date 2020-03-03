
#pragma once

#include <string>

#if !__winvc
#include <QString>

#define __A2Q(str, len) QString::fromUtf8(str, len)
#define __S2Q(s) __A2Q(s.c_str(), s.size())

#define __W2Q(str, len) QString::fromWCharArray(str, len)
#define __WS2Q(ws) __W2Q(ws.c_str(), ws.size())

#define to_string(x) QString::number(x).toStdString()
#define to_wstring(x) QString::number(x).toStdWString()

#define sscanf_s sscanf
#endif

#define __npos string::npos
#define __wnpos wstring::npos

#define __substr strutil::substr

class __UtilExt strutil
{
public:
	static const wchar_t wcSpace = L' ';

public:
	static int collate(const wstring& lhs, const wstring& rhs);
	static int collate_cn(const wstring& lhs, const wstring& rhs);

	template <class S>
	static S substr(const S& str, size_t pos, size_t len = -1)
	{
		if (str.length() < pos)
		{
			return S();
		}

		return str.substr(pos, len);
	}

        static bool endWith(const wstring& str, const wstring& strEnd);
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
		str.erase(pos+1);
	}
	template <class S, typename T = decltype(*S().c_str())>
	static S rtrim_r(const S& str, T t = ' ')
	{
		auto pos = str.find_last_not_of(t);
		return str.substr(0, pos+1);
	}

	template <class S>
	static void trim(S& str, const S& t)
	{
		ltrim(str, t);
		rtrim(str, t);
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

	static void split(const wstring& strText, wchar_t wcSplitor, vector<wstring>& vecRet, bool bTrim = false);
	static void split(const string& strText, char wcSplitor, vector<string>& vecRet, bool bTrim = false);

	static bool matchIgnoreCase(const wstring& str1, const wstring& str2, size_t maxlen = 0);

	static void lowerCase(wstring& str)
	{
		for (auto& chr : str)
		{
			chr = ::towlower(chr);
		}
	}

	static void lowerCase(string& str)
	{
		for (auto& chr : str)
		{
			chr = ::tolower(chr);
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
			chr = ::towupper(chr);
		}
	}

	static void upperCase(string& str)
	{
		for (auto& chr : str)
		{
			chr = ::toupper(chr);
		}
	}

	template <class S>
	static S upperCase_r(const S& str)
	{
		auto strRet = str;
		upperCase(strRet);
		return strRet;
	}

	static void replace(wstring& str, const wstring& strFind, const wchar_t *pszReplace = NULL);
	static void replace(string& str, const string& strFind, const char *pszReplace = NULL);

	static void replace(wstring& str, const wstring& strFind, const wstring& strReplace);
	static void replace(string& str, const string& strFind, const string& strReplace);

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
	
	template <class S, typename T=decltype(S()[0])>
    static void replaceChar(S& str, T chrFind, T chrReplace)
	{
		for (auto& chr : str)
		{
			if (chrFind == chr)
			{
				chr = chrReplace;
			}
		}
	}
	template <class S, typename T = decltype(S()[0])>
    static S replaceChar_r(const S& str, T chrFind, T chrReplace)
    {
        auto strRet = str;
        replaceChar(strRet, chrFind, chrReplace);
        return strRet;
    }

	template <class S, typename T = decltype(S()[0])>
    static void replaceChars(S& str, const S& strFindChars, T chrReplace)
	{
		for (auto& chr : str)
		{
			if (S::npos != strFindChars.find(chr))
			{
				chr = chrReplace;
			}
		}
	}
	template <class S, typename T = decltype(S()[0])>
    static S replaceChars_r(const S& str, const S& strFindChars, T chrReplace)
    {
        auto strRet = str;
        replaceChars(strRet, strFindChars, chrReplace);
        return strRet;
    }

	template <class S, typename T = decltype(S()[0])>
	static void eraseChar(S& str, T chrFind)
	{
		for (auto itr = str.begin(); itr != str.end(); )
		{
			if (*itr == chrFind)
			{
				itr = str.erase(itr);
			}
			else
			{
				++itr;
			}
		}
	}
	template <class S, typename T = decltype(S()[0])>
    static S eraseChar_r(const S& str, T chrFind)
    {
        auto strRet = str;
        eraseChar(strRet, chrFind);
        return strRet;
    }

    template <class S>
    static void eraseChars(S& str, const S& strFindChars)
    {
        for (auto itr=str.begin(); itr!=str.end(); )
        {
            if (S::npos != strFindChars.find(*itr))
            {
                itr = str.erase(itr);
            }
            else
            {
                ++itr;
            }
        }
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
		return base64_encode(pStr, (size_t)strlen(pStr), pszBase, chrTail);
	}
	static string base64_encode(const string& str, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_encode(str.c_str(), str.length(), pszBase, chrTail);
	}

	static string base64_decode(const char *pStr, size_t len, const char *pszBase = NULL, char chrTail = 0);
	static string base64_decode(const char *pStr, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_decode(pStr, (size_t)strlen(pStr), pszBase, chrTail);
	}
	static string base64_decode(const string& str, const char *pszBase = NULL, char chrTail = 0)
	{
		return base64_decode(str.c_str(), str.length(), pszBase, chrTail);
	}

	static bool checkUtf8(const char *pStr, int len = -1);
	static bool checkUtf8(const string& str)
	{
		return checkUtf8(str.c_str(), str.size());
	}

	static wstring fromUtf8(const char *pStr, int len = -1);
	static wstring fromUtf8(const string& str)
	{
		return fromUtf8(str.c_str(), str.size());
	}

	static string toUtf8(const wchar_t *pStr, int len = -1);
	static string toUtf8(const wstring& str)
	{
		return toUtf8(str.c_str(), str.size());
	}

	static wstring& transEndian(wstring& str);
	static wstring transEndian(const wchar_t *pStr)
	{
		wstring str(pStr);
		return transEndian(str);
	}
	static wstring transEndian(const wchar_t *pStr, size_t len)
	{
		wstring str(pStr, len);
		return transEndian(str);
	}
		
    static wstring toWstr(const char *pStr, int len = -1);
    static wstring toWstr(const string& str)
    {
        return toWstr(str.c_str(), str.size());
    }

    static string toStr(const wchar_t *pStr, int len = -1);
    static string toStr(const wstring& str)
    {
        return toStr(str.c_str(), str.size());
    }

#if !__winvc
    static QString toQstr(const wchar_t *pStr, int len = -1)
    {
        return __W2Q(pStr, len);
    }
    static QString toQstr(const wstring& str)
	{
        return __WS2Q(str);
    }

    static QString toQstr(const char* pStr, int len = -1)
    {
        return __A2Q(pStr, len);
    }
    static QString toQstr(const string& str)
    {
        return __S2Q(str);
    }
#endif

	template <typename T>
	static wstring ContainerToStr(const T& container, const wstring& strSplitor)
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
		bool operator()(const wstring& lhs, const wstring& rhs) const
		{
			return strutil::collate(lhs, rhs)<0;
		}
    };
};

#include "WString.h"
