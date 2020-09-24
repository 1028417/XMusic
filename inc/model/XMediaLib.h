
#pragma once

#if !__winvc
#define __OnlineMediaLib 1
//#define __CheckXUrl 1
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
    bool checkXUrl(cwstr strPath);

    string getXUrl(cwstr strPath) const;

    bool loadXUrl(Instream& ins);

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
    bool _loadXUrl(const string& strText);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(cwstr strFileType, const JValue& jRoot, CMediaDir *pParent);
#endif

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

    CMedia* findRelatedMedia(cwstr strPath, E_MediaSetType eMediaSetType, const CSinger*& pRelatedSinger) override;

	void renameMedia(IMedia& media, cwstr strNewName) override;

	UINT checkDuration(IMedia& media, long long& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};

#define __wholeTrackDuration 60 * 8

#if __OnlineMediaLib
class CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(const tagFileInfo& fileInfo, UINT uDuration)
        : CMediaRes(E_MediaFileType::MFT_Null, fileInfo)
        , m_uDuration(uDuration)
        , m_CueFile(uDuration > __wholeTrackDuration ? __xmedialib.cuelist().find(GetTitle()) : CCueFile::NoCue)
    {
    }

private:
    UINT m_uDuration = 0;

    CRCueFile m_CueFile;

public:
    UINT duration() const override
    {
        return m_uDuration;
    }

    CRCueFile cueFile()
    {
        return m_CueFile;
    }
};

class CSnapshotDir : public CMediaDir
{
public:
    CSnapshotDir() = default;

    CSnapshotDir(cwstr strPath, class CPath *pParent = NULL)
        : CMediaDir(strPath, pParent)
    {
    }

    E_MediaDirType dirType() override
    {
        return E_MediaDirType::MDT_Snapshot;
    }
};
#endif
