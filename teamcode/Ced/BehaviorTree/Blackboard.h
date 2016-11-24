#pragma once

#include "Globals.h"
#include "MyBotLogic/NPC.h"

class Blackboard
{
	std::vector<Action*> mActionList;

public:
	Blackboard() {}


	void setActionList(std::vector<Action*> actionList)
	{
		setActionList(actionList);
	}
	std::vector<Action*> getActionList() const { return mActionList; }
};