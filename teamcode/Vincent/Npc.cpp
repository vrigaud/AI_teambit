#include "Npc.h"
#include "Map.h"

/* INCLUDE FROM CARLE */
#include "Globals.h"

#include <algorithm>


Npc::Npc(unsigned int a_id, unsigned int a_tileId, std::string a_path)
    : m_currentState{NONE}, m_nextState{EXPLORING}, m_id{a_id}, m_goal{}, m_hasGoal{false}, m_path{a_tileId}, m_nextActions{}, m_historyTiles{a_tileId}, m_turnCount{0}
{
#ifdef BOT_LOGIC_DEBUG_NPC
    m_logger.Init(a_path, "Npc_" + std::to_string(m_id) + ".log");
#endif

    BOT_LOGIC_NPC_LOG(m_logger, "Configure", true);

};

void Npc::update()
{
    ++m_turnCount;
    BOT_LOGIC_NPC_LOG(m_logger, "\nTurn #" + std::to_string(m_turnCount) + "\n\tCurrent Tile : " + std::to_string(getCurrentTileId()), true);
    updatePath();
    BOT_LOGIC_NPC_LOG(m_logger, "\tEntering State Machine : ", true);
    do
    {
        m_currentState = m_nextState;
        switch(m_currentState)
        {
            case(MOVING):
                followPath();
                break;
            case(WAITING):
                wait();
                break;
            case(EXPLORING):
                explore();
                break;
            case(INTERACTING):
                interact();
                break;
            case(ARRIVED):
                m_nextState = ARRIVED; // May be useless atm
                m_currentState = ARRIVED;
                break;
        }
    } while(m_currentState != m_nextState);
    int test = 0;
}

bool Npc::stopEverything()
{
    // deleting item
    for(std::vector< Action* >::iterator it = m_nextActions.begin(); it != m_nextActions.end(); ++it)
    {
        delete (*it);
    }
    m_nextActions.clear();
    return true;
}

void Npc::stopMoving()
{
    auto it = std::partition(std::begin(m_nextActions),
                             std::end(m_nextActions),
                             [](const Action* curAction) { return curAction->actionType != Action_Move; });

    for(std::vector< Action* >::iterator itDelete = it; itDelete != m_nextActions.end(); ++itDelete)
    {
        delete (*itDelete);
    }
    m_nextActions.erase(it, std::end(m_nextActions));
}

void Npc::stopInteract()
{
    auto it = std::partition(std::begin(m_nextActions),
                             std::end(m_nextActions),
                             [](const Action* curAction) { return curAction->actionType == Action_Interact; });

    for(std::vector< Action* >::iterator itDelete = it; itDelete != m_nextActions.end(); ++itDelete)
    {
        delete (*itDelete);
    }
    m_nextActions.erase(it, std::end(m_nextActions));
}

void Npc::unstackActions()
{
    while(m_nextActions.size())
    {
        Action* curAction{m_nextActions.back()};
        switch(curAction->actionType)
        {
            case Action_None:
                // Do nothing
                break;
            case Action_Move:
                moveForwardOnPath();
                break;
            case Action_Interact:
                // TODO - do nothing atm
                break;
        }
        m_nextActions.pop_back();
    }
}

void Npc::calculPath()
{
    if(!hasGoal())
    {
        return;
    }
    m_path = Map::get()->getNpcPath(getCurrentTileId(), m_goal);
}

bool Npc::updatePath()
{
    BOT_LOGIC_NPC_LOG(m_logger, "\tUpdating Path ", true);
    DisplayVector("\t\tOld path: ", m_path);
    std::vector<unsigned> reversePath;
    reversePath.resize(m_path.size());
    std::reverse_copy(begin(m_path), end(m_path), begin(reversePath));
    unsigned int oldTileId{reversePath.front()};
    for(unsigned int tileId : reversePath)
    {
        if(!Map::get()->canMoveOnTile(oldTileId, tileId))
        {
            m_path = Map::get()->getNpcPath(getCurrentTileId(), m_goal);
            DisplayVector("\t\tPath Updated : ", m_path);
            return true;
        }
        oldTileId = tileId;
    }
    BOT_LOGIC_NPC_LOG(m_logger, "\t\tNo update needed", true);
    return false;
}

int Npc::getNextPathTile() const
{
    if(m_path.size() == 1)
    {
        return -1;
    }
    unsigned int index = m_path[m_path.size() - 2];
    return index;
}

void Npc::explore()
{
    BOT_LOGIC_NPC_LOG(m_logger, "-Explore", true);
    if(hasGoal())
    {
        BOT_LOGIC_NPC_LOG(m_logger, "\tNPC have a goal : " + std::to_string(m_goal), true);
        DisplayVector("\tNpc base path :", m_path);
        m_nextState = MOVING;
        return;
    }

    std::vector<unsigned int> v = Map::get()->getNearInfluencedTile(getCurrentTileId());

    if(v.size() <= 0)
    {
        std::vector<unsigned> nonVisitedTiles = Map::get()->getNonVisitedTile();
        DisplayVector("\t-Looking for the non visited tiles : ", nonVisitedTiles);
        for(unsigned index : nonVisitedTiles)
        {
            std::vector<unsigned> temp = Map::get()->getNpcPath(getCurrentTileId(), index, {Node::NodeType::FORBIDDEN, Node::NodeType::NONE});
            if(!temp.empty())
            {
                m_path = temp;
                m_target = index;
                m_nextState = MOVING;
                break;
            }
        }
    }
    else
    {
        unsigned int tileId = v[0];

        m_path = {tileId, getCurrentTileId()};
        m_historyTiles.push_back(tileId);

        m_nextActions.push_back(new Move{m_id, Map::get()->getNextDirection(getCurrentTileId(), getNextPathTile())});
        Map::get()->visitTile(tileId);

        BOT_LOGIC_NPC_LOG(m_logger, "Deplacement vers " + std::to_string(tileId), true);

        m_nextState = EXPLORING;
    }
}

void Npc::followPath()
{
    BOT_LOGIC_NPC_LOG(m_logger, "-FollowPath", true);
    // Get the direction between the two last nodes of m_path
    if(getCurrentTileId() == m_goal)
    {
        m_nextState = ARRIVED;
        return;
    }
    if(getCurrentTileId() == m_target && !hasGoal())
    {
        m_nextState = EXPLORING;
        return;
    }
    m_nextActions.push_back(new Move{m_id, Map::get()->getNextDirection(getCurrentTileId(), getNextPathTile())});
    Map::get()->visitTile(getNextPathTile());
    BOT_LOGIC_NPC_LOG(m_logger, "\tDeplacement vers " + std::to_string(getNextPathTile()), true);
    m_nextState = MOVING;
}

void Npc::wait()
{
    BOT_LOGIC_NPC_LOG(m_logger, "-Wait", true);
    // TODO - Test why we are blocked ?
}

void Npc::interact()
{
    BOT_LOGIC_NPC_LOG(m_logger, "-Interact", true);
    // TODO - interact with some fancy stuff
}

template<class T>
void Npc::DisplayVector(std::string info, const std::vector<T> v)
{
    std::string s{""};
    for(T u : v)
    {
        s += std::to_string(u) + " ";
    }
    BOT_LOGIC_NPC_LOG(m_logger, info + s, true);
}