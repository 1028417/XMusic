#include "stdafx.h"

#include "PlayCtrl.h"

CPlayCtrl::CPlayCtrl(class __view& view)
	: m_view(view)
{
}

CPlayer& CPlayCtrl::player()
{
	return m_view.getPlayMgr().player();
}

BOOL CPlayCtrl::showPlaySpirit()
{
	auto& PlaySpiritOption = m_view.getOption().PlaySpiritOption;
	
	wstring strSkinPath;
	if (!PlaySpiritOption.strSkinName.empty())
	{
		strSkinPath = __PlaySpiritSkinDir + PlaySpiritOption.strSkinName + L".skb";
	}

	cauto rtWorkArea =	getWorkArea(false);
	PlaySpiritOption.nPosX = MAX(PlaySpiritOption.nPosX, rtWorkArea.left + 30);
	PlaySpiritOption.nPosY = MAX(PlaySpiritOption.nPosY, rtWorkArea.top + 30);
	PlaySpiritOption.nPosX = MIN(PlaySpiritOption.nPosX, rtWorkArea.right - 400);
	PlaySpiritOption.nPosY = MIN(PlaySpiritOption.nPosY, rtWorkArea.bottom - 100);

	CPlaySpirit::inst()->LoadSkin(bstr_t(strSkinPath.c_str()));
	CPlaySpirit::inst()->SetShadowLevel(1);

	CPlaySpirit::inst()->PutVolum(PlaySpiritOption.uVolume);

	if (!CPlaySpirit::inst().Show([&](E_PlaySpiritEvent eEvent, UINT uButton, short para) {
		this->_handlePlaySpiritEvent(eEvent, uButton, para);
	}, m_view.m_MainWnd.m_wndSysToolBar, PlaySpiritOption.nPosX, PlaySpiritOption.nPosY))
	{
		return false;
	}
	
	return true;
}

void CPlayCtrl::_handlePlaySpiritEvent(E_PlaySpiritEvent eEvent, UINT uButton, short para)
{
	switch (eEvent)
	{
	case E_PlaySpiritEvent::PSE_RButtonUp:
		if (m_view.m_MainWnd.IsIconic())
		{
			static bool s_bMenuShowed = false;
			if (!s_bMenuShowed)
			{
				s_bMenuShowed = true;

				__async([&]() {
					CMenuGuard MenuGuard(m_view.m_ResModule, IDR_PLAYSPIRIT, 180);
					MenuGuard.Popup(&m_view.m_MainWnd, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);

					s_bMenuShowed = false;
				});
			}
		}
		else
		{
			m_view.m_MainWnd.setForeground();
		}
		
		break;
	case E_PlaySpiritEvent::PSE_DblClick:
		m_view.m_MainWnd.setForeground();

		break;
	case E_PlaySpiritEvent::PSE_ButtonClick:
		this->handlePlaySpiritButtonClick((ST_PlaySpiritButton)uButton, para);

		break;
	}
}

static bool g_bFailRetry = false;

void CPlayCtrl::onPlay(CPlayItem& PlayItem)
{
	g_bFailRetry = false;
	
	UINT uDuration = PlayItem.duration();
	if (uDuration > 0)
	{
		uDuration++;

		CPlaySpirit::inst()->EnableButton(ST_PlaySpiritButton::PSB_Play, false);
		CPlaySpirit::inst()->EnableButton(ST_PlaySpiritButton::PSB_Stop, true);
	}

	CPlaySpirit::inst()->EnableButton(ST_PlaySpiritButton::PSB_Last, true);
	CPlaySpirit::inst()->EnableButton(ST_PlaySpiritButton::PSB_Next, true);
	
	m_strPlayingFile = PlayItem.GetTitle();
	CPlaySpirit::inst()->SetPlayState(_bstr_t(m_strPlayingFile.c_str()), uDuration, 0);
}

