/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#include "../MyClasses/AStarState/MyNeighbours.h"
#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/FromTo.h"
#include "Globals.h"

#include <algorithm>

class NeighbourNotFound {};

MyNeighbours::MyNeighbours(Node::id_value_type id) : fromId{ id } {}

EDirection MyNeighbours::getNeighbourDirection(const Node::id_value_type id) const
{
	auto dir = id_dir.find(id);
	return dir == id_dir.end() ? throw NeighbourNotFound{} : dir->second;
}

Node::id_value_type MyNeighbours::getNeighbourId(const EDirection dir) const
{
	auto id = dir_id.find(dir);
	return id == dir_id.end() ? throw NeighbourNotFound{} : id->second;
}

void MyNeighbours::addNeighbour(const Node::id_value_type toId, const EDirection toDir)
{
	id_dir[toId] = toDir;
	dir_id[toDir] = toId;
	fromMoves[toDir] = FromTo{ fromId, toId};
}

void MyNeighbours::addNeighbour(const Node::id_value_type toId, const EDirection toDir, const double toCost)
{
	addNeighbour(toId, toDir);
	fromMoves[toDir] = FromTo{ fromId, toId, toCost };
}

double MyNeighbours::neighbourCost(const Node::id_value_type toId)
{
	auto dir = id_dir.find(toId);
	return dir == id_dir.end() ? throw NeighbourNotFound{} : fromMoves[dir->second].cost;
}

double MyNeighbours::neighbourCost(const EDirection toDir)
{
	auto id = dir_id.find(toDir);
	return id == dir_id.end() ? throw NeighbourNotFound{} : fromMoves[toDir].cost;
}

std::vector<Node::id_value_type> MyNeighbours::getNeighboursList() const
{
	std::vector<Node::id_value_type> res{};

	for_each(id_dir.begin(), id_dir.end(), [&res](auto& id) {res.push_back(id.first);});

	return res;
}
