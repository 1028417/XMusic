
#pragma once

class XUrlMap
{
public:
    XUrlMap(){}

private:
    unordered_map<string, pair<string, string>> m_mapXUrl;

public:
    void add(const string& strFileTitle, const string& strUrl);

    bool check(const string& strFileTitle);

    void checkRedundant();

    string get(const string& strFileTitle);
};
