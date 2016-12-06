#ifndef BLOCFINDBGBG_H
#define BLOCFINDBGBG_H

#include "GeneralAction.h"
#include "..\Map.h"

/* Finds best goal for each Npc, by looking in targets list*/
BehaviourTree::BaseBloc* getBlocFindBGBG(BlackBoard &bboard)
{
	auto findBGBGLambda = [&bboard]()
    {
        MiCoMa* ourMicoma = MiCoMa::getInstance();
        Map* ourMap = Map::getInstance();
        std::vector<unsigned int> targetList = Map::getInstance()->getGoalIDs();
        std::vector<Npc*> ourNpcs = ourMicoma->getNpcs();

        std::map<unsigned, unsigned> tempGoalMap{};
        for (unsigned goal : targetList)
        {
            if (ourNpcs.size() <= 0)
            { 
                break;
            }

            bool alreadyAssign = false;
            for (Npc* npc : ourNpcs)
            {// Check if goal is already assign
                if (npc->hasGoal() && npc->getGoal() == goal)
                {
                    alreadyAssign = true;
                    break;
                }
            }
            if (alreadyAssign)
            {
                continue;
            }

            int bestDist = 666;
            int npcId = -1;
            for (Npc* npc : ourNpcs)
            {
                int distance = Map::getInstance()->calculateDistance(npc->getCurrentTile(), goal);
                if (distance < bestDist && !npc->hasGoal())
                {
                    npcId = npc->getID();
                    bestDist = distance;
                }
            }
            tempGoalMap[npcId] = goal;
            ourNpcs.erase(find_if(begin(ourNpcs), end(ourNpcs), [npcId](Npc* npc) {return npc->getID() == npcId;}));
        }

		bboard.setGoalMap(tempGoalMap);
        return BehaviourTree::result::SUCCESS;
    };
    return BehaviourTree::createAction(findBGBGLambda);
}

#endif //!BLOCFINDBGBG_H

