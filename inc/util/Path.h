
#pragma once

#include "fsutil.h"

//CPath
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

class __UtilExt CPath
{
	friend struct tagPathSortor;

public:
	CPath(const wstring& strDir=L"");

	CPath(const tagFileInfo& FileInfo)
        : m_fi(FileInfo)
	{
	}
	
	virtual ~CPath()
	{
		Clear();
	}

private:
    tagFileInfo m_fi;

	enum class E_FindFileStatus
	{
		FFS_None
		, FFS_Exists
		, FFS_NotExists
	};
	E_FindFileStatus m_eFindFileStatus = E_FindFileStatus::FFS_None;

	TD_PathList m_paSubDir;
	TD_PathList m_paSubFile;

protected:
    const tagFileInfo& m_FileInfo = m_fi;

private:
	virtual CPath* _newSubPath(const tagFileInfo& FileInfo)
	{
		return new CPath(FileInfo);
	}

	void _sort(TD_PathList& paSubPath);

	using CB_PathScan = function<bool(CPath& dir, TD_PathList& paSubFile)>;
	bool _scan(const CB_PathScan& cb);

protected:
	void _findFile();

	virtual void _onFindFile(TD_PathList& paSubDir, TD_PathList& paSubFile);

	virtual int _sort(const CPath& lhs, const CPath& rhs) const;

	size_t count() const
	{
		return m_paSubDir.size() + m_paSubFile.size();
	}

public:
	const tagFileInfo& fileInfo() const
    {
        return m_FileInfo;
    }

    void SetDir(const wstring& strDir);

	void SetName(const wstring& strNewName)
	{
        m_fi.strName = strNewName;
	}

	wstring GetName() const;

	wstring absPath() const;

    wstring oppPath() const;

	void scan(const CB_PathScan& cb);

	const TD_PathList& dirs()
	{
		_findFile();
		return m_paSubDir;
	}

	const TD_PathList& files()
	{
		_findFile();
		return m_paSubFile;
	}

	void subPath(const function<void(CPath&)>& cb)
	{
		_findFile();

		m_paSubDir(cb);
		m_paSubFile(cb);
	}

	/*void CPath::subPath(UINT uIdx, const function<void(CPath&)>& cb)
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
	}*/

	CPath *FindSubPath(wstring strSubPath, bool bDir);

	void RemoveSelf();

	void Remove(CPath *pSubPath);

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

	CPathObject(const tagFileInfo& FileInfo)
		: CPath(FileInfo)
	{
	}

	virtual ~CPathObject()
	{
	}

protected:
	virtual CPath* _newSubPath(const tagFileInfo& FileInfo) override
	{
		return new CPathObject(FileInfo);
	}
};

class __UtilExt CDirObject : public CPathObject, public CTreeObject
{
public:
	CDirObject(const wstring& strDir = L"")
		: CPathObject(strDir)
	{
	}

	CDirObject(const tagFileInfo& FileInfo)
		: CPathObject(FileInfo)
	{
	}

	virtual ~CDirObject()
	{
	}

protected:
	virtual CPath* _newSubPath(const tagFileInfo& FileInfo) override
	{
		if (FileInfo.bDir)
		{
			return new CDirObject(FileInfo);
		}

		return NULL;
	}

public:
	wstring GetTreeText() const override
	{
		return CPath::GetName();
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds)
	{
		lstChilds.add(TD_DirObjectList(this->dirs()));
	}
};
