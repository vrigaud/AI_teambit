#include "SearchMap.h"
#include "Map.h"
#include "SearchNode.h"

void SearchMap::prepareNode(Node* refNode, unsigned int newGValue, SearchNode* parent)
{
    auto nodeType = refNode->getType();
    if (std::find(begin(m_forbiddenType), end(m_forbiddenType), nodeType) != end(m_forbiddenType))
    {
        return;
    }
    unsigned int id = refNode->getId();
    SearchNode* node = new SearchNode(refNode->getPosition()->x, refNode->getPosition()->y, id, parent);
    node->setG(newGValue);
    node->setH(calculateManathan(node, m_goal));

    for(int i = 0; i < closedList.size(); i++)
    {
        if(id == closedList[i]->getId())
        {
            return;
        }
    }

    for (int i = 0; i < openList.size(); i++)
    {
        if (id == openList[i]->getId())
        {
            if (node->getF() < openList[i]->getF())
            {
                openList[i]->setG(newGValue);
                openList[i]->setParent(parent);
            }
            else
            {
                delete node;
                return;
            }
        }
    }

    openList.push_back(node);
}

std::vector<unsigned int> SearchMap::search()
{
    while (!m_isGoalFound)
    {
        if (openList.empty())
        {
            return std::vector<unsigned int>{};
        }

        SearchNode* currentSearchNode = getNextNodeToSearch();
        Node* currentNode = Map::get()->getNode(currentSearchNode->getId());

        if (currentSearchNode->getId() == m_goal->getId())
        {
            SearchNode* getPath;
            for (getPath = currentSearchNode; getPath != nullptr; getPath = getPath->getParent())
            {
                m_pathToGoal.push_back(getPath->getId());
            }
            m_isGoalFound = true;
            return m_pathToGoal;
        }

        for (int i = N; i <= NW; ++i)
        {
            EDirection dir = static_cast<EDirection>(i);
            EDirection invDir = static_cast<EDirection>((dir + 4) % 8);
            Node* tempNode = currentNode->getNeighboor(dir);
            if (tempNode != nullptr && (!currentNode->isEdgeBlocked(dir) && !tempNode->isEdgeBlocked(invDir)))
            {
                prepareNode(tempNode, currentSearchNode->getG() + 10, currentSearchNode);
            }
        }
    }
}

SearchMap::SearchMap(Node* start, Node* goal)
{
    initSearchMap(start, goal);
}
SearchMap::SearchMap(Node* start, Node* goal, std::set<Node::NodeType> forbiddenType)
{
    initSearchMap(start, goal, forbiddenType);
}

void SearchMap::initSearchMap(Node* start, Node* goal, std::set<Node::NodeType> forbiddenType)
{
    if(!m_isInitialized)
    {
        m_forbiddenType = forbiddenType;

        for(int i = 0; i < openList.size(); i++)
        {
            delete openList[i];
        }
        openList.clear();

        for(int i = 0; i < closedList.size(); i++)
        {
            delete closedList[i];
        }
        closedList.clear();

        m_pathToGoal.clear();

        m_isPathFinished = false;
        m_isGoalFound = false;

        setStartNode(new SearchNode(start->getPosition()->x, start->getPosition()->y, start->getId()));
        setGoalNode(new SearchNode(goal->getPosition()->x, goal->getPosition()->y, goal->getId()));

        m_start->setH(calculateManathan(m_start, m_goal));

        openList.push_back(m_start);
        m_isInitialized = true;
    }
}

SearchNode* SearchMap::getNextNodeToSearch()
{
    unsigned int bestF = 9999999;
    int index = -1;
    SearchNode* nextNode = nullptr;

    for(int i = 0; i < openList.size(); i++)
    {
        if(openList[i]->getF() < bestF)
        {
            index = i;
            bestF = openList[i]->getF();
        }
    }

    if(index >= 0)
    {
        nextNode = openList[index];
        closedList.push_back(nextNode);
        openList.erase(openList.begin() + index);
    }

    return nextNode;
}

unsigned int SearchMap::calculateManathan(const SearchNode* start, const SearchNode* goal) const
{
    int x = goal->getX() - start->getX();
    int y = goal->getY() - start->getY();
    return (abs(x) + abs(y)) * 10;
}

int SearchMap::getNextPathTile()
{
    if(m_pathToGoal.size() == 1)
    {
        return -1;
    }
    unsigned int index = m_pathToGoal[m_pathToGoal.size() - 2];
    return index;
}

void SearchMap::FindAnotherPath()
{
    m_isInitialized = false;
    initSearchMap(Map::get()->getNode(m_pathToGoal.back()), Map::get()->getNode(m_goal->getId()));
    search();
}

bool SearchMap::checkPathIntegrity()
{
    if(m_isGoalFound)
    {
        for(int i = 0; i < m_pathToGoal.size(); i++)
        {
            if(Map::get()->getNode(m_pathToGoal[i])->getType() == Node::FORBIDDEN)
            {
                return false;
            }
        }
    }
    return true;
}
