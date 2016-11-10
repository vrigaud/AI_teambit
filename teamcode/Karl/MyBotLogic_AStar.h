/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __MYBOTLOGIC_ASTAR_C__
#define __MYBOTLOGIC_ASTAR_C__

#include "MyBotLogic\MyBotLogic.h"
#include "../MyClasses/AStarState/Node.h"

/*
	Child class of MyBotLogic implementing the A* algorithm for decision making...
*/
class MyBotLogic_AStar : public MyBotLogic
{
	virtual void FillActionList(TurnInfo&, std::vector<Action*>&);
};

#endif // __MYBOTLOGIC_ASTAR_C__
