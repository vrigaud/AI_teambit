/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#include "../MyClasses/AStarState/MyBotLogic_AStar.h"
#include "../MyClasses/AStarState/Graph.h"

void MyBotLogic_AStar::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
	//We get the (visible) targets
	Graph::getInstance().findVisibleTargets(_turnInfo);

	//We get the (visible) bots
	Graph::getInstance().refreshBots(_turnInfo);

	//We get the (visible) objects
	Graph::getInstance().refreshVisibleInfo(_turnInfo);

	//Entry for our state machine
	MyBot::evalState(_turnInfo, _actionList);
}
