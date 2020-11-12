#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

class CSingerAttachDir : public CMediaDir
{
public:
	virtual ~CSingerAttachDir() = default;

	CSingerAttachDir() = default;

	CSingerAttachDir(cwstr strPath)
		: CMediaDir(strPath)
	{
	}

private:
	void genMediaResListItem(E_ListViewType eViewType, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger) override
	{
		CMediaDir::genMediaResListItem(eViewType, vecText, iImage, bGenRelatedSinger);

		iImage = (int)E_GlobalImage::GI_AttachDir;
	}

	wstring GetPath() const override
	{
		return m_fi.strName;
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
