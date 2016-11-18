#include "Map.h"
#include "TurnInfo.h"
#include "NPCInfo.h"
#include "LoggerPath.h"
#include <Math.h>
#include <algorithm>

Map Map::mInstance;

void Map::setLoggerPath()
{
    std::string a_path = LoggerPath::getInstance()->getPath();
#ifdef BOT_LOGIC_DEBUG_MAP
    mLogger.Init(a_path, "Map.log");
    mLoggerInfluence.Init(a_path, "Map_Influence.log");
    mLoggerEdges.Init(a_path, "Map_Edges.log");
#endif

    BOT_LOGIC_MAP_LOG(mLogger, "Configure", true);
    BOT_LOGIC_MAP_LOG(mLogger, " - - NONE | F - Forbidden | G - Goal | X - Occupied | P - Path | S - Pressure plate \n", true);
    BOT_LOGIC_MAP_LOG(mLoggerInfluence, "Configure", true);
    BOT_LOGIC_MAP_LOG(mLoggerEdges, "Configure", true);
    BOT_LOGIC_MAP_LOG(mLoggerEdges, " 0 - HighWall | 1 - Window | 2 - Door \n", true);
    BOT_LOGIC_MAP_LOG(mLoggerEdges, " 0 - N | 1 - NE | 2 - E | 3 - SE | 4 - S | 5 - SW | 6 - W | 7 - NW \n", true);
}

void Map::initMap(int height, int width, int visionRange)
{
    setHeight(height);
    setWidth(width);
    setInfluenceRange(std::max(visionRange + 2, 4));
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
}

