/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#ifndef __PATHS__
#define __PATHS__

#include "../MyClasses/AStarState/Node.h"

#include <map>
#include <vector>

/*
	This class stores the different paths that lead to a target and how to access that information...
*/
class Paths
{
	std::map<Node::id_value_type, std::pair<unsigned int, std::vector<Node::id_value_type>>> targetPaths; //Path to each of the targets
	void clearNodesToTargetsOtherThan(Node::id_value_type);
public:
	
	void addNodeToTargetPath(Node::id_value_type, Node::id_value_type);
	Node::id_value_type getNextNodeToTarget(Node::id_value_type);
	Node::id_value_type peekNextTileToTarget(Node::id_value_type);
	size_t getPathSizeToTarget(Node::id_value_type);
	Node::id_value_type findNextBestTarget(Node::id_value_type);
	bool hasTarget()const;
	size_t targetNb()const { return targetPaths.size(); };
	void clearNodesToTargets();
};

#endif // __PATHS__
