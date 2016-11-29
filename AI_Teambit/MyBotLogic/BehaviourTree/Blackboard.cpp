#include "Blackboard.h"

#include "Globals.h"
#include "TurnInfo.h"
#include "LevelInfo.h"

void BlackBoard::init(const LevelInfo &levelInfo)
{
	mActionList.reserve(levelInfo.npcs.size());
}

void BlackBoard::update(const TurnInfo &turnInfo)
{
	mTurnInfo = turnInfo;
	mActionList.clear();
}

void BlackBoard::pushAction(Action* newAction)
{
	mActionList.emplace_back(newAction);
}
std::vector<Action*> BlackBoard::getActionList() const noexcept
{
	return mActionList;
}
TurnInfo BlackBoard::getTurnInfo() const noexcept
{
	return mTurnInfo;
}