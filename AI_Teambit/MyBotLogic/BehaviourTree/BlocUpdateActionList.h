#ifndef BLOCUPDATEACTIONLIST_H
#define BLOCUPDATEACTIONLIST_H

#include "Npc.h"
#include "GeneralAction.h"
#include "Blackboard.h"


BehaviourTree::BaseBloc* getBlocUpdateActionList()
{

    auto forUpdateLambda = []()
    {
        std::vector<Action*> _actionList = BlackBoard::getInstance()->getActionList();

        for (Npc* npc : MiCoMa::getInstance()->getNpcs())
        {
            npc->update();
            if (npc->getAction())
            {
                _actionList.push_back(npc->getAction()->Clone());
            }
        }
        BlackBoard::getInstance()->setActionList(_actionList);
        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(forUpdateLambda);
}

#endif //!BLOCUPDATEACTIONLIST_H
