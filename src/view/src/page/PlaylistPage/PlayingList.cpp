
#include "StdAfx.h"

#include "PlayingList.h"

#include "../../dlg/TrackDetailDlg.h"

#define BkgColor_Select RGB(204, 232, 255)
#define BkgColor_Hit RGB(229, 243, 255)

#define __PrevTrack L"上一曲"
#define __NextTrack L"下一曲"

BOOL CPlayingList::InitCtrl(UINT uItemHeight)
{
	(void)this->ModifyStyle(LVS_SINGLESEL
		, LVS_NOCOLUMNHEADER | LVS_EDITLABELS);
	(void)this->ModifyStyleEx(0, WS_EX_LEFTSCROLLBAR);

	tagListPara ListPara({ { _T(""), 0 } });
	ListPara.uItemHeight = uItemHeight;
	__super::InitCtrl(ListPara);
		
	__super::SetCusomDrawNotify();

	__super::SetTrackMouse();

	auto fBigFontSize = m_view.m_globalSize.m_fBigFontSize * .95f;
	__AssertReturn(m_font.create(*this, fBigFontSize), FALSE);

	__AssertReturn(m_fontPlaying.create(*this, fBigFontSize, 650), FALSE);

	__AssertReturn(m_fontPlayed.create(*this, fBigFontSize, 0, true), FALSE);

	float fSmallFontSize = m_view.m_globalSize.m_fSmallFontSize * .75f;
	__AssertReturn(m_fontSmall.create(*this, fSmallFontSize), FALSE);

	__AssertReturn(m_fontUnderline.create(*this, fSmallFontSize, 0, false, true), FALSE);

	this->SetFont(&m_font);

	return TRUE;
}

void CPlayingList::fixColumnWidth(int width)
{
	if (GetItemCount() > GetCountPerPage())
	{
		width -= m_view.m_globalSize.m_uScrollbarWidth;
	}

	(void)SetColumnWidth(0, width);
}

void CPlayingList::OnTrackMouseEvent(E_TrackMouseEvent eMouseEvent, const CPoint& point)
{
	__super::OnTrackMouseEvent(eMouseEvent, point);

	if (E_TrackMouseEvent::LME_MouseMove == eMouseEvent)
	{
		int iPreItem = m_nMouseMoveItem;
		
		m_nMouseMoveItem = this->HitTest(point);
		if (m_nMouseMoveItem != iPreItem)
		{
			if (0 <= m_nMouseMoveItem)
			{
				this->Update(m_nMouseMoveItem);
			}

			if (0 <= iPreItem)
			{
				this->Update(iPreItem);
			}
		}
	}
	else if (E_TrackMouseEvent::LME_MouseLeave == eMouseEvent)
	{
		int iPreItem = m_nMouseMoveItem;
		if (-1 != iPreItem)
		{
			m_nMouseMoveItem = -1;
			this->Update(iPreItem);
		}
	}
}

void CPlayingList::OnCustomDraw(tagLvCustomDraw& lvcd)
{
	CRect rcItem(lvcd.rcItem);
	if (0 == rcItem.right || 0 == rcItem.bottom)
	{
		return;
	}

	rcItem.left = 0;

	m_view.getPlayMgr().getPlayingItems().get(lvcd.uItem, [&](CPlayItem& PlayItem) {
		HDC hDC = lvcd.hDC;
		CCompDC CompDC;
		(void)CompDC.create(rcItem.Width(), rcItem.Height(), hDC);
		CDC& MemDC = CompDC.getDC();

		DrawItem(MemDC, CRect(0,0, rcItem.Width(), rcItem.Height()), lvcd.uItem, PlayItem);

		(void)::BitBlt(hDC, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), MemDC, 0, 0, SRCCOPY);

		lvcd.bSkipDefault = true;
	});
}

