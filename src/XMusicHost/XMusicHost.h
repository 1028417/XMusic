#pragma once

#include "controller.h"

class CController : public CPlayerController, public IController
{
public:
	CController(IPlayerView& view, IModel& model)
		: CPlayerController(view, model)
	{
	}

public:
	bool init() override;

	bool start() override;

	void stop() override;

	bool handleCommand(UINT uID) override;
};
