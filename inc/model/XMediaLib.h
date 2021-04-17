
#pragma once

wstring getPathCatName(cwstr strPath);

#if !__winvc
#define __OnlineMediaLib 1

#if __OnlineMediaLib
#define __arti 1
//#define __xurl 0
#define __CheckOpaque 1
#endif
#endif

#define __xmedialib ((XMediaLib&)__medialib)

#define __checkMedia(media) __xmedialib.CheckMedia(media)

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
    XMediaLib(class CModel& model, class IModelObserver& ModelObserver);

private:
    class CModel& m_model;
    class IModelObserver& m_ModelObserver;

#if !__winvc
protected:
    CCueList m_cuelist;

public:
    const CCueList& cuelist() const
    {
        return m_cuelist;
    }

    bool loadCue(Instream& ins, cwstr strFileTitle)
    {
        return m_cuelist.load(ins, strFileTitle);
    }

    void scanXpk();

    bool loadXSnapshot(Instream& ins);

    void check();
#endif

public:
	void FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult);

	UINT CheckMedia(CMedia& media);
	
private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

    const CMedia* findRelatedPlayItem(cwstr strPath) override;
    const CMedia* findRelatedAlbumItem(cwstr strPath, const CSinger*& pRelatedSinger) override;

	bool renameMedia(IMedia& media, cwstr strNewName) override;

	UINT getSingerImgPos(UINT uSingerID) override;

#if !__winvc
    void _scanXpk(cwstr strDir, E_AttachDirType);
#endif
};

#if !__winvc
#define __wholeTrackDuration 60 * 10

enum class E_SSCatType
{
    CT_DSD = 0,
    CT_Hires,
    CT_MQS,
    CT_DTS,
    CT_Disc,
    CT_CD,
    CT_SQ,
    CT_SQ24,
    CT_Pure,
    CT_Max
};

class __ModelExt CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(class CSnapshotMediaDir& parent, cwstr strFileName, uint64_t uFileSize, UINT uDuration)
        : CMediaRes(E_MediaFileType::MFT_Null, (CMediaDir&)parent, strFileName, uFileSize)
        , m_uDuration(uDuration)
        , m_CueFile(uDuration>__wholeTrackDuration? __xmedialib.cuelist().find(GetTitle()) : CCueFile::NoCue)
    {
    }

    bool available = false;

private:
    UINT m_uDuration = 0;

    CRCueFile m_CueFile;

public:
    E_SSCatType catType() const;

    UINT duration() const override
    {
        return m_uDuration;
    }

    E_TrackType trackType() const override
    {
        if (m_uDuration > __wholeTrackDuration)
        {
            if (quality() >= E_MediaQuality::MQ_CD)
            {
                return E_TrackType::TT_HDWhole;
            }
            return E_TrackType::TT_SQWhole;
        }
        return E_TrackType::TT_Single;
    }

    CRCueFile cueFile() override
    {
        return m_CueFile;
    }

    CMediaSet* mediaSet() const override
    {
        //崩溃return (class CSnapshotMediaDir*)m_fi.pParent;
        return ((CMediaDir*)m_fi.pParent)->mediaSet();
    }
};

class __ModelExt CSnapshotMediaDir : public CMediaDir, public CMediaSet
{
public:
    CSnapshotMediaDir(cwstr strDir, CSnapshotMediaDir *pParent)
        : CMediaDir(strDir, pParent?(CMediaDir*)pParent:&__medialib)
        , CMediaSet(pParent, E_MediaSetType::MST_SnapshotMediaDir)
    {
    }

public:
    E_SSCatType catType() const;

    wstring catName() const
    {
        return getPathCatName(GetPath());
    }

    void attachToSinger(CSinger& singer, const tagSingerAttachDir& attachDir);

    CSinger* singer()
    {
        if (m_pParent && E_MediaSetType::MST_Singer == m_pParent->m_eType)
        {
            return (CSinger*)m_pParent;
        }
        return NULL;
    }

    CMediaSet* mediaSet() const override
    {
        return (CMediaSet*)this;
    }

    wstring name() const override;

    void GetSubSets(TD_MediaSetList& paMediaSet) override
    {
        for (auto pSubDir : m_paSubDir)
        {
            paMediaSet.add((CSnapshotMediaDir*)pSubDir);
        }
    }

    void GetMedias(TD_IMediaList& paMedia) override
    {
        for (auto pSubFile : m_paSubFile)
        {
            paMedia.add((CMediaRes*)pSubFile);
        }
    }

    void GetAllMedias(TD_IMediaList& paMedia) override;

    bool scanAvailable();

public:
    bool available = false;
};
#endif
