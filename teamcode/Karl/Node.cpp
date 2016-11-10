/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/MyNeighbours.h"
#include "../MyClasses/AStarState/Graph.h"

#include <algorithm>

class NeighbourDirectionError {};

std::map<std::pair<Node::id_value_type, Node::id_value_type>, unsigned int> Node::Heuristics{};
std::mutex Node::hMutex;

Node::Node(id_value_type id) : id{ id }, nodeTip{ EDirection::N }, antiNodeTip{ EDirection::S }
{
	if (nodeTip == EDirection::N || nodeTip == EDirection::S)
	{
		abstractWalls[EDirection::N] = true;
		abstractWalls[EDirection::S] = true;
	}
	else if (nodeTip == EDirection::NE || nodeTip == EDirection::SW)
	{
		abstractWalls[EDirection::NE] = true;
		abstractWalls[EDirection::SW] = true;
	}
	else if (nodeTip == EDirection::E || nodeTip == EDirection::W)
	{
		abstractWalls[EDirection::E] = true;
		abstractWalls[EDirection::W] = true;
	}
	else
	{
		abstractWalls[EDirection::SE] = true;
		abstractWalls[EDirection::NW] = true;
	}

	neighbours = new MyNeighbours{ id };
}

Node::~Node()
{
	delete neighbours;
}

void Node::findNeighbours(const size_t x, const size_t y)
{
	const double MOVE_COST{ 1.0 };

	using namespace std;

	unordered_map<EDirection, bool> theDirectNeighbours = findNeighboursDirection(x, y);

	for (auto neighbour : theDirectNeighbours)
	{
		if (neighbour.second == false)
			continue;

		// The neighbour's id is based on how the grid is layed out and the row's parity
		Node::id_value_type neighbourId;
		bool isUnpairRow = (id / x) % 2 == 1;

		switch (neighbour.first)
		{
		case(EDirection::N):
			neighbourId = (((id / x) - 1) * x) + (id - ((id / x) * x));
			break;
		case(EDirection::NE):
			neighbourId = isUnpairRow ? (((id / x) - 1) * x) + (id - ((id / x) * x)) + 1 : (((id / x) - 1) * x) + (id - ((id / x) * x));
			break;
		case(EDirection::E):
			neighbourId = id + 1;
			break;
		case(EDirection::SE):
			neighbourId = isUnpairRow ? (((id / x) + 1) * x) + (id - ((id / x) * x)) + 1 : (((id / x) + 1) * x) + (id - ((id / x) * x));
			break;
		case(EDirection::S):
			neighbourId = (((id / x) + 1) * x) + (id - ((id / x) * x));
			break;
		case(EDirection::SW):
			neighbourId = isUnpairRow ? (((id / x) + 1) * x) + (id - ((id / x) * x)) : (((id / x) + 1) * x) + (id - ((id / x) * x)) - 1;
			break;
		case(EDirection::W):
			neighbourId = id - 1;
			break;
		case(EDirection::NW):
			neighbourId = isUnpairRow ? (((id / x) - 1) * x) + (id - ((id / x) * x)) : (((id / x) - 1) * x) + (id - ((id / x) * x)) - 1;
			break;
		default:
			throw NeighbourDirectionError();
		}

		neighbours->addNeighbour(neighbourId, neighbour.first, MOVE_COST);
	}
}

