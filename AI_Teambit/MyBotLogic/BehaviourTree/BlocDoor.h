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
            
//             MiCoMa* ourMicoma = MiCoMa::getInstance();
//             std::vector<Npc*> ourNpcs = ourMicoma->getNpcs();
// 
//             std::vector<Npc*> npcs{};
//             npcs.reserve(ourMicoma->getNpcs().size());
// 
//             std::vector<Node*> goals{};
//             goals.reserve(ourMap->getGoalIDs().size());
// 
//             unsigned int currentRecursionZoneId = bboard.getZoneIdRecursion().top();
// 
//             copy_if(begin(ourNpcs), end(ourNpcs), back_inserter(npcs), [&](Npc* npc)
//             {
//                 return ourMap->getNode(npc->getCurrentTile())->getZoneID() == currentRecursionZoneId;
//             });
// 
//             auto currentZoneNodes = ourMap->getZoneList()[currentRecursionZoneId].mNodeOnZone;
//             copy_if(begin(currentZoneNodes), end(currentZoneNodes), back_inserter(goals), [&](Node* n)
//             {
//                 return (n->getZoneID() == currentRecursionZoneId) && (n->getType() == Node::GOAL);
//             });

            auto validZone = *find_if(begin(zoneList), end(zoneList), [](std::pair<unsigned, Zone> zone)
            {
                return zone.second.isClosed && (zone.second.mControllerOnZone.size() || zone.second.mDoorOnZone.size());
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
