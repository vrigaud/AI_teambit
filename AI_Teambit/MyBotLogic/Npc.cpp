#include "Npc.h"
#include "LoggerPath.h"
#include <set>
#include <vector>

using namespace std;

Npc::Npc(unsigned int a_id, unsigned int a_tileId)
    : mCurrentState{}, mNextState{}, mId{ a_id }, mTurnCount{ 0 }, mPath{}
{
#ifdef BOT_LOGIC_DEBUG_NPC
    mLogger.Init(LoggerPath::getInstance()->getPath(), "Npc_" + std::to_string(mId) + ".log");
#endif

    BOT_LOGIC_NPC_LOG(mLogger, "Configure", true);

};

void Npc::update()
{
    updateState();
    enterStateMachine();
}


void Npc::updateState()
{
    // Atm it's an if/ elseif cause we dont really know yet what impact the choice...
    // care at the order for more efficiency
    if (mObjective.mType == Objective::NONE)
    {
        mCurrentState = WAITING;
    }
    else if (mObjective.mType == Objective::SEARCH_MAP)
    {
        mCurrentState = EXPLORE_MAP;
    }
    else if (mObjective.mType == Objective::GO_TO)
    {
        mCurrentState = SEARCH_PATH;
    }
    else if (mObjective.mType == Objective::SEARCH_H_DOOR)
    {
        mCurrentState = EXPLORE_H_DOOR;
    }
}

void Npc::enterStateMachine()
{
    BOT_LOGIC_NPC_LOG(mLogger, "Enter state machine ->", false);
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
}

void Npc::exploring()
{
    BOT_LOGIC_NPC_LOG(mLogger, " Exploring", true);
    do
    {
        mCurrentState = mNextState;
        switch (mCurrentState & STATE_LEVEL_1)
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
    // TODO - moving fsm
    do
    {
        mCurrentState = mNextState;
        switch (mCurrentState & STATE_LEVEL_1)
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

void Npc::waiting()
{
    BOT_LOGIC_NPC_LOG(mLogger, " Waiting", true);
    // TODO - waiting fsm ... wait, you just have to wait..
    mCurrentState = WAITING;
    mNextState = WAITING;
}


// Exploring fsm function
void Npc::exploreMap()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-ExploreMap", true);
    // TODO - pull influence tile
}

void Npc::exploreHiddenDoor()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-ExploreHiddenDoor", true);
    // TODO - pull influence tile
}

inline void Npc::exploreWaiting()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-ExploreWaiting", true);
    // TODO - wait cause someone want to go on the same tile
    mCurrentState = EXPLORE_WAITING;
    mNextState = EXPLORE_WAITING;
}

void Npc::exploreDNpc()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-ExploreDNpc", true);
    // TODO - see if other npc want to go on same tile
}

void Npc::move()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-Move", true);
    // TODO - just push action in the list?
}


// Moving fsm function
void Npc::searchPath()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-SearchPath", true);

    //declarations
    Node* current{ Map::getInstance()->getNode(mId) };
    size_t maxNbNode = Map::getInstance()->getHeight() * Map::getInstance()->getWidth();

    multiset<Node*, NodeComparator> openedNodes{};
    vector<Node*> closedNodes{};



    closedNodes.reserve(maxNbNode);

    openedNodes.insert(current);

    while (!openedNodes.empty())
    {
        current = *begin(openedNodes);

        openedNodes.erase(current);
        closedNodes.emplace_back(current); //TODO check the complexity of emplace_back vs emplace

        if (current->getId() == mGoal)
        {
            //TODO MakePath or something like that...
            break;
        }

        for (size_t i{}; i < 8; ++i) //TODO add constant
        {
            Node* neighbour = current->getNeighboor(static_cast<EDirection>(i));
            int transitionCost = neighbour->getCost();

            if (transitionCost < 1 || neighbour->getId() == current->getId())
            {
                continue;
            }

            neighbour->setParent(current);

            auto testClosed = std::find(begin(closedNodes), end(closedNodes), neighbour);
            auto testOpened = std::find(begin(openedNodes), end(openedNodes), neighbour);

            if (
                testClosed != closedNodes.end() && ((*testClosed)->getCost() < transitionCost) ||
                testOpened != openedNodes.end() && ((*testOpened)->getCost() < transitionCost)
                )
            {
                continue;
            }
            else
            {
                neighbour->setCost(transitionCost + current->getCost());

                /*
                TODO
                MANHATTAN DISTANCE
                */
                neighbour->setHeuristic(neighbour->getCost()); // PLUS HEURISTIC
                openedNodes.emplace(neighbour);
            }
        }


    }
}

void Npc::followPath()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-FollowPath", true);
    // TODO - 
}

void Npc::movingDNpc()
{
    BOT_LOGIC_NPC_LOG(mLogger, "-MovingDNpc", true);
    // TODO - 
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
    mNextState = ARRIVED;
}
