#pragma once

using CB_XmlGetChild = const function<bool(struct tagXmlElementInfo&)>&;
using CB_XmlGetChildAttr = const function<bool(struct tagXmlElementInfo&, const string&)>&;

struct __UtilExt tagXmlElementInfo
{
    string strName;

    map<string, string> mapAttr;

    string strText;

    vector<tagXmlElementInfo> vecChildInfo;

    void getChild(const list<string>& lstChildName, CB_XmlGetChild cb);
    void getChildAttr(const list<string>& lstChildName, const string& strAttrName, CB_XmlGetChildAttr cb);
};

class __UtilExt xmlutil
{
public:
        static bool loadXml(const string& strFile, tagXmlElementInfo& rootElementInfo, bool bUtf8 = false);

        static bool loadHtml(const string& strFile, tagXmlElementInfo& rootElementInfo, bool bUtf8 = false);
};
