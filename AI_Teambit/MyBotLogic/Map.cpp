#include "Map.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "NPCInfo.h"
#include "LoggerPath.h"
#include <Math.h>
#include <algorithm>

Map Map::mInstance;
unsigned int Map::mGreatestZoneID = 0;

void Map::setLoggerPath()
{
	std::string a_path = LoggerPath::getInstance()->getPath();
#ifdef BOT_LOGIC_DEBUG_MAP
	mLogger.Init(a_path, "Map.log");
	mLoggerInfluence.Init(a_path, "Map_Influence.log");
	mLoggerEdges.Init(a_path, "Map_Edges.log");
	mLoggerGalacticZones.Init(a_path, "Map_Zones.log");
#endif

	BOT_LOGIC_MAP_LOG(mLogger, "Configure", true);
	BOT_LOGIC_MAP_LOG(mLogger, " - - NONE | F - Forbidden | G - Goal | X - Occupied | P - Path | S - Pressure plate \n", true);

	BOT_LOGIC_MAP_LOG(mLoggerInfluence, "Configure : ", true);

	BOT_LOGIC_MAP_LOG(mLoggerEdges, "Configure", true);
	BOT_LOGIC_MAP_LOG(mLoggerEdges, " 0 - HighWall | 1 - Window | 2 - Door \n", true);
	BOT_LOGIC_MAP_LOG(mLoggerEdges, " 0 - N | 1 - NE | 2 - E | 3 - SE | 4 - S | 5 - SW | 6 - W | 7 - NW \n", true);

	BOT_LOGIC_MAP_LOG(mLoggerGalacticZones, "WELCOME TO THE GALACTIC ZONES LOGGER!\n", true);
	BOT_LOGIC_MAP_LOG(mLoggerEdges, "Configure", true);
	BOT_LOGIC_MAP_LOG(mLoggerGalacticZones, "First line : Tile IDs \nSecond line : Zone IDs", true);
}

void Map::initMap(const LevelInfo& levelInfo)
{
	setHeight(levelInfo.rowCount);
	setWidth(levelInfo.colCount);
	setInfluenceRange(std::max(levelInfo.visionRange + 2, 4));
	unsigned int countIndex = 0;
	for (int i = 0; i < mHeight; ++i)
	{
		for (int j = 0; j < mWidth; ++j)
		{
			createNode(new Node{ j, i, countIndex, Node::NONE });
			countIndex++;
		}
	}
	connectNodes();

	initZones(levelInfo.npcs);
	logZones(0);
}

void Map::initZones(std::map<unsigned int, NPCInfo> npcsInfo)
{
	for (auto npc : npcsInfo)
	{
		getNode(npc.second.tileID)->setZoneID(++mGreatestZoneID);
	}
}

// find influent tiles and set their base influence
void Map::createInfluenceMap(const InfluenceData::InfluenceType& aType)
{
	mInterestingNodes.clear();
	for (auto node : mKnownTilesAndVisitedStatus)
	{
		Node* myNode = getNode(node.first);
		myNode->setInfluence(0.0f);
		if (!node.second)
		{
			float tempInflu = 0.0f;

			for (int i = N; i <= NW; ++i)
			{
				EDirection dir = static_cast<EDirection>(i);
				EDirection invDir = inverseDirection(dir);
				EdgeData::EdgeType edgeType = myNode->getEdgeType(dir);
				Node* neighbour = myNode->getNeighbour(dir);
				EdgeData::EdgeType edgeNeibType = EdgeData::FREE;
				if (neighbour != nullptr)
				{
					edgeNeibType = neighbour->getEdgeType(invDir);
					if (neighbour->getType() == Node::NONE && !myNode->isEdgeBlocked(dir))
					{
						neighbour->setInfluence(1.0f);
						mInterestingNodes.push_back(neighbour);
					}
				}
				if (edgeType == EdgeData::WINDOW || edgeType == EdgeData::DOOR_W
					|| edgeNeibType == EdgeData::WINDOW || edgeNeibType == EdgeData::DOOR_W)
				{
					tempInflu += 1.0f;
				}
			}
			if (tempInflu > 0.0f)
			{
				myNode->setInfluence(tempInflu);
				mInterestingNodes.push_back(myNode);
			}
		}
	}

	std::sort(begin(mInterestingNodes), end(mInterestingNodes), [](const Node* a, const Node* b) {
		return a->getInfluence() > b->getInfluence();
	});
	propagateInfluence();

}

