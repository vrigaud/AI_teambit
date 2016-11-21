#ifndef BLOCFORUPDATE_H
#define BLOCFORUPDATE_H

#include "Npc.h"
#include "MiCoMa.h"
#include "GeneralAction.h"

BehaviourTree::BaseBloc* getBlocForUpdate(MiCoMa& micoma, std::vector<Action*>& _actionList)
{
    auto forUpdateLambda = [&]()
    {
        for (Npc* npc : MiCoMa::getInstance()->getNpcs())
        {
            npc->update();
            if (npc->getAction())
            {
                _actionList.push_back(npc->getAction()->Clone());
            }
        }
        return BehaviourTree::result::SUCCESS;
    };

    return BehaviourTree::createAction(forUpdateLambda);
}

#endif //!BLOCFORUPDATE_H
