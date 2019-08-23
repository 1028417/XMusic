#pragma once

#include "controller.h"

class CPlayerController : public CController, public IController
{
public:
	CPlayerController(IPlayerView& view, IModel& model)
		: CController(view, model)
	{
	}

public:
	bool init() override;

	bool start() override;

	void stop() override;

	bool handleCommand(UINT uID) override;
};
