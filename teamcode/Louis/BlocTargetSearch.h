#ifndef BLOCTARGETSEARCH_H
#define BLOCTARGETSEARCH_H

#include "BlackBoard.h"
#include "GeneralAction.h"
#include "ExploreNode.h"
#include "NPCTarget.h"



BehaviourTree::BaseBloc* getBlocTargetSearch(BlackBoard& blackBoard)
{
    auto targetSearchLambda = [&]() 
    {
        auto npcIter = blackBoard.currentNpcMapped();

        if (npcIter == blackBoard.getTurnInfo().npcs.end())
        {
            return BehaviourTree::general::result::FAIL;
        }

        ExplorationPart::TargetArray* targetArray = &blackBoard.getExplorationGraph().getMapperTargetNPC();

        if (targetArray->getAllTarget().empty())
        {
            return BehaviourTree::general::result::FAIL;
        }

        ExplorationPart::NPCTarget* npcTarget = targetArray->find(npcIter->second.npcID);

        if (npcTarget->hasTarget())
        {
            return BehaviourTree::general::result::SUCCESS;
        }

        unsigned int currentNpcManhatthan;

        for (size_t iter = 0; iter < targetArray->getAllTarget().size(); ++iter)
        {
            if (!targetArray->getAllTarget()[iter].isTargeted)
            {
                currentNpcManhatthan = ExploreNode::manhattanDistance(
                    npcIter->second.tileID,
                    targetArray->getAllTarget()[iter].targetNode->getID(),
                    blackBoard.getLevelInfo()
                );

                bool goodTarget = true;

                for (
                    std::map<unsigned int, NPCInfo>::const_iterator it = blackBoard.getTurnInfo().npcs.begin(); 
                    it != blackBoard.getTurnInfo().npcs.end();
                    ++it
                    )
                {
                    if (ExploreNode::manhattanDistance(
                        it->second.tileID,
                        targetArray->getAllTarget()[iter].targetNode->getID(),
                        blackBoard.getLevelInfo()
                    ) < currentNpcManhatthan)
                    {
                        goodTarget = false;
                        break;
                    }
                }

                if (goodTarget)
                {
                    npcTarget->changeTarget(&targetArray->getAllTarget()[iter]);
                    return BehaviourTree::general::result::SUCCESS;
                }
            }
        }

        return BehaviourTree::general::result::FAIL;
    };

    return BehaviourTree::createAction(targetSearchLambda);
}

#endif //!BLOCTARGETSEARCH_H
