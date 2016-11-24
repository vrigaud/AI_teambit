#include "BlackBoard.h"

#include "Graph.h"

#include "BlocSequence.h"
#include "BlocExploration.h"
#include "BlocAStar.h"
#include "BlocTargetSearch.h"
#include "BlocFinished.h"

#include <sstream>

using namespace BehaviourTree;

BlackBoard::BlackBoard(const LevelInfo& _levelInfo)
{
    mLevelInfo = new LevelInfo(_levelInfo);

    if (_levelInfo.bOmnicientMode)
    {
        initWhenOmniscient();
    }
    else
    {
        initWhenNotOmniscient();
    }
}


void BlackBoard::createBTWhenNotOmniscient()
{

    BlocSequence* sequence = new BlocSequence();
    BlocSelect* selectAstar = new BlocSelect();

    selectAstar->connect(*getBlocFinished(*this));
    selectAstar->connect(*getBlocAStar(*this));

    sequence->connect(*getBlocTargetSearch(*this));
    sequence->connect(*selectAstar);

    mRoot.connect(*sequence);
    mRoot.connect(*getBlocExplore(*this));
}


void BlackBoard::initWhenOmniscient()
{
    Graph::getSingleton().init(*mLevelInfo);

    if (Graph::getSingleton().isEmpty())
        Graph::getSingleton().createGraph(*mLevelInfo);


    for (auto it = mLevelInfo->npcs.rbegin(); it != mLevelInfo->npcs.rend(); ++it)
    {
        mPaths.push_back(Graph::getSingleton().pathFinder(*Graph::getSingleton().getGraph()[it->second.tileID],
            *Graph::getSingleton().getGraph()[getTargetForNPC(*it)], *mLevelInfo));
    }

    std::reverse(std::begin(mPaths), std::end(mPaths));

    setAction = &BlackBoard::giveActionWhenOmniscient;
}

void BlackBoard::initWhenNotOmniscient()
{
    mExploreGraph = new ExplorationPart::ExplorationGraph();

    mExploreGraph->init(*mLevelInfo);
    mExploreGraph->createExplorationGraph();

    mNotOmniscientPaths.resize(mLevelInfo->npcs.size(), std::list<unsigned int>{});

    createBTWhenNotOmniscient();

    setAction = &BlackBoard::giveActionWhenNotOmniscient;
}


void BlackBoard::giveActionForThisTurn(std::vector<Action*>& inputAction)
{
    (this->*setAction)(inputAction);
}

void BlackBoard::giveActionWhenOmniscient(std::vector<Action*>& inputAction)
{
    size_t iter = 0;
    for (std::pair<unsigned int, NPCInfo> it : mTurnInfo.npcs)
    {
        Node*current = Graph::getSingleton().getGraph()[it.second.tileID];

        Node* nodeToGo = &*mPaths[iter].begin();


        if (mPaths[iter].size() != 1 || current->getID() != nodeToGo->getID())
        {
            if (current->getID() == nodeToGo->getID())
            {
                mPaths[iter].pop_front();
                nodeToGo = &*mPaths[iter].begin();
            }

            EDirection direct = chooseDirection(current->getID(), nodeToGo->getID());

            if (current->getID() != nodeToGo->getID())
            {
                inputAction.push_back(new Move(it.second.npcID, direct));
            }
        }

        ++iter;
    }
}

unsigned int BlackBoard::getTargetForNPC(const std::pair<unsigned int, NPCInfo>& npc)// retourner la target pour chaque npc
{
    unsigned int target = 0;
    unsigned int minDist = 0xFFFFFFFF;
    unsigned int res;
    size_t memoryIndex = 0;

    for (size_t iter = 0; iter < Graph::getSingleton().getTargets().size(); ++iter)
    {
        if (Graph::getSingleton().getTargets().at(iter).isTargeted)
        {
            continue;
        }

        res = Node::manhattanDistance(npc.second.tileID, Graph::getSingleton().getTargets().at(iter).targetTile->tileID, *mLevelInfo);
        if (minDist > res)
        {
            minDist = res;
            target = Graph::getSingleton().getTargets().at(iter).targetTile->tileID;
            memoryIndex = iter;
        }
    }

    Graph::getSingleton().getTargets().at(memoryIndex).isTargeted = true;

    return target;
}

