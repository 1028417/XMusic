#pragma once

#ifdef _MSC_VER
#include "Common/Common.h"
#endif

#include "ControllerDef.h"

class IPlayerView
#ifdef _MSC_VER
	: public IView
#endif
{
public:
	virtual IModelObserver& getModelObserver() = 0;

	virtual bool showMsgBox(const wstring& strMsg, bool bWarning = false)
	{
		(void)strMsg;
		(void)bWarning;
		return false;
	}

	virtual void updateMediaRelated(const tagMediaSetChanged& MediaSetChanged)
	{
		(void)MediaSetChanged;
	}
};

#ifdef __ViewPrj
#define __ViewExt __dllexport
#else
#define __ViewExt __dllimport
#endif

__ViewExt IPlayerView& genView(IPlayerController& controller, IModel& model);
