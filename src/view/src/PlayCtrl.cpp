#include "stdafx.h"

#include "PlayCtrl.h"

CPlayCtrl::CPlayCtrl(class __view& view)
	: m_view(view)
	, m_PlaySpirit(view.m_MainWnd)
{
}

CPlayer& CPlayCtrl::player()
{
	return m_view.getPlayMgr().player();
}

bool CPlayCtrl::init()
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

	__EnsureReturn(_initPlaySpirit(PlaySpiritOption.nPosX
		, PlaySpiritOption.nPosY, strSkinPath, PlaySpiritOption.uVolume), false);

	return true;
}

bool CPlayCtrl::_initPlaySpirit(int nPosX, int nPosY, const wstring& strSkin, UINT uVolume)
{
	if (!m_PlaySpirit.Create([&](E_PlaySpiritEvent eEvent, UINT uButton, short para) {
		this->_handlePlaySpiritEvent(eEvent, uButton, para);
	}, nPosX, nPosY))
	{
		return false;
	}

	m_PlaySpirit->PutVolum(uVolume);

	m_PlaySpirit->SetShadowLevel(1);
	
	m_PlaySpirit.SetSkin(strSkin);

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
			m_view.foregroundMainWnd();
		}
		
		break;
	case E_PlaySpiritEvent::PSE_DblClick:
		m_view.foregroundMainWnd();

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

		m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Play, false);
		m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Stop, true);
	}

	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Last, true);
	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Next, true);
	
	m_strPlayingFile = PlayItem.GetTitle();
	m_PlaySpirit->SetPlayState(_bstr_t(m_strPlayingFile.c_str()), uDuration, 0);
}

void CPlayCtrl::onPlayFinish(bool bOpenFail)
{
	CMainApp::async([&, bOpenFail]() {
		if (bOpenFail)
		{
			//m_PlaySpirit->clear();

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
	});
}

void CPlayCtrl::handlePlaySpiritButtonClick(ST_PlaySpiritButton eButton, short para)
{
	auto& PlayMgr = m_view.getPlayMgr();
	switch (eButton)
	{
	case ST_PlaySpiritButton::PSB_Play:
		m_PlaySpirit->SetPause(false);

		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);

		break;
	case ST_PlaySpiritButton::PSB_Pause:
		g_bFailRetry = false;

		m_PlaySpirit->SetPause(true);

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
	
	m_PlaySpirit->PutVolum(uVolume);
}

void CPlayCtrl::seek(UINT uPos)
{
	player().Seek(uPos);
	m_PlaySpirit->SetPlayState(_bstr_t(m_strPlayingFile.c_str())
		, (long)player().GetDuration() + 1, uPos);
}

void CPlayCtrl::close()
{
	if (m_PlaySpirit)
	{
		tagPlaySpiritOption& PlaySpiritOption = m_view.getOption().PlaySpiritOption;
		PlaySpiritOption.uVolume = (UINT)m_PlaySpirit->GetVolum();

		cauto rcPos = m_PlaySpirit.rect();
		PlaySpiritOption.nPosX = rcPos.left;
		PlaySpiritOption.nPosY = rcPos.top;

		m_PlaySpirit.Destroy();
	}
}

bool CPlayCtrl::addPlayingItem(const SArray<wstring>& arrOppPaths, int nPos)
{
	__EnsureReturn(arrOppPaths, false);

	if (nPos >= 0 || CMainApp::getKeyState(VK_CONTROL))
	{
		bool bNeedPlay = (m_view.getPlayMgr().GetPlayStatus() == E_PlayStatus::PS_Stop);
		return m_view.getPlayMgr().insert(arrOppPaths, bNeedPlay, nPos);
	}
	else
	{
		return m_view.getPlayMgr().assign(arrOppPaths);
	}
}

bool CPlayCtrl::addPlayingItem(const TD_IMediaList& lstMedias, int nPos)
{
	SArray<wstring> arrOppPaths = lstMedias.map([](const IMedia& media) {
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
