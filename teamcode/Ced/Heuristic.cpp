#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"


Heuristic::cost_type Heuristic::estimate(Node * start, const LevelInfo& lvlInfo)
{
    return manhattanDistance(start, mGoalNode, lvlInfo);
}

Heuristic::cost_type Heuristic::manhattanDistance(Node* start, Node* goal, const LevelInfo& lvlInfo)
{
    return subUInt(start->getID() % lvlInfo.colCount, goal->getID() % lvlInfo.colCount)
    + subUInt(start->getID() / lvlInfo.colCount, goal->getID() / lvlInfo.colCount);
}