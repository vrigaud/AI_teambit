/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __MYBOT__
#define __MYBOT__

#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/Paths.h"

#include "TurnInfo.h"
#include "NPCInfo.h"

#include <unordered_map>
#include <vector>

/*
	This class represents the THINK and ACT aspects of a npc
*/
struct MyBot
{
private:
	Paths myPaths; //Paths to the targets

	static std::unordered_map<Node::id_value_type, unsigned int> nodesVisit; //Semi-heatmap
	static unsigned int maxNodesVisit;
	static unsigned int minNodesVisit;
	
	EDirection getRandomDirection() const;
	bool findNextBestTarget();
	void visitedNode(Node::id_value_type, bool isTarget = false);
	static std::vector<unsigned int> getLeastVisitedNeighbours(Node::id_value_type,TurnInfo&);
public:

	//State machine states
	enum State
	{
		TurnStart,			//State machine loop entry point
		IHaveTargets,		//The npc currently knows of targets on the map
		IHaveNoTargets,		//The npcs does not know about any target
		NeedAnotherTarget,	//A npc has the same target as another
		IHaveSpecificTarget,//The npc will absolutely move towards a target
		SpecificMove,		//The npc will fill the action list with a move towards its target
		RandomMove,			//The npc will fill the action list with a semi-random move
		ReachedTarget,		//The npc reached its target
		TurnEnd				//State machine loop exit point
	};

	State currentState;
	State nextState;

	MyBot(unsigned int, Node::id_value_type);

	Node::id_value_type currentTargetTileId;
	Node::id_value_type nextTileId;
	unsigned int npcID;

	static void evalState(TurnInfo&, std::vector<Action*>&);
	bool hasTarget() const;
	Node::id_value_type getNextNodeToCurrentTarget();
	void addNodeToTargetPath(Node::id_value_type, Node::id_value_type);
	void clearNodesToTargets();
	
	// A*
	bool findTargets(TurnInfo&);
};

#endif // __MYBOT__
