
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

void CModelObserver::renameMedia(IMedia& media, const wstring& strNewName)
{
	CRedrawLock RedrawLock(m_view.m_MainWnd, true);

	(void)m_view.getController().renameMedia(media, strNewName);
}

void CModelObserver::onPlayingListUpdated(int nPlayingItem, bool bSetActive)
{
	if (!m_view.getPlaylistMgr().playinglist().playable())
	{
		CPlaySpirit::inst()->clear();
	}

	m_view.m_PlayingPage.RefreshList(nPlayingItem);

	if (bSetActive)
	{
		(void)m_view.m_MainWnd.ActivePage(m_view.m_PlayingPage, false);
	}
}

void CModelObserver::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
	m_view.m_PlayingPage.m_wndList.onPlay(uPlayingItem, bManual);

	m_view.m_PlayCtrl.onPlay(PlayItem);
}

void CModelObserver::onPlayStop(bool bCanceled, bool bOpenFail)
{
	if (!bCanceled)
	{
		m_view.m_PlayCtrl.onPlayFinish(bOpenFail);
	}
}

UINT CModelObserver::GetSingerImgPos(UINT uSingerID)
{
	return m_view.m_ImgMgr.getSingerImgPos(uSingerID);
}
