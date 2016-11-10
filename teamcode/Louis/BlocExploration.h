#ifndef BLOCEXPLORATION_H
#define BLOCEXPLORATION_H

#include "BlackBoard.h"
#include "GeneralAction.h"
#include "ExploreNode.h"

#include <algorithm>


BehaviourTree::BaseBloc* getBlocExplore(BlackBoard& blackBoard)
{
    auto exploreLambda = [&]()
    {
        auto npcIter = blackBoard.currentNpcMapped();

        if (npcIter == blackBoard.getTurnInfo().npcs.end())
        {
            return BehaviourTree::general::result::FAIL;
        }

        ExploreNode* current = blackBoard.getExplorationGraph().getGraph()[npcIter->second.tileID];

        ExploreNode* nodeToGo = *current->getNeighbours().begin();



        for(auto neighbour = current->getNeighbours().begin();
            neighbour != current->getNeighbours().end();
            ++neighbour)
        {
            if ((*neighbour)->isForbidden())
            {
                continue;
            }
            
            if (nodeToGo->getExploredCount() > (*neighbour)->getExploredCount() || 
                nodeToGo->isForbidden())
            {
                nodeToGo = *neighbour;
            }
        }

        
        if (nodeToGo->isForbidden())
        {
            blackBoard.emptyCurrentNpcPath();
            return BehaviourTree::general::result::FAIL;
        }

        blackBoard.emptyCurrentNpcPath();
        blackBoard.addAPathAt(std::list<unsigned int>{ nodeToGo->getID() });

        return BehaviourTree::general::result::SUCCESS;
    };

    return BehaviourTree::createAction(exploreLambda);
}

#endif //!BLOCEXPLORATION_H
