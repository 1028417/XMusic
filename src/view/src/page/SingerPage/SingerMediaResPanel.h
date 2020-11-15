#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

class CSingerAttachDir : public CMediaDir
{
public:
	CSingerAttachDir() = default;

	CSingerAttachDir(cwstr strPath, cwstr strName)
		: CMediaDir(strPath)
		, m_strPath(strPath)
		, m_strName(strName)
	{
	}

public:
	wstring m_strPath;
	wstring m_strName;

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
		return m_strName;
	}

	bool GetRenameText(wstring& strRenameText) const override
	{
		strRenameText = m_strName;
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

	UINT _onShowDir() override;

	void OnMenuCommand(UINT uID, UINT uVkKey) override;
	
public:
	void SetSinger(const CSinger& singer);

	void HittestMediaRes(CMediaRes& MediaRes) override;
};
