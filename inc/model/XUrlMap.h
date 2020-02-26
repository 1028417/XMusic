
#pragma once

struct tagXUrl
{
    tagXUrl(const string& t_strFileTitle, const string& t_strUrl)
        : strFileTitle(t_strFileTitle)
        , strUrl(t_strUrl)
    {
    }

    string strFileTitle;
    string strUrl;
    bool bUsed = false;
};

class XUrlMap
{
public:
    XUrlMap(){}

private:
    unordered_map<string, tagXUrl> m_mapXUrl;

public:
    void add(const string& strFileTitle, const string& strUrl);

    bool check(const string& strFileTitle);

    void checkRedundant();

    string get(const string& strFileTitle);
};
