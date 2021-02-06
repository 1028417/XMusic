#pragma once

#include "model/model.h"

enum class E_PlayCmd
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

struct tagPlayCmd
{
    tagPlayCmd(E_PlayCmd ePlayCmd = E_PlayCmd::PC_Null)
        : ePlayCmd(ePlayCmd)
    {
    }

    void get(tagPlayCmd& PlayCmd)
    {
        if (ePlayCmd != E_PlayCmd::PC_Null)
        {
            PlayCmd = *this;

            ePlayCmd = E_PlayCmd::PC_Null;
            paMedias.clear();
        }
    }

    E_PlayCmd ePlayCmd = E_PlayCmd::PC_Null;

    UINT uPlayIdx = 0;

    E_DemandMode eDemandMode = E_DemandMode::DM_Null;
    E_LanguageType eDemandLanguage = E_LanguageType::LT_None;

    TD_IMediaList paMedias;
};

struct tagPlayIndexCmd : tagPlayCmd
{
    tagPlayIndexCmd(UINT uIdx)
        : tagPlayCmd(E_PlayCmd::PC_PlayIndex)
    {
        uPlayIdx = uIdx;
    }
};

struct tagAssignMediaCmd : tagPlayCmd
{
    tagAssignMediaCmd(const TD_IMediaList& paMedias)
        : tagPlayCmd(E_PlayCmd::PC_Assign)
    {
        tagPlayCmd::paMedias.assign(paMedias);
    }
};

struct tagAssignMediaSetCmd : tagPlayCmd
{
    tagAssignMediaSetCmd(CMediaSet& MediaSet)
        : tagPlayCmd(E_PlayCmd::PC_Assign)
    {
        MediaSet.GetAllMedias(paMedias);
    }
};

struct tagAppendMediaCmd : tagPlayCmd
{
    tagAppendMediaCmd(IMedia& media)
        : tagPlayCmd(E_PlayCmd::PC_Append)
    {
        paMedias.assign(media);
    }
};

struct tagPlayMediaCmd : tagPlayCmd
{
    tagPlayMediaCmd(IMedia& media)
        : tagPlayCmd(E_PlayCmd::PC_AppendPlay)
    {
        paMedias.assign(media);
    }
};

struct tagDemandCmd : tagPlayCmd
{
    tagDemandCmd(E_DemandMode eMode, E_LanguageType eLanguage)
        : tagPlayCmd(E_PlayCmd::PC_Demand)
    {
        eDemandMode = eMode;
        eDemandLanguage = eLanguage;
    }
};

class IXController
{
public:
	virtual tagOption& getOption() = 0;

#if __winvc
	virtual bool setupMediaLib() = 0;

    virtual CMediaDir* attachDir(cwstr strDir) = 0;

    virtual bool renameMedia(const IMedia& media, cwstr strNewName) = 0;

    virtual void moveMediaFile(const TD_IMediaList& lstMedias, cwstr strDir) = 0;

	virtual bool replaceMediaRes(const map<CMediaRes*, CMediaRes*>& mapMediaRes, bool bUseNewName) = 0;
    virtual bool removeMediaRes(const TD_MediaResList& paMediaRes, bool bRemoveMedia=false) = 0;

    virtual int AddPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist) = 0;

	virtual int AddAlbumItems(const list<wstring>& lstAbsPaths, CAlbum& Album, int nPos = -1) = 0;
	virtual int AddAlbumItems(const TD_IMediaList& paMedias, CAlbum& album, int nPos = -1) = 0;

    virtual E_RenameRetCode renameMediaSet(CMediaSet& MediaSet, cwstr strNewName) = 0;

    virtual bool removeMediaSet(CMediaSet& MediaSet) = 0;

    virtual bool autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
            , const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedias) = 0;

#else
    virtual void callPlayCmd(const tagPlayCmd& PlayCmd) = 0;
#endif
};
