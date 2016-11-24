#ifndef BLOCFINISHED_H
#define BLOCFINISHED_H

#include "BlackBoard.h"
#include "GeneralAction.h"
#include "ExploreNode.h"

#include <algorithm>


BehaviourTree::BaseBloc* getBlocFinished(BlackBoard& blackBoard)
{
    auto finishedLambda = [&]()
    {
        auto npcIter = blackBoard.currentNpcMapped();

        if (npcIter == blackBoard.getTurnInfo().npcs.end())
        {
            return BehaviourTree::general::result::FAIL;
        }
        
        auto targetArray = blackBoard.getExplorationGraph().getMapperTargetNPC().find(npcIter->second.npcID);
        
        auto targetNode = targetArray->getTarget()->targetNode;

        if (targetNode && npcIter->second.tileID == targetNode->getID())
        {
            blackBoard.emptyCurrentNpcPath();
            return BehaviourTree::general::result::SUCCESS;
        }

        return BehaviourTree::general::result::FAIL;
    };

    return BehaviourTree::createAction(finishedLambda);
}

#endif //!BLOCFINISHED_H
