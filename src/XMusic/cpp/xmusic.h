#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include "androidutil.h"

#define __pkgName L"com.musicrossoft.xmusic"

#define __mediaPng(f) ":/img/medialib/" #f ".png"

extern cwstr g_strWorkDir;

#define __cyIPhoneXBangs __size(128)
bool checkIPhoneXBangs(int cx, int cy);

const WString& mediaQualityString(E_MediaQuality eQuality);

void async(cfn_void cb);
void async(UINT uDelayTime, cfn_void cb);

bool installApp(const CByteBuffer& bbfData);

#include "app.h"