std::unordered_map<EDirection, bool> Node::findNeighboursDirection(const size_t x, const size_t y) noexcept
{
	using namespace std;
	unordered_map<EDirection, bool> myDirectNeighbours{};

	myDirectNeighbours[EDirection::N] = (nodeTip == EDirection::N || nodeTip == EDirection::S) ? false : true;
	myDirectNeighbours[EDirection::NE] = (nodeTip == EDirection::NE || nodeTip == EDirection::SW) ? false : true;
	myDirectNeighbours[EDirection::E] = (nodeTip == EDirection::E || nodeTip == EDirection::W) ? false : true;
	myDirectNeighbours[EDirection::SE] = (nodeTip == EDirection::SE || nodeTip == EDirection::NW) ? false : true;
	myDirectNeighbours[EDirection::S] = (nodeTip == EDirection::N || nodeTip == EDirection::S) ? false : true;
	myDirectNeighbours[EDirection::SW] = (nodeTip == EDirection::NE || nodeTip == EDirection::SW) ? false : true;
	myDirectNeighbours[EDirection::W] = (nodeTip == EDirection::E || nodeTip == EDirection::W) ? false : true;
	myDirectNeighbours[EDirection::NW] = (nodeTip == EDirection::SE || nodeTip == EDirection::NW) ? false : true;

	if (id%x == (x - 1))
	{
		myDirectNeighbours[EDirection::E] = false;
		abstractWalls[EDirection::E] = true;
		if (((id / x) % 2) == 1)
		{
			myDirectNeighbours[EDirection::NE] = false;
			abstractWalls[EDirection::NE] = true;
			myDirectNeighbours[EDirection::SE] = false;
			abstractWalls[EDirection::SE] = true;
		}
	}

	if (id%x == 0)
	{
		myDirectNeighbours[EDirection::W] = false;
		abstractWalls[EDirection::W] = true;
		if (((id / x) % 2) != 1)
		{
			myDirectNeighbours[EDirection::NW] = false;
			abstractWalls[EDirection::NW] = true;
			myDirectNeighbours[EDirection::SW] = false;
			abstractWalls[EDirection::SW] = true;
		}
	}

	if (id < x)
	{
		myDirectNeighbours[EDirection::NW] = false;
		abstractWalls[EDirection::NW] = true;
		myDirectNeighbours[EDirection::N] = false;
		abstractWalls[EDirection::N] = true;
		myDirectNeighbours[EDirection::NE] = false;
		abstractWalls[EDirection::NE] = true;
	}

	if (id >= ((x*y) - x))
	{
		myDirectNeighbours[EDirection::SE] = false;
		abstractWalls[EDirection::SE] = true;
		myDirectNeighbours[EDirection::S] = false;
		abstractWalls[EDirection::S] = true;
		myDirectNeighbours[EDirection::SW] = false;
		abstractWalls[EDirection::SW] = true;
	}

	return myDirectNeighbours;
}

// Move cost
double Node::checkTransition(Node::id_value_type to, const TurnInfo& _turnInfo)
{
	double res{};

	if (isThereAWall(neighbours->getNeighbourDirection(to)))
	{
		res = INFINITY;
	}

	return res;
}

bool Node::isThereAWall(EDirection dir)
{
	bool isThere{};

	try
	{
		isThere = abstractWalls.at(dir);
	}
	catch (...)
	{}

	return isThere;
}

void Node::peekedAtSchrodingersWall(EDirection dir, bool deadAlive)
{
	abstractWalls[dir] = deadAlive;
}

bool Node::allInfo()
{
	return abstractWalls.size() == NB_WALLS;
}

bool Node::isTowardsTip(EDirection dir)
{
	return (dir == nodeTip || dir == antiNodeTip) ? true : false;
}

unsigned int Node::getHeuristicTo(Node::id_value_type t)
{
	// Euclidian distance (squared)

	Node::id_value_type smaller{}, bigger{};

	(id < t) ? (smaller = id, bigger = t) : (smaller = t, bigger = id);

	std::pair<Node::id_value_type, Node::id_value_type> hPair{ smaller,bigger };

	{
		std::lock_guard<std::mutex> lock(Node::hMutex);

		if (Node::Heuristics.find(hPair) == Node::Heuristics.end())
		{
			auto w = Graph::getInstance().width;

			auto bY = (bigger - ((bigger / w) * w));
			auto sY = (smaller - ((smaller / w) * w));
			unsigned int dx = bY > sY ? bY - sY : sY - bY;

			unsigned int dy{ (bigger / w) - (smaller / w) };

			Node::Heuristics[hPair] = dx*dx + dy*dy;
		}
	}

	return Node::Heuristics.at(hPair);
}