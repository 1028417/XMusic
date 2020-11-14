#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

class CSingerAttachDir : public CMediaDir
{
public:
	CSingerAttachDir() = default;

	CSingerAttachDir(cwstr strAliasName, cwstr strPath)
		: CMediaDir(strPath)
		, m_strAliasName(strAliasName)
	{
	}

private:
	wstring m_strAliasName;

private:
	int getImage() override
	{
		return (int)E_GlobalImage::GI_AttachDir;
	}

	wstring GetPath() const override
	{
		return m_fi.strName;
	}

	wstring GetDisplayTitle() const override
	{
		return m_strAliasName;
	}

	bool GetRenameText(wstring& strRenameText) const override
	{
		strRenameText = m_strAliasName;
		return true;
	}
};

class CSingerMediaResPanel : public CMediaResPanel
{
	DECLARE_MESSAGE_MAP()

public:
	CSingerMediaResPanel(__view& view, class CAlbumPage& wndAlbumPage)
		: CMediaResPanel(view, false)
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

	UINT _onShowDir() override;

	void OnMenuCommand(UINT uID, UINT uVkKey) override;

public:
	void SetSinger(const CSinger& singer);

	void HittestMediaRes(CMediaRes& MediaRes) override;
};
