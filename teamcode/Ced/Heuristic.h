#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Graph.h"

class Node;

class Heuristic
{
    Node* mGoalNode;

    //Subtracts max(a, b) with min(a,b)
    static unsigned int subUInt(const unsigned int& a, const unsigned int& b)
    {
        return std::max(a, b) - std::min(a, b) /*(a < b) ? (b-a) : (a-b)*/;
    }

public:
    using cost_type = Graph::cost_type;
    Heuristic(Node* goal) : mGoalNode{ goal } {}
    ~Heuristic() {};

    cost_type estimate(Node* start, const LevelInfo&);

private:
    cost_type manhattanDistance(Node* start, Node* goal, const LevelInfo&);

};

#endif // !HEURISTIC_H