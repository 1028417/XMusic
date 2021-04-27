
#include "model/model.h"

#include "OptionMgr.h"

#if __winvc
#define __confFile "/xmusicstudio.conf"
#else
#define __confFile "/xmusic.conf"
#endif

#define _jRoot(x)          jRoot[#x]

#define _jRootDir           _jRoot(RootDir)
#define _jAttachDir         _jRoot(AttachDir)

#define _jPlaySpiritSkin    _jRoot(PlaySpiritSkin)
#define _jPlaySpiritLeft	_jRoot(PlaySpiritLeft)
#define _jPlaySpiritTop     _jRoot(PlaySpiritTop)
#define _jPlaySpiritVolum	_jRoot(PlaySpiritVolum)

#define _jPlayingItem		_jRoot(PlayingItem)
#define _jRandomPlay		_jRoot(RandomPlay)
#define _jForce48KHz		_jRoot(Force48KHz)

#define _jFullScreen		_jRoot(FullScreen)
#define _jHideMenuBar		_jRoot(HideMenuBar)

#define _jAlarmHour		    _jRoot(AlarmHour)
#define _jAlarmMinute		_jRoot(AlarmMinute)

#define _jUseBkgColor       _jRoot(UseBkgColor)
#define _jBkgColor          _jRoot(BkgColor)
#define _jForeColor         _jRoot(ForeColor)

#define _jHBkg				_jRoot(HBkg)
#define _jHBkgX             _jRoot(HBkgX)
#define _jHBkgY             _jRoot(HBkgY)
#define _jVBkg				_jRoot(VBkg)
#define _jVBkgX             _jRoot(VBkgX)
#define _jVBkgY             _jRoot(VBkgY)
#define _jAddBkgDir         _jRoot(AddBkgDir)

#define _jHSingerImgPos     _jRoot(HSingerImgPos)
#define _jVSingerImgPos     _jRoot(VSingerImgPos)

#define _jNetworkWarn       _jRoot(NetworkWarn)

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

    int nPlayingItem = 0;
    jsonutil::get(_jPlayingItem, nPlayingItem);
    if (nPlayingItem >= 0)
    {
        m_opt.uPlayingItem = (UINT)nPlayingItem;
    }

    jsonutil::get(_jRandomPlay, m_opt.bRandomPlay);
    jsonutil::get(_jForce48KHz, m_opt.bForce48KHz);

    jsonutil::get(_jFullScreen, m_opt.bFullScreen);

#if __winvc
    jsonutil::get(_jRootDir, true, m_opt.strRootDir);

    const JValue& jValue = _jAttachDir;
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

    jsonutil::get(_jHideMenuBar, m_opt.bHideMenuBar);

    jsonutil::get(_jPlaySpiritSkin, true, m_opt.PlaySpiritOption.strSkinName);
    jsonutil::get(_jPlaySpiritLeft, m_opt.PlaySpiritOption.nPosX);
    jsonutil::get(_jPlaySpiritTop, m_opt.PlaySpiritOption.nPosY);
    jsonutil::get(_jPlaySpiritVolum, m_opt.PlaySpiritOption.uVolume);

    jsonutil::get(_jAlarmHour, m_opt.nAlarmHour);
    jsonutil::get(_jAlarmMinute, m_opt.nAlarmMinute);

#else
    jsonutil::get(_jUseBkgColor, m_opt.bUseBkgColor);
    jsonutil::get(_jBkgColor, m_opt.crBkg);
    jsonutil::get(_jForeColor, m_opt.crFore);

    jsonutil::get(_jHBkg, true, m_opt.strHBkg);
    jsonutil::get(_jHBkgX, m_opt.prHBkgOffset.first);
    jsonutil::get(_jHBkgY, m_opt.prHBkgOffset.second);
    jsonutil::get(_jVBkg, true, m_opt.strVBkg);
    jsonutil::get(_jVBkgX, m_opt.prVBkgOffset.first);
    jsonutil::get(_jVBkgY, m_opt.prVBkgOffset.second);
    jsonutil::get(_jAddBkgDir, true, m_opt.strAddBkgDir);

    jsonutil::get(_jHSingerImgPos, m_opt.uHSingerImgPos);
    jsonutil::get(_jVSingerImgPos, m_opt.uVSingerImgPos);

    jsonutil::get(_jNetworkWarn, m_opt.bNetworkWarn);

    /*const JValue& jValue = _jXpk];
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

    _jPlayingItem = m_opt.uPlayingItem;

    _jRandomPlay = m_opt.bRandomPlay;
    _jForce48KHz = m_opt.bForce48KHz;

    _jFullScreen = m_opt.bFullScreen;

#if __winvc
    _jRootDir = strutil::toUtf8(m_opt.strRootDir);

    if (m_opt.plAttachDir)
    {
        auto& jValue = _jAttachDir;
        for (cauto pr : m_opt.plAttachDir)
        {
            jValue.append(JValue(strutil::toUtf8(pr.first)));
        }
    }

    _jHideMenuBar = m_opt.bHideMenuBar;

    _jPlaySpiritSkin = strutil::toAsc(m_opt.PlaySpiritOption.strSkinName);
    _jPlaySpiritLeft = m_opt.PlaySpiritOption.nPosX;
    _jPlaySpiritTop = m_opt.PlaySpiritOption.nPosY;
    _jPlaySpiritVolum = m_opt.PlaySpiritOption.uVolume;

    _jAlarmHour = m_opt.nAlarmHour;
    _jAlarmMinute = m_opt.nAlarmMinute;

#else
    _jUseBkgColor = m_opt.bUseBkgColor;
    if (m_opt.crBkg >= 0)
    {
        _jBkgColor = m_opt.crBkg;
    }
    if (m_opt.crFore >= 0)
    {
        _jForeColor = m_opt.crFore;
    }

    _jHBkg = strutil::toAsc(m_opt.strHBkg);
    _jHBkgX = m_opt.prHBkgOffset.first;
    _jHBkgY = m_opt.prHBkgOffset.second;
    _jVBkg = strutil::toAsc(m_opt.strVBkg);
    _jVBkgX = m_opt.prVBkgOffset.first;
    _jVBkgY = m_opt.prVBkgOffset.second;
    _jAddBkgDir = strutil::toAsc(m_opt.strAddBkgDir);

    _jHSingerImgPos = m_opt.uHSingerImgPos;
    _jVSingerImgPos = m_opt.uVSingerImgPos;

    _jNetworkWarn = m_opt.bNetworkWarn;

    /*auto& jValue = _jXpk;
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