void Map::propagateInfluence()
{
	unsigned maxDist = getInfluenceRange();
	for (auto node : mInterestingNodes)
	{
		propagate(node, 0, maxDist, node->getInfluence());
	}
}

void Map::propagate(Node* myNode, unsigned curDist, unsigned maxDist, float initialInfluence) const
{
	if (curDist > maxDist)
	{
		return;
	}
	for (int i = N; i <= NW; ++i)
	{
		EDirection dir = static_cast<EDirection>(i);
		EDirection invDir = inverseDirection(dir);
		Node* tempNode = myNode->getNeighbour(dir);
		if (tempNode != nullptr && (!myNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
		{
			if (tempNode->getType() == Node::PATH)
			{
				auto newInfluence = myNode->getInfluence() - (initialInfluence / getInfluenceRange());
				if (newInfluence > tempNode->getInfluence())
				{
					tempNode->setInfluence(newInfluence);
				}
				propagate(tempNode, ++curDist, maxDist, initialInfluence);
			}
		}
	}
}

// return the first best influente tile return empty vector if no tile have influence
std::vector<unsigned int> Map::getCloseMostInfluenteTile(unsigned int tileId) const
{
	Node * current = getNode(tileId);
	std::vector<unsigned int> returnVector;

	float bestInflu = 0.0f;
	int bestTile = -1;
	//unsigned int counterTileMax{};
	//unsigned int counterTileBestInflu{};
	for (int i = N; i <= NW; ++i)
	{
		if (current->isEdgeBlocked(static_cast<EDirection>(i)))
		{ // No need to process inaccessible tile
			continue;
		}
		Node* neighboor = current->getNeighbour(static_cast<EDirection>(i));
		if (neighboor && neighboor->getType() != Node::FORBIDDEN)
		{ // neighboor is existing and autorized and accessible
			//++counterTileMax;
			float nodeinf = neighboor->getInfluence();

			if (bestInflu < nodeinf)
			{// Update new best infl
				bestInflu = nodeinf;
				bestTile = neighboor->getId();
				//++counterTileBestInflu;
			}
		}
	}
	if (bestTile >= 0)
	{
		returnVector.emplace_back(bestTile);
	}
	return returnVector;
}

void Map::updateMap(TurnInfo& turnInfo)
{
	//Edges need to be updated first to avoid adding unaccessible goal
	updateEdges(turnInfo);
	updateTiles(turnInfo);
	updateZones(turnInfo);

	//Log
	logZones(turnInfo.turnNb);

	// TODO - relocate in MiCoMa or Npc : up to you bruh
	createInfluenceMap();
	logInfluenceMap(turnInfo.turnNb);
}

void Map::updateEdges(TurnInfo& turnInfo)
{
	BOT_LOGIC_MAP_LOG(mLoggerEdges, "\nUpdate Edges", true);
	for (std::pair<unsigned, ObjectInfo> info : turnInfo.objects)
	{
		Node* node = getNode(info.second.tileID);
		for (int i = N; i <= NW; ++i)
		{
			ObjectInfo object = info.second;
			if (info.second.edgesCost[i] == 0)
			{
				auto typeHighWall = find(begin(object.objectTypes), end(object.objectTypes), ObjectType_HighWall);
				if (typeHighWall != end(object.objectTypes))
				{
					node->setEdgeType(static_cast<EDirection>(i), EdgeData::WALL);
					BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : WALL", true);
					continue;
				}

				auto typeWindow = find(begin(object.objectTypes), end(object.objectTypes), ObjectType_Window);
				auto typeDoor = find(begin(object.objectTypes), end(object.objectTypes), ObjectType_Door);
				if (typeWindow != end(object.objectTypes))
				{
					if (typeDoor != end(object.objectTypes))
					{
						BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : DOOR_W", false);
						node->setEdgeType(static_cast<EDirection>(i), EdgeData::DOOR_W);
						processDoorState(object, node, i);
					}
					else
					{
						BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : WINDOW", true);
						node->setEdgeType(static_cast<EDirection>(i), EdgeData::WINDOW);
					}
				}
				else if (typeDoor != end(object.objectTypes))
				{
					BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : DOOR", false);
					node->setEdgeType(static_cast<EDirection>(i), EdgeData::DOOR);
					processDoorState(object, node, i);
				}
			}
		}
	}
}

void Map::updateTiles(TurnInfo& turnInfo)
{
	for (auto info : turnInfo.tiles)
	{
		auto tileInfo = info.second;

		if (find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Forbidden) != tileInfo.tileAttributes.end())
		{
			setNodeType(tileInfo.tileID, Node::FORBIDDEN);

		}
		else if (find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Target) != tileInfo.tileAttributes.end())
		{
			setNodeType(tileInfo.tileID, Node::GOAL);
			addGoalTile(tileInfo.tileID);
			addSeenTile(tileInfo.tileID);
		}
		else if (find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_PressurePlate) != tileInfo.tileAttributes.end())
		{
			setNodeType(tileInfo.tileID, Node::PRESSURE_PLATE);
			addSeenTile(tileInfo.tileID);
		}
		else
		{
			setNodeType(tileInfo.tileID, Node::PATH);
			addSeenTile(tileInfo.tileID);
		}
	}
}

