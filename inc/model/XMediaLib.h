
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

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

    const CMedia* findRelatedPlayItem(cwstr strPath) override;
    const CMedia* findRelatedAlbumItem(cwstr strPath, const CSinger*& pRelatedSinger) override;

	bool renameMedia(IMedia& media, cwstr strNewName) override;

	UINT checkDuration(IMedia& media, int64_t& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};
