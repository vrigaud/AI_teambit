#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "..\MiCoMa.h"
#include <map>
#include "..\Singleton.h"

class BlackBoard : public Singleton
{

private:

    std::map<unsigned int, unsigned int> mGoalMap;
    std::vector<Action*> mActionList;
    TurnInfo mTurnInfo;

public:
    
    BlackBoard() : mGoalMap{} {};

    static BlackBoard* getInstance() noexcept
    {
        static BlackBoard mInstance;
        return &mInstance;
    }

    void update(const TurnInfo &turnInfo)
    {
        mTurnInfo = turnInfo;
        mActionList.clear();
    }

    void setGoalMap(std::map<unsigned int, unsigned int> newGoalMap)
    {
        mGoalMap = newGoalMap;
    }
    std::map<unsigned int, unsigned int> getGoalMap() const noexcept
    {
        return mGoalMap;
    }
    void setActionList(std::vector<Action*>& newActionList)
    {
        mActionList = newActionList; 
    }
    std::vector<Action*> getActionList() const noexcept
    {
        return mActionList;
    }
    TurnInfo getTurnInfo() const noexcept
    {
        return mTurnInfo;
    }
};


#endif //!BLACKBOARD_H