#pragma once

#include "viewdef.h"

class CXController : public IXController
{
public:
	CXController(IPlayerView& view, IModel& model)
		: m_view(view)
		, m_model(model)
	{
	}

protected:
	IPlayerView& m_view;

	IModel& m_model;

	COptionMgr m_OptionMgr;

#if !__winvc
private:
        XThread m_threadPlayCtrl;
        CMtxLock<tagPlayCtrl> m_mtxPlayCtrl;
#endif

#if __winvc
protected:
	bool setupMediaLib();
#endif

public:
        tagOption& initOption() override
        {
                return m_OptionMgr.init();
        }

	tagOption& getOption() override
	{
		return m_OptionMgr.getOption();
	}

        void start();

        void stop();

#if __winvc
	CMediaDir* attachDir(const wstring& strDir) override;

	bool renameMedia(const IMedia& media, const wstring& strNewName) override;

	void moveMediaFile(const TD_IMediaList& lstMedias, const wstring& strDir) override;

	bool removeMediaRes(const TD_MediaResList& lstMediaRes) override;

	int addPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist) override;
	
	int addAlbumItems(const list<wstring>& lstFiles, CAlbum& Album) override;
	
	E_RenameRetCode renameMediaSet(CMediaSet& MediaSet, const wstring& strNewName) override;

	bool removeMediaSet(CMediaSet& MediaSet) override;

	bool autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
		, const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedia) override;

#else
        void callPlayCtrl(const tagPlayCtrl& PlayCtrl) override
        {
            m_mtxPlayCtrl.set(PlayCtrl); //m_sigPlayCtrl.set(PlayCtrl);
        }
#endif
};
