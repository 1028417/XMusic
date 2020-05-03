#pragma once

using CB_XmlGetChild = cfn_bool_t<struct tagXmlElementInfo&>;
using CB_XmlGetChildAttr = const function<bool(struct tagXmlElementInfo&, const string&)>&;

struct __UtilExt tagXmlElementInfo
{
    string strName;

    map<string, string> mapAttr;

    string strText;

    vector<tagXmlElementInfo> vecChildInfo;

    void getChild(const list<string>& lstChildName, CB_XmlGetChild cb);
    void getChild(const list<string>& lstChildName, const string& strAttrName
                  , const string& strAttrValue, CB_XmlGetChild cb);

    void getChildAttr(const list<string>& lstChildName, const string& strAttrName, CB_XmlGetChildAttr cb);
    void getChildAttr(const list<string>& lstChildName, const string& strAttrName, list<string>& lstAttrValue);
};

class __UtilExt xmlutil
{
public:
	static bool loadXml(char* buf, size_t length, bool bUtf8, bool bHtml, tagXmlElementInfo& rootElementInfo);

	static bool loadXml(Instream& ins, bool bHtml, tagXmlElementInfo& rootElementInfo);
	static bool loadXml(cwstr strFile, bool bHtml, tagXmlElementInfo& rootElementInfo)
	{
		IFStream ifs(strFile);
		__EnsureReturn(ifs, false);
		return loadXml(ifs, bHtml, rootElementInfo);
	}
};
