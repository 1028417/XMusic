
#pragma once

#include <string>

#if !__winvc
#include <QString>

#define __A2Q QString::fromUtf8
#define __S2Q(s) __A2Q(s.c_str(), s.size())

#define __W2Q QString::fromWCharArray
#define __WS2Q(ws) __W2Q(ws.c_str(), ws.size())

#define to_string(x) QString::number(x).toStdString()
#define to_wstring(x) QString::number(x).toStdWString()

#define sscanf_s sscanf
#endif

#define __substr strutil::substr

class __UtilExt strutil
{
public:
	static const wchar_t wcSpace = L' ';

public:
	static int collate(const wstring& lhs, const wstring& rhs);
	static int collate_cn(const wstring& lhs, const wstring& rhs);

	static wstring substr(const wstring& str, size_t pos, size_t len = wstring::npos);
	static string substr(const string& str, size_t pos, size_t len = string::npos);

	static bool endWith(const wstring& str, const wstring& strEnd);
	static bool endWith(const string& str, const string& strEnd);

	static void trim(wstring& strText, wchar_t chr = ' ');
	static void trim(string& strText, char chr = ' ');
	static wstring trim_r(const wstring& strText, wchar_t chr = ' ');
	static string trim_r(const string& strText, char chr = ' ');

	static void ltrim(wstring& strText, wchar_t chr = ' ');
	static void ltrim(string& strText, char chr = ' ');
	static wstring ltrim_r(const wstring& strText, wchar_t chr = ' ');
	static string ltrim_r(const string& strText, char chr = ' ');

	static void rtrim(wstring& strText, wchar_t chr = ' ');
	static void rtrim(string& strText, char chr = ' ');
	static wstring rtrim_r(const wstring& strText, wchar_t chr = ' ');
	static string rtrim_r(const string& strText, char chr = ' ');

	static void split(const wstring& strText, wchar_t wcSplitor, vector<wstring>& vecRet, bool bTrim = false);
	static void split(const string& strText, char wcSplitor, vector<string>& vecRet, bool bTrim = false);

	static bool matchIgnoreCase(const wstring& str1, const wstring& str2, size_t maxlen = 0);

	static void lowerCase(wstring& str);
	static void lowerCase(string& str);
	static wstring lowerCase_r(const wstring& str);
	static string lowerCase_r(const string& str);

	static void upperCase(wstring& str);
	static void upperCase(string& str);
	static wstring upperCase_r(const wstring& str);
	static string upperCase_r(const string& str);

	static void replace(wstring& str, const wstring& strFind, const wstring& strReplace = L"");
	static void replace(string& str, const string& strFind, const string& strReplace = "");
	static wstring replace_r(const wstring& str, const wstring& strFind, const wstring& strReplace = L"")
	{
		wstring strRet = str;
		replace(strRet, strFind, strReplace);
		return strRet;
	}
	static string replace_r(const string& str, const string& strFind, const string& strReplace = "")
	{
		string strRet = str;
		replace(strRet, strFind, strReplace);
		return strRet;
	}

    static void replaceChar(wstring& str, wchar_t chrFind, wchar_t chrReplace);
    static wstring replaceChar_r(const wstring& str, wchar_t chrFind, wchar_t chrReplace)
    {
        wstring strRet(str);
        replaceChar(strRet, chrFind, chrReplace);
        return strRet;
    }

    static void replaceChars(wstring& str, const wstring& strFindChars, wchar_t chrReplace);
    static wstring replaceChars_r(const wstring& str, const wstring& strFindChars, wchar_t chrReplace)
    {
        wstring strRet(str);
        replaceChars(strRet, strFindChars, chrReplace);
        return strRet;
    }

	static void eraseChar(wstring& str, wchar_t chrFind);
    static wstring eraseChar_r(const wstring& str, wchar_t chrFind)
    {
        wstring strRet(str);
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
        S strRet(str);
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
