#ifndef GRAPH_H
#define GRAPH_H

#include "Incopiable.h"
#include "LevelInfo.h"
#include <algorithm>
#include <vector>
#include <list>

class Heuristic;
class Node;
class NPC;

struct TileInfo;
struct TurnInfo;

class Graph : Incopiable
{
public: 
    using cost_type = unsigned int;
	using map_type = std::map<unsigned int, Node*>;
	const cost_type CONNECTION_COST{ 10 };

private:
    static Graph singletonGraph;

	map_type mGraph;
    std::vector<Node*> mTargets;
	LevelInfo mLevelInfo;

    Graph() noexcept;
	void addNode(Node* node);
    void connectSurroundings();
	void connectionEvenLinesOnRight(const unsigned int& idNode, const int& idNeighbour);

public : 
	~Graph();
    static Graph& Instance() noexcept { return singletonGraph; }
    void init(const LevelInfo&);
	void initNodeFlags();

	void updateMapInfo(TurnInfo& turnInfo);
	void updateNpcGoal(const std::vector<NPC*> &npcs);

	map_type getGraph() const noexcept { return mGraph; }
	LevelInfo getLevelInfo() const { return mLevelInfo; }
	Node* getNode(int i) const { return mGraph.at(i); }
	Node* getNode(int i) { return mGraph[i]; }
    std::vector<Node*> getTargetList() const noexcept { return mTargets; }

    bool isEmpty() const noexcept { return mGraph.empty(); }

};

#endif // !GRAPH_H

#ifndef NODE_RECORD_H
#define NODE_RECORD_H

struct NodeRecord
{
    using cost_type = Graph::cost_type;

    Node* mNode;
    NodeRecord* mPrevious;
    cost_type mCostSoFar;
    cost_type mEstimatedTotalCost;
    
    NodeRecord()
        : mNode{ nullptr }, mPrevious{ nullptr }, mCostSoFar{ 0 }, mEstimatedTotalCost{ 0 }/*, mState{ UNVISITED }, mNextRecord{ nullptr }*/
    {}
    NodeRecord(NodeRecord* other)
        : mNode{ other->mNode }, mPrevious{ other->mPrevious }, mCostSoFar{ other->mCostSoFar }, mEstimatedTotalCost{ other->mEstimatedTotalCost }
    {}
    NodeRecord(Node* n, const cost_type& costSoFar, const cost_type& estimatedCost)
        : mNode{ n }, mPrevious{ nullptr }, mCostSoFar{ costSoFar }, mEstimatedTotalCost{ estimatedCost }/*, mState{ UNVISITED }, mNextRecord{}*/
    {}
	~NodeRecord() {}

    bool operator<(NodeRecord* other)
    {
        return mEstimatedTotalCost < other->mEstimatedTotalCost;
    }
    bool operator==(NodeRecord* other)
    {
        return mNode == other->mNode;
    }

    bool empty() { return !mNode; }

    static NodeRecord* findIn(std::list<NodeRecord*> nrList, Node* n)
    {
        for (auto nr : nrList)
        {
            if (nr->mNode == n)
                return nr;
        }

        return nullptr;
    }
};

#endif // !NODE_RECORD_H