
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

#define OI_NetworkWarn      "NetworkWarn"

#define OI_User             "user"
#define OI_Pwd              "pwd"

void COptionMgr::init()
{
    m_bInited = true;

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
    jsonutil::get(jRoot[OI_RootDir], true, m_Option.strRootDir);

    const JValue& jValue = jRoot[OI_AttachDir];
    if (jValue.isArray())
    {
        for (UINT uIdx = 0; uIdx < jValue.size(); uIdx++)
        {
            wstring strAttachDir;
            jsonutil::get(jValue[uIdx], true, strAttachDir);
            if (!strAttachDir.empty())
            {
                m_Option.plAttachDir.add(strAttachDir, E_AttachDirType::ADT_Internal);
            }
        }
    }
#endif

    int nPlayingItem = 0;
    jsonutil::get(jRoot[OI_PlayingItem], nPlayingItem);
    if (nPlayingItem >= 0)
    {
        m_Option.uPlayingItem = (UINT)nPlayingItem;
    }

    jsonutil::get(jRoot[OI_RandomPlay], m_Option.bRandomPlay);
    jsonutil::get(jRoot[OI_Force48KHz], m_Option.bForce48KHz);

    m_Option.bFullScreen = true; // 默认开启全屏
    jsonutil::get(jRoot[OI_FullScreen], m_Option.bFullScreen);

#if __winvc
    jsonutil::get(jRoot[OI_HideMenuBar], m_Option.bHideMenuBar);

    jsonutil::get(jRoot[OI_PlaySpiritSkin], true, m_Option.PlaySpiritOption.strSkinName);
    jsonutil::get(jRoot[OI_PlaySpiritLeft], m_Option.PlaySpiritOption.nPosX);
    jsonutil::get(jRoot[OI_PlaySpiritTop], m_Option.PlaySpiritOption.nPosY);
    jsonutil::get(jRoot[OI_PlaySpiritVolum], m_Option.PlaySpiritOption.uVolume);

    jsonutil::get(jRoot[OI_AlarmHour], m_Option.nAlarmHour);
    jsonutil::get(jRoot[OI_AlarmMinute], m_Option.nAlarmMinute);

#else
    jsonutil::get(jRoot[OI_UseBkgColor], m_Option.bUseBkgColor);
    jsonutil::get(jRoot[OI_BkgColor], m_Option.crBkg);
    jsonutil::get(jRoot[OI_ForeColor], m_Option.crFore);

    jsonutil::get(jRoot[OI_HBkg], true, m_Option.strHBkg);
    jsonutil::get(jRoot[OI_HBkgX], m_Option.prHBkgOffset.first);
    jsonutil::get(jRoot[OI_HBkgY], m_Option.prHBkgOffset.second);
    jsonutil::get(jRoot[OI_VBkg], true, m_Option.strVBkg);
    jsonutil::get(jRoot[OI_VBkgX], m_Option.prVBkgOffset.first);
    jsonutil::get(jRoot[OI_VBkgY], m_Option.prVBkgOffset.second);
    jsonutil::get(jRoot[OI_AddBkgDir], true, m_Option.strAddBkgDir);

    jsonutil::get(jRoot[OI_NetworkWarn], m_Option.bNetworkWarn);

    jsonutil::get(jRoot[OI_User], m_Option.strUser);
    jsonutil::get(jRoot[OI_Pwd], m_Option.strPwd);

    /*const JValue& jValue = jRoot[OI_XPkg];
    if (jValue.isArray())
    {
        for (UINT uIdx = 0; uIdx < jValue.size(); uIdx++)
        {
            wstring strXPkg;
            jsonutil::get(jValue[uIdx], true, strXPkg);
            if (!strXPkg.empty())
            {
                m_Option.lstXPkg.push_back(strXPkg);
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
    jRoot[OI_RootDir] = strutil::toUtf8(m_Option.strRootDir);

    if (m_Option.plAttachDir)
    {
        auto& jValue = jRoot[OI_AttachDir];
        for (cauto pr : m_Option.plAttachDir)
        {
            jValue.append(JValue(strutil::toUtf8(pr.first)));
        }
    }
#endif

    jRoot[OI_PlayingItem] = m_Option.uPlayingItem;

    jRoot[OI_RandomPlay] = m_Option.bRandomPlay;
    jRoot[OI_Force48KHz] = m_Option.bForce48KHz;

    jRoot[OI_FullScreen] = m_Option.bFullScreen;

#if __winvc
    jRoot[OI_HideMenuBar] = m_Option.bHideMenuBar;

    jRoot[OI_PlaySpiritSkin] = strutil::toAsc(m_Option.PlaySpiritOption.strSkinName);
    jRoot[OI_PlaySpiritLeft] = m_Option.PlaySpiritOption.nPosX;
    jRoot[OI_PlaySpiritTop] = m_Option.PlaySpiritOption.nPosY;
    jRoot[OI_PlaySpiritVolum] = m_Option.PlaySpiritOption.uVolume;

    jRoot[OI_AlarmHour] = m_Option.nAlarmHour;
    jRoot[OI_AlarmMinute] = m_Option.nAlarmMinute;

#else
    jRoot[OI_UseBkgColor] = m_Option.bUseBkgColor;
    if (m_Option.crBkg >= 0)
    {
        jRoot[OI_BkgColor] = m_Option.crBkg;
    }
    if (m_Option.crFore >= 0)
    {
        jRoot[OI_ForeColor] = m_Option.crFore;
    }

    jRoot[OI_HBkg] = strutil::toAsc(m_Option.strHBkg);
    jRoot[OI_HBkgX] = m_Option.prHBkgOffset.first;
    jRoot[OI_HBkgY] = m_Option.prHBkgOffset.second;
    jRoot[OI_VBkg] = strutil::toAsc(m_Option.strVBkg);
    jRoot[OI_VBkgX] = m_Option.prVBkgOffset.first;
    jRoot[OI_VBkgY] = m_Option.prVBkgOffset.second;
    jRoot[OI_AddBkgDir] = strutil::toAsc(m_Option.strAddBkgDir);

    jRoot[OI_NetworkWarn] = m_Option.bNetworkWarn;

    jRoot[OI_User] = m_Option.strUser;
    jRoot[OI_Pwd] = m_Option.strPwd;

    /*auto& jValue = jRoot[OI_XPkg];
    for (cauto strXPkg : m_Option.lstXPkg)
    {
        jValue.append(JValue(strutil::toUtf8(strXPkg)));
    }*/
#endif

    jsonutil::writeFile(jRoot, fsutil::workDir() + __confFile, true);
}

#if __winvc
E_TimerOperate COptionMgr::checkTimerOperate()
{
    int nHour = 0, nMinute = 0;
    tmutil::getCurrentTime(nHour, nMinute);

    auto& TimerOption = m_Option.TimerOption;
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

    return nHour == m_Option.nAlarmHour && nMinute == m_Option.nAlarmMinute;
}
#endif