void Map::createInfluenceMap(const InfluenceData::InfluenceType& aType)
{
	mInterestingNodes.clear();
	for (auto node : mSeenTiles)
	{
		Node* myNode = getNode(node.first);
		myNode->setInfluence(0.0f);
		if (!node.second)
		{
			float tempInflu = 0.0f;

			//TODO : [REFACTO] mergeable loops ?
			for (int i = N; i <= NW; ++i)
			{
				EDirection dir = static_cast<EDirection>(i);
				EDirection invDir = static_cast<EDirection>((dir + 4) % 8);

				EdgeData::EdgeType edgeType = myNode->getEdgeType(dir);
				Node* tempNode = myNode->getNeighboor(dir);
				EdgeData::EdgeType edgeNeibType = EdgeData::FREE;
				if (tempNode != nullptr)
				{
					edgeNeibType = tempNode->getEdgeType(invDir);
				}
				if (edgeType == EdgeData::WINDOW || edgeNeibType == EdgeData::WINDOW)
				{
					tempInflu += 1.0f;
				}
			}

			for (int i = N; i <= NW; ++i)
			{
				EDirection dir = static_cast<EDirection>(i);
				EDirection invDir = static_cast<EDirection>((dir + 4) % 8);

				Node* tempNode = myNode->getNeighboor(dir);
				if (tempNode != nullptr && (!myNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
				{
					if (tempNode->getType() == Node::NONE)
					{
						tempNode->setInfluence(1.0f);
						mInterestingNodes.push_back(tempNode);
					}
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
		EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
		Node* tempNode = myNode->getNeighboor(dir);
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

void Map::updateMap(TurnInfo& turnInfo)
{
    updateTiles(turnInfo);
    updateEdges(turnInfo);

	//Test
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
            if (info.second.edgesCost[i] == 0)
            {
                // TODO - Update with new doors in engine
                if (!info.second.associatedControllers.empty())
                {
                    if (info.second.objectType == ObjectType_HighWall)
                    {
                        BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : DOOR", true);
                        node->setEdgeCost(static_cast<EDirection>(i), EdgeData::DOOR);
                    }
                    else if (info.second.objectType == ObjectType_Window)
                    {
                        BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : DOOR_W", true);
                        node->setEdgeCost(static_cast<EDirection>(i), EdgeData::DOOR_W);
                    }
                }
                else
                {
                    BOT_LOGIC_MAP_LOG(mLoggerEdges, "\tTileID : " + std::to_string(info.second.tileID) + " - Dir : " + std::to_string(i) + " - Type : " + std::to_string(info.second.objectType), true);
                    node->setEdgeCost(static_cast<EDirection>(i), static_cast<EdgeData::EdgeType>(info.second.objectType + 1));
                }
            }
        }
    }
}

void Map::updateTiles(TurnInfo& turnInfo)
{
    for(auto info : turnInfo.tiles)
    {
        auto tileInfo = info.second;

//        auto ITisForbidden = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Forbidden);
//        auto ITisTarget = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Target);
//        auto ITisDescriptor = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_Descriptor);
//        auto ITisPressurePlate = find(begin(tileInfo.tileAttributes), end(tileInfo.tileAttributes), TileAttribute_PressurePlate);
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

void Map::addSeenTile(unsigned tileId)
{
	if (mSeenTiles[tileId])
	{
		return;
	}
	mSeenTiles[tileId] = false;
}

void Map::createNode(Node* node)
{
    mNodeMap.emplace_back(node);
}

void Map::connectNodes()
{
    for(Node* curNode : mNodeMap)
    {
        // connecting
        Node* nw;
        Node* ne;
        Node* e;
        Node* se;
        Node* sw;
        Node* w;
        if(curNode->getPosition()->y % 2 == 0)
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
        curNode->setNeighboor(NW, nw);
        curNode->setNeighboor(NE, ne);
        curNode->setNeighboor(E, e);
        curNode->setNeighboor(SE, se);
        curNode->setNeighboor(SW, sw);
        curNode->setNeighboor(W, w);
    }
}

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
    mNodeMap[tileId]->setType(tileType);
}

Node* Map::getNode(unsigned int x, unsigned int y)
{
    if(x < 0 || x > getWidth() - 1 || y < 0 || y > getHeight() - 1)
    {
        return nullptr;
    }
    unsigned int index = x + y * mWidth;
    return mNodeMap[index];
}

Node* Map::getNode(unsigned int index)
{
    return mNodeMap[index];
}

float Map::calculateDistance(int indexStart, int indexEnd)
{
    unsigned int firstX = indexStart % mWidth;
    unsigned int firstY = indexStart / mWidth;
    unsigned int sndX = indexEnd % mWidth;
    unsigned int sndY = indexEnd / mWidth;

    int deltaX = static_cast<int>(sndX - firstX);
    int deltaY = static_cast<int>(sndY - firstY);
    int deltas = deltaY - deltaX;
    return std::max(abs(deltaX), std::max(abs(deltaY), abs(deltas)));
}


void Map::addGoalTile(unsigned int number)
{
    if(std::find(begin(mGoalTiles), end(mGoalTiles), number) == end(mGoalTiles))
    {
        bool canAccessTile = false;
        Node* currentNode = getNode(number);

        for(int i = N; i <= NW; ++i)
        {
            EDirection dir = static_cast<EDirection>(i);
            EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
            Node* tempNode = currentNode->getNeighboor(dir);
            if(tempNode != nullptr && (!currentNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
            {
                canAccessTile = true;
            }
        }

        if(canAccessTile)
        {
            mGoalTiles.push_back(number);
        }
    }
}


//----------------------------------------------------------------------
// LOGGER
//----------------------------------------------------------------------
void Map::logMap(unsigned nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
    std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

    // Printing some infos
    myLog += "\tAccessible Goal Number : " + std::to_string(mGoalTiles.size()) + "\n";

    // Printing the map
    myLog += "Map : \n";
    unsigned int currentTileId{};
    for(int row = 0; row < mHeight; ++row)
    {
        if(row % 2)
        {
            myLog += "   ";
        }
        for(int col = 0; col < mWidth; ++col)
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


void Map::logInfluenceMap(unsigned nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
    std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

    // Printing the map
    myLog += "Map : \n";
    unsigned int currentTileId{};
    for(int row = 0; row < mHeight; ++row)
    {
        if(row % 2)
        {
            myLog += "   ";
        }
        for(int col = 0; col < mWidth; ++col)
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