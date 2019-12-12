#pragma once

class CPlayingList : public CObjectList
{
private:
	enum class E_ItemLinkType
	{
		ILT_None = 0,
		ILT_SingerImg,
		ILT_SingerAlbum,
		ILT_TrackCount,
		ILT_PrevTrack,
		ILT_NextTrack
	};

	struct tagItemLink
	{
		tagItemLink(E_ItemLinkType t_eLinkType)
			: eLinkType(t_eLinkType)
		{
		}

		operator E_ItemLinkType() const
		{
			return eLinkType;
		}

		E_ItemLinkType eLinkType;

		CRect rcPos;
		bool bHittest = false;

		bool hittest(const CPoint& ptPos, bool *pbChanged = NULL)
		{
			if (bHittest != (TRUE == rcPos.PtInRect(ptPos)))
			{
				bHittest = !bHittest;

				if (NULL != pbChanged)
				{
					*pbChanged = true;
				}
			}

			return bHittest;
		}
	};

	struct tagItemLinks
	{
		tagItemLinks()
			: lnkSingerImg(E_ItemLinkType::ILT_SingerImg)
			, lnkSingerAlbum(E_ItemLinkType::ILT_SingerAlbum)
			, lnkTrackCount(E_ItemLinkType::ILT_TrackCount)
		{
		}

		tagItemLink lnkSingerImg;

		tagItemLink lnkSingerAlbum;

		tagItemLink lnkTrackCount;

		E_ItemLinkType hittest(const CPoint& ptPos, bool *pbChanged = NULL)
		{
			if (lnkSingerImg.hittest(ptPos, pbChanged))
			{
				return lnkSingerImg;
			}

			if (lnkSingerAlbum.hittest(ptPos, pbChanged))
			{
				return lnkSingerAlbum;
			}

			if (lnkTrackCount.hittest(ptPos, pbChanged))
			{
				return lnkTrackCount;
			}

			return E_ItemLinkType::ILT_None;
		}
	};

public:
	CPlayingList(__view& view)
		: m_view(view)
		, m_lnkPrevTrack(E_ItemLinkType::ILT_PrevTrack)
		, m_lnkNextTrack(E_ItemLinkType::ILT_NextTrack)
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
	
	tagItemLink m_lnkPrevTrack;
	tagItemLink m_lnkNextTrack;

	vector<tagItemLinks> m_vecItemLinks;
	
public:
	BOOL InitCtrl(UINT uItemHeight);

	void fixColumnWidth(int width);

	void refresh(int nPlayingItem);

	void onPlay(UINT uPlayingItem, bool bManual);
	
	void rename();
	
	void GetSelItems(TD_PlayItemList& arrSelPlayItem);

private:
	void OnCustomDraw(tagLVDrawSubItem& lvcd);
	
	void DrawItem(CPlayItem& PlayItem, CDC& dc, int cx, int cy, tagLVDrawSubItem& lvcd);
	
	bool GetRenameText(UINT uItem, wstring& strRenameText) override;

	void OnListItemRename(UINT uItem, const CString& cstrNewText) override;

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
	void handleLinkClick(UINT uItem, CPlayItem& PlayItem, tagItemLinks& ItemLinks, E_ItemLinkType eLinkType);
};
