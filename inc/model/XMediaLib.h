
#pragma once

#if !__winvc
#define __OnlineMediaLib 1
//#define __CheckXmsc 1
#endif

#define __xmedialib ((XMediaLib&)__medialib)

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

    CCueList m_cuelist;

public:
    bool loadXSnapshot(Instream& ins);

    bool loadXCue(Instream& ins, cwstr strFileTitle)
    {
        return m_cuelist.load(ins, strFileTitle);
    }

    const CCueList& cuelist() const
    {
        return m_cuelist;
    }

private:
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;
#endif

public:
	void FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult);

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

    const CMedia* findRelatedPlayItem(cwstr strPath) override;
    const CMedia* findRelatedAlbumItem(cwstr strPath, const CSinger*& pRelatedSinger) override;

	bool renameMedia(IMedia& media, cwstr strNewName) override;

	UINT checkDuration(IMedia& media, long long& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};

#define __wholeTrackDuration 60 * 10

#if !__winvc
class __ModelExt CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(CSnapshotMediaDir& parent, cwstr strFileName, uint64_t uFileSize, UINT uDuration)
        : CMediaRes(E_MediaFileType::MFT_Null, parent, strFileName, uFileSize)
        , m_uDuration(uDuration)
        , m_CueFile(uDuration>__wholeTrackDuration? __xmedialib.cuelist().find(GetTitle()) : CCueFile::NoCue)
    {
    }

private:
    UINT m_uDuration = 0;

    CRCueFile m_CueFile;

public:
    bool isLocal() const override
    {
        return false;
    }

    UINT duration() const override
    {
        return m_uDuration;
    }

    CMediaSet* mediaSet() const override
    {
        if (m_fi.pParent)
        {
            return (CSnapshotMediaDir*)m_fi.pParent;
        }

        return NULL;
    }

    CRCueFile cueFile() override
    {
        return m_CueFile;
    }
};
#endif
