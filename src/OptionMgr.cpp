
#include "model/model.h"

#include "OptionMgr.h"

#if __winvc
#define __confFile "/xmusicstudio.conf"
#else
#define __confFile "/xmusic.conf"
#endif

#define OI_RootDir          "RootDir"
#define OI_AttachDir        "AttachDir"

#define OI_PlaySpiritSkin	"PlaySpiritSkin"
#define OI_PlaySpiritLeft	"PlaySpiritLeft"
#define OI_PlaySpiritTop	"PlaySpiritTop"
#define OI_PlaySpiritVolum	"PlaySpiritVolum"

#define OI_PlayingItem		"PlayingItem"
#define OI_RandomPlay		"RandomPlay"
#define OI_Force48KHz		"Force48KHz"

#define OI_FullScreen		"FullScreen"
#define OI_HideMenuBar		"HideMenuBar"

#define OI_AlarmHour		"AlarmHour"
#define OI_AlarmMinute		"AlarmMinute"

#define OI_UseBkgColor      "UseBkgColor"
#define OI_BkgColor         "BkgColor"
#define OI_ForeColor        "ForeColor"

#define OI_HBkg				"HBkg"
#define OI_HBkgX		    "HBkgX"
#define OI_HBkgY		    "HBkgY"
#define OI_VBkg				"VBkg"
#define OI_VBkgX		    "VBkgX"
#define OI_VBkgY		    "VBkgY"
#define OI_AddBkgDir		"AddBkgDir"

#define OI_HSingerImgPos    "HSingerImgPos"
#define OI_VSingerImgPos    "VSingerImgPos"

#define OI_NetworkWarn      "NetworkWarn"

void COptionMgr::init()
{
    m_bInited = true;

    m_opt.bFullScreen = true; // 默认开启全屏

    cauto strFile = fsutil::workDir() + __confFile;
#if __android
    if (!fsutil::existFile(strFile))
    {
        // 安卓配置迁移
        fsutil::copyFile(__androidOrgPath __confFile, strutil::fromAsc(strFile));
    }
#endif

    JValue jRoot;
    if (!jsonutil::loadFile(strFile, jRoot))
    {
        return; //不能返回false
    }

#if !__OnlineMediaLib
    const JValue& jValue = jRoot[OI_AttachDir];
    if (jValue.isArray())
    {
        for (UINT uIdx = 0; uIdx < jValue.size(); uIdx++)
        {
            wstring strAttachDir;
            jsonutil::get(jValue[uIdx], true, strAttachDir);
            if (!strAttachDir.empty())
            {
                m_opt.plAttachDir.add(strAttachDir, E_AttachDirType::ADT_Internal);
            }
        }
    }
#endif

    int nPlayingItem = 0;
    jsonutil::get(jRoot[OI_PlayingItem], nPlayingItem);
    if (nPlayingItem >= 0)
    {
        m_opt.uPlayingItem = (UINT)nPlayingItem;
    }

    jsonutil::get(jRoot[OI_RandomPlay], m_opt.bRandomPlay);
    jsonutil::get(jRoot[OI_Force48KHz], m_opt.bForce48KHz);

    jsonutil::get(jRoot[OI_FullScreen], m_opt.bFullScreen);

#if __winvc
    jsonutil::get(jRoot[OI_RootDir], true, m_opt.strRootDir);

    jsonutil::get(jRoot[OI_HideMenuBar], m_opt.bHideMenuBar);

    jsonutil::get(jRoot[OI_PlaySpiritSkin], true, m_opt.PlaySpiritOption.strSkinName);
    jsonutil::get(jRoot[OI_PlaySpiritLeft], m_opt.PlaySpiritOption.nPosX);
    jsonutil::get(jRoot[OI_PlaySpiritTop], m_opt.PlaySpiritOption.nPosY);
    jsonutil::get(jRoot[OI_PlaySpiritVolum], m_opt.PlaySpiritOption.uVolume);

    jsonutil::get(jRoot[OI_AlarmHour], m_opt.nAlarmHour);
    jsonutil::get(jRoot[OI_AlarmMinute], m_opt.nAlarmMinute);

#else
    jsonutil::get(jRoot[OI_UseBkgColor], m_opt.bUseBkgColor);
    jsonutil::get(jRoot[OI_BkgColor], m_opt.crBkg);
    jsonutil::get(jRoot[OI_ForeColor], m_opt.crFore);

    jsonutil::get(jRoot[OI_HBkg], true, m_opt.strHBkg);
    jsonutil::get(jRoot[OI_HBkgX], m_opt.prHBkgOffset.first);
    jsonutil::get(jRoot[OI_HBkgY], m_opt.prHBkgOffset.second);
    jsonutil::get(jRoot[OI_VBkg], true, m_opt.strVBkg);
    jsonutil::get(jRoot[OI_VBkgX], m_opt.prVBkgOffset.first);
    jsonutil::get(jRoot[OI_VBkgY], m_opt.prVBkgOffset.second);
    jsonutil::get(jRoot[OI_AddBkgDir], true, m_opt.strAddBkgDir);

    jsonutil::get(jRoot[OI_HSingerImgPos], m_opt.uHSingerImgPos);
    jsonutil::get(jRoot[OI_VSingerImgPos], m_opt.uVSingerImgPos);

    jsonutil::get(jRoot[OI_NetworkWarn], m_opt.bNetworkWarn);

    /*const JValue& jValue = jRoot[OI_Xpk];
    if (jValue.isArray())
    {
        for (UINT uIdx = 0; uIdx < jValue.size(); uIdx++)
        {
            wstring strXpk;
            jsonutil::get(jValue[uIdx], true, strXpk);
            if (!strXpk.empty())
            {
                m_opt.lstXpk.push_back(strXpk);
            }
        }
    }*/
#endif
}

