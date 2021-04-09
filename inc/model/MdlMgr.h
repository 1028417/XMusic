#pragma once

class __ModelExt CUpgradeUrl
{
public:
    CUpgradeUrl(const string& strBaseUrl, const string& strPostFix)
        : m_strBaseUrl(strBaseUrl)
        , m_strPostFix(strPostFix)
    {
    }

private:
    string m_strBaseUrl;
    string m_strPostFix;

public:
    string mdlconf() const;

    string mdl() const;

    string singerimg(const string& strFile) const;

    string app() const;
};

struct __ModelExt tagMdlConf
{
    string strAppVersion;

    UINT uMdlVersion = 0;

    UINT uCompatibleCode = 0;

    list<CUpgradeUrl> lstUpgradeUrl;
};

struct __ModelExt tagOlBkg
{
    tagOlBkg() = default;
    tagOlBkg(cwstr strFile, UINT cx, UINT cy, uint64_t uFileSize)
        : strFile(strFile), cx(cx), cy(cy), uFileSize(uFileSize)
    {
    }
    wstring strFile;
    UINT cx = 0;
    UINT cy = 0;
    uint64_t uFileSize = 0;
};

struct __ModelExt tagOlBkgList
{
    wstring catName;
    string accName;
    string prjName;
    string artiName;

    list<tagOlBkg> lstBkg;

    string url();
};

enum class E_UpgradeResult
{
    UR_Success,
    UR_Fail,

    UR_DownloadFail,
    UR_MedialibInvalid,
    UR_ReadMedialibFail,

    UR_MedialibUnmatch,
    UR_MedialibUncompatible,
    UR_AppUpgradeFail,
    UR_AppUpgraded,

    UR_InitMediaLibFail
};

class __ModelExt CMdlMgr
{
public:
    CMdlMgr(class CModel& model)
        : m_model(model)
    {
    }

private:
    CModel& m_model;

    int m_nAppUpgradeProgress = -1;

    list<CUpgradeUrl> m_lstUpgradeUrl;
	
	list<tagOlBkgList> m_lstOlBkg;

private:
    E_UpgradeResult _loadMdl(const tagMdlConf& MdlConf, CByteBuffer& bbfMdl, bool bUpgradeDB);

    void _initOlBkg(CByteBuffer& bbfOlBkg);

public:
    const int& appUpgradeProgress() const
    {
        return m_nAppUpgradeProgress;
    }

    bool loadMdlConf(CByteBuffer& bbfMdlConf, tagMdlConf& mdlConf);

    E_UpgradeResult upgradeMdl(signal_t bRunSignal, const tagMdlConf& orgMdlConf);

    const list<CUpgradeUrl>& upgradeUrl() const
    {
        return m_lstUpgradeUrl;
    }

    const list<tagOlBkgList>& olBkg() const
    {
        return m_lstOlBkg;
    }
};
