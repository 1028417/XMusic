
#pragma once

class __UtilExt winfsutil
{
public:
	static void getSysDrivers(list<wstring>& lstDrivers);
	
	static bool removeDir(cwstr strPath, HWND hwndParent, cwstr strTitle=L"");

	static void exploreDir(cwstr strDir, bool bAsRoot=false);
	static void exploreFile(cwstr strFile);

	// 获取文件夹图标
	static HICON getFolderIcon();
	// 获取文件图标
	static HICON getFileIcon(cwstr extention);
};
