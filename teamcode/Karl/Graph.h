/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __GRAPH_C__
#define __GRAPH_C__

#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/MyBot.h"

#include "TurnInfo.h"

#include <unordered_map>
#include <set>


/*
	This class represents the map onto which the npcs move. It serves as an information hub for the bots, nodes, etc...
*/
class Graph
{
	Graph() = default;

	void deleteGraph();
public:
	Graph(const Graph&) = delete;
	Graph& operator=(const Graph&) = delete;
	~Graph();

	//Get the singleton's instance
	static Graph& getInstance() noexcept
	{
		static Graph instance{};
		return instance;
	}

	unsigned int width;
	unsigned int height;

	std::unordered_map<unsigned int, MyBot*> bots;
	std::unordered_map<Node::id_value_type, Node*> nodes;
	std::set<Node::id_value_type> targets;

	void build(const size_t x, const size_t y);
	void refreshBots(const TurnInfo&);
	void findVisibleTargets(const TurnInfo&);
	void refreshVisibleInfo(const TurnInfo&);
};

#endif //__GRAPH_C__
