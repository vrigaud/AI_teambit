#ifndef GRAPH_H
#define GRAPH_H

#include "Incopiable.h"
#include <algorithm>
#include <vector>
#include <list>

class Node;
struct LevelInfo;
struct TurnInfo;
struct TileInfo;

struct Target
{
    bool isTargeted;
    TileInfo* targetTile;

    Target(TileInfo* tile) :
        isTargeted{false},
        targetTile{tile}
    {}
};

class Graph : Incopiable
{


    static Graph singletonGraph;


    int mRowCount, mColCount;
    std::vector<Node*> mGraph;
    std::vector<Target> mTargets;

    Graph() noexcept;
    void addNode(Node* &&node) { mGraph.emplace_back(node); }
    void connectSurroundings(LevelInfo&);

    void connectionEvenLinesOnRight(std::pair<unsigned int, TileInfo>, int);

public:

    ~Graph() {}
    static Graph& getSingleton() noexcept { return singletonGraph; }

    void createGraph(LevelInfo& levelInfo);
    void init(LevelInfo&);

    bool isEmpty() { return mGraph.empty(); }

    std::vector<Target>& getTargets()
    {
        return mTargets;
    }

    std::vector<Node*> getGraph()
    {
        return mGraph;
    }

    std::list<Node> pathFinder(Node& start, Node& goal, const LevelInfo& levelInfo);


};



#endif // !GRAPH_H


