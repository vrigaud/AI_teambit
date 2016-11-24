#include "Map.h"
#include "TurnInfo.h"
#include "SearchMap.h"
#include "NPCInfo.h"
#include <Math.h>

Map Map::m_instance;

void Map::setLoggerPath(const std::string &a_path)
{
#ifdef BOT_LOGIC_DEBUG_MAP
    m_logger.Init(a_path, "Map.log");
    m_loggerInfluence.Init(a_path, "Map_Influence.log");
#endif

    BOT_LOGIC_MAP_LOG(m_logger, "Configure", true);
    BOT_LOGIC_MAP_LOG(m_loggerInfluence, "Configure", true);
}

void Map::setNodeType(unsigned int tileId, Node::NodeType tileType)
{
    m_nodeMap[tileId]->setType(tileType);
}

void Map::createNode(Node* node)
{
    m_nodeMap[node->getId()] = node;
}

void Map::connectNodes()
{
    for(std::pair<unsigned int, Node*> curNode : m_nodeMap)
    {
        // connecting
        Node* nw;
        Node* ne;
        Node* e;
        Node* se;
        Node* sw;
        Node* w;
        if(curNode.second->getPosition()->y % 2 == 0)
        {
            nw = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y - 1);
            ne = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y - 1);
            e = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y);
            se = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y + 1);
            sw = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y + 1);
            w = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y);
        }
        else
        {
            nw = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y - 1);
            ne = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y - 1);
            e = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y);
            se = getNode(curNode.second->getPosition()->x + 1, curNode.second->getPosition()->y + 1);
            sw = getNode(curNode.second->getPosition()->x, curNode.second->getPosition()->y + 1);
            w = getNode(curNode.second->getPosition()->x - 1, curNode.second->getPosition()->y);
        }
        curNode.second->setNeighboor(NW, nw);
        curNode.second->setNeighboor(NE, ne);
        curNode.second->setNeighboor(E, e);
        curNode.second->setNeighboor(SE, se);
        curNode.second->setNeighboor(SW, sw);
        curNode.second->setNeighboor(W, w);
    }
}

Node* Map::getNode(unsigned int x, unsigned int y)
{
    if(x < 0 || x > getWidth() - 1 || y < 0 || y > getHeight() - 1)
    {
        return nullptr;
    }
    unsigned int index = x + y * m_width;
    return m_nodeMap[index];
}

Node* Map::getNode(unsigned int index)
{
    return m_nodeMap[index];
}

float Map::calculateDistance(int indexStart, int indexEnd)
{
    Node* start = m_nodeMap[indexStart];
    Node* end = m_nodeMap[indexEnd];
    int x = start->getPosition()->x - end->getPosition()->x;
    int y = start->getPosition()->y - end->getPosition()->y;
    return abs(x) + abs(y);
}

std::map<unsigned, unsigned> Map::getBestGoalTile(std::map<unsigned, NPCInfo> npcInfo)
{
    std::map<unsigned, unsigned> goalMap;
    if(m_goalTiles.size() > npcInfo.size())
    {
        auto copyMapGoal = m_goalTiles;
        for(std::pair<unsigned, NPCInfo> npc : npcInfo)
        {
            int bestDist = 666;
            unsigned goalId = -1;
            std::vector<unsigned>::iterator goalIt = begin(copyMapGoal);
            for(; goalIt != end(copyMapGoal); ++goalIt)
            {
                float distance = calculateDistance(npc.second.tileID, *goalIt);
                if(distance < bestDist)
                {
                    goalId = *goalIt;
                    bestDist = distance;
                }
            }
            goalMap[npc.second.npcID] = goalId;
            copyMapGoal.erase(std::find(begin(copyMapGoal), end(copyMapGoal), goalId));
        }
    }
    else
    {
        for(unsigned goal : m_goalTiles)
        {
            if(npcInfo.size() <= 0)
            {
                break;
            }
            int bestDist = 666;
            int npcId = -1;
            for(std::pair<unsigned, NPCInfo> npc : npcInfo)
            {
                float distance = calculateDistance(npc.second.tileID, goal);
                if(distance < bestDist)
                {
                    npcId = npc.second.npcID;
                    bestDist = distance;
                }
            }
            goalMap[npcId] = goal;
            npcInfo.erase(npcId);
        }
    }

    return goalMap;
}

// TODO - See if we can implement this
//std::vector<unsigned> Map::getMostInfluencedTile()
//{
//    std::vector<unsigned> v;
//    for(auto node : m_seenTiles)
//    {
//        if(!node.second)
//        {
//            
//        }
//    }
//    for(auto node : m_interestingNodes)
//    {
//        v.push_back(node->getId());
//    }
//    return v;
//}

