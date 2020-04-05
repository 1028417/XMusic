
#pragma once

#if !__winvc
class CUpgradeUrl
{
public:
    CUpgradeUrl(const string& strBaseUrl) : m_strBaseUrl(strBaseUrl)
    {
    }

private:
    string m_strBaseUrl;

public:
    string appUrl() const
    {
#if __android
#define __appFilePostfix "android.apk"
#elif __windows
#define __appFilePostfix "win32.zip"
#elif __mac
#define __appFilePostfix "macOs.zip"
#else
#define __appFilePostfix ""
        return "";
#endif

        return m_strBaseUrl + "/app/XMusic-" __appFilePostfix;
    }

    string medialibUrl() const
    {
        return m_strBaseUrl + "/medialib/medialib";
    }

    string singerimgUrl() const
    {
        return m_strBaseUrl + "/singerimg/";
    }

    string hbkgUrl() const
    {
        return m_strBaseUrl + "/hbkg";
    }

    string vbkgUrl() const
    {
        return m_strBaseUrl + "/vbkg";
    }
};

struct __ModelExt tagMedialibConf
{
    string strAppVersion;

    UINT uMedialibVersion = 0;

    UINT uCompatibleCode = 0;

    list<CUpgradeUrl> lstUpgradeUrl;

    //list<string> lstOnlineHBkg;
    //list<string> lstOnlineVBkg;

    void clear()
    {
        strAppVersion.clear();

        uMedialibVersion = 0;

        uCompatibleCode = 0;

        lstUpgradeUrl.clear();

        //lstOnlineHBkg.clear();
        //lstOnlineVBkg.clear();
    }
};

class CSnapshotDir : public CMediaDir
{
public:
    CSnapshotDir() = default;

    CSnapshotDir(const wstring& strPath, class CPath *t_pParent = NULL)
        : CMediaDir(strPath, t_pParent)
    {
    }

    E_MediaDirType dirType() override
    {
        return E_MediaDirType::MDT_Snapshot;
    }
};

class CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(const tagFileInfo& fileInfo, UINT uDuration)
        : CMediaRes(E_MediaFileType::MFT_Null, fileInfo)
        , m_uDuration(uDuration)
    {
    }

private:
    UINT m_uDuration = 0;

private:
    UINT duration() const override
    {
        return m_uDuration;
    }
};
#endif

#define __xmedialib ((XMediaLib&)CMediaLib::inst())

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
    XMediaLib(class CModel& model, class IModelObserver& ModelObserver);

private:
    class CModel& m_model;
    class IModelObserver& m_ModelObserver;

#if __OnlineMediaLib
private:
    tagMedialibConf m_newMedialibConf;

    list<JValue> m_lstSnapshot;

public:
    tagMedialibConf& medialibConf()
    {
        return m_newMedialibConf;
    }

    bool loadXUrl(Instream& ins);

    bool checkXUrl(const wstring& strPath);

    string getXUrl(const wstring& strPath) const;

    bool loadXSnapshot(Instream& ins);

private:
    bool _loadXUrl(const string& strText);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(const wstring& strFileType, const JValue& jRoot, CMediaDir *pParent);
#endif

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

	CMedia* findRelatedMedia(const wstring& strPath, E_MediaSetType eMediaSetType, CSinger*& pRelatedSinger) override;

	void renameMedia(IMedia& media, const wstring& strNewName) override;

	UINT checkDuration(IMedia& media, long long& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};