void Map::updateZones(TurnInfo& turnInfo)
{
	for (auto npc : turnInfo.npcs)
	{
		diffuseZone(npc.second.tileID);
	}
}

void Map::diffuseZone(const unsigned int startTileID)
{
	std::set<Node*, NodeZoneIDComparator> diffusionOpenNodes{};
	diffusionOpenNodes.emplace(getNode(startTileID));

	std::set<Node*>::iterator currentNodeIT{ diffusionOpenNodes.begin() };
	do
	{
		diffuseZoneRec((*currentNodeIT)->getZoneID(), (*currentNodeIT)->getId(), diffusionOpenNodes);
		currentNodeIT = diffusionOpenNodes.upper_bound(*currentNodeIT);

	} while (currentNodeIT != diffusionOpenNodes.end());
}

void Map::diffuseZoneRec(const unsigned int currentZoneID, const unsigned int startTileID, std::set<Node*, NodeZoneIDComparator>& diffusionOpenNodes)
{
	Node* currentNode = getNode(startTileID);

	for (int i = N; i <= NW; ++i)
	{
		Node* neighbour = currentNode->getNeighbour(static_cast<EDirection>(i));
		if (!neighbour)
		{
			continue;
		}

		unsigned int nZoneID{ neighbour->getZoneID() };
		if (nZoneID)
		{
			if (nZoneID == currentZoneID ||
				currentNode->isEdgeBlocked(static_cast<EDirection>(i)) ||
				neighbour->getType() == Node::FORBIDDEN ||
				neighbour->getType() == Node::NONE)
			{
				continue;
			}

			if (currentZoneID < nZoneID)
			{
				neighbour->setZoneID(currentZoneID);
				diffuseZoneRec(currentZoneID, neighbour->getId(), diffusionOpenNodes);
				continue;
			}
		}

		if (neighbour->getType() == Node::NONE ||
			neighbour->getType() == Node::FORBIDDEN)
		{
			++mGreatestZoneID;
		}
		else
		{
			if (currentNode->isEdgeBlocked(static_cast<EDirection>(i)))
			{
				++mGreatestZoneID;
				neighbour->setZoneID(mGreatestZoneID);
				diffusionOpenNodes.emplace(neighbour);
			}
			else
			{
				neighbour->setZoneID(currentZoneID);
				diffuseZoneRec(currentZoneID, neighbour->getId(), diffusionOpenNodes);
			}
		}

	}
}

