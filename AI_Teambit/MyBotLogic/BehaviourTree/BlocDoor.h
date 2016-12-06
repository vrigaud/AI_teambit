#ifndef BLOCDOORS_H
#define BLOCDOORS_H

#include <algorithm>
#include "Blackboard.h"
#include "BlocLoop.h"
#include "..\MiCoMa.h"
#include "..\Map.h"

using namespace std;

/*
	This block's purpose is to check for completely explored zone containing npcs in which there are 
	matching pressure plates and doors (or just interactive doors). Once found, we fill the blackboard
	with the information needed for opening the door and sending a scout npc in the new zone (See BlocDoorRecursion).
*/

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

            auto validZone = *find_if(begin(zoneList), end(zoneList), [](std::pair<unsigned, Zone> zone)
            {
                return zone.second.isClosed && (zone.second.mControllerOnZone.size() || zone.second.mDoorOnZone.size());
            });

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
            }); // We should check that we send the npc closest to the pressure plate or door. WON'T BE DONE.

            return res;
        }
    };

    return BehaviourTree::createAction(blocDoorLambda);
}

#endif //!BLOCDOORS_H
