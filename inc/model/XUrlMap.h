
#pragma once

struct tagXUrl
{
    tagXUrl(const string& strUtf8Title, const string& strUrl, const wstring& strFileTitle)
        : m_strUtf8Title(strUtf8Title)
        , m_strUrl(strUrl)
        , m_strFileTitle(strFileTitle)
    {
    }

    string m_strUtf8Title;
    string m_strUrl;
    wstring m_strFileTitle;
    bool bUsed = false;

    string genUrl();
};

class XUrlMap
{
public:
    XUrlMap(){}

private:
    unordered_map<wstring, tagXUrl> m_mapXUrl;

public:
    void add(const string& strFileTitle, const string& strUrl);

    bool check(const wstring& strFileTitle);

    void checkRedundant();

    string get(const wstring& strFileTitle);
};
