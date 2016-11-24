#ifndef BLOCHASGOAL_H
#define BLOCHASGOAL_H

#include "GeneralAction.h"
#include "..\Npc.h"
#include "..\MiCoMa.h"
#include "Blackboard.h"

/* Sets an objective to each Npc.
If no goal exists, he gets in the exploration mode. */
BehaviourTree::BaseBloc* getBlocHasGoal()
{
    auto forHasGoalLambda = []()
    {
        std::map<unsigned int, unsigned int> goalMap = BlackBoard::getInstance()->getGoalMap();
        for (Npc* curNpc : MiCoMa::getInstance()->getNpcs())
        {
            if (!curNpc->hasGoal())
            {
                if (goalMap.find(curNpc->getID()) != end(goalMap))
                {
                    unsigned int goalTile = goalMap[curNpc->getID()];
                    curNpc->setGoal(goalTile);
                }
                else
                {
                    curNpc->setObjective(Objective::SEARCH_MAP);
                }
            }
        }
        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(forHasGoalLambda);
}

#endif //!BLOCHASGOAL_H
