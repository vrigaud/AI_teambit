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
    //Map* mapRef = Map::getInstance();
    //Npc* currentNpc = *MiCoMa::getInstance()->getNpcs().begin();
    //auto currentNode = mapRef->getNode(currentNpc->getID());
    //std::vector<Node*> npcsOnOurZone{};
    //npcsOnOurZone.reserve(MiCoMa::getInstance()->getNpcs().size());

    ////function to find brothers/npcs we have on the same zone as our
    //currentNpc->findNpcOnTheSameZone(currentNode, npcsOnOurZone);

    //if ((mapRef->getGoalIDs)->size() < npcsOnOurZone.size()) //TODO modify
    //{
    //    if ( ( mapRef->isLocallyLinked( currentNode->getZoneID() ) ).empty() )
    //    {

    //    }
    //}

    auto blocDoorLambda = [&bboard]()
    {

        Map* ourMap = Map::getInstance();
        MiCoMa* ourMicoma = MiCoMa::getInstance();

        if (!ourMap->getHasDoors())
        {
            return BehaviourTree::result::FAIL;
        }
        else if (bboard.mZoneIdRecursion.size())
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

            auto ourNpcs = ourMicoma->getNpcs();
            for_each(begin(ourNpcs), end(ourNpcs), [&](Npc* npc)
            {
                if (ourMap->getNode(npc->getCurrentTile())->getZoneID() == validZone.first)
                {
                    bboard.setPpNpc(npc);
                    bboard.mZoneIdRecursion.push(validZone.first);
                    
                    return BehaviourTree::result::SUCCESS;
                }
            }); // we have to be sure that the npc is the closer to the pp TODO - DO IT

            return BehaviourTree::result::FAIL;
        }
    };

    return BehaviourTree::createAction(blocDoorLambda);
}

#endif //!BLOCDOORS_H

////////////////////////////////////////////////////////////////////////// DO NOT FORGET RETURN SUCCESS OR FAIL