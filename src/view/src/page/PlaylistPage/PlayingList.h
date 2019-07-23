#pragma once

struct tagLink
{
	CRect rcPos;
	bool bHittest = false;
};

struct tagSingerAlbumLink : tagLink
{
	CRect rcSingerImg;
};

class CPlayingList : public CObjectList
{
public:
	CPlayingList(__view& view)
		: m_view(view)
	{
	}

private:
	__view& m_view;
	
	CCompatableFont m_font;
	CCompatableFont m_fontPlaying;
	CCompatableFont m_fontPlayed;

	CCompatableFont m_fontSmall;
	CCompatableFont m_fontUnderline;

	int m_nPlayingItem = -1;
	int m_nMouseMoveItem = -1;
	int m_nRenameItem = -1;

	bool m_bMouseDown = false;

	bool m_bHighlightTracks = false;
	bool m_bHighlightPrevLink = false;
	bool m_bHighlightNextLink = false;

	tagLink m_TracksLink;
	tagLink m_PrevLink;
	tagLink m_NextLink;

	vector<tagSingerAlbumLink> m_vecSingerAlbumLink;
	
public:
	BOOL InitCtrl(UINT uItemHeight);

	void fixColumnWidth(int width);

	void refresh(int nPlayingItem);

	void onPlay(UINT uPlayingItem, bool bManual);
	
	void rename();
	
	void GetSelItems(TD_PlayItemList& arrSelPlayItem);

private:
	virtual BOOL handleNMNotify(NMHDR& NMHDR, LRESULT* pResult) override;

	virtual void OnTrackMouseEvent(E_TrackMouseEvent eMouseEvent, const CPoint& point) override;
	
	void OnCustomDraw(tagLvCustomDraw& lvcd) override;

	void DrawItem(CDC& dc, CRect& rcItem, int iItem, CPlayItem& PlayItem);
	
	bool GetRenameText(UINT uItem, wstring& strRenameText) override;

	void OnListItemRename(UINT uItem, const CString& cstrNewText) override;

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	bool HittestTrackLink(UINT uItem, const CPoint& ptPos);

	bool HittestSingerAlbumLink(tagLink& SingerAlbumLink, const CPoint& ptPos);
	
	void OnTrackLinkClick();

	bool handleItemLButtonDown(UINT uItem, CPlayItem& PlayItem, const CPoint& ptPos);
};