void CPlayingList::DrawItem(CDC& dc, CRect& rcItem, int iItem, CPlayItem& PlayItem)
{
	CFont *pFontPrev = dc.GetCurrentFont();

	int iItemHeight = rcItem.Height();
	int iYMiddlePos = rcItem.top + iItemHeight / 2 + 1;

	bool bPlayingItem = (iItem == m_nPlayingItem);
	bool bMouseMoveItem = (iItem == m_nMouseMoveItem);

	CFont *pFont = &m_font;
	COLORREF crTextColor = __Color_Text;
	COLORREF crBkColor = GetSysColor(COLOR_WINDOW);
	if (bPlayingItem)
	{
		pFont = &m_fontPlaying;
		crTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		crBkColor = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		if (m_view.getPlayMgr().checkPlayedID(PlayItem.m_uID))
		{
			pFont = &m_fontPlayed;
			crTextColor = RGB(85, 26, 139);
		}

		if (bMouseMoveItem)
		{
			crBkColor = BkgColor_Hit;
		}

		if (this->GetItemState(iItem, LVIS_SELECTED))
		{
			crBkColor = BkgColor_Select;
		}
	}

	(void)dc.SetTextColor(crTextColor);
	dc.FillSolidRect(rcItem, crBkColor);

	rcItem.right -= 8;

	if (iItem == m_nRenameItem)
	{
		auto *pEdit = GetEditControl();
		if (NULL != pEdit && pEdit->IsWindowVisible())
		{
			return;
		}
	}

	wstring strSingerAlbum;

	int iImage = -1;
	CRCueFile cueFile = PlayItem.getCueFile();
	if (cueFile)
	{
		iImage = (int)E_GlobalImage::GI_WholeTrack;
	}
	else
	{
		auto& strAlbumName = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Album);

		iImage = PlayItem.getSingerImg();
		if (iImage < 0)
		{
			wstring strSingerName = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
			if (!strSingerName.empty())
			{
				if (!strAlbumName.empty())
				{
					strSingerAlbum = strSingerName + __CNDot + strAlbumName;
				}
				else
				{
					strSingerAlbum = strSingerName;
				}
			}
		}
		else
		{
			if (!strAlbumName.empty())
			{
				strSingerAlbum = strAlbumName;
			}
		}
	}
	
	auto& SingerAlbumLink = m_vecSingerAlbumLink[iItem];
	memset(&SingerAlbumLink, 0, sizeof SingerAlbumLink);
	auto& rcSingerAlbum = SingerAlbumLink.rcPos;
	
	int iLeft = rcItem.left;
	if (iImage >= 0)
	{
#define __Margin 8
		CRect& rcSingerImg = SingerAlbumLink.rcSingerImg;
		rcSingerImg = CRect(CPoint(rcItem.left + __Margin, rcItem.top + __Margin)
			, CSize(iItemHeight - __Margin * 2, iItemHeight - __Margin * 2));

		m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Big).DrawEx(&dc, iImage
			, rcSingerImg.TopLeft(), rcSingerImg.Size(), CLR_DEFAULT, CLR_DEFAULT, ILD_SCALE);

		iLeft += iItemHeight;
	}
	else
	{
		iLeft += 8;
	}

	int iXPosDuration = rcItem.right;
	auto& strDuration = PlayItem.GetDurationString();
	if (!strDuration.empty())
	{
		(void)dc.SelectObject(m_fontSmall);

		iXPosDuration -= 43 + int(strDuration.size() - 4) * 12;
		RECT rcPos = { iXPosDuration, iYMiddlePos, rcItem.right, rcItem.bottom };
		dc.DrawText(strDuration.c_str(), &rcPos, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	auto& rcTracksLink = m_TracksLink.rcPos;
	auto& rcNextLink = m_NextLink.rcPos;
	auto& rcPrevLink = m_PrevLink.rcPos;
	if (bPlayingItem)
	{
		rcTracksLink = rcPrevLink = rcNextLink = { 0, 0, 0, 0 };
	}

	bool bFlag = false;
	if (cueFile)
	{
		bFlag = true;

		if (!m_bMouseDown && m_bHighlightTracks)
		{
			(void)dc.SelectObject(m_fontUnderline);
		}
		else
		{
			(void)dc.SelectObject(m_fontSmall);
		}

		wstring strTracks = to_wstring(cueFile.m_alTrackInfo.size()) + L"首";
		CSize szLink = dc.GetTextExtent(strTracks.c_str());

		int iTop = iYMiddlePos + (iItemHeight / 2 - szLink.cy) / 2 - 2;
		int iBottom = iTop + szLink.cy;
		rcTracksLink = { iLeft, iTop, iLeft + szLink.cx, iBottom };

		dc.DrawText(strTracks.c_str(), &rcTracksLink, 0);

		if (bPlayingItem)
		{
			if (!m_bMouseDown && m_bHighlightPrevLink)
			{
				(void)dc.SelectObject(m_fontUnderline);
			}
			else
			{
				(void)dc.SelectObject(m_fontSmall);
			}

			szLink = dc.GetTextExtent(__PrevTrack);
			int iXMiddlePos = (iLeft + szLink.cx * 2 / 3 + iXPosDuration) / 2;
			rcPrevLink = { iXMiddlePos - 5 - szLink.cx, iTop, iXMiddlePos - 5, iBottom };
			dc.DrawText(__PrevTrack, &rcPrevLink, 0);

			if (!m_bMouseDown && m_bHighlightNextLink)
			{
				(void)dc.SelectObject(m_fontUnderline);
			}
			else
			{
				(void)dc.SelectObject(m_fontSmall);
			}

			rcNextLink = { iXMiddlePos + 5, iTop, iXMiddlePos + 5 + szLink.cx, iBottom };
			dc.DrawText(__NextTrack, &rcNextLink, 0);
		}
	}
	else if (!strSingerAlbum.empty())
	{
		bFlag = true;

		if (SingerAlbumLink.bHittest)
		{
			(void)dc.SelectObject(m_fontUnderline);
		}
		else
		{
			(void)dc.SelectObject(m_fontSmall);
		}
		CSize szLink = dc.GetTextExtent(strSingerAlbum.c_str());

		rcSingerAlbum.left = iLeft;
		rcSingerAlbum.top = iYMiddlePos + (iItemHeight / 2 - szLink.cy) / 2 - 2;
		rcSingerAlbum.right = MIN(rcSingerAlbum.left + szLink.cx, iXPosDuration - 20);
		rcSingerAlbum.bottom = rcSingerAlbum.top + szLink.cy;

		dc.DrawText(strSingerAlbum.c_str(), rcSingerAlbum, DT_NOPREFIX | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	}

	CRect rcTitle(iLeft, rcItem.top, rcItem.right, rcItem.bottom);
	if (bFlag)
	{
		rcTitle.bottom = iYMiddlePos+5;
	}
	else
	{
		if (!strDuration.empty())
		{
			rcTitle.bottom -= 20;
		}
	}

	UINT uFormat = DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE;
	if (!bPlayingItem && !bMouseMoveItem)
	{
		uFormat |= DT_WORD_ELLIPSIS;
	}

	dc.SelectObject(pFont);
	dc.DrawText(PlayItem.GetTitle().c_str(), rcTitle, uFormat);

	(void)dc.SelectObject(pFontPrev);
}

void CPlayingList::refresh(int nPlayingItem)
{
	if (nPlayingItem >= 0)
	{
		m_nPlayingItem = nPlayingItem;
	}

	auto uItemCount = m_view.getPlayMgr().getPlayingItems().size();
	m_vecSingerAlbumLink.resize(uItemCount);

	vector<vector<wstring>> vecTexts(uItemCount, { L"" });
	this->SetTexts(vecTexts);
}

void CPlayingList::onPlay(UINT uPlayingItem, bool bManual)
{
	int nPrevItem = m_nPlayingItem;
	m_nPlayingItem = uPlayingItem;

	if (nPrevItem >= 0 && nPrevItem != m_nPlayingItem)
	{
		(void)Update(nPrevItem);
	}

	(void)Update(m_nPlayingItem);

	if (bManual)
	{
		(void)EnsureVisible(m_nPlayingItem, FALSE);
	}
}

void CPlayingList::rename()
{
	int iSelItem = this->GetSelItem();
	if (iSelItem >= 0)
	{
		EnsureVisible(iSelItem, FALSE);

		(void)EditLabel(iSelItem);
	}
}

bool CPlayingList::GetRenameText(UINT uItem, wstring& strRenameText)
{
	m_nRenameItem = uItem;
	(void)Update(m_nRenameItem);
	
	m_view.getPlayMgr().getPlayingItems().get(uItem, [&](auto& PlayItem) {
		strRenameText = PlayItem.GetTitle();
	});

	return true;
}

void CPlayingList::OnListItemRename(UINT uItem, const CString& cstrNewText)
{
	m_view.getPlayMgr().getPlayingItems().get(uItem, [&](CPlayItem& PlayItem) {
		m_view.getController().renameMedia(PlayItem, (wstring)cstrNewText);
	});
}

void CPlayingList::GetSelItems(TD_PlayItemList& arrSelPlayItem)
{
	list<UINT> lstSelItems;
	__super::GetSelItems(lstSelItems);

	auto& lstPlayingItems = m_view.getPlayMgr().getPlayingItems();
	for (auto uItem : lstSelItems)
	{
		lstPlayingItems.get(uItem, [&](CPlayItem& PlayItem) {
			arrSelPlayItem.add(PlayItem);
		});
	}
}

bool CPlayingList::HittestTrackLink(UINT uItem, const CPoint& ptPos)
{
	bool bHighlightTracks = false;
	bool bHighlightPrevLink = false;
	bool bHighlightNextLink = false;
	if (uItem == m_nPlayingItem)
	{
		if (m_TracksLink.rcPos.PtInRect(ptPos))
		{
			bHighlightTracks = true;
		}
		else if (m_PrevLink.rcPos.PtInRect(ptPos))
		{
			bHighlightPrevLink = true;
		}
		else if (m_NextLink.rcPos.PtInRect(ptPos))
		{
			bHighlightNextLink = true;
		}
	}

	if (m_bHighlightTracks != bHighlightTracks || m_bHighlightPrevLink != bHighlightPrevLink || m_bHighlightNextLink != bHighlightNextLink)
	{
		m_bHighlightTracks = bHighlightTracks;
		m_bHighlightPrevLink = bHighlightPrevLink;
		m_bHighlightNextLink = bHighlightNextLink;
		return true;
	}

	return false;
}

bool CPlayingList::HittestSingerAlbumLink(tagLink& SingerAlbumLink, const CPoint& ptPos)
{
	bool bHighlightSingerAlbumLink = false;
	if (SingerAlbumLink.rcPos.PtInRect(ptPos))
	{
		bHighlightSingerAlbumLink = true;
	}
	if (SingerAlbumLink.bHittest != bHighlightSingerAlbumLink)
	{
		SingerAlbumLink.bHittest = bHighlightSingerAlbumLink;
		return true;
	}

	return false;
}

void CPlayingList::OnTrackLinkClick()
{
	auto& PlayMgr = m_view.getPlayMgr();

	PlayMgr.getPlayingItems().get(m_nPlayingItem, [&](CPlayItem& PlayItem) {
		CRCueFile cueFile = PlayItem.getCueFile();
		CRTrackInfo trackInfo = cueFile.getTrack(UINT(PlayMgr.getPlayer().getClock()/1000));
		
		UINT uTrackIndex = trackInfo.uIndex;
		if (m_bHighlightNextLink)
		{
			uTrackIndex++;
			if (uTrackIndex >= cueFile.m_alTrackInfo.size())
			{
				uTrackIndex = 0;
			}
		}
		else
		{
			if (0 == uTrackIndex)
			{
				uTrackIndex = cueFile.m_alTrackInfo.size() - 1;
			}
			else
			{
				uTrackIndex--;
			}
		}
		
		cueFile.m_alTrackInfo.get(uTrackIndex, [&](auto& trackInfo) {
			m_view.m_PlayCtrl.seek(trackInfo.uMsBegin /1000);
			m_view.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Play);
		});
	});
}

BOOL CPlayingList::handleNMNotify(NMHDR& NMHDR, LRESULT* pResult)
{
	BOOL bRet = __super::handleNMNotify(NMHDR, pResult);
	
	switch (NMHDR.code)
	{
	case NM_DBLCLK:
	{
		NMLISTVIEW& NMList = (NMLISTVIEW&)NMHDR;
		if (NMList.iItem >= 0)
		{
			m_view.getPlayMgr().play((UINT)NMList.iItem);
		}
	}

	break;
	}

	return bRet;
}

bool CPlayingList::handleItemLButtonDown(UINT uItem, CPlayItem& PlayItem, const CPoint& ptPos)
{
	auto& SingerAlbumLink = m_vecSingerAlbumLink[uItem];
	if (SingerAlbumLink.rcSingerImg.PtInRect(ptPos))
	{
		(void)m_view.hittestRelatedMediaSet(PlayItem, E_MediaSetType::MST_Singer);
		return true;
	}

	HittestSingerAlbumLink(SingerAlbumLink, ptPos);
	if (SingerAlbumLink.bHittest)
	{
		SingerAlbumLink.bHittest = false;
		this->Update(uItem);

		PlayItem.AsyncTask();
		this->UpdateItem(uItem, &PlayItem);

		if (!m_view.hittestRelatedMediaSet(PlayItem, E_MediaSetType::MST_Album))
		{
			(void)m_view.hittestRelatedMediaSet(PlayItem, E_MediaSetType::MST_Singer);
		}

		return true;
	}

	(void)HittestTrackLink(uItem, ptPos);
	if (m_bHighlightTracks)
	{
		m_bMouseDown = true;
		this->Update(m_nPlayingItem);

		CMediaRes *pMediaRes = PlayItem.GetMediaRes();
		if (NULL != pMediaRes)
		{
			(void)CTrackDetailDlg(m_view, pMediaRes->getCueFile(), pMediaRes).DoModal();
		}

		return true;
	}
	else if (m_bHighlightPrevLink || m_bHighlightNextLink)
	{
		m_bMouseDown = true;
		this->Update(m_nPlayingItem);

		OnTrackLinkClick();

		return true;
	}

	return false;
}

BOOL CPlayingList::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_LBUTTONDOWN == message)
	{
		CPoint ptPos(lParam);
		int iItem = this->HitTest(ptPos);
		if (iItem >= 0)
		{
			POINT ptItem{ 0,0 };
			this->GetItemPosition(iItem, &ptItem);
			ptPos.x -= ptItem.x;
			ptPos.y -= ptItem.y;

			UINT uItem = (UINT)iItem;

			bool bRet = false;
			m_view.getPlayMgr().getPlayingItems().get(uItem, [&](CPlayItem& PlayItem) {
				bRet = handleItemLButtonDown(uItem, PlayItem, ptPos);
			});
			if (bRet)
			{
				return TRUE;
			}
		}
	}
	else if (WM_MOUSEMOVE == message || WM_LBUTTONUP == message)
	{
		m_bMouseDown = false;

		CPoint ptPos(lParam);
		int iItem = this->HitTest(ptPos);
		if (iItem >= 0)
		{
			POINT ptItem{ 0,0 };
			this->GetItemPosition(iItem, &ptItem);
			ptPos.x -= ptItem.x;
			ptPos.y -= ptItem.y;

			tagLink& SingerAlbumLink = m_vecSingerAlbumLink[(UINT)iItem];
			if (HittestSingerAlbumLink(SingerAlbumLink, ptPos))
			{
				this->Update((UINT)iItem);
			}
			else
			{
				if (HittestTrackLink((UINT)iItem, ptPos))
				{
					this->Update(m_nPlayingItem);
				}

				if (WM_LBUTTONUP == message)
				{
					this->Update((UINT)iItem);
				}
			}
		}
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}