void Map::processDoorState(ObjectInfo &object, Node* node, int i)
{
	auto openDoor = find(begin(object.objectStates), end(object.objectStates), ObjectState_Opened);
	if (openDoor != end(object.objectStates))
	{

		BOT_LOGIC_MAP_LOG(mLoggerEdges, " open", true);
		node->setDoorState(static_cast<EDirection>(i), true);
	}
	else
	{
		BOT_LOGIC_MAP_LOG(mLoggerEdges, " close", true);
		node->setDoorState(static_cast<EDirection>(i), false);
	}
}

void Map::addSeenTile(unsigned tileId)
{
	if (mKnownTilesAndVisitedStatus[tileId])
	{   // no need to update cause tile already visited
		return;
	}
	mKnownTilesAndVisitedStatus[tileId] = false; // Add unvisited tile to known tiles
}

void Map::createNode(Node* node)
{
	mNodeMap.emplace_back(node);
}

void Map::connectNodes()
{
	for (Node* curNode : mNodeMap)
	{
		// connecting
		Node* nw;
		Node* ne;
		Node* e;
		Node* se;
		Node* sw;
		Node* w;
		if (curNode->getPosition()->y % 2 == 0)
		{
			nw = getNode(curNode->getPosition()->x - 1, curNode->getPosition()->y - 1);
			ne = getNode(curNode->getPosition()->x, curNode->getPosition()->y - 1);
			e = getNode(curNode->getPosition()->x + 1, curNode->getPosition()->y);
			se = getNode(curNode->getPosition()->x, curNode->getPosition()->y + 1);
			sw = getNode(curNode->getPosition()->x - 1, curNode->getPosition()->y + 1);
			w = getNode(curNode->getPosition()->x - 1, curNode->getPosition()->y);
		}
		else
		{
			nw = getNode(curNode->getPosition()->x, curNode->getPosition()->y - 1);
			ne = getNode(curNode->getPosition()->x + 1, curNode->getPosition()->y - 1);
			e = getNode(curNode->getPosition()->x + 1, curNode->getPosition()->y);
			se = getNode(curNode->getPosition()->x + 1, curNode->getPosition()->y + 1);
			sw = getNode(curNode->getPosition()->x, curNode->getPosition()->y + 1);
			w = getNode(curNode->getPosition()->x - 1, curNode->getPosition()->y);
		}
		curNode->setNeighbour(NW, nw);
		curNode->setNeighbour(NE, ne);
		curNode->setNeighbour(E, e);
		curNode->setNeighbour(SE, se);
		curNode->setNeighbour(SW, sw);
		curNode->setNeighbour(W, w);
	}
}

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
	mNodeMap[tileId]->setType(tileType);
}

Node* Map::getNode(unsigned int x, unsigned int y) const
{
	if (x < 0 || x > getWidth() - 1 || y < 0 || y > getHeight() - 1)
	{
		return nullptr;
	}
	unsigned int index = x + y * mWidth;
	return mNodeMap[index];
}

Node* Map::getNode(unsigned int index) const
{
	return mNodeMap[index];
}

unsigned int Map::calculateDistance(int indexStart, int indexEnd) const
{
	Node* nStart = getNode(indexStart);
	Node* nEnd = getNode(indexEnd);

	int x = nEnd->getPosition()->x - nStart->getPosition()->x;
	int y = nEnd->getPosition()->y - nStart->getPosition()->y;
	return (abs(x) + abs(y));
}

EDirection Map::getDirection(unsigned int from, unsigned int to)
{
	Node* fromN = getNode(from);
	Node* toN = getNode(to);
	for (int i{}; i < Node::NBNEIGHBOURS; ++i)
	{
		if (fromN->getNeighbour(static_cast<EDirection>(i)) == toN)
		{
			return static_cast<EDirection>(i);
		}
	}
}

// Check if you can go from startTile to tileToGo in one turn
bool Map::canMoveOnTile(unsigned int startTile, unsigned int tileToGo)
{
	if (startTile == tileToGo)
	{
		return true;
	}
	EDirection dir = getDirection(startTile, tileToGo);
	return (Node::FORBIDDEN || getNode(tileToGo)->getType() == Node::OCCUPIED) && !getNode(startTile)->isEdgeBlocked(dir);
}

