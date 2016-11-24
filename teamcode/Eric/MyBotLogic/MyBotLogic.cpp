#include "MyBotLogic.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "Globals.h"
#include "graph.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <thread>
#include <xtree>
#include <stdlib.h>

MyBotLogic::MyBotLogic()
{
}

/*virtual*/ MyBotLogic::~MyBotLogic()
{
}

/*virtual*/ void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
#ifdef BOT_LOGIC_DEBUG
    mLogger.Init(_logpath, "MyBotLogic.log");
#endif

    BOT_LOGIC_LOG(mLogger, "Configure", true);
}

/*virtual*/ void MyBotLogic::Load()
{
	//Write Code Here COMMIT
#ifdef BOT_LOGIC_DEBUG
	std::chrono::seconds sec(5);
	std::this_thread::sleep_for(sec);
#endif
}

/*virtual*/ void MyBotLogic::Init(LevelInfo& _levelInfo)
{
    //Write Code Here
    m_levelInfo = _levelInfo;
}

/*virtual*/ void MyBotLogic::OnBotInitialized()
{
}

/*virtual*/ void MyBotLogic::OnGameStarted()
{
}

/*virtual*/ void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
	m_graph.updateGraph(_turnInfo, m_goal, m_occupiedTiles, m_levelInfo, m_graph);

	/* ENVOYER LES COMMANDES*/
	for (std::map<unsigned int, NPCInfo>::iterator it = _turnInfo.npcs.begin();it != _turnInfo.npcs.end();++it)
	{
		TileInfo pos = _turnInfo.tiles.at(it->second.tileID);
		if (pos.tileAttributes.count(ETileAttribute::TileAttribute_Target) == 0)
		{
			unsigned int firstNode = it->second.tileID;
			/* CALCULER CHEMIN*/
			std::vector<EDirection> path;
			if (m_goal.size())
			{
				unsigned int closest = closestTarget(firstNode);
				path = m_graph.bestPath(firstNode, closest, _turnInfo, it->first);
			}
			else {
				int rnd = rand();
				path.push_back(EDirection::NE);
			}
			if(path.size() > 0)
			{
				Action* action = new Move(it->second.npcID, path.front());
				_actionList.push_back(action);
			}
		}
	}
}

unsigned int MyBotLogic::closestTarget(unsigned int pos)
{
	unsigned int minLength = m_graph.length(pos, m_goal.at(0));
	unsigned int closest = m_goal.at(0);
	for (auto it : m_goal)
	{
		unsigned int newLength = m_graph.length(pos, it);
		if (newLength <= minLength)
		{
			minLength = newLength;
			closest = it;
		}
	}
	return closest;
}

/*virtual*/ void MyBotLogic::Exit()
{
}

