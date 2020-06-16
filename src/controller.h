#pragma once

#include "viewdef.h"

#include "OptionMgr.h"

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
        tagPlayCmd m_PlayCmd;
        mutex m_mutex;

        UINT m_uAutoPlayNextSeq = 0;
#endif

#if __winvc
protected:
	bool setupMediaLib();
#endif

private:
	void _moveMediaFile(const TD_IMediaList& lstMedias, cwstr strOppDir);

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
	CMediaDir* attachDir(cwstr strDir) override;

	bool renameMedia(const IMedia& media, cwstr strNewName) override;

	void moveMediaFile(const TD_IMediaList& lstMedias, cwstr strOppDir) override;

	bool removeMediaRes(const TD_MediaResList& lstMediaRes) override;

	int AddPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist) override;
	
	int AddAlbumItems(const list<wstring>& lstAbsPaths, CAlbum& album, int nPos = -1) override;
	int AddAlbumItems(const TD_IMediaList& paMedias, CAlbum& album, int nPos = -1) override;
		
	E_RenameRetCode renameMediaSet(CMediaSet& MediaSet, cwstr strNewName) override;

	bool removeMediaSet(CMediaSet& MediaSet) override;

	bool autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
		, const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedia) override;

#else
        void callPlayCmd(const tagPlayCmd& PlayCmd) override
        {
            m_mutex.lock();

            if (E_PlayCmd::PC_AutoPlayNext == PlayCmd.ePlayCmd)
            {
                if (m_PlayCmd.ePlayCmd != E_PlayCmd::PC_Null && m_PlayCmd.ePlayCmd != E_PlayCmd::PC_Append)
                {
                    m_mutex.unlock();
                    return;
                }
            }

            m_PlayCmd = PlayCmd;
            m_mutex.unlock();
        }
#endif
};
