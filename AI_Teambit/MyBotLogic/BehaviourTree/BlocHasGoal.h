#ifndef BLOCHASGOAL_H
#define BLOCHASGOAL_H

#include "GeneralAction.h"
#include "..\MiCoMa.h"
#include "..\Npc.h"


/* Sets an objective to each Npc.
If no goal exists, he gets in the exploration mode. */
BehaviourTree::BaseBloc* getBlocHasGoal(BlackBoard &bboard)
{
    auto forHasGoalLambda = [&bboard]()
    {
        std::map<unsigned int, unsigned int> goalMap = bboard.getGoalMap();
		auto npcs = MiCoMa::getInstance()->getNpcs();
        for (Npc* curNpc : npcs)
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
