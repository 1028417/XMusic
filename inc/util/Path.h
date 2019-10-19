
#pragma once

#include "fsutil.h"

//CPath
class XFile;
using TD_XFileList = PtrArray<XFile>;
class CPath;
using TD_PathList = PtrArray<CPath>;

class CListObject;
using TD_ListObjectList = PtrArray<CListObject>;

class CTreeObject;
using TD_TreeObjectList = PtrArray<CTreeObject>;

class CPathObject;
using TD_PathObjectList = PtrArray<CPathObject>;

class CDirObject;
using TD_DirObjectList = PtrArray<CDirObject>;

class __UtilExt XFile
{
public:
	XFile() {}

	XFile(const wstring& strName)
		: m_fileInfo(strName)
	{
	}

	XFile(const tagFileInfo& fileInfo)
		: m_fileInfo(fileInfo)
	{
	}

	virtual ~XFile() {}

private:
	tagFileInfo m_fileInfo;

protected:
	const tagFileInfo& fileinfo = m_fileInfo;

public:
	tagFileInfo& fileInfo()
	{
		return m_fileInfo;
	}

	const tagFileInfo& fileInfo() const
	{
		return m_fileInfo;
	}

	void SetName(const wstring& strNewName)
	{
		m_fileInfo.strName = strNewName;
	}

	wstring GetName() const;

	wstring absPath() const;

	wstring oppPath() const;

	void Remove();
};

class __UtilExt CPath : public XFile
{
	friend struct tagPathSortor;

public:
	CPath(const wstring& strDir = L"")
		: XFile(wsutil::rtrim_r(strDir, __wcFSSlant))
	{
	}

    CPath(const tagFileInfo& fileInfo)
        : XFile(fileInfo)
	{
	}
	
	virtual ~CPath()
	{
		Clear();
	}

private:
	enum class E_FindFileStatus
	{
		FFS_None
		, FFS_Exists
		, FFS_NotExists
	};
	E_FindFileStatus m_eFindFileStatus = E_FindFileStatus::FFS_None;

	TD_PathList m_paSubDir;
	TD_XFileList m_paSubFile;

private:
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo)
	{
		return new CPath(fileInfo);
	}

    virtual XFile* _newSubFile(const tagFileInfo& fileInfo)
	{
		return new XFile(fileInfo);
	}

	using CB_PathScan = function<bool(CPath& dir, TD_XFileList& paSubFile)>;
	bool _scan(const CB_PathScan& cb);

protected:
	void _findFile();

	virtual void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile);

	virtual int _sort(const XFile& lhs, const XFile& rhs) const;

	size_t count() const
	{
		return m_paSubDir.size() + m_paSubFile.size();
	}

public:
	void SetDir(const wstring& strDir)
	{
		Clear();

		XFile::SetName(wsutil::rtrim_r(strDir, __wcFSSlant));
	}

	void scan(const CB_PathScan& cb);

	const TD_PathList& dirs()
	{
		_findFile();
		return m_paSubDir;
	}

	const TD_XFileList& files()
	{
		_findFile();
		return m_paSubFile;
	}

	void get(const function<void(XFile&)>& cb)
	{
		_findFile();

		m_paSubDir(cb);
		m_paSubFile(cb);
	}

    void get(UINT uIdx, const function<void(XFile&)>& cb)
	{
		_findFile();

		if (uIdx < m_paSubDir.size())
		{
			m_paSubDir.get(uIdx, cb);
		}
		else
		{
			m_paSubFile.get(uIdx - m_paSubDir.size(), cb);
		}
	}

	XFile *FindSubPath(wstring strSubPath, bool bDir);

	void RemoveSubObject(XFile *subPath);
	
	virtual void Clear();
};

class __UtilExt CListObject
{
public:
    virtual void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage)
	{
        (void)bReportView;
        (void)vecText;
        (void)iImage;
    }

	virtual bool GetRenameText(wstring& strRenameText) const
	{
        (void)strRenameText;
		return true;
	}

	virtual void OnListItemRename(const wstring& strNewName)
	{
        (void)strNewName;
	}
};

class __UtilExt CTreeObject
{
public:
	CTreeObject() {}

public:
	virtual wstring GetTreeText() const	{ return L""; }

	virtual int GetTreeImage() { return 0; }

	virtual bool hasCheckState() { return true; }

	virtual void GetTreeChilds(TD_TreeObjectList&) {}
};

class __UtilExt CPathObject : public CPath, public CListObject
{
public:
	CPathObject(const wstring& strDir=L"")
		: CPath(strDir)
	{
	}

    CPathObject(const tagFileInfo& fileInfo)
        : CPath(fileInfo)
	{
	}

	virtual ~CPathObject()
	{
	}

protected:
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override
	{
		return new CPathObject(fileInfo);
	}

	virtual XFile* _newSubFile(const tagFileInfo& fileInfo) override
	{
		return new CPathObject(fileInfo);
	}
};

class __UtilExt CDirObject : public CPathObject, public CTreeObject
{
public:
	CDirObject(const wstring& strDir = L"")
		: CPathObject(strDir)
	{
	}

    CDirObject(const tagFileInfo& fileInfo)
        : CPathObject(fileInfo)
	{
	}

	virtual ~CDirObject()
	{
	}

protected:
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override
	{
		return new CDirObject(fileInfo);
	}

        virtual XFile* _newSubFile(const tagFileInfo&) override
	{
		return NULL;
	}

public:
	wstring GetTreeText() const override
	{
		return CPath::GetName();
	}

    void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_DirObjectList(this->dirs()));
	}
};
