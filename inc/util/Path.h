
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
    virtual ~XFile() = default;

    XFile() = default;

	XFile(const tagFileInfo& fileInfo)
		: m_fileInfo(fileInfo)
	{
	}

	XFile(bool t_bDir, const wstring& t_strName, class CPath *t_pParent)
		: m_fileInfo(t_bDir, t_strName, t_pParent)
	{
	}

private:
	tagFileInfo m_fileInfo;

protected:
	const tagFileInfo& m_fi = m_fileInfo;

public:
	CPath* parent() const
	{
		return m_fileInfo.pParent;
	}

	bool isDir() const
	{
		return m_fileInfo.bDir;
	}

    void setName(const wstring& strNewName)
	{
		m_fileInfo.strName = strNewName;
	}

	wstring fileName() const;

	wstring parentDir() const;

	wstring path() const;
	
	const CPath* rootDir() const;

    void remove();
};

class __UtilExt CPath : public XFile
{
	friend struct tagPathSortor;

public:
	virtual ~CPath()
	{
        clear();
	}

    CPath() = default;

    CPath(const tagFileInfo& fileInfo)
        : XFile(fileInfo)
	{
	}
	
	CPath(bool t_bDir, const wstring& t_strName, class CPath *t_pParent)
		: XFile(t_bDir, t_bDir? strutil::rtrim_r(t_strName, __wcPathSeparator) : t_strName, t_pParent)
	{
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
        virtual void _onClear() {}

	void _findFile();

	virtual void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile);

	virtual int _sort(const XFile& lhs, const XFile& rhs) const;

	size_t count() const
	{
		return m_paSubDir.size() + m_paSubFile.size();
	}

public:
    void setDir(const wstring& strDir)
	{
        clear();

        XFile::setName(strutil::rtrim_r(strDir, __wcPathSeparator));
	}

	void assign(const TD_PathList& paSubDir, const TD_XFileList& paSubFile)
	{
		m_paSubDir.assign(paSubDir);
		m_paSubFile.assign(paSubFile);

		m_eFindFileStatus = E_FindFileStatus::FFS_Exists;
	}
	void swap(TD_PathList& paSubDir, TD_XFileList& paSubFile)
	{
		m_paSubDir.swap(paSubDir);
		m_paSubFile.swap(paSubFile);

		m_eFindFileStatus = E_FindFileStatus::FFS_Exists;
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

	void get(cfn_void_t<XFile&> cb)
	{
		_findFile();

		m_paSubDir(cb);
		m_paSubFile(cb);
	}

    void get(size_t uIdx, cfn_void_t<XFile&> cb)
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

    XFile* findSubPath(wstring strSubPath, bool bDir);
    XFile* findSubFile(const wstring& strSubFile)
    {
        return findSubPath(strSubFile, false);
    }
    CPath* findSubDir(const wstring& strSubDir)
    {
        return (CPath*)findSubPath(strSubDir, true);
    }

    void remove()
    {
        return XFile::remove();
    }

    void remove(XFile *subPath);

	void clear();
};

#if __winvc
#define LV_VIEW_ICON            0x0000
#define LV_VIEW_DETAILS         0x0001
#define LV_VIEW_SMALLICON       0x0002
#define LV_VIEW_LIST            0x0003
#define LV_VIEW_TILE            0x0004

enum class E_ListViewType
{
	LVT_Icon = LV_VIEW_ICON,
	LVT_Report = LV_VIEW_DETAILS,
	LVT_SmallIcon = LV_VIEW_SMALLICON,
	LVT_List = LV_VIEW_LIST,
	LVT_Tile = LV_VIEW_TILE,
};

using TD_ListImgIdx = int;

class __UtilExt CListObject
{
public:
    virtual void GenListItem(E_ListViewType, vector<wstring>& vecText, TD_ListImgIdx&)
	{
        (void)vecText;
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
    CTreeObject() = default;

public:
	virtual wstring GetTreeText() const	{ return L""; }

	virtual int GetTreeImage() { return 0; }

	virtual bool hasCheckState() { return true; }

	virtual void GetTreeChilds(TD_TreeObjectList&) {}
};
#endif

class __UtilExt CPathObject : public CPath
#if __winvc
	, public CListObject
#endif
{
public:
    virtual ~CPathObject() = default;
	
    CPathObject() = default;

    CPathObject(const tagFileInfo& fileInfo)
        : CPath(fileInfo)
	{
	}

	CPathObject(bool t_bDir, const wstring& t_strName, class CPath *t_pParent)
		: CPath(t_bDir, t_strName, t_pParent)
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

class __UtilExt CDirObject : public CPathObject
#if __winvc
	, public CTreeObject
#endif
{
public:
    virtual ~CDirObject() = default;

    CDirObject() = default;

    CDirObject(const tagFileInfo& fileInfo)
        : CPathObject(fileInfo)
	{
	}

	CDirObject(const wstring& t_strName, class CPath *t_pParent = NULL)
		: CPathObject(true, t_strName, t_pParent)
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

#if __winvc
public:
	wstring GetTreeText() const override
	{
        return XFile::fileName();
	}

    void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_DirObjectList(this->dirs()));
	}
#endif
};
