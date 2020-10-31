
#include "StdAfx.h"

#include "PlayingList.h"

#include "../../dlg/TrackDetailDlg.h"

#define __PrevTrack L"上一曲"
#define __NextTrack L"下一曲"

BEGIN_MESSAGE_MAP(CPlayingList, CObjectList)
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

void CPlayingList::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	(void)this->ModifyStyle(0, LVS_OWNERDRAWFIXED);
}

void CPlayingList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	RECT rcClient{ 0,0,0,0 };
	GetClientRect(&rcClient);
	UINT cy = rcClient.bottom - rcClient.top;

	UINT uRowCount = cy/m_view.m_globalSize.m_uBigIconSize;
	lpMeasureItemStruct->itemHeight = (UINT)round(float(cy)/uRowCount);
}

void CPlayingList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemID >= 0)
	{
		_drawItem(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem, (UINT)lpDrawItemStruct->itemID);
	}
}

BOOL CPlayingList::InitCtrl()
{
	(void)this->ModifyStyle(LVS_SINGLESEL
		, LVS_NOCOLUMNHEADER | LVS_EDITLABELS);
	(void)this->ModifyStyleEx(0, WS_EX_LEFTSCROLLBAR);

	__super::InitCtrl(tagListPara(0));

	/*__super::SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		_drawItem(lvcd.dc, lvcd.rc, lvcd.uItem);
		lvcd.bSkipDefault = true;
	});*/

	__super::SetTrackMouse([&](E_TrackMouseEvent eMouseEvent, const CPoint& point) {
		if (E_TrackMouseEvent::LME_MouseLeave == eMouseEvent)
		{
			if (-1 != m_nMouseMoveItem)
			{
				int iPreItem = m_nMouseMoveItem;
				m_nMouseMoveItem = -1;
				this->Update(iPreItem);
			}
		}
	});

	auto fBigFontSize = m_view.m_globalSize.m_fMidFontSize;
	__AssertReturn(m_font.create(*this, fBigFontSize), FALSE);

	__AssertReturn(m_fontPlaying.create(*this, fBigFontSize, 500), FALSE);

	__AssertReturn(m_fontPlayed.create(*this, fBigFontSize), FALSE);

	float fSmallFontSize = m_view.m_globalSize.m_fSmallFontSize * .6f;
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

void CPlayingList::_drawItem(HDC hDC, RECT& rc, UINT uItem)
{
	m_view.getPlayMgr().playingItems().get(uItem, [&](cauto PlayItem) {
#define __xOffset 1
		int cx = (rc.right- __xOffset) - rc.left;
		int cy = rc.bottom - rc.top;

		CCompDC CompDC;
		(void)CompDC.create(cx, cy, hDC);
		CDC& MemDC = CompDC.getDC();

		_drawItem(MemDC, cx, cy, uItem, PlayItem);

		(void)::BitBlt(hDC, __xOffset, rc.top, cx, cy, MemDC, 0, 0, SRCCOPY);
	});
}

void CPlayingList::_drawItem(CDC& dc, int cx, int cy, int nItem, const CPlayItem& PlayItem)
{
	bool bPlayingItem = (nItem == m_nPlayingItem);
	bool bMouseMoveItem = (nItem == m_nMouseMoveItem);

	int iYMiddlePos = cy / 2 + 1;

	CFont *pFontPrev = dc.GetCurrentFont();

	COLORREF crBkColor = GetSysColor(COLOR_WINDOW);
	COLORREF crTextColor = __Color_Text;
	CFont *pFont = &m_font;
	if (bPlayingItem)
	{
		crBkColor = GetSysColor(COLOR_HIGHLIGHT);
		crTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		pFont = &m_fontPlaying;
	}
	else
	{
		if (this->GetItemState(nItem, LVIS_SELECTED))
		{
			crBkColor = __crSelect;
		}
		else if (bMouseMoveItem)
		{
			crBkColor = __crHit;
		}

		if (m_view.getPlayMgr().checkPlayedID(PlayItem.m_uID))
		{
			crTextColor = RGB(85, 32, 139);
			pFont = &m_fontPlayed;
		}
	}

	RECT rcItem { 0, 0, cx, cy };
	dc.FillSolidRect(&rcItem, crBkColor);

	(void)dc.SetTextColor(crTextColor);
	
	if (nItem == m_nRenameItem)
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
		cauto strAlbumName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Album);

		iImage = PlayItem.getSingerImg();
		if (iImage < 0)
		{
			wstring strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
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

	tagItemLinks& ItemLinks = m_vecItemLinks[nItem];

	auto& lnkSingerAlbum = ItemLinks.lnkSingerAlbum;
	auto& rcSingerAlbum = lnkSingerAlbum.rcPos;
	memzero(rcSingerAlbum);

	CRect& rcSingerImg = ItemLinks.lnkSingerImg.rcPos;
	
	int x = 0;
	if (iImage >= 0)
	{
		if ((int)E_GlobalImage::GI_WholeTrack == iImage)
		{
			auto offset = (cy - (int)m_view.m_globalSize.m_uBigIconSize)/2;
			m_view.m_ImgMgr.bigImglst().Draw(&dc, iImage, { offset, offset }, 0);
		}
		else
		{
#define __Margin 0
			m_view.m_ImgMgr.bigImglst().DrawEx(&dc, iImage, { __Margin, __Margin }
				, { cy - __Margin * 2, cy - __Margin * 2 }, CLR_NONE, CLR_NONE, ILD_SCALE);

			rcSingerImg.SetRect(0, 0, cy, cy);
		}

		x = cy;
	}
	else
	{
		memzero(rcSingerImg);
	}

	x += 6;
	cx -= 6;

	int iXPosDuration = cx;
	auto& strDuration = PlayItem.durationString();
	if (!strDuration.empty())
	{
		(void)dc.SelectObject(m_fontSmall);

		iXPosDuration -= 37 + int(strDuration.size() - 4) * 8;
		RECT rcPos { iXPosDuration, iYMiddlePos, cx, cy };
		dc.DrawText(strDuration.c_str(), &rcPos, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	auto& rcTrackCountLink = ItemLinks.lnkTrackCount.rcPos;
	auto& rcNextTrackLink = m_lnkNextTrack.rcPos;
	auto& rcPrevTrackLink = m_lnkPrevTrack.rcPos;
	if (bPlayingItem)
	{
		rcTrackCountLink = rcPrevTrackLink = rcNextTrackLink = { 0, 0, 0, 0 };
	}

	bool bFlag = false;
	if (cueFile)
	{
		bFlag = true;

		if (!m_bMouseDown && ItemLinks.lnkTrackCount.bHittest)
		{
			(void)dc.SelectObject(m_fontUnderline);
		}
		else
		{
			(void)dc.SelectObject(m_fontSmall);
		}

		wstring strTracks = to_wstring(cueFile.m_alTrackInfo.size()) + L"首";
		CSize szLink = dc.GetTextExtent(strTracks.c_str());

		int iTop = iYMiddlePos + (cy / 2 - szLink.cy) / 2 - 2;
		int iBottom = iTop + szLink.cy;
		rcTrackCountLink = { x, iTop, x + szLink.cx, iBottom };

		dc.DrawText(strTracks.c_str(), &rcTrackCountLink, 0);

		if (bPlayingItem)
		{
			if (!m_bMouseDown && m_lnkPrevTrack.bHittest)
			{
				(void)dc.SelectObject(m_fontUnderline);
			}
			else
			{
				(void)dc.SelectObject(m_fontSmall);
			}

			szLink = dc.GetTextExtent(__PrevTrack);
			int iXMiddlePos = (x + szLink.cx * 2 / 3 + iXPosDuration) / 2;
			rcPrevTrackLink = { iXMiddlePos - 5 - szLink.cx, iTop, iXMiddlePos - 5, iBottom };
			dc.DrawText(__PrevTrack, &rcPrevTrackLink, 0);

			if (!m_bMouseDown && m_lnkNextTrack.bHittest)
			{
				(void)dc.SelectObject(m_fontUnderline);
			}
			else
			{
				(void)dc.SelectObject(m_fontSmall);
			}

			rcNextTrackLink = { iXMiddlePos + 5, iTop, iXMiddlePos + 5 + szLink.cx, iBottom };
			dc.DrawText(__NextTrack, &rcNextTrackLink, 0);
		}
	}
	else if (!strSingerAlbum.empty())
	{
		bFlag = true;

		if (lnkSingerAlbum.bHittest)
		{
			(void)dc.SelectObject(m_fontUnderline);
		}
		else
		{
			(void)dc.SelectObject(m_fontSmall);
		}
		CSize szLink = dc.GetTextExtent(strSingerAlbum.c_str());

		rcSingerAlbum.left = x;
		rcSingerAlbum.top = iYMiddlePos + (cy / 2 - szLink.cy) / 2 - 2;
		rcSingerAlbum.right = MIN(rcSingerAlbum.left + szLink.cx, iXPosDuration - 20);
		rcSingerAlbum.bottom = rcSingerAlbum.top + szLink.cy;

		dc.DrawText(strSingerAlbum.c_str(), rcSingerAlbum, DT_NOPREFIX | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	}

	RECT rcTitle { x, 0, cx, cy };
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
	dc.DrawText(PlayItem.GetTitle().c_str(), &rcTitle, uFormat);

	(void)dc.SelectObject(pFontPrev);
}

void CPlayingList::refresh(int nPlayingItem)
{
	if (nPlayingItem >= 0)
	{
		m_nPlayingItem = nPlayingItem;
	}

	auto uItemCount = m_view.getPlayMgr().playingItems().size();
	m_vecItemLinks.resize(uItemCount);

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
	
	m_view.getPlayMgr().playingItems().get(uItem, [&](auto& PlayItem) {
		strRenameText = PlayItem.GetTitle();
	});

	return true;
}

void CPlayingList::OnListItemRename(UINT uItem, const CString& cstrNewText)
{
	m_view.getPlayMgr().playingItems().get(uItem, [&](cauto PlayItem) {
		m_view.getController().renameMedia(PlayItem, (wstring)cstrNewText);
	});
}

void CPlayingList::GetSelItems(TD_PlayItemList& arrSelPlayItem)
{
	list<UINT> lstSelItems;
	__super::GetSelItems(lstSelItems);

	cauto lstPlayingItems = m_view.getPlayMgr().playingItems();
	for (auto uItem : lstSelItems)
	{
		lstPlayingItems.get(uItem, [&](cauto PlayItem) {
			arrSelPlayItem.add((CPlayItem&)PlayItem);
		});
	}
}

BOOL CPlayingList::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_MOUSEMOVE == message || WM_LBUTTONDOWN == message || WM_LBUTTONUP == message || WM_LBUTTONDBLCLK == message)
	{
		m_bMouseDown = (WM_LBUTTONDOWN == message);
		
		CPoint ptPos(lParam);
		int nItem = HitTest(ptPos);
		if (nItem >= 0)
		{
			UINT uItem = (UINT)nItem;

			POINT ptItem{ 0,0 };
			this->GetItemPosition(uItem, &ptItem);
			ptPos.x -= ptItem.x;
			ptPos.y -= ptItem.y;

			switch (message)
			{
			case WM_LBUTTONDBLCLK:
				(void)m_view.getPlayMgr().play(uItem);

				break;
			case WM_LBUTTONUP:
				if (uItem == m_nPlayingItem)
				{
					if (m_lnkPrevTrack.hittest(ptPos) || m_lnkNextTrack.hittest(ptPos))
					{
						this->Update(uItem);
					}
				}

				break;
			case WM_LBUTTONDOWN:
			{
				bool bRet = false;
				tagItemLinks& ItemLinks = m_vecItemLinks[uItem];
				E_ItemLinkType eLinkType = ItemLinks.hittest(ptPos);
				if (E_ItemLinkType::ILT_None == eLinkType)
				{
					if (uItem == m_nPlayingItem)
					{
						if (m_lnkPrevTrack.hittest(ptPos))
						{
							eLinkType = m_lnkPrevTrack;
						}
						else if (m_lnkNextTrack.hittest(ptPos))
						{
							eLinkType = m_lnkNextTrack;
						}
					}
				}

				if (E_ItemLinkType::ILT_None != eLinkType)
				{
					if (GetSelectedCount()<=1 && !CMainApp::getKeyState(VK_SHIFT) && !CMainApp::getKeyState(VK_CONTROL))
					{
						__super::AsyncLButtondown([=]() {
							m_view.getPlayMgr().playingItems().get(uItem, [&](cauto PlayItem) {
								handleLinkClick(uItem, (CPlayItem&)PlayItem, m_vecItemLinks[uItem], eLinkType);
							});
						});
					}

					return TRUE;
				}
			}

			break;
			case WM_MOUSEMOVE:
			{
				bool bNeedUpdate = false;

				if (m_nMouseMoveItem != uItem)
				{
					int nPreItem = m_nMouseMoveItem;
					m_nMouseMoveItem = uItem;
					bNeedUpdate = true;

					if (nPreItem >= 0)
					{
						this->Update(nPreItem);
					}
				}

				tagItemLinks& ItemLinks = m_vecItemLinks[uItem];

				bool bChanged = false;
				(void)ItemLinks.hittest(ptPos, &bChanged);
				if (!bChanged)
				{
					(void)m_lnkPrevTrack.hittest(ptPos, &bChanged);
					if (!bChanged)
					{
						(void)m_lnkNextTrack.hittest(ptPos, &bChanged);
					}
				}

				if (bChanged || bNeedUpdate)
				{
					this->Update(uItem);
				}
			}

			break;
			};
		}
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CPlayingList::handleLinkClick(UINT uItem, CPlayItem& PlayItem, tagItemLinks& ItemLinks, E_ItemLinkType eLinkType)
{
	switch (eLinkType)
	{
	case E_ItemLinkType::ILT_SingerImg:
		(void)m_view.hittestRelatedMediaSet(PlayItem, E_RelatedMediaSet::RMS_Singer);
		break;
	case E_ItemLinkType::ILT_SingerAlbum:
		(void)PlayItem.findRelatedMedia();

		ItemLinks.lnkSingerAlbum.bHittest = false;
		this->Update(uItem);

		if (!m_view.hittestRelatedMediaSet(PlayItem, E_RelatedMediaSet::RMS_Album))
		{
			(void)m_view.hittestRelatedMediaSet(PlayItem, E_RelatedMediaSet::RMS_Singer);
		}

		break;
	case E_ItemLinkType::ILT_TrackCount:
	{
		this->Update(uItem);

		CMediaRes *pMediaRes = __medialib.findSubFile(PlayItem);
		if (pMediaRes)
		{
			(void)CTrackDetailDlg(m_view, pMediaRes->getCueFile(), pMediaRes).DoModal();
		}
	}

	break;
	case E_ItemLinkType::ILT_PrevTrack:
	case E_ItemLinkType::ILT_NextTrack:
	{
		this->Update(uItem);

		auto& playMgr = m_view.getPlayMgr();
		if (playMgr.playStatus() == E_PlayStatus::PS_Stop)
		{
			(void)playMgr.play(uItem);
			return;
		}

		CRCueFile cueFile = PlayItem.getCueFile();

		UINT uTrackIndex = 0;
		auto uClock = UINT(playMgr.mediaOpaque().clock() / 1000);
		if (uClock > 0)
		{
			uTrackIndex = cueFile.getTrack(uClock).uIndex;
		}

		if (E_ItemLinkType::ILT_PrevTrack == eLinkType)
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
		else
		{
			uTrackIndex++;
			if (uTrackIndex >= cueFile.m_alTrackInfo.size())
			{
				uTrackIndex = 0;
			}
		}

		cueFile.m_alTrackInfo.get(uTrackIndex, [&](auto& trackInfo) {
			m_view.m_PlayCtrl.seek(trackInfo.uMsBegin / 1000);
		});
	}

	break;
	};
}
