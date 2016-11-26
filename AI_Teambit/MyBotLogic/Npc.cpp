#include "Npc.h"
#include "LoggerPath.h"

#include "Map.h"

#include <set>
#include <vector>
#include <list>
#include "MiCoMa.h"

using namespace std;

Npc::Npc(unsigned int a_id, unsigned int a_tileId)
	: mCurrentState{}, mNextState{}, mId{ a_id }, mCurrentTile{ a_tileId }, mTurnCount {
	0
}, mPath{ a_tileId },
	mHasGoal{}, mAction{}
{
#ifdef BOT_LOGIC_DEBUG_NPC
	mLogger.Init(LoggerPath::getInstance()->getPath(), "Npc_" + std::to_string(mId) + ".log");
#endif

	BOT_LOGIC_NPC_LOG(mLogger, "Configure\n", true);

};

void Npc::update()
{
    BOT_LOGIC_NPC_LOG(mLogger, "TURN #" + std::to_string(++mTurnCount), true);
	updateState();
	enterStateMachine();
}

void Npc::setGoal(const unsigned int goalID)
{
	mGoal = goalID;
	mHasGoal = true;
	setObjective(Objective::GO_TO, mGoal);
	BOT_LOGIC_NPC_LOG(mLogger, "GoalID : " + std::to_string(mGoal) + "\n", true);
	searchPath();
}

void Npc::updateState()
{
	// Atm it's an if/ elseif cause we dont really know yet what impact the choice...
	// care at the order for more efficiency
	if (mObjective.mType == Objective::NONE)
	{
		mNextState = WAITING;
	}
	else if (mObjective.mType == Objective::SEARCH_MAP)
	{
		mNextState = EXPLORE_MAP;
	}
	else if (mObjective.mType == Objective::GO_TO)
	{
		mNextState = SEARCH_PATH;
	}
	else if (mObjective.mType == Objective::SEARCH_H_DOOR)
	{
		mNextState = EXPLORE_H_DOOR;
	}
}

void Npc::enterStateMachine()
{
	BOT_LOGIC_NPC_LOG(mLogger, "Enter state machine ->", false);
	do {
		mCurrentState = mNextState;
		switch (mCurrentState & STATE_LEVEL_0)
		{
		case EXPLORING:
			exploring();
			break;
		case MOVING:
			moving();
			break;
		case WAITING:
			waiting();
			break;
		}
	} while (mCurrentState != mNextState);
	BOT_LOGIC_NPC_LOG(mLogger, "", true);
}

void Npc::exploring()
{
	BOT_LOGIC_NPC_LOG(mLogger, " Exploring", true);
	do
	{
		mCurrentState = mNextState;
		switch (mCurrentState)
		{
		case EXPLORE_MAP:
			exploreMap();
			break;
		case EXPLORE_H_DOOR:
			exploreHiddenDoor();
			break;
		case EXPLORE_DNPC:
			exploreDNpc();
			break;
		case EXPLORE_WAITING:
			exploreWaiting();
			break;
		case MOVE:
			move();
			break;
		}
	} while (mCurrentState != mNextState);
}

void Npc::moving()
{
	BOT_LOGIC_NPC_LOG(mLogger, " Moving", true);

	do
	{
		mCurrentState = mNextState;
		switch (mCurrentState)
		{
		case SEARCH_PATH:
			searchPath();
			break;
		case FOLLOW_PATH:
			followPath();
			break;
		case MOVING_DNPC:
			movingDNpc();
			break;
		case MOVING_WAITING:
			movingWaiting();
			break;
		case ARRIVED:
			arrived();
			break;
		}
	} while (mCurrentState != mNextState);
}

// You just have to wait..
void Npc::waiting()
{
	BOT_LOGIC_NPC_LOG(mLogger, " Waiting", true);

	mCurrentState = WAITING;
	mNextState = WAITING;
}


// Exploring fsm function
void Npc::exploreMap()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-ExploreMap", true);

	std::vector<unsigned int> v = Map::getInstance()->getCloseMostInfluenteTile(mPath.back());
	if (v.empty())
	{
		throw ThisIsNoGoodAtAllMiFriend{};
	}

	mPath = { v[0], mPath.back() };

	//Logger
	DisplayVector("\tExploration path : ", mPath);

	mNextState = EXPLORE_DNPC;
}

void Npc::exploreHiddenDoor()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-ExploreHiddenDoor", true);
	// TODO - pull influence tile

	mNextState = EXPLORE_H_DOOR;
}

// Wait cause someone want to go on the same tile
inline void Npc::exploreWaiting()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-ExploreWaiting", true);

	mCurrentState = EXPLORE_WAITING;
	mNextState = EXPLORE_WAITING;
}

void Npc::exploreDNpc()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-ExploreDNpc", true);
	mNextState = MOVE;

	const std::vector<Npc*> npcList = MiCoMa::getInstance()->getNpcs();
	const Map *map = Map::getInstance();

	for (Npc* npc : npcList)
	{
		if (npc == this)
		{
			continue;
		}

		if (getNextStepTile() == npc->getCurrentTile())
		{
			mNextState = EXPLORE_WAITING;
			break;
		}
	}
}

