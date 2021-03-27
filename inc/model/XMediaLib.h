
#pragma once

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

#if __OnlineMediaLib
private:
    list<JValue> m_lstSnapshot;

public:
    bool loadXSnapshot(Instream& ins);

    bool loadXCue(Instream& ins, cwstr strFileTitle)
    {
        return m_cuelist.load(ins, strFileTitle);
    }

private:
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;
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
};

#if !__winvc
#define __wholeTrackDuration 60 * 10

enum class E_SSCatType
{
    CT_None,
    CT_DSD,
    CT_Hires,
    CT_MQS,
    CT_DTS,
    CT_Disc,
};

class __ModelExt CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(class CSnapshotMediaDir& parent, cwstr strFileName, uint64_t uFileSize, UINT uDuration)
        : CMediaRes(E_MediaFileType::MFT_Null, (CMediaDir&)parent, strFileName, uFileSize)
        , m_uDuration(uDuration)
        , m_CueFile(uDuration>__wholeTrackDuration? __medialib.cuelist().find(GetTitle()) : CCueFile::NoCue)
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

    CMediaSet* mediaSet() const override
    {
        //崩溃return (class CSnapshotMediaDir*)m_fi.pParent;
        return ((CMediaDir*)m_fi.pParent)->mediaSet();
    }

    CRCueFile cueFile() override
    {
        return m_CueFile;
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

    void attachToSinger(CSinger& singer, const tagSingerAttachDir& attachDir)
    {
        if (!attachDir.strAliasName.empty())
        {
            m_strName = attachDir.strAliasName;
        }
        else// if (strutil::matchIgnoreCase(m_fi.strName, singer.m_strName))
        {
            //m_strName = attachDir.strDir.substr(0, attachDir.strDir.find(__wcPathSeparator, 1));
            //m_strName.erase(0, 4);
            //m_strName.append(fileName());

            for (CMediaSet *pMediaSet = this; ; pMediaSet = pMediaSet->m_pParent)
            {
                if (NULL == pMediaSet->m_pParent)
                {
                    m_strName = pMediaSet->m_strName + __wcPathSeparator + fileName();
                    break;
                }
            }
        }

        m_pParent = &singer;
        SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer, singer.m_uID, singer.m_strName);
    }

    UINT singerID() const
    {
        auto uSingerID = GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
        if (0 == uSingerID && m_pParent)
        {
            return ((CSnapshotMediaDir*)m_pParent)->singerID();
        }
        return uSingerID;
    }

    cwstr singerName() const
    {
        cauto strSingerName = GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
        if (strSingerName.empty() && m_pParent)
        {
            return ((CSnapshotMediaDir*)m_pParent)->singerName();
        }
        return strSingerName;
    }

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

    void GetAllMedias(TD_IMediaList& paMedia) override
    {
        for (auto pSubFile : m_paSubFile)
        {
            auto pSnapshotMediaRes = (CSnapshotMediaRes*)pSubFile;
            if (pSnapshotMediaRes->available)
            {
                paMedia.add(pSnapshotMediaRes);
            }
        }

        for (auto pSubDir : m_paSubDir)
        {
            ((CSnapshotMediaDir*)pSubDir)->GetAllMedias(paMedia);
        }
    }

    wstring name() const override
    {
        if (!m_strName.empty())
        {
            return m_strName;
        }

        return m_fi.strName;
    }

    CMediaSet* mediaSet() const override
    {
        return (CMediaSet*)this;
    }

    bool available = false;

    bool scanAvailable()
    {
        for (auto pSubFile : m_paSubFile)
        {
            if (((CSnapshotMediaRes*)pSubFile)->available)
            {
                available = true;
                break;
            }
        }

        for (auto pSubDir : m_paSubDir)
        {
            if (((CSnapshotMediaDir*)pSubDir)->scanAvailable())
            {
                available = true;
            }
        }

        return available;
    }
};
#endif
