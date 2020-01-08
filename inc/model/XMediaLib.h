
#pragma once

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
#define __appFilePostfix "macOs.dmg"
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

#if !__winvc
struct __ModelExt tagMedialibConf
{
    string strAppVersion;

    UINT uMedialibVersion = 0;

    UINT uCompatibleCode = 0;

    list<CUpgradeUrl> lstUpgradeUrl;

    list<string> lstOnlineHBkg;
    list<string> lstOnlineVBkg;

    void clear()
    {
        strAppVersion.clear();

        uMedialibVersion = 0;

        uCompatibleCode = 0;

        lstUpgradeUrl.clear();

        lstOnlineHBkg.clear();
        lstOnlineVBkg.clear();
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
    int duration() const override
    {
        return m_uDuration;
    }
};
#endif

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
	static bool m_bOnlineMediaLib;

    XMediaLib(class IModelObserver& ModelObserver, class CModel& model);

private:
	class IModelObserver& m_ModelObserver;
    class CModel& m_model;

#if !__winvc
private:
    tagMedialibConf m_newMedialibConf;

    unordered_map<string, string> m_mapXurl;
    unordered_map<string, string> m_mapShareUrl;

    list<JValue> m_lstSnapshot;

public:
    bool readMedialibConf(tagMedialibConf& medialibConf, Instream *pins = NULL);

    bool upgradeMediaLib(const tagMedialibConf& prevMedialibConf, CB_DownloadProgress& cbProgress);

    const tagMedialibConf& medialibConf() const
    {
        return m_newMedialibConf;
    }

    static string genUrl(const string& strUrl, const string& strFileTitle);

    bool checkUrl(const string& strFileTitle);
    bool checkUrl(const wstring& strFileTitle)
    {
       return checkUrl(strutil::toUtf8(strFileTitle));
    }

    string getXurl(const wstring& strFileTitle);
    string getShareUrl(const wstring& strFileTitle);

private:
    bool _upgradeMediaLib(CZipFile& zipFile, const tagMedialibConf& prevMedialibConf);
    bool _upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl);

    bool _loadXurl(Instream& ins);

    bool _loadShareLib(CZipFile& zipFile);

    void _insertUrl(const string& strFileName, const string& strUrl);

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