void Npc::move()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-Move", true);
	// TODO - just push action in the list?

	mAction = new Move(mId, Map::getInstance()->getDirection(mPath.back(), mPath[mPath.size() > 1 ? mPath.size() - 2 : 0]));
	if(mPath.size())
		mPath.pop_back();

	mNextState = MOVE;
}


// Moving fsm function
void Npc::searchPath()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-SearchPath", true);

	// TODO - update path if needed (TC_40 : Maze)
	if (mPath.size() == 1)
	{
		unsigned int currentTileID = getCurrentTile();
		aStar(mPath.back(), mObjective.mTileId);

		//No path found
		if (mPath.empty())
		{
			mPath.emplace_back(currentTileID);
		}
	}
    else
    { // Path existing, just check if it`s still good path
        updatePath();
    }

	DisplayVector("\tPath to Objective Tile -> ", mPath);

	mNextState = MOVING_DNPC;
}

void Npc::followPath()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-FollowPath", true);

	if (mPath.size() > 1)
	{

		EDirection direction = Map::getInstance()->getDirection(mPath.back(), mPath[mPath.size() > 1 ? mPath.size() - 2 : 0]);

		mAction = new Move(mId, direction);

		//TODO: EM: A DISCUTER. La tuile courrante devient celle apres la commande MOVE est envoyee?
		//unsigned int newTile = Map::getInstance()->getTileNeighborIndex(mCurrentTile, direction);
		//mCurrentTile = newTile;

		mPath.pop_back();

		mNextState = FOLLOW_PATH;
	

		if (mPath.back() == mGoal)
		{
			mNextState = ARRIVED;
		}

	}
}

void Npc::movingDNpc()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-MovingDNpc", true);
	mNextState = FOLLOW_PATH;

	const std::vector<Npc*> npcList = MiCoMa::getInstance()->getNpcs();
	const Map *map = Map::getInstance();

	unsigned int myDistanceToTarget = mPath.size();
	for (Npc* npc : npcList)
	{
		if (npc == this)
		{
			continue;
		}

		if (isBlockedByNpc(npc))
		{
			if (myDistanceToTarget < npc->getPath().size())
			{
				mNextState = MOVING_WAITING;
				break;
			}
			if (hasShorterPath(npc))
			{
				mNextState = MOVING_WAITING;
				break;
			}

			// We have no reason to stop
			mNextState = FOLLOW_PATH;
		}
	}
}

inline void Npc::movingWaiting()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-MovingWaiting", true);
	mCurrentState = MOVING_WAITING;
	mNextState = MOVING_WAITING;
}

inline void Npc::arrived()
{
	BOT_LOGIC_NPC_LOG(mLogger, "-Arrived", true);
	mCurrentState = ARRIVED;
	setObjective(Objective::NONE);
	mNextState = ARRIVED;
}

