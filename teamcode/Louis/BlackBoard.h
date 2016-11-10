#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <vector>
#include <list>

#include "BlocSelect.h"

#include "ExplorationGraph.h"
#include "ExploreNode.h"

#include "LevelInfo.h"
#include "TurnInfo.h"
#include "Node.h"

struct Action;

class isOmniscient{};
class notOmniscient{};


class BlackBoard
{
    //BT root
    BehaviourTree::BlocSelect mRoot;


    //Attributs dont on aura besoin
    std::vector<std::list<Node>> mPaths;
    std::vector<std::list<unsigned int>> mNotOmniscientPaths;

    LevelInfo* mLevelInfo;
    TurnInfo mTurnInfo;

    ExplorationPart::ExplorationGraph* mExploreGraph;

    size_t mCurrentNpcMapper;

    void (BlackBoard::*setAction)(std::vector<Action*>& inputAction);

public:    

    BlackBoard(const LevelInfo& _levelInfo);
    ~BlackBoard() 
    {
        if (!mLevelInfo->bOmnicientMode)
        {
            delete mExploreGraph;
        }

        delete mLevelInfo;
    }

    //TODO : algo de recherche BT

    TurnInfo& getTurnInfo() noexcept
    {
        return mTurnInfo;
    }

    LevelInfo& getLevelInfo() noexcept
    {
        return *mLevelInfo;
    }

    size_t getCurrentNpcMapperValue() const noexcept
    {
        return mCurrentNpcMapper;
    }

    void createBTWhenNotOmniscient();
    
    void update(const TurnInfo& infoThisTurn)
    {
        mCurrentNpcMapper = 0;
        mTurnInfo = infoThisTurn;
        
        if (!mLevelInfo->bOmnicientMode)
        {  
            mExploreGraph->updateExploreNode(infoThisTurn);
        }
    }

    void giveActionForThisTurn(std::vector<Action*>& inputAction);

    std::map<unsigned int, NPCInfo>::iterator currentNpcMapped()
    {
        auto npcIter = getTurnInfo().npcs.begin();

        for (
            size_t npcCounter = 0;
            npcCounter != getCurrentNpcMapperValue();
            ++npcCounter
            )
        {
            npcIter = std::next(npcIter);
        }

        return npcIter;
    }

    ExplorationPart::ExplorationGraph& getExplorationGraph()
    {
        return *mExploreGraph;
    }

    void addAPathAt(std::list<unsigned int>& path)
    {
        std::for_each(
            path.begin(),
            path.end(),
            [&](unsigned int nodeId) {
                mNotOmniscientPaths[mCurrentNpcMapper].push_back(nodeId);
            }
        );
    }

    void emptyCurrentNpcPath()
    {
        mNotOmniscientPaths[mCurrentNpcMapper] = std::list<unsigned int>{};
    }

    std::string toStdString() const noexcept;

private:
    void initWhenOmniscient();
    void initWhenNotOmniscient();

    unsigned int getTargetForNPC(const std::pair<unsigned int, NPCInfo>&npc);

    EDirection chooseDirection(unsigned int current, unsigned int next);


    Node* fromListAt(size_t iter, std::list<Node>& list);

    void giveActionWhenOmniscient(std::vector<Action*>& inputAction);
    void giveActionWhenNotOmniscient(std::vector<Action*>& inputAction);
};

#endif // !BLACKBOARD_H