Node* BlackBoard::fromListAt(size_t iter, std::list<Node>& list)
{
    std::list<Node>::iterator it = list.begin();

    std::advance(it, iter);

    return &(*it);
}


EDirection BlackBoard::chooseDirection(unsigned int current, unsigned int next)
{
    unsigned int delta;

    if (next < current)
    {
        delta = current - next;
        if ((current / mLevelInfo->colCount) % 2) // ligne impaire
        {
            if (delta == 1)
            {
                return W;
            }
            else if (delta == mLevelInfo->colCount)
            {

                return NW;
            }
            else
            {
                return NE;
            }
        }
        else // ligne paire
        {
            if (delta == 1)
            {
                return W;
            }
            else if (delta == mLevelInfo->colCount)
            {

                return NE;
            }
            else
            {
                return NW;
            }
        }
    }
    else
    {
        delta = next - current;

        if ((current / mLevelInfo->colCount) % 2) // ligne impaire
        {
            if (delta == 1)
            {
                return E;
            }
            else if (delta == mLevelInfo->colCount)
            {
                return SW;
            }
            else
            {
                return SE;
            }
        }
        else
        {
            if (delta == 1)
            {
                return E;
            }
            else if (delta == mLevelInfo->colCount)
            {
                return SE;
            }
            else
            {
                return SW;
            }
        }
    }
}



void BlackBoard::giveActionWhenNotOmniscient(std::vector<Action*>& inputAction)
{
    for (mCurrentNpcMapper = 0; mCurrentNpcMapper < mTurnInfo.npcs.size(); ++mCurrentNpcMapper)
    {
        mRoot();
    }

    
    mCurrentNpcMapper = 0;

    for (std::map<unsigned int, NPCInfo>::const_iterator it = mTurnInfo.npcs.begin(); it != mTurnInfo.npcs.end(); ++it)
    {
        ExploreNode* current = mExploreGraph->getGraph()[it->second.tileID];

        current->setKnown(true);

        if (mNotOmniscientPaths[mCurrentNpcMapper] == std::list<unsigned int>())
        {
            current->incrementExploredCount();
            current->incrementExploredCount();
            current->incrementExploredCount();
            current->incrementExploredCount();
            current->incrementExploredCount();
            ++mCurrentNpcMapper;
            continue;
        }

        ExploreNode* nodeToGo = mExploreGraph->getGraph()[*(mNotOmniscientPaths[mCurrentNpcMapper].begin())];

        while (mNotOmniscientPaths[mCurrentNpcMapper].size() != 1 && current->getID() == nodeToGo->getID())
        {
            mNotOmniscientPaths[mCurrentNpcMapper].pop_front();
            nodeToGo = mExploreGraph->getGraph()[*(mNotOmniscientPaths[mCurrentNpcMapper].begin())];
        }

        {
            EDirection direct = chooseDirection(current->getID(), nodeToGo->getID());

            inputAction.push_back(new Move(it->second.npcID, direct));
        }
        
        current->incrementExploredCount();
        ++mCurrentNpcMapper;
    }
}

std::string BlackBoard::toStdString() const noexcept
{
    if (mLevelInfo->bOmnicientMode)
        return "no log";

    std::stringstream stream;

    size_t iter = 0;

    stream << "Turn : " << mTurnInfo.turnNb << std::endl;
    std::for_each(
        mTurnInfo.npcs.begin(),
        mTurnInfo.npcs.end(),
        [&](std::pair<unsigned int, NPCInfo> npc) {
        stream << "Npc : " << npc.second.npcID << " ("
            << npc.second.tileID << ",";
            if (mNotOmniscientPaths[iter].empty())
            {
                stream << "null)\n";
            }
            else
            {
                stream << *mNotOmniscientPaths[iter].begin() << ")\n";
            }
            ++iter;
        }
    );

    stream << mExploreGraph->toStdString() << "\n\n\n\n\n";

    return stream.str();
}