void Npc::aStar(unsigned int startNodeId, unsigned int goalNodeId)
{
	BOT_LOGIC_NPC_LOG(mLogger, "- aStar", true);

	//***v2.0 : NodeRecord version
	Map* map = Map::getInstance();
	Node* start{ Map::getInstance()->getNode(startNodeId) };
	Node* goal{ Map::getInstance()->getNode(goalNodeId) };
	mPath.clear();

	//Init record
	NodeRecord* currentRecord = new NodeRecord{ start, 0, map->calculateDistance(startNodeId, goalNodeId) };

	//Init lists
	std::list<NodeRecord*> openedList{}, closedList{};
	openedList.emplace_back(currentRecord);

	NodeRecord* prevRecord;
	NodeRecord* endNodeRecord;
	while (!openedList.empty())
	{
		//Get smallest element
        currentRecord = *std::min_element(std::begin(openedList),
            std::end(openedList), 
            [](const NodeRecord *a, const NodeRecord *b) {  return a->mEstimatedTotalCost < b->mEstimatedTotalCost;});
		Node* currentNode{ currentRecord->mNode };

		//Found goal - yay!
		if (currentNode == goal)
			break;

		//If not, check neighbours to find smallest cost step
		for (size_t i{}; i < Node::NBNEIGHBOURS; ++i)
		{
			Node* neighbour = currentNode->getNeighbour(static_cast<EDirection>(i));
			if (!neighbour || (neighbour->getType() == Node::FORBIDDEN)
				|| currentNode->isEdgeBlocked(static_cast<EDirection>(i)))
			{
				continue;
			}

			NodeRecord::cost_type endNodeCost = currentRecord->mCostSoFar + 1;
			NodeRecord::cost_type endNodeHeuristic{ 0 };

			//If closed node, may have to skip or remove it from closed list
			if ((endNodeRecord = NodeRecord::findIn(closedList, neighbour)))
			{
				//If not a shorter route, skip
				if (endNodeRecord->mCostSoFar <= endNodeCost)
					continue;

				//If shorter, we need to put it back in the opened list
				closedList.remove(endNodeRecord);

				//Put record back in opened list
				openedList.emplace_back(endNodeRecord);

				//Update heuristic
				endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
			}
			else if ((endNodeRecord = NodeRecord::findIn(openedList, neighbour)))
			{
				if (endNodeRecord->mCostSoFar <= endNodeCost)
					continue;

				endNodeHeuristic = endNodeRecord->mEstimatedTotalCost - endNodeRecord->mCostSoFar;
			}
			//Unvited node : need a new record
			else
			{
				endNodeRecord = new NodeRecord{ neighbour, 0, 0};

                endNodeHeuristic = map->calculateDistance(neighbour->getId(), goalNodeId);
				//Put record back in opened list
				openedList.emplace_back(endNodeRecord);
			}

			//Update record
			endNodeRecord->mCostSoFar = endNodeCost;
			endNodeRecord->mPrevious = currentRecord;
			endNodeRecord->mEstimatedTotalCost = endNodeCost + endNodeHeuristic;

		}
		//Update previous
		prevRecord = currentRecord;

		//Looked at all current node's neighbours : remove opened, put it in closed
		openedList.remove(currentRecord);
		closedList.emplace_back(currentRecord);
	}

	//Found our goal or run out of nodes
	std::list<unsigned int> finalPath{};
	if (currentRecord->mNode != goal)
		return;

	else
	{
		//Success
		while (currentRecord)
		{
			mPath.emplace_back(currentRecord->mNode->getId());
			currentRecord = currentRecord->mPrevious;
		}
	}


	//Cleanup
	for (NodeRecord *nr : openedList)
		delete nr;

	for (NodeRecord *nr : closedList)
		delete nr;

	//***v1.0 : K-E & Louis
	//TODO : update to make this work with v2.0
	/*
	//We need a clear path whether we fail or not
	mPath.clear();

	//declarations
	Node* current{ Map::getInstance()->getNode(startNodeId) };
	size_t maxNbNode = Map::getInstance()->getHeight() * Map::getInstance()->getWidth();

	multiset<Node*, NodeComparator> openedNodes{};
	vector<Node*> closedNodes{};

	closedNodes.reserve(maxNbNode);
	openedNodes.insert(current);

	while (!openedNodes.empty())
	{
		current = *begin(openedNodes);
		openedNodes.erase(current);

		//Success
		if (current->getId() == goalNodeId)
		{
			while (current)
			{
				mPath.emplace_back(current->getId());
				current = current->getParent();
			}
			break;
		}

		for (size_t i{}; i < Node::NBNEIGHBOURS; ++i)
		{
			Node* neighbour = current->getNeighbour(static_cast<EDirection>(i));
			if (!neighbour)
			{
				continue;
			}

			int transitionCost = current->getCost() + 1;
			Node* parent = current->getParent();
			//TODO : [OPTI] struct NodeRecord to store temporary parent
			if (neighbour->getCost() < 1)
			{
				continue;
			}

			auto testClosed = std::find(begin(closedNodes), end(closedNodes), neighbour);
			auto testOpened = std::find(begin(openedNodes), end(openedNodes), neighbour);
			if (
				testClosed != closedNodes.end() && ((*testClosed)->getCost() < transitionCost) ||
				testOpened != openedNodes.end() && ((*testOpened)->getCost() < transitionCost)
				)
			{
				continue;
			}

			neighbour->setParent(current);
			neighbour->setCost(transitionCost + current->getCost());
			neighbour->setHeuristic(neighbour->getCost() + Map::getInstance()->calculateDistance(neighbour->getId(), goalNodeId));

			openedNodes.emplace(neighbour);
		}
		closedNodes.emplace_back(current);
	}
	*/
}

void Npc::updatePath()
{
    BOT_LOGIC_NPC_LOG(mLogger, "\tUpdating Path ", true);
    DisplayVector("\t\tOld path: ", mPath);

    Map* pMap = Map::getInstance();
    unsigned int oldTileID = mPath.front();
    for (unsigned int tileId : mPath)
    {
        if (!pMap->canMoveOnTile(oldTileID, tileId))
        {
			aStar(getCurrentTile(), mObjective.mTileId);
            DisplayVector("\t\tPath Updated : ", mPath);
            return;
        }
        oldTileID = tileId;
    }
    BOT_LOGIC_NPC_LOG(mLogger, "\t\tNo update needed", true);
}

bool Npc::isBlockedByNpc(Npc* npc)
{
	return getNextStepTile() == npc->getNextStepTile();
}

bool Npc::hasShorterPath(Npc* npc)
{
	return getID() < npc->getID()
		&& mPath.size() == npc->getPath().size();
}


//-------------------------------------------------------------------------------------------
// DEbug Functions
//-------------------------------------------------------------------------------------------
template<class T>
void Npc::DisplayVector(std::string info, const std::vector<T> v)
{
	std::string s{ "" };
	for (T u : v)
	{
		s += std::to_string(u) + " ";
	}
	BOT_LOGIC_NPC_LOG(mLogger, info + s, true);
}
