#ifndef GRAPH_H
#define GRAPH_H
#include "Globals.h"
#include "LevelInfo.h"
#include <vector>
#include <array>
#include "TurnInfo.h"

struct NODE_INFO {

	unsigned int id;
	unsigned int gScore = { std::numeric_limits<unsigned int>::max() - 500 }; // Reduit valeur maximale du a un crash.
	unsigned int fScore = { std::numeric_limits<unsigned int>::max() - 500 }; // Reduit valeur maximale du a un crash.
	unsigned int parent;
};

class Graph
{

public:
    Graph(std::vector<unsigned int> nodes, std::vector<std::array<unsigned int, 4>> edges,LevelInfo& lvl);
    Graph::Graph();
    ~Graph();
	void updateGraph(TurnInfo& turnInfo, std::vector<unsigned int> &goal, std::vector<unsigned int> &occupiedTiles, LevelInfo levelInfo, Graph& graph);
    unsigned int length(unsigned int firstNode, unsigned int sndNode);
    std::vector<EDirection> bestPath(unsigned int firstNode, unsigned int sndNode, TurnInfo& turnInfo, unsigned int playedId);
	std::vector<EDirection> reversePath(unsigned int firstNode, unsigned int sndNode, std::map<unsigned int, NODE_INFO> ni);
    unsigned int size() { return m_nodes.size(); }
	std::vector<std::array<unsigned int, 2>> Graph::neighbours(unsigned int otherNode);
private:
	std::vector<unsigned int> m_nodes;
	std::vector<std::array<unsigned int, 4>> m_edges;
	LevelInfo m_lvl;
    void setEdges  (std::vector<std::array<unsigned int, 4>> edges);
    void setNodes (std::vector<unsigned int> nodes);
	void setLvl(LevelInfo& lvl);
};
#endif