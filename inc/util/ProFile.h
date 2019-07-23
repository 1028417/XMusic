
#pragma once

class __UtilExt CProFile
{
public:
	CProFile(const string& strIniPath);

private:
	string m_strIniPath;

public:
	BOOL ReadString(const string& strSection, const string& strKey, string& strValue);
	BOOL ReadInt(const string& strSection, const string& strKey, int& nValue);
	double ReadDouble(const string& strSection, const string& strKey, double& dblValue);

	BOOL WriteString(const string& strSection, const string& strKey, const string& strValue);
	BOOL WriteInt(const string& strSection, const string& strKey, const int nValue);
	BOOL WriteDouble(const string& strSection, const string& strKey, const double dblValue);
};
