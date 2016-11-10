#ifndef EXPLORATION_GRAPH
#define EXPLORATION_GRAPH

#include "NPCTarget.h"

#include <algorithm>
#include <vector>
#include <list>

#include <string>

class ExploreNode;
struct LevelInfo;
struct TurnInfo;
struct TileInfo;

namespace ExplorationPart
{
    class ExplorationGraph
    {
        unsigned int mRowCount;
        unsigned int mColCount;
        std::vector<ExploreNode*> mGraph;
        TargetArray* mTargetArray;

        void addNode(ExploreNode* &&node) { mGraph.emplace_back(node); }

    public:
        ExplorationGraph() = default;
        ~ExplorationGraph()
        {
            delete mTargetArray;
        }

        void createExplorationGraph();
        void init(LevelInfo&);

        bool isEmpty() { return mGraph.empty(); }

        std::vector<Target>& getTargets()
        {
            return mTargetArray->getAllTarget();
        }

        std::vector<ExploreNode*>& getGraph()
        {
            return mGraph;
        }

        //std::list<ExploreNode> pathFinder(ExploreNode& start, ExploreNode& goal, const LevelInfo& levelInfo);

        TargetArray& getMapperTargetNPC() noexcept
        {
            return *mTargetArray;
        }

        std::string toStdString() const noexcept;

        void updateExploreNode(const TurnInfo &turnInfo);

        ExploreNode* chooseTile(unsigned int current, EDirection wallDirection);


    private:
        void ExplorationGraph::connectSurroundings();

        void ExplorationGraph::connectionEvenLinesOnRight(size_t tileId, size_t posPrevNode);
    };
}


#endif // !EXPLORATION_GRAPH
