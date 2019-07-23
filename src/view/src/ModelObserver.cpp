
#include "stdafx.h"

#include "ModelObserver.h"

void CModelObserver::refreshMedia()
{
	m_view.m_PlayingPage.RefreshList();

	if (m_view.m_PlayItemPage)
	{
		m_view.m_PlayItemPage.ShowPlaylist(m_view.m_PlayItemPage.m_pPlaylist, false);
	}

	if (m_view.m_AlbumPage)
	{
		m_view.m_AlbumPage.RefreshAlbum();
	}

	CMediaResPanel::RefreshMediaResPanel();
}

void CModelObserver::refreshPlayingList(int nPlayingItem, bool bSetActive)
{
	if (!m_view.getPlayMgr().getPlayinglist().avalible())
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

void CModelObserver::onPlayFinish()
{
	CMainApp::async([=]() {
		m_view.m_PlayCtrl.onPlayFinish();
	});
}

UINT CModelObserver::GetSingerImgPos(UINT uSingerID)
{
	return m_view.m_ImgMgr.getSingerImgPos(uSingerID);
}

bool CModelObserver::renameMedia(IMedia& media, const wstring& strNewName)
{
	return m_view.getController().renameMedia(media, strNewName);
}
