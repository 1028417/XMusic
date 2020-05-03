#pragma once

#include "model/model.h"

enum class E_PlayCtrl
{
    PC_Null = 0,

    PC_PlayIndex,

    PC_PlayPrev,
    PC_PlayNext,
    PC_AutoPlayNext,

    PC_Assign,
    PC_Append,
    PC_AppendPlay,

    PC_Demand
};

struct tagPlayCtrl
{
    tagPlayCtrl() = default;

    tagPlayCtrl(E_PlayCtrl t_ePlayCtrl)
    {
        ePlayCtrl = t_ePlayCtrl;
    }

    tagPlayCtrl(UINT uIdx)
    {
        ePlayCtrl = E_PlayCtrl::PC_PlayIndex;
        uPlayIdx = uIdx;
    }

    tagPlayCtrl(const TD_IMediaList& t_paMedias)
    {
        ePlayCtrl = E_PlayCtrl::PC_Assign;
        paMedias.assign(t_paMedias);
    }
    tagPlayCtrl(IMedia& media, bool bPlay)
    {
        ePlayCtrl = bPlay ? E_PlayCtrl::PC_AppendPlay : E_PlayCtrl::PC_Append;
        paMedias.assign(media);
    }

    tagPlayCtrl(E_DemandMode eMode, E_LanguageType eLanguage = E_LanguageType::LT_None)
    {
        ePlayCtrl = E_PlayCtrl::PC_Demand;
        eDemandMode = eMode;
        eDemandLanguage = eLanguage;
    }

    void get(tagPlayCtrl& PlayCtrl)
    {
        if (ePlayCtrl != E_PlayCtrl::PC_Null)
        {
            PlayCtrl = *this;

            ePlayCtrl = E_PlayCtrl::PC_Null;
            paMedias.clear();
        }
    }

    E_PlayCtrl ePlayCtrl = E_PlayCtrl::PC_Null;

    UINT uPlayIdx = 0;

    E_DemandMode eDemandMode = E_DemandMode::DM_Null;
    E_LanguageType eDemandLanguage = E_LanguageType::LT_None;

    TD_IMediaList paMedias;
};

class IXController
{
public:
	virtual tagOption& initOption() = 0;
	virtual tagOption& getOption() = 0;

#if __winvc
    virtual CMediaDir* attachDir(cwstr strDir) = 0;

    virtual bool renameMedia(const IMedia& media, cwstr strNewName) = 0;

    virtual void moveMediaFile(const TD_IMediaList& lstMedias, cwstr strDir) = 0;

    virtual bool removeMediaRes(const TD_MediaResList& lstMediaRes) = 0;

    virtual int AddPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist) = 0;

	virtual int AddAlbumItems(const list<wstring>& lstAbsPaths, CAlbum& Album, int nPos = -1) = 0;
	virtual int AddAlbumItems(const TD_IMediaList& paMedias, CAlbum& album, int nPos = -1) = 0;

    virtual E_RenameRetCode renameMediaSet(CMediaSet& MediaSet, cwstr strNewName) = 0;

    virtual bool removeMediaSet(CMediaSet& MediaSet) = 0;

    virtual bool autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
            , const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedias) = 0;

#else
    virtual void callPlayCtrl(const tagPlayCtrl& PlayCtrl) = 0;
#endif
};
