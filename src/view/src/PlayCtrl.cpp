#include "stdafx.h"

#include "PlayCtrl.h"

CPlayCtrl::CPlayCtrl(class __view& view)
	: m_view(view)
	, m_PlaySpirit(view.m_MainWnd)
{
}

CPlayer& CPlayCtrl::player()
{
	return m_view.getPlayMgr().getPlayer();
}

bool CPlayCtrl::init()
{
	auto& PlaySpiritOption = m_view.getDataMgr().getPlaySpiritOption();
	
	wstring strSkinPath;
	if (!PlaySpiritOption.strSkinName.empty())
	{
		strSkinPath = __PlaySpiritSkinDir + PlaySpiritOption.strSkinName + L".skb";
	}

	cauto& rtWorkArea =	CMainApp::getWorkArea(false);
	PlaySpiritOption.iPosX = MAX(PlaySpiritOption.iPosX, rtWorkArea.left);
	PlaySpiritOption.iPosY = MAX(PlaySpiritOption.iPosY, rtWorkArea.top);

	__EnsureReturn(_initPlaySpirit(PlaySpiritOption.iPosX
		, PlaySpiritOption.iPosY, strSkinPath, PlaySpiritOption.iVolume), false);

	CRect rcPlaySpirit;
	::GetWindowRect(m_PlaySpirit.m_hWndPlaySpirit, &rcPlaySpirit);
	PlaySpiritOption.iPosX = MIN(PlaySpiritOption.iPosX, rtWorkArea.right - rcPlaySpirit.Width());
	PlaySpiritOption.iPosY = MIN(PlaySpiritOption.iPosY, rtWorkArea.bottom - rcPlaySpirit.Height());
	(void)::SetWindowPos(m_PlaySpirit.m_hWndPlaySpirit, NULL, PlaySpiritOption.iPosX, PlaySpiritOption.iPosY
		, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	return true;
}

bool CPlayCtrl::_initPlaySpirit(int iPosX, int iPosY, const wstring& strSkin, UINT uVolume)
{
	if (!m_PlaySpirit.Create([&](E_PlaySpiritEvent eEvent, UINT uButton, short para) {
		this->_handlePlaySpiritEvent(eEvent, uButton, para);
	}, iPosX, iPosY))
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

				CMainApp::async([&]() {
					CMenuGuard MenuGuard(m_view.m_ResModule, IDR_PLAYSPIRIT, 180);
					MenuGuard.Popup(&m_view.m_MainWnd, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);

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

static UINT g_iFlag = 0;
void CPlayCtrl::onPlay(CPlayItem& PlayItem)
{
	UINT iFlag = ++g_iFlag;

	int nDuration = PlayItem.GetDuration();
	if (nDuration <= 0)
	{
		m_PlaySpirit->clear();

		CMainApp::async([&, iFlag]() {
			if (iFlag != g_iFlag)
			{
				return;
			}
			(void)m_view.getPlayMgr().playNext(false);
		}, 3000);

		return;
	}

	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Play, false);
	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Stop, true);

	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Last, true);
	m_PlaySpirit->EnableButton(ST_PlaySpiritButton::PSB_Next, true);

	m_PlaySpirit->SetPlayState(_bstr_t(PlayItem.GetTitle().c_str())
		, nDuration + 1, long(player().getClock()/__1e6));
}

void CPlayCtrl::onPlayFinish()
{
	if (!m_view.getPlayMgr().playNext(false))
	{
		//m_PlaySpirit->clear();
	}
}

void CPlayCtrl::handlePlaySpiritButtonClick(ST_PlaySpiritButton eButton, short para)
{
	auto& PlayMgr = m_view.getPlayMgr();
	switch (eButton)
	{
	case ST_PlaySpiritButton::PSB_Play:
		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);

		break;
	case ST_PlaySpiritButton::PSB_Pause:
		m_PlaySpirit->SetPause(true);

		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Pause);

		break;
	case ST_PlaySpiritButton::PSB_Stop:
		PlayMgr.SetPlayStatus(E_PlayStatus::PS_Stop);

		break;
	case ST_PlaySpiritButton::PSB_Last:
		PlayMgr.playPrev();

		break;
	case ST_PlaySpiritButton::PSB_Next:
		(void)PlayMgr.playNext();
		
		break;
	case ST_PlaySpiritButton::PSB_Volume:
		player().SetVolume(para);
		m_view.getDataMgr().getPlaySpiritOption().iVolume = para;

		break;
	case ST_PlaySpiritButton::PSB_Mute:
		if (para)
		{
			player().SetVolume(0);
		}
		else
		{
			player().SetVolume(m_view.getDataMgr().getPlaySpiritOption().iVolume);
		}

		break;
	case ST_PlaySpiritButton::PSB_Progress:
		player().Seek(para);
		break;
	};
}

void CPlayCtrl::setVolume(int offset)
{
	auto& iVolume = m_view.getDataMgr().getPlaySpiritOption().iVolume;
	iVolume += offset;
	iVolume = MAX(iVolume, 0);
	iVolume = MIN(iVolume, 100);

	player().SetVolume((UINT)iVolume);
	
	m_PlaySpirit->PutVolum(iVolume);
}

void CPlayCtrl::seek(UINT uPos)
{
	player().Seek(uPos);
	m_PlaySpirit->SetPlayState(_bstr_t(m_view.getPlayMgr().GetPlayingFile().c_str())
		, (long)player().GetDuration() + 1, uPos);
}

void CPlayCtrl::close()
{
	if (m_PlaySpirit)
	{
		tagPlaySpiritOption& PlaySpiritOption = m_view.getDataMgr().getPlaySpiritOption();
		PlaySpiritOption.iVolume = m_PlaySpirit->GetVolum();

		CRect rcPos;
		(void)::GetWindowRect(m_PlaySpirit.m_hWndPlaySpirit, rcPos);
		PlaySpiritOption.iPosX = rcPos.left;
		PlaySpiritOption.iPosY = rcPos.top;

		m_PlaySpirit.Destroy();
	}
}

bool CPlayCtrl::addPlayingItem(const TD_IMediaList& lstMedias, int iPos)
{
	bool bNeedPlay = (m_view.getPlayMgr().GetPlayStatus() == E_PlayStatus::PS_Stop);

	__EnsureReturn(lstMedias, false);

	if (iPos >= 0 || CMainApp::getKeyState(VK_CONTROL))
	{
		return m_view.getPlayMgr().insert(lstMedias, bNeedPlay, iPos);
	}
	else
	{
		return m_view.getPlayMgr().assign(lstMedias);
	}
}

bool CPlayCtrl::addPlayingItem(CMediaSet& MediaSet)
{
	TD_MediaList lstMedias;
	MediaSet.GetAllMedias(lstMedias);
	return addPlayingItem(TD_IMediaList(lstMedias));
}
