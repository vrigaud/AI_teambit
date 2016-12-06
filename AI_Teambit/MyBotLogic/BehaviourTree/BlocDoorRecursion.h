#ifndef BLOCDOORRECURSION_H
#define BLOCDOORRECURSION_H

#include "Blackboard.h"
#include "BlocLoop.h"
#include "..\MiCoMa.h"
#include "..\Map.h"

#include <iterator>
#include <algorithm>

using namespace std;

class DOHiddenDoors {};
class DoorRecException {};

BehaviourTree::BaseBloc* getBlocDoorRecursion(BlackBoard &bboard)
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

    auto blocDoorRecursionLambda = [&bboard]()
    {

        Map* ourMap = Map::getInstance();
        MiCoMa* ourMicoma = MiCoMa::getInstance();
        std::vector<Npc*> ourNpcs = ourMicoma->getNpcs();
        Npc* scout = bboard.getScout();
        Npc* ppNpc = bboard.getPpNpc();
        int targetedPP = bboard.getTargetedPP();

        Objective ppNpcObj = ppNpc->getObjective();
        if (targetedPP == -1)
        {
            std::vector<Npc*> npcs{};
            npcs.reserve(ourMicoma->getNpcs().size());

            std::vector<Node*> goals{};
            goals.reserve(ourMap->getGoalIDs().size());

            unsigned int currentRecursionZoneId = bboard.getZoneIdRecursion().top();

            copy_if(begin(ourNpcs), end(ourNpcs), back_inserter(npcs), [&](Npc* npc)
            {
                return ourMap->getNode(npc->getCurrentTile())->getZoneID() == currentRecursionZoneId;
            });

            auto currentZoneNodes = ourMap->getZoneList()[currentRecursionZoneId].mNodeOnZone;
            copy_if(begin(currentZoneNodes), end(currentZoneNodes), back_inserter(goals), [&](Node* n)
            {
                return (n->getZoneID() == currentRecursionZoneId) && (n->getType() == Node::GOAL);
            });

            if (goals.size() < npcs.size())
            {
                auto localController = ourMap->getLocallyLinkedControllers(currentRecursionZoneId);
               
                // Looking for door that can be crossed alone by pressur plate
                unsigned int target{};
                std::vector<Door> currentZoneDoors = ourMap->getZoneList()[currentRecursionZoneId].mDoorOnZone;
                for (Controller c : localController)
                {
                    auto doorFound = find_if(begin(currentZoneDoors), end(currentZoneDoors),
                        [&c](Door door) {
                        return (door.mTileId == c.mTileID || door.mOtherSideTileId == c.mTileID) && c.mIdDoor == door.mIdDoor;
                    });
                    if (doorFound != end(currentZoneDoors))
                    {
                        target = c.mTileID;
                        ppNpc->setObjective(Objective::GO_TO,target);
                        bboard.setTargetedPP(target);
                        bboard.setTargetedDoor(doorFound->mIdDoor);
                        return BehaviourTree::result::SUCCESS;
                    }
                }

                // Looking for door that can be crossed alone by action
                auto doorFound = find_if(begin(currentZoneDoors), end(currentZoneDoors),
                    [](Door door) {
                    return !door.hasPressurePlate();
                });
                if (doorFound != end(currentZoneDoors))
                {
                    target = ourMap->getNode(doorFound->mTileId)->getZoneID() == currentRecursionZoneId ?
                        doorFound->mTileId : doorFound->mOtherSideTileId;
                    ppNpc->setObjective(Objective::GO_TO_BUTTON_DOOR, target, doorFound->mIdDoor);
                    bboard.setTargetedPP(target);
                    bboard.setTargetedDoor(doorFound->mIdDoor);
                    return BehaviourTree::result::SUCCESS;
                }

                // Looking for other pressure plate
                target = begin(localController)->mTileID;
                ppNpc->setObjective(Objective::GO_TO, target);
                bboard.setTargetedPP(target);
                bboard.setTargetedDoor(doorFound->mIdDoor);
                return BehaviourTree::result::SUCCESS;
                
            }
            throw DoorRecException();
        }
        else
        {
            if (ppNpc->hasFinishedJob() || ppNpc->isArrived())
            {
                auto tile = ourMap->getNode(ppNpc->getCurrentTile());
                auto cntrllrOnZone = ourMap->getZoneList()[tile->getZoneID()].mControllerOnZone;
                unsigned int doorId = bboard.getTargetedDoor();
                std::vector<Door> ppDoorZone = ourMap->getZoneList()[tile->getZoneID()].mDoorOnZone;
                auto ppDoor = find_if(begin(ppDoorZone), end(ppDoorZone), [&doorId](Door d) {return d.mIdDoor == doorId;});

//                 auto controllerFound = find_if(begin(cntrllrOnZone), end(cntrllrOnZone), [&tile](Controller cntrllr)
//                 {
//                     return cntrllr.mTileID == tile->getId();
//                 });
// 
//                 auto ppDoor = find_if(begin(ppDoorZone), end(ppDoorZone), [&controllerFound](Door d) {return d.mIdDoor == controllerFound->mIdDoor;});
                if (bboard.getZoneIdRecursion().top() == tile->getZoneID())
                {
                    bboard.getZoneIdRecursion().pop();
                }

                if (tile->getId() == ppDoor->mTileId || tile->getId() == ppDoor->mOtherSideTileId) // PP is direct neighbour of door
                {
                    unsigned int target = tile->getId() == ppDoor->mTileId ? ppDoor->mOtherSideTileId : ppDoor->mTileId;
                    ppNpc->setObjective(Objective::GO_TO, target);
                    bboard.setTargetedPP(target);
                }

                //SCOUT AND NPC
            }

            return BehaviourTree::result::SUCCESS;
        }

    };

    return BehaviourTree::createAction(blocDoorRecursionLambda);
}

#endif //!BLOCDOORRECURSION_H
