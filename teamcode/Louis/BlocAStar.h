#ifndef BLOCASTAR_H
#define BLOCASTAR_H

#include "BlackBoard.h"
#include "GeneralAction.h"
#include "ExploreNode.h"



BehaviourTree::BaseBloc* getBlocAStar(BlackBoard& blackboard)
{
    const size_t maxAStarPath = 120;

    auto aStarLambda = [&, maxAStarPath]()
    {
        auto npcIter = blackboard.currentNpcMapped();

        if (npcIter == blackboard.getTurnInfo().npcs.end())
        {
            return BehaviourTree::general::result::FAIL;
        }

        ExploreNode& start = *blackboard.getExplorationGraph().getGraph()[npcIter->second.tileID];
        ExploreNode& goal = *blackboard.getExplorationGraph().getMapperTargetNPC().find(npcIter->second.npcID)->getTarget()->targetNode;

        std::list<ExploreNode>closedNodes{};

        start.setParent(nullptr);
        std::list<ExploreNode>openedNode;

        openedNode.push_back(start);

        size_t nodeCount = 0;

        while (!openedNode.empty() && nodeCount < maxAStarPath)
        {
            ExploreNode current = (*std::min_element(openedNode.begin(), openedNode.end()));

            blackboard.emptyCurrentNpcPath();

            openedNode.erase(std::find(openedNode.begin(), openedNode.end(), current));

            if ((current.getX(blackboard.getLevelInfo()) == goal.getX(blackboard.getLevelInfo())) && 
                (current.getY(blackboard.getLevelInfo()) == goal.getY(blackboard.getLevelInfo())))
            {
                std::list<unsigned int> result;
                ExploreNode* curr = &current;

                while (curr->getParent() != nullptr)
                {
                    result.push_back(curr->getID());
                    curr = curr->getParent();
                }

                result.reverse();

                blackboard.addAPathAt(result);

                return BehaviourTree::general::result::SUCCESS;
            }

            for (size_t i = 0; i < current.getNeighbours().size(); ++i)
            {
                ExploreNode* v = current.getNeighbours()[i];

                if (v->isForbidden())
                {
                    continue;
                }

                v->setParent(&current);


                auto testClosed = std::find(closedNodes.begin(), closedNodes.end(), *v);
                auto testOpened = std::find(openedNode.begin(), openedNode.end(), *v);

                if (
                    (testClosed != closedNodes.end() && 
                    ((*testClosed).getCost() < v->getCost()) && 
                    (*testClosed).getExploredCount() < v->getExploredCount()) 
                    ||
                    (testOpened != openedNode.end() && 
                    ((*testOpened).getCost() < v->getCost()) && 
                    (*testOpened).getExploredCount() < v->getExploredCount())
                    )
                {
                    continue;
                }

                v->setCost(current.getCost() + 1);
                v->setHeuristic(v->getCost() + v->manhattanDistance(&goal, blackboard.getLevelInfo()));
                openedNode.push_back(*v);
            }

            closedNodes.push_back(current);

            ++nodeCount;
        }

        blackboard.emptyCurrentNpcPath();
        return BehaviourTree::general::result::FAIL;
    };

    return BehaviourTree::createAction(aStarLambda);
}


#endif //!BLOCASTAR_H