void CPlayCtrl::onPlayFinish(bool bRet)
{
	if (m_view.getPlayMgr().mediaOpaque().decodeStatus() == E_DecodeStatus::DS_Cancel)
	{
		return;
	}

	__appSync([&, bRet]() {
		if (!bRet)
		{
			//CPlaySpirit::inst()->clear();

			g_bFailRetry = true;
			__async(2000, [&]() {
				if (g_bFailRetry)
				{
					(void)m_view.getPlayMgr().playNext(false);
				}
			});
		}
		else
		{
			(void)m_view.getPlayMgr().playNext(false);
		}
	}, false);
}

void CPlayCtrl::handlePlaySpiritButtonClick(ST_PlaySpiritButton eButton, short para)
{
	auto& PlayMgr = m_view.getPlayMgr();
	switch (eButton)
	{
	case ST_PlaySpiritButton::PSB_Play:
		CPlaySpirit::inst()->SetPause(false);

		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);

		break;
	case ST_PlaySpiritButton::PSB_Pause:
		g_bFailRetry = false;

		CPlaySpirit::inst()->SetPause(true);

		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Pause);

		break;
	case ST_PlaySpiritButton::PSB_Stop:
		g_bFailRetry = false;

		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Stop);

		break;
	case ST_PlaySpiritButton::PSB_Last:
		PlayMgr.playPrev();

		break;
	case ST_PlaySpiritButton::PSB_Next:
		(void)PlayMgr.playNext();
		
		break;
	case ST_PlaySpiritButton::PSB_Volume:
		player().SetVolume((UINT)para);
		m_view.getOption().PlaySpiritOption.uVolume = (UINT)para;

		break;
	case ST_PlaySpiritButton::PSB_Mute:
		if (para)
		{
			player().SetVolume(0);
		}
		else
		{
			player().SetVolume(m_view.getOption().PlaySpiritOption.uVolume);
		}

		break;
	case ST_PlaySpiritButton::PSB_Progress:
		player().Seek(para);
		break;
	};
}

void CPlayCtrl::setVolume(int offset)
{
	UINT& uVolume = m_view.getOption().PlaySpiritOption.uVolume;

	int nVolume = uVolume;
	nVolume += offset;
	nVolume = MAX(nVolume, 0);
	nVolume = MIN(nVolume, 100);
	
	uVolume = (UINT)nVolume;
	player().SetVolume(uVolume);
	
	CPlaySpirit::inst()->PutVolum(uVolume);
}

void CPlayCtrl::seek(UINT uPos)
{
	player().Seek(uPos);
	CPlaySpirit::inst()->SetPlayState(_bstr_t(m_strPlayingFile.c_str())
		, (long)m_view.getPlayMgr().mediaOpaque().duration() + 1, uPos);
}

void CPlayCtrl::close()
{
	if (CPlaySpirit::inst())
	{
		tagPlaySpiritOption& PlaySpiritOption = m_view.getOption().PlaySpiritOption;
		PlaySpiritOption.uVolume = (UINT)CPlaySpirit::inst()->GetVolum();

		cauto rcPos = CPlaySpirit::inst().rect();
		PlaySpiritOption.nPosX = rcPos.left;
		PlaySpiritOption.nPosY = rcPos.top;

		CPlaySpirit::inst().Destroy();
	}
}

bool CPlayCtrl::addPlayingItem(const SArray<wstring>& arrOppPaths, int nPos)
{
	__EnsureReturn(arrOppPaths, false);

	if (nPos >= 0 || CMainApp::getKeyState(VK_CONTROL))
	{
		bool bNeedPlay = (m_view.getPlayMgr().playStatus() == E_PlayStatus::PS_Stop);
		return m_view.getPlayMgr().insert(arrOppPaths, bNeedPlay, nPos);
	}
	else
	{
		return m_view.getPlayMgr().assign(arrOppPaths);
	}
}

bool CPlayCtrl::addPlayingItem(const TD_IMediaList& lstMedias, int nPos)
{
	cauto arrOppPaths = lstMedias.map([](const IMedia& media) {
		return media.GetPath();
	});

	return addPlayingItem(arrOppPaths, nPos);
}

bool CPlayCtrl::addPlayingItem(CMediaSet& MediaSet)
{
	TD_MediaList lstMedias;
	MediaSet.GetAllMedias(lstMedias);
	return addPlayingItem(TD_IMediaList(lstMedias));
}
