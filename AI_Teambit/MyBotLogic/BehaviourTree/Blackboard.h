#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "..\Npc.h"
#include "TurnInfo.h"

#include <map>
#include <vector>
#include <stack>

struct Action;
struct LevelInfo;

class BlackBoard
{

private:

	std::map<unsigned int, unsigned int> mGoalMap;
	std::vector<Action*> mActionList;
    Npc* mScout;
    Npc* mPpNpc; // npc to go onto pressure plate


	TurnInfo mTurnInfo;

public:
    
    std::stack<unsigned int> mZoneIdRecursion; //Zone's id of the recursion

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

    Npc* getScout() const 
    { 
        return mScout;
    }

    void setScout(Npc* val) 
    {
        mScout = val; 
    }

    Npc* getPpNpc() const 
    {
        return mPpNpc; 
    }

    void setPpNpc(Npc* val)
    {
        mPpNpc = val; 
    }
};


#endif //!BLACKBOARD_H