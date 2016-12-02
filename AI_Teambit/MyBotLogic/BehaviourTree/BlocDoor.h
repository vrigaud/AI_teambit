#ifndef BLOCDOORS_H
#define BLOCDOORS_H

#include <algorithm>
#include "Blackboard.h"
#include "BlocLoop.h"
#include "..\MiCoMa.h"
#include "..\Map.h"

using namespace std;


BehaviourTree::BaseBloc* getBlocDoor(BlackBoard &bboard)
{
    auto blocDoorLambda = [&bboard]()
    {

        Map* ourMap = Map::getInstance();
        MiCoMa* ourMicoma = MiCoMa::getInstance();

        if (!ourMap->getHasDoors())
        {
            return BehaviourTree::result::FAIL;
        }
        else if (bboard.getZoneIdRecursion().size())
        {
            return BehaviourTree::result::SUCCESS;
        }
        else
        {
            auto zoneList = ourMap->getZoneList();
            auto validZone = *find_if(begin(zoneList), end(zoneList), [](auto zone)
            {
                return zone.second.isClosed && zone.second.mControllerOnZone.size();
            });

            //validzone peut il etre == end ou nullptr??

            BehaviourTree::result res = BehaviourTree::result::FAIL;

            auto ourNpcs = ourMicoma->getNpcs();
            for_each(begin(ourNpcs), end(ourNpcs), [&](Npc* npc)
            {
                if (ourMap->getNode(npc->getCurrentTile())->getZoneID() == validZone.first)
                {
                    bboard.setPpNpc(npc);
                    bboard.getZoneIdRecursion().push(validZone.first);
                    
                    res = BehaviourTree::result::SUCCESS;
                }
            }); // we have to be sure that the npc is the closer to the pp TODO - DO IT

            return res;
        }
    };

    return BehaviourTree::createAction(blocDoorLambda);
}

#endif //!BLOCDOORS_H
