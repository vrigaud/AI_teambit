/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __FROMTO__
#define __FROMTO__

#include "../MyClasses/AStarState/Node.h"

/*
	This struct embodies a transition or move from one tile (from) to another (to) with a certain cost (cost)...
*/

struct FromTo
{
	Node::id_value_type from;
	Node::id_value_type to;
	double cost;

	FromTo() = default;
	FromTo(Node::id_value_type f, Node::id_value_type t, double c = INFINITY) noexcept : from{ f }, to{ t }, cost{ c } {};

	friend bool operator<(const FromTo& ft0, const FromTo& ft1)
	{
		return ft0.to == ft1.to && ft0.from < ft1.from;
	}
};

/*
	This functor is required in order to keep the FromTos sorted in A*'s "open" list
*/
struct FromToComparator
{
	bool operator() (const FromTo& ft0, const FromTo& ft1) const
	{
		return ft0.cost < ft1.cost;
	}
};

#endif // __FROMTO__
