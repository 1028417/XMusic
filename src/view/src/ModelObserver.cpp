
#include "stdafx.h"

#include "ModelObserver.h"

void CModelObserver::initView()
{
	m_view.initView();
}

void CModelObserver::clearView()
{
	m_view.clearView();
}

void CModelObserver::refreshMedia()
{
	CMediaResPanel::RefreshMediaResPanel();

	if (m_view.m_PlayItemPage)
	{
		m_view.m_PlayItemPage.ShowPlaylist(m_view.m_PlayItemPage.m_pPlaylist, false);
	}

	if (m_view.m_AlbumPage)
	{
		m_view.m_AlbumPage.RefreshAlbum();
	}

	m_view.m_PlayingPage.RefreshList();
}

void CModelObserver::onPlayingListUpdated(int nPlayingItem, bool bSetActive)
{
	if (!m_view.getPlayMgr().getPlayinglist().available())
	{
		m_view.m_PlayCtrl.getPlaySpirit()->clear();
	}

	m_view.m_PlayingPage.RefreshList(nPlayingItem);

	if (bSetActive)
	{
		m_view.m_PlayingPage.Active(false);
	}
}

void CModelObserver::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
	m_view.m_PlayingPage.m_wndList.onPlay(uPlayingItem, bManual);
	m_view.m_PlayCtrl.onPlay(PlayItem);
}

void CModelObserver::onPlayStoped(E_DecodeStatus decodeStatus)
{
	if (decodeStatus != E_DecodeStatus::DS_Cancel)
	{
		CMainApp::sync(0, [&, decodeStatus]() {
			bool bOpenFail = E_DecodeStatus::DS_OpenFail == decodeStatus;
			m_view.m_PlayCtrl.onPlayStoped(bOpenFail);
		});
	}
}

UINT CModelObserver::GetSingerImgPos(UINT uSingerID)
{
	return m_view.m_ImgMgr.getSingerImgPos(uSingerID);
}

bool CModelObserver::renameMedia(IMedia& media, const wstring& strNewName)
{
	CRedrawLockGuard RedrawLockGuard(m_view.m_MainWnd);

	return m_view.getController().renameMedia(media, strNewName);
}
