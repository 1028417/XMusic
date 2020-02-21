#pragma once

#include "page/MediaResPanel.h"

#include "AlbumPage.h"

class CSingerMediaResPanel : public CMediaResPanel
{
public:
	CSingerMediaResPanel(__view& view, class CAlbumPage& wndAlbumPage)
		: CMediaResPanel(view, false)
		, m_wndAlbumPage(wndAlbumPage)
	{
	}
	
private:
	class CAlbumPage& m_wndAlbumPage;

private:
	void _OnInitDialog() override;

	void UpdateTitle(const wstring& strTitle) override;

public:
	void HittestMediaRes(CMediaRes& MediaRes) override;
};
