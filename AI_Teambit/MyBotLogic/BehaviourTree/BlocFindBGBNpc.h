#ifndef BLOCFINDBGBNPC_H
#define BLOCFINDBGBNPC_H

#include "GeneralAction.h"
#include "NPCInfo.h"
#include "TurnInfo.h"
#include "Blackboard.h"
#include "..\Map.h"

/* Finds best goal for each Npc, by looking in the Npcs list */
BehaviourTree::BaseBloc* getBlocFindBGBNpc()
{


    auto findBGBNpcLambda = []()
    {
		Map *map = Map::getInstance();
        std::vector<unsigned int> targetList = map->getGoalIDs();
        std::map<unsigned int, NPCInfo> npcInfo = BlackBoard::getInstance()->getTurnInfo().npcs;
        std::map<unsigned int, unsigned int> tempGoalMap{};

        if (!(targetList.size() > npcInfo.size()))
        {
            return BehaviourTree::result::FAIL;
        }

        for (std::pair<unsigned, NPCInfo> npc : npcInfo)
        {
            int bestDist = 666;
            unsigned goalId = -1;
            std::vector<unsigned>::iterator goalIt = begin(targetList);
            for (; goalIt != end(targetList); ++goalIt)
            {
				//If npc & goal are in the same zone
				if (map->getNode(goalId)->getZoneID() == map->getNode(npc.second.tileID)->getZoneID())
				{
					float distance = map->calculateDistance(npc.second.tileID, *goalIt);
					if (distance < bestDist)
					{
						goalId = *goalIt;
						bestDist = distance;
					}
				}
            }
            tempGoalMap[npc.second.npcID] = goalId;
            targetList.erase(std::find(begin(targetList), end(targetList), goalId));
        }
        BlackBoard::getInstance()->setGoalMap(tempGoalMap);

        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(findBGBNpcLambda);
}

#endif //!BLOCFINDBGBNPC_H