void Map::addGoalTile(unsigned int number)
{
    if(std::find(begin(m_goalTiles), end(m_goalTiles), number) == end(m_goalTiles))
    {
        bool canAccesTile = false;
        Node* currentNode = getNode(number);

        for(int i = N; i <= NW; ++i)
        {
            EDirection dir = static_cast<EDirection>(i);
            EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
            Node* tempNode = currentNode->getNeighboor(dir);
            if(tempNode != nullptr && (!currentNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
            {
                canAccesTile = true;
            }
        }

        if(canAccesTile)
        {
            m_goalTiles.push_back(number);
        }
    }
}

void Map::createInfluenceMap()
{
    m_interestingNodes.clear();
    for(auto node : m_seenTiles)
    {
        Node* myNode = getNode(node.first);
        myNode->setInfluence(0.0f);
        if(!node.second)
        {
            float tempInflu = 0.0f;
            for(int i = N; i <= NW; ++i)
            {
                EDirection dir = static_cast<EDirection>(i);
                EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
                auto edgeType = myNode->getEdge(dir);
                Node* tempNode = myNode->getNeighboor(dir);
                auto edgeNeibType = Node::FREE;
                if(tempNode != nullptr)
                {
                    edgeNeibType = tempNode->getEdge(invDir);
                }
                if(edgeType == Node::WINDOW || edgeNeibType == Node::WINDOW)
                {
                    tempInflu += 1.0f;
                }
            }

            for(int i = N; i <= NW; ++i)
            {
                EDirection dir = static_cast<EDirection>(i);
                EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
                Node* tempNode = myNode->getNeighboor(dir);
                if(tempNode != nullptr && (!myNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
                {
                    if(tempNode->getType() == Node::NONE)
                    {
                        tempNode->setInfluence(1.0f);
                        m_interestingNodes.push_back(tempNode);
                    }
                }
            }
            if(tempInflu > 0.0f)
            {
                myNode->setInfluence(tempInflu);
                m_interestingNodes.push_back(myNode);
            }
        }
    }
    std::sort(begin(m_interestingNodes), end(m_interestingNodes), [](const Node* a, const Node* b) {
        return a->getInfluence() > b->getInfluence();
    });
    propagateInfluence();
}

void Map::propagateInfluence()
{
    unsigned maxDist = m_influenceRange;
    for(auto node : m_interestingNodes)
    {
        propage(node, 0, maxDist, node->getInfluence());
    }
}

void Map::propage(Node* myNode, unsigned curDist, unsigned maxDist, float initialInfluence) const
{
    if(curDist > maxDist)
    {
        return;
    }
    for(int i = N; i <= NW; ++i)
    {
        EDirection dir = static_cast<EDirection>(i);
        EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
        Node* tempNode = myNode->getNeighboor(dir);
        if(tempNode != nullptr && (!myNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
        {
            if(tempNode->getType() == Node::PATH)
            {
                auto newInfluence = myNode->getInfluence() - (initialInfluence / m_influenceRange);
                if(newInfluence > tempNode->getInfluence())
                {
                    tempNode->setInfluence(newInfluence);
                }
                propage(tempNode, ++curDist, maxDist, initialInfluence);
            }
        }
    }
}

EDirection Map::getNextDirection(unsigned int a_start, unsigned int a_end)
{
    std::string direction = getStringDirection(a_start, a_end);

    if(direction == "N")
    {
        return N;
    }
    if(direction == "E")
    {
        return E;
    }
    if(direction == "W")
    {
        return W;
    }
    if(direction == "S")
    {
        return S;
    }
    if(direction == "NW")
    {
        return NW;
    }
    if(direction == "NE")
    {
        return NE;
    }
    if(direction == "SW")
    {
        return SW;
    }
    if(direction == "SE")
    {
        return SE;
    }
    return NE;
}

std::string Map::getStringDirection(unsigned int start, unsigned int end)
{
    Node* nStart = m_nodeMap[start];
    Node* nEnd = m_nodeMap[end];
    int x = nEnd->getPosition()->x - nStart->getPosition()->x;
    int y = nEnd->getPosition()->y - nStart->getPosition()->y;

    std::string direction{""};

    // Set the North/South direction
    if(y < 0)
    {
        direction += "N";
    }
    else if(y > 0)
    {
        direction += "S";
    }

    // Depending on the row, we need to adapt the West.East direction
    if(nStart->getPosition()->y % 2 == 0)
    {
        if(x < 0)
        {
            direction += "W";
        }
        else
        {
            direction += "E";
        }
    }
    else
    {
        if(x > 0)
        {
            direction += "E";
        }
        else
        {
            direction += "W";
        }
    }

    return direction;
}

std::vector<unsigned int> Map::getNpcPath(unsigned int a_start, unsigned int a_end, std::set<Node::NodeType> forbiddenType)
{
    SearchMap mySearch{getNode(a_start), getNode(a_end), forbiddenType};
    return mySearch.search();
}

bool Map::canMoveOnTile(unsigned int a_fromTileId, unsigned int a_toTileId)
{
    if(a_fromTileId == a_toTileId)
    {
        return true;
    }

    bool isStateOk = !(getNode(a_toTileId)->getType() == Node::FORBIDDEN || getNode(a_toTileId)->getType() == Node::OCCUPIED);
    EDirection dir = getNextDirection(a_fromTileId, a_toTileId);
    EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
    return isStateOk && !getNode(a_fromTileId)->isEdgeBlocked(dir) && !getNode(a_toTileId)->isEdgeBlocked(invDir);
}

std::vector<unsigned int> Map::getNearInfluencedTile(unsigned int a_currentId)
{
    Node* current = getNode(a_currentId);
    std::vector<unsigned int> v;

    if(isAllNeighboorHaveSameInfluence(current))
    {
        return v;
    }

    float bestinf = 0.0f;
    unsigned bestTile;
    for(int i = N; i <= NW; ++i)
    {
        Node* neighboor = current->getNeighboor(static_cast<EDirection>(i));
        if(neighboor && canMoveOnTile(a_currentId, neighboor->getId()))
        {
            float nodeinf = neighboor->getInfluence();
            if(nodeinf > 0.0f)
            {
                if(bestinf < nodeinf)
                {
                    bestinf = nodeinf;
                    bestTile = neighboor->getId();
                }
            }
        }
    }
    v.push_back(bestTile);
    return v;
}

bool Map::isAllNeighboorHaveSameInfluence(Node* node)
{
    float startInf = 0.0f;
    int counterTile = 0;
    for(int i = N; i <= NW; ++i)
    {
        Node* neighboor = node->getNeighboor(static_cast<EDirection>(i));
        if(neighboor && canMoveOnTile(node->getId(), neighboor->getId()))
        {
            counterTile++;
            if(startInf == 0.0f)
            {
                startInf = neighboor->getInfluence();
            }

            if(startInf != neighboor->getInfluence())
            {
                return false;
            }
        }
    }
    // Ensure moving on the only available tile !
    if(counterTile == 1)
    {
        return false;
    }
    return true;
}

void Map::testAddTile(std::vector<unsigned int>& v, unsigned int fromTileId, unsigned int toTileId)
{
    if(canMoveOnTile(fromTileId, toTileId) && !isVisited(toTileId))
    {
        v.push_back(toTileId);
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
    myLog += "\tAccessible Goal Number : " + std::to_string(m_goalTiles.size()) + "\n";

    // Printing the map
    myLog += "Map : \n";
    unsigned int currentTileId{};
    for(int row = 0; row < m_height; ++row)
    {
        if(row % 2)
        {
            myLog += "   ";
        }
        for(int col = 0; col < m_width; ++col)
        {
            Node* tempNode = getNode(currentTileId++);
            if(tempNode->getNpcIdOnNode() > 0)
            {
                myLog += std::to_string(tempNode->getNpcIdOnNode()) + "----";
            }
            else
            {
                switch(tempNode->getType())
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
                }
            }
            myLog += "  ";
        }
        myLog += "\n";
    }
    BOT_LOGIC_MAP_LOG(m_logger, myLog, false);
#endif

}

void Map::logInfluenceMap(unsigned nbTurn)
{
#ifdef BOT_LOGIC_DEBUG_MAP
    std::string myLog = "\nTurn #" + std::to_string(nbTurn) + "\n";

    // Printing the map
    myLog += "Map : \n";
    unsigned int currentTileId{};
    for(int row = 0; row < m_height; ++row)
    {
        if(row % 2)
        {
            myLog += "   ";
        }
        for(int col = 0; col < m_width; ++col)
        {
            Node* tempNode = getNode(currentTileId++);
            float influ = std::trunc(100 * tempNode->getInfluence()) / 100;
            if(tempNode->getNpcIdOnNode() > 0)
            {
                myLog += std::to_string(tempNode->getNpcIdOnNode()) + "-" + std::to_string(influ);
            }
            else
            {
                myLog += std::to_string(influ);
            }
            myLog += "  ";
        }
        myLog += "\n";
    }
    BOT_LOGIC_MAP_LOG(m_loggerInfluence, myLog, false);
#endif

}