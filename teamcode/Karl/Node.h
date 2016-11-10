/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __NODE_C__
#define __NODE_C__

#include "Globals.h"
#include "TurnInfo.h"

#include <unordered_map>
#include <map>
#include <mutex>

class MyNeighbours;

/*
	This class represents a tile and its information (walls, neighbours, etc.) and acts as a Graph's node...
*/
struct Node
{
	using id_value_type = unsigned int;
	enum {NB_WALLS = 8};

	Node(id_value_type id);
	~Node();

	id_value_type id;
	EDirection nodeTip;//Tile orientation
	EDirection antiNodeTip;
	MyNeighbours* neighbours;

	void findNeighbours(const size_t, const size_t);
	double checkTransition(Node::id_value_type, const TurnInfo&);
	unsigned int getHeuristicTo(Node::id_value_type);
	bool isThereAWall(EDirection);
	void peekedAtSchrodingersWall(EDirection, bool alive = true); //We don't know if there's a wall until we take a look at it...
	bool allInfo(); //Do we know everything about this node
	bool isTowardsTip(EDirection);

private:

	std::unordered_map<EDirection, bool> findNeighboursDirection(const size_t, const size_t) noexcept;
	std::unordered_map<EDirection, bool> abstractWalls; //Because some walls are not really walls (i.e. map edges, forbidden tiles, lack of walls, etc..)

	static std::map<std::pair<Node::id_value_type, Node::id_value_type>, unsigned int> Node::Heuristics;
	static std::mutex hMutex;
};

#endif //__NODE_C__
