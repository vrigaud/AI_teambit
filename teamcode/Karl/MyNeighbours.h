/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __MY_NEIGHBOUR__
#define __MY_NEIGHBOUR__

#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/FromTo.h"
#include "Globals.h"

#include <unordered_map>
#include <vector>

/*
	This class is used by the Node class to keep track of all the information pertaining to its neighbours.
*/
class MyNeighbours
{
	Node::id_value_type fromId;
	std::unordered_map<EDirection, FromTo> fromMoves;

	std::unordered_map<Node::id_value_type, EDirection> id_dir;
	std::unordered_map<EDirection, Node::id_value_type> dir_id;
public:
	MyNeighbours() = default;
	MyNeighbours(Node::id_value_type id);

	EDirection getNeighbourDirection(const Node::id_value_type id) const;
	Node::id_value_type getNeighbourId(const EDirection dir) const;
	void addNeighbour(const Node::id_value_type toId, const EDirection toDir);
	void addNeighbour(const Node::id_value_type toId, const EDirection toDir, const double toCost);
	double neighbourCost(const Node::id_value_type toId);
	double neighbourCost(const EDirection toDir);
	std::vector<Node::id_value_type> getNeighboursList() const;
};

#endif //__MY_NEIGHBOUR__