void COptionMgr::saveOption()
{
    if (!m_bInited)
    {
        return;
    }

    JValue jRoot;

#if !__OnlineMediaLib
    if (m_opt.plAttachDir)
    {
        auto& jValue = jRoot[OI_AttachDir];
        for (cauto pr : m_opt.plAttachDir)
        {
            jValue.append(JValue(strutil::toUtf8(pr.first)));
        }
    }
#endif

    jRoot[OI_PlayingItem] = m_opt.uPlayingItem;

    jRoot[OI_RandomPlay] = m_opt.bRandomPlay;
    jRoot[OI_Force48KHz] = m_opt.bForce48KHz;

    jRoot[OI_FullScreen] = m_opt.bFullScreen;

#if __winvc
    jRoot[OI_RootDir] = strutil::toUtf8(m_opt.strRootDir);

    jRoot[OI_HideMenuBar] = m_opt.bHideMenuBar;

    jRoot[OI_PlaySpiritSkin] = strutil::toAsc(m_opt.PlaySpiritOption.strSkinName);
    jRoot[OI_PlaySpiritLeft] = m_opt.PlaySpiritOption.nPosX;
    jRoot[OI_PlaySpiritTop] = m_opt.PlaySpiritOption.nPosY;
    jRoot[OI_PlaySpiritVolum] = m_opt.PlaySpiritOption.uVolume;

    jRoot[OI_AlarmHour] = m_opt.nAlarmHour;
    jRoot[OI_AlarmMinute] = m_opt.nAlarmMinute;

#else
    jRoot[OI_UseBkgColor] = m_opt.bUseBkgColor;
    if (m_opt.crBkg >= 0)
    {
        jRoot[OI_BkgColor] = m_opt.crBkg;
    }
    if (m_opt.crFore >= 0)
    {
        jRoot[OI_ForeColor] = m_opt.crFore;
    }

    jRoot[OI_HBkg] = strutil::toAsc(m_opt.strHBkg);
    jRoot[OI_HBkgX] = m_opt.prHBkgOffset.first;
    jRoot[OI_HBkgY] = m_opt.prHBkgOffset.second;
    jRoot[OI_VBkg] = strutil::toAsc(m_opt.strVBkg);
    jRoot[OI_VBkgX] = m_opt.prVBkgOffset.first;
    jRoot[OI_VBkgY] = m_opt.prVBkgOffset.second;
    jRoot[OI_AddBkgDir] = strutil::toAsc(m_opt.strAddBkgDir);

    jRoot[OI_HSingerImgPos] = m_opt.uHSingerImgPos;
    jRoot[OI_VSingerImgPos] = m_opt.uVSingerImgPos;

    jRoot[OI_NetworkWarn] = m_opt.bNetworkWarn;

    /*auto& jValue = jRoot[OI_Xpk];
    for (cauto strXpk : m_opt.lstXpk)
    {
        jValue.append(JValue(strutil::toUtf8(strXpk)));
    }*/
#endif

    jsonutil::writeFile(jRoot, fsutil::workDir() + __confFile, true);
}

#if __winvc
E_TimerOperate COptionMgr::checkTimerOperate()
{
    int nHour = 0, nMinute = 0;
    tmutil::getCurrentTime(nHour, nMinute);

    auto& TimerOption = m_opt.TimerOption;
    if (TO_Null != TimerOption.eTimerOperate)
    {
        if (nHour == TimerOption.nHour && nMinute == TimerOption.nMinute)
        {
            E_TimerOperate eRet = TimerOption.eTimerOperate;
            TimerOption.eTimerOperate = TO_Null;

            return eRet;
        }
    }

    return TO_Null;
}

bool COptionMgr::checkAlarm()
{
    int nHour = 0, nMinute = 0;
    tmutil::getCurrentTime(nHour, nMinute);

    return nHour == m_opt.nAlarmHour && nMinute == m_opt.nAlarmMinute;
}
#endif
