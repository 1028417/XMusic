
#pragma once

#if !__winvc
#include "XUrlMap.h"

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

class CSnapshotMediaRes : public CMediaRes
{
public:
    CSnapshotMediaRes(E_MediaFileType eFileType, const tagFileInfo& fileInfo)
        : CMediaRes(eFileType, fileInfo)
    {
    }

    UINT m_uDuration = 0;

private:
    UINT duration() const override
    {
        return m_uDuration;
    }
};

#define __OnlineMediaLib 1
#else
#define __OnlineMediaLib 0
#endif

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
    XMediaLib(class CModel& model, class IModelObserver& ModelObserver);

private:
    class CModel& m_model;
    class IModelObserver& m_ModelObserver;

#if !__winvc
private:
    tagMedialibConf m_newMedialibConf;

    XUrlMap m_xurlMap;

    list<JValue> m_lstSnapshot;

public:
    tagMedialibConf& medialibConf()
    {
        return m_newMedialibConf;
    }

    bool loadShareUrl(Instream& ins);

    bool loadXUrl(Instream& ins);

    bool loadXSnapshot(Instream& ins);

    bool checkXUrl(const wstring& strPath);

    string getXUrl(const wstring& strFileTitle)
    {
        return m_xurlMap.get(strutil::toUtf8(strFileTitle));
    }

private:
    bool _checkXUrl(const string& strFileTitle);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(CMediaDir& parent, const JValue& jRoot);
#endif

private:
    void GetSubSets(TD_MediaSetList& lstSubSets) override;

	CMedia* findRelatedMedia(const wstring& strPath, E_MediaSetType eMediaSetType, CMediaSet*& pSinger) override;

	void renameMedia(IMedia& media, const wstring& strNewName) override;

	UINT checkDuration(IMedia& media, long long& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};
