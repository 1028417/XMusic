
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

class __UtilExt CPath : protected tagFileInfo
{
	friend struct tagPathSortor;

public:
	CPath()
	{
	}

	CPath(const wstring& strName, bool bDir);

	CPath(const tagFileInfo& FileInfo, CPath& ParentDir)
		: tagFileInfo(FileInfo)
		, m_pParentDir(&ParentDir)
	{
	}
	
	virtual ~CPath()
	{
		Clear();
	}

protected:	
	CPath *m_pParentDir = NULL;

    bool m_bDirExists = false;

    TD_PathList m_lstSubPath;

	bool m_bFinded = false;

protected:
	virtual void _onFindFile();

	virtual void _sortSubPath();

    virtual int _sortCompare(const CPath& lhs, const CPath& rhs) const
    {
        return lhs._sortCompare(rhs);
    }

    virtual int _sortCompare(const CPath& rhs) const;

private:
    void _findFile();

	virtual CPath* NewSubPath(const tagFileInfo& FileInfo, CPath& ParentDir)
	{
		return new CPath(FileInfo, ParentDir);
	}
	
	void _GetSubPath(TD_PathList *plstSubDir, TD_PathList *plstSubFile = NULL);

public:
    void SetDir(const wstring& strDir, bool bFindFile=false);

	void SetName(const wstring& strNewName)
	{
        m_strName = strNewName;
	}

	wstring GetName() const;

	inline bool IsDir() const
	{
		return m_bDir;
	}

	bool DirExists() const
	{
		return m_bDirExists;
	}

	wstring GetPath() const;

	wstring GetParentDir() const;
	
	const TD_PathList& GetSubPath();

	void GetSubPath(TD_PathList& lstSubDir, TD_PathList& lstSubFile)
	{
		_GetSubPath(&lstSubDir, &lstSubFile);
	}

	void GetSubDir(TD_PathList& lstSubDir)
	{
		_GetSubPath(&lstSubDir);
	}

	void GetSubFile(TD_PathList& lstSubFile)
	{
		_GetSubPath(NULL, &lstSubFile);
	}

	CPath *GetSubPath(UINT uIdx) const;

	CPath *FindSubPath(wstring strSubPath, bool bDir);

	bool enumSubFile(const function<bool(CPath& dir, TD_PathList& lstSubFile)>& cb);

	void RemoveSubPath(CPath *pSubPath);

	void RemoveSelf();

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
	CPathObject()
	{
	}

	CPathObject(const wstring& strName, bool bDir)
		: CPath(strName, bDir)
	{
	}

	CPathObject(const tagFileInfo& FileInfo, CPath& ParentDir)
		: CPath(FileInfo, ParentDir)
	{
	}

	virtual ~CPathObject()
	{
	}

protected:
	virtual CPath* NewSubPath(const tagFileInfo& FileInfo, CPath& ParentDir) override
	{
		return new CPathObject(FileInfo, ParentDir);
	}
};

class __UtilExt CDirObject : public CPathObject, public CTreeObject
{
public:
	CDirObject(const wstring& strDir = L"")
		: CPathObject(strDir, true)
	{
	}

	CDirObject(const tagFileInfo& FileInfo, CPath& ParentDir)
		: CPathObject(FileInfo, ParentDir)
	{
	}

	virtual ~CDirObject()
	{
	}

protected:
	virtual CPath* NewSubPath(const tagFileInfo& FileInfo, CPath& ParentDir) override
	{
		if (FileInfo.m_bDir)
		{
			return new CDirObject(FileInfo, ParentDir);
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
		lstChilds.add(TD_DirObjectList(this->GetSubPath()));
	}
};
