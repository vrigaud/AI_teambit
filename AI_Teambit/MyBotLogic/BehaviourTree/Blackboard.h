#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <map>
#include <vector>
#include "TurnInfo.h"

struct Action;
struct LevelInfo;

class BlackBoard
{

private:

	std::map<unsigned int, unsigned int> mGoalMap;
	std::vector<Action*> mActionList;
	TurnInfo mTurnInfo;

public:

	BlackBoard()
		: mGoalMap{}, mActionList{}
	{}

	void init(const LevelInfo &levelInfo);
	void update(const TurnInfo &turnInfo);

    void setGoalMap(std::map<unsigned int, unsigned int> newGoalMap)
    {
        mGoalMap = newGoalMap;
    }
    std::map<unsigned int, unsigned int> getGoalMap() const noexcept
    {
        return mGoalMap;
    }
	void pushAction(Action* newAction);
	std::vector<Action*> getActionList() const noexcept;
	TurnInfo getTurnInfo() const noexcept;
};


#endif //!BLACKBOARD_H