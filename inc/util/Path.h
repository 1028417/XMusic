
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
	virtual ~XFile() {}

	XFile() {}

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
	const tagFileInfo& fileInfo() const
	{
		return m_fileInfo;
	}

    void setName(const wstring& strNewName)
	{
		m_fileInfo.strName = strNewName;
	}

	wstring fileName() const;

	wstring parentDir() const;

	wstring absPath() const;

	wstring oppPath() const;

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

	CPath() {}

    CPath(const tagFileInfo& fileInfo)
        : XFile(fileInfo)
	{
	}
	
	CPath(bool t_bDir, const wstring& t_strName, class CPath *t_pParent)
		: XFile(t_bDir, t_bDir? strutil::rtrim_r(t_strName, __wchPathSeparator) : t_strName, t_pParent)
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

    virtual void _onClear() {}

protected:
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

        XFile::setName(strutil::rtrim_r(strDir, __wchPathSeparator));
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
    XFile* findSubFile(wstring strSubFile)
    {
        return findSubPath(strSubFile, false);
    }
    CPath* findSubDir(wstring strSubDir)
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
	virtual ~CPathObject() {}
	
	CPathObject() {}

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

class __UtilExt CDirObject : public CPathObject, public CTreeObject
{
public:
	virtual ~CDirObject() {}

	CDirObject() {}

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

public:
	wstring GetTreeText() const override
	{
        return XFile::fileName();
	}

    void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_DirObjectList(this->dirs()));
	}
};
