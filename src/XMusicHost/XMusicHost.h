#pragma once

#include "../controller.h"

class CController : public CXController, public IController
{
public:
	CController(IPlayerView& view, IModel& model)
		: CXController(view, model)
	{
		fsutil::setWorkDir(fsutil::getModuleDir());
	}

public:
	bool init() override;

	void start() override;

	void stop() override;

	bool handleCommand(UINT uID) override;
};
