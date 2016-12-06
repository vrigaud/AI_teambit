#ifndef BLOCUPDATEACTIONLIST_H
#define BLOCUPDATEACTIONLIST_H

#include "GeneralAction.h"
#include "..\MiCoMa.h"
#include "..\Npc.h"



BehaviourTree::BaseBloc* getBlocUpdateActionList(BlackBoard &bboard)
{
	auto forUpdateLambda = [&bboard]()
    {
		std::vector<Action*> _actionList = bboard.getActionList();

        for (Npc* npc : MiCoMa::getInstance()->getNpcs())
        {
            npc->update();
            if (npc->getAction())
            {
                bboard.pushAction(npc->forwardAction()->Clone());
            }
        }
        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(forUpdateLambda);
}

#endif //!BLOCUPDATEACTIONLIST_H
