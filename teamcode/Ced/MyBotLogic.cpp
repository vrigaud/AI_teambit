#include "MyBotLogic.h"
#include "Graph.h"
#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"
#include "NPC.h"
#include "NPCInfo.h"
#include "TurnInfo.h"

#include <cassert>
#include "windows.h"

MyBotLogic::MyBotLogic()
	: mNPCs{}
{
}

 MyBotLogic::~MyBotLogic()
{
	for (NPC* npc : mNPCs)
		delete npc;
}

 void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);


	//Write Code Here
}

 void MyBotLogic::Load()
{
	//Write Code Here
}

 void MyBotLogic::Init(LevelInfo& _levelInfo)
{
	//Sleep(8000);

	Graph::Instance().init(_levelInfo);
	initNpcs();
}

 void MyBotLogic::OnBotInitialized()
{
	//Write Code Here
}

 void MyBotLogic::OnGameStarted()
{
	//Write Code Here
}

 void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{

	updateTurn(_turnInfo);

	for (auto& npc : mNPCs)
	{
		npc->update(_actionList);
	}
}

 void MyBotLogic::Exit()
{
	//Write Code Here
}


MyBotLogic::distance_id_pair_type MyBotLogic::findNearestTargetsByNPC(const Graph& graph, const NPCInfo& npc)
{
	//Set new heuristic with node corresponding to NPC's tile
	Heuristic h{ graph.getNode(npc.tileID) };
	distance_id_pair_type distancesToTargets;
	for (Node* target : Graph::Instance().getTargetList())
	{
		distancesToTargets.emplace(std::make_pair(h.estimate(target, graph.getLevelInfo()), target->getID()));
	}

	return distancesToTargets;
}

void MyBotLogic::initNpcs()
{
	for (auto& npcInfoPair : Graph::Instance().getLevelInfo().npcs)
	{
		mNPCs.emplace_back(new NPC{ npcInfoPair.second });
	}
}

void MyBotLogic::updateTurn(TurnInfo& turnInfo)
{
	int i{ 0 };
	for (auto& npcInfoPair : turnInfo.npcs)
	{
		mNPCs[i]->updateInfo(npcInfoPair.second);
		++i;
	}

	Graph::Instance().updateMapInfo(turnInfo);
	Graph::Instance().updateNpcGoal(mNPCs);

}
