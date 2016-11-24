/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#include "../MyClasses/AStarState/Paths.h"

#include <limits>

class NoMoreTargets {};

void Paths::addNodeToTargetPath(Node::id_value_type t, Node::id_value_type n)
{
	targetPaths[t].second.push_back(n);
}

void Paths::clearNodesToTargets()
{
	for (auto target{ targetPaths.begin() }; target != targetPaths.end();)
	{
		target = targetPaths.erase(target);
	}
}

void Paths::clearNodesToTargetsOtherThan(Node::id_value_type t)
{
	for (auto target{targetPaths.begin()}; target != targetPaths.end();)
	{
		if (target->first != t)
		{
			target = targetPaths.erase(target);
		}
		else
		{
			++target;
		}
	}
}

Node::id_value_type Paths::getNextNodeToTarget(Node::id_value_type t)
{
	Node::id_value_type next;
	try
	{
		auto& currentPath = targetPaths[t];

		next = currentPath.second[currentPath.first++];

		clearNodesToTargetsOtherThan(t);
	}
	catch (...){}

	return next;
}

Node::id_value_type Paths::peekNextTileToTarget(Node::id_value_type t)
{
	return targetPaths[t].second[targetPaths[t].first];
}

size_t Paths::getPathSizeToTarget(Node::id_value_type t)
{
	auto size = targetPaths[t].second.size();
	return size == 0 ? std::numeric_limits<size_t>::max() : size;
}

Node::id_value_type Paths::findNextBestTarget(Node::id_value_type t)
{
	auto nextBest = targetPaths.upper_bound(t);

	return nextBest == targetPaths.end() ? throw NoMoreTargets{} : nextBest->first;
}

bool Paths::hasTarget() const
{
	return !targetPaths.empty();
}