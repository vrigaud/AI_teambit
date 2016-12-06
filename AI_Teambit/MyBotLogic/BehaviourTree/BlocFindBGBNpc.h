#ifndef BLOCFINDBGBNPC_H
#define BLOCFINDBGBNPC_H

#include "GeneralAction.h"

/* Finds best goal for each Npc, by looking in the Npcs list */
BehaviourTree::BaseBloc* getBlocFindBGBNpc(BlackBoard &bboard)
{
    auto findBGBNpcLambda = [&bboard]()
    {
        std::vector<unsigned int> targetList = Map::getInstance()->getGoalIDs();
        std::map<unsigned int, NPCInfo> npcInfo = bboard.getTurnInfo().npcs;
        std::map<unsigned int, unsigned int> tempGoalMap{};

        if (!(targetList.size() > npcInfo.size()))
        {
            return BehaviourTree::FAIL;
        }

        for (std::pair<unsigned, NPCInfo> npc : npcInfo)
        {
            int bestDist = 666;
            unsigned goalId = -1;
            std::vector<unsigned>::iterator goalIt = begin(targetList);
            for (; goalIt != end(targetList); ++goalIt)
            {
                float distance = Map::getInstance()->calculateDistance(npc.second.tileID, *goalIt);
                if (distance < bestDist)
                {
                    goalId = *goalIt;
                    bestDist = distance;
                }
            }
            tempGoalMap[npc.second.npcID] = goalId;
            targetList.erase(std::find(begin(targetList), end(targetList), goalId));
        }
		bboard.setGoalMap(tempGoalMap);

        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(findBGBNpcLambda);
}

#endif //!BLOCFINDBGBNPC_H
