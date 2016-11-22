#include "MiCoMa.h"

#include "LoggerPath.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "Globals.h"
#include "Npc.h"

void MiCoMa::init(const LevelInfo& levelInfos)
{
#ifdef BOT_LOGIC_DEBUG_MICOMA
    mLogger.Init(LoggerPath::getInstance()->getPath(), "MiCoMa.log");
#endif

    BOT_LOGIC_MICOMA_LOG(mLogger, "Configure", true);
    BOT_LOGIC_MICOMA_LOG(mLogger, "MiCoMa", true);

    for (auto npc : levelInfos.npcs)
    {
        mNpcs.push_back(new Npc(npc.second.npcID, npc.second.tileID));
    }
}

void MiCoMa::update(const TurnInfo& turnInfo, std::vector<Action*>& _actionList)
{
	//***NOTE : 
    // ATM default npc state is exploring so just change objective when we've got
    // a goal for the npc.

    std::map<unsigned int, unsigned int> goalMap;
    std::vector<unsigned int> targetList = Map::getInstance()->getGoalIDs();
    if (targetList.size() > turnInfo.npcs.size())
    {
        goalMap = findBestGoalByNpc(turnInfo.npcs, targetList);
    }
    else
    {
        goalMap = findBestGoalByGoal(turnInfo.npcs, targetList);
    }

	//If not enough or no goals, someone needs to explore
	if (goalMap.size() < mNpcs.size())
	{
		Map::getInstance()->createInfluenceMap();
		Map::getInstance()->logInfluenceMap(turnInfo.turnNb);
	}

    for (Npc* curNpc : mNpcs)
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

    for (Npc* npc : mNpcs)
    {
        npc->update();
        if (npc->getAction())
        {
            _actionList.push_back(npc->forwardAction()->Clone());
        }
    }
}

std::map<unsigned int, unsigned int> MiCoMa::findBestGoalByNpc(const std::map<unsigned int, NPCInfo>& npcInfo, std::vector<unsigned int>& targetList)
{
    std::map<unsigned int, unsigned int> goalMap{};
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
        goalMap[npc.second.npcID] = goalId;
        targetList.erase(std::find(begin(targetList), end(targetList), goalId));
    }
    return goalMap;
}

std::map<unsigned int, unsigned int> MiCoMa::findBestGoalByGoal(std::map<unsigned int, NPCInfo> npcInfo, const std::vector<unsigned int>& targetList)
{
    std::map<unsigned int, unsigned int> goalMap{};
    for (unsigned goal : targetList)
    {
        if (npcInfo.size() <= 0)
        {
            break;
        }
        int bestDist = 666;
        int npcId = -1;
        for (std::pair<unsigned, NPCInfo> npc : npcInfo)
        {
            float distance = Map::getInstance()->calculateDistance(npc.second.tileID, goal);
            if (distance < bestDist)
            {
                npcId = npc.second.npcID;
                bestDist = distance;
            }
        }
        goalMap[npcId] = goal;
        npcInfo.erase(npcId);
    }
    return goalMap;
}