void Map::addGoalTile(unsigned int number)
{
	if (std::find(begin(mGoalTiles), end(mGoalTiles), number) == end(mGoalTiles))
	{
		bool canAccessTile = false;
		Node* currentNode = getNode(number);

		for (int i = N; i <= NW; ++i)
		{
			EDirection dir = static_cast<EDirection>(i);

			Node* tempNode = currentNode->getNeighbour(dir);
			if (tempNode != nullptr && !currentNode->isEdgeBlocked(dir))
			{
				canAccessTile = true;
			}
		}

		if (canAccessTile)
		{
			mGoalTiles.push_back(number);
		}
	}
}


//----------------------------------------------------------------------
// LOGGER
//----------------------------------------------------------------------
void Map::logMap(const unsigned int nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
	std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

	// Printing some infos
	myLog += "\tAccessible Goal Number : " + std::to_string(mGoalTiles.size()) + "\n";

	// Printing the map
	myLog += "Map : \n";
	unsigned int currentTileId{};
	for (int row = 0; row < mHeight; ++row)
	{
		if (row % 2)
		{
			myLog += "   ";
		}
		for (int col = 0; col < mWidth; ++col)
		{
			Node* tempNode = getNode(currentTileId++);
			switch (tempNode->getType())
			{
			case Node::NodeType::NONE:
				myLog += "-----";
				break;
			case Node::NodeType::FORBIDDEN:
				myLog += "F----";
				break;
			case Node::NodeType::GOAL:
				myLog += "G----";
				break;
			case Node::NodeType::OCCUPIED:
				myLog += "X----";
				break;
			case Node::NodeType::PATH:
				myLog += "P----";
				break;
			case Node::NodeType::PRESSURE_PLATE:
				myLog += "S----";
				break;
			}
			myLog += "  ";
		}
		myLog += "\n";
	}
	BOT_LOGIC_MAP_LOG(mLogger, myLog, false);
#endif

}

void Map::logZones(const unsigned int nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
	std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

	// Printing some infos
	myLog += "\tGreatest Galatic Zone : " + std::to_string(mGreatestZoneID) + "\n\n";

	// Printing the map
	myLog += "Galactic Zones : \n\n";
	unsigned int currentTileId{};
	for (int row = 0; row < mHeight; ++row)
	{
		//if (row % 2)
		//{
		//	myLog += "   ";
		//}
		////Printing TileID
		//for (int col = 0; col < mWidth; ++col)
		//{
		//	Node* tempNode = getNode(currentTileId++);
		//	if (tempNode)
		//	{
		//		std::string sTID = std::to_string(tempNode->getId());
		//		for (int i{}; i < 5 - sTID.size(); i++)
		//		{
		//			myLog += "-";
		//		}
		//		myLog += sTID;
		//	}
		//	myLog += "  ";
		//}
		//myLog += "\n";

		////Restart from first tile of the current line
		//currentTileId -= mWidth;

		if (row % 2)
		{
			myLog += "   ";
		}

		//Printing ZoneID
		for (int col = 0; col < mWidth; ++col)
		{
			Node* tempNode = getNode(currentTileId++);
			if (tempNode)
			{
				std::string sZID = std::to_string(tempNode->getZoneID());

				myLog += sZID;
				for (int i(sZID.size()); i < 5; i++)
				{
					myLog += "-";
				}
			}
			myLog += "  ";
		}
		myLog += "\n\n";

	}
	BOT_LOGIC_MAP_LOG(mLoggerGalacticZones, myLog, false);
#endif
}

void Map::logInfluenceMap(const unsigned int nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
	std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

	// Printing the map
	myLog += "Map : \n";
	unsigned int currentTileId{};
	for (int row = 0; row < mHeight; ++row)
	{
		if (row % 2)
		{
			myLog += "   ";
		}
		for (int col = 0; col < mWidth; ++col)
		{
			Node* tempNode = getNode(currentTileId++);
			float influ = std::trunc(100 * tempNode->getInfluence()) / 100;
			myLog += std::to_string(influ);
			myLog += "  ";
		}
		myLog += "\n";
	}
	BOT_LOGIC_MAP_LOG(mLoggerInfluence, myLog, false);
#endif

}