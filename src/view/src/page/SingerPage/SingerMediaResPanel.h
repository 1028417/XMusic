#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

class CSingerAttachDir : public CMediaDir
{
public:
	CSingerAttachDir() = default;

	CSingerAttachDir(cwstr strPath)//, cwstr strName=L"")
		: CMediaDir(__medialib.toAbsPath(strPath, true))
		, m_strPath(strPath)
		//, m_strName(strName)
	{
	}

public:
	wstring m_strPath;
	//wstring m_strName;

private:
	int getImage() override
	{
		return (int)E_GlobalImage::GI_AttachDir;
	}

	wstring GetPath() const override
	{
		return m_strPath;
	}

	wstring GetDisplayTitle() const override
	{
		cauto strDirName = fsutil::GetFileName(m_strPath);
		//if (m_strName.empty() || m_strName == strDirName)
		{
			cauto strParentDir = fsutil::GetParentDir(m_strPath);
			if (strParentDir.empty())
			{
				return m_strPath;
			}

			return strDirName + L" | " + strParentDir;
		}

		//return m_strName + L" | " + m_strPath;
	}

	bool GetRenameText(wstring& strRenameText) const override
	{
		/*if (!m_strName.empty())
		{
			strRenameText = m_strName;
		}
		else*/
		{
			strRenameText = fileName();
		}

		return true;
	}
};

class CSingerMediaResPanel : public CMediaResPanel
{
	DECLARE_MESSAGE_MAP()

public:
	CSingerMediaResPanel(__view& view, class CAlbumPage& wndAlbumPage)
		: CMediaResPanel(view, true)
		, m_wndAlbumPage(wndAlbumPage)
	{
	}
	
private:
	class CAlbumPage& m_wndAlbumPage;

	list<CSingerAttachDir> m_lstAttachDir;

private:
	void _OnInitDialog() override;

	void UpdateTitle(cwstr strTitle) override;

	void OnNMSetFocusList(NMHDR *pNMHDR, LRESULT *pResult);

	void _onShowDir(TD_MediaResList& paMediaRes) override;

	void OnMenuCommand(UINT uID, UINT uVkKey) override;

	bool OnListItemRename(UINT uItem, CListObject *pObject, cwstr strNewName) override;

public:
	void SetSinger(const CSinger& singer);

	void HittestMediaRes(CMediaRes& MediaRes) override;
};
