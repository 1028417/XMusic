#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

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

	list<CMediaDir> m_lstAttachDir;

private:
	void _OnInitDialog() override;

	void UpdateTitle(cwstr strTitle) override;

	void OnNMSetFocusList(NMHDR *pNMHDR, LRESULT *pResult);

	bool _onShowDir() override;

	void OnMenuCommand(UINT uID, UINT uVkKey) override;

public:
	void SetSinger(const CSinger& singer);

	void HittestMediaRes(CMediaRes& MediaRes) override;
};
