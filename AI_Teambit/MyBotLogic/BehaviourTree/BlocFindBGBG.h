#ifndef BLOCFINDBGBG_H
#define BLOCFINDBGBG_H

#include "GeneralAction.h"
#include "..\Map.h"

/* Finds best goal for each Npc, by looking in targets list*/
BehaviourTree::BaseBloc* getBlocFindBGBG(BlackBoard &bboard)
{
	auto findBGBGLambda = [&bboard]()
    {
        std::vector<unsigned int> targetList = Map::getInstance()->getGoalIDs();
		std::map<unsigned int, NPCInfo> npcInfo = bboard.getTurnInfo().npcs;
        std::map<unsigned int, unsigned int> tempGoalMap{};

        for (unsigned goal : targetList)
        {
            if (npcInfo.size() <= 0)
            {
                break;
            }
            int bestDist = 666;
            int npcId = -1;for (std::pair<unsigned, NPCInfo> npc : npcInfo)
            {
                float distance = Map::getInstance()->calculateDistance(npc.second.tileID, goal);
                if (distance < bestDist)
                {
                    npcId = npc.second.npcID;
                    bestDist = distance;
                }
            }
            tempGoalMap[npcId] = goal;
            npcInfo.erase(npcId);
        }

		bboard.setGoalMap(tempGoalMap);
        return BehaviourTree::result::SUCCESS;
    };
    return BehaviourTree::createAction(findBGBGLambda);
}

#endif //!BLOCFINDBGBG_H

