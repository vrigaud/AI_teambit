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

            auto currentZone = ourMap->getZoneList()[currentRecursionZoneId].mNodeOnZone;
            copy_if(begin(currentZone), end(currentZone), back_inserter(goals), [&](Node* n)
            {
                return (n->getZoneID() == currentRecursionZoneId) && (n->getType() == Node::GOAL);
            });

            if (goals.size() < npcs.size())
            {
                auto localController = ourMap->getLocallyLinkedControllers(currentRecursionZoneId);
                if (localController.empty())
                {
                    throw DOHiddenDoors();
                    //HIDDEN DOORS - TODO - DO IT
                }
                else
                {
                    unsigned int target = begin(localController)->mTileID;
                    ppNpc->setGoal(target);
                    bboard.setTargetedPP(target);
                    return BehaviourTree::result::SUCCESS;
                }
            }
            throw DoorRecException();
        }
        else
        {
            if (ppNpc->isArrived())
            {
                auto tile = ourMap->getNode(ppNpc->getCurrentTile());
                auto cntrllrOnZone = ourMap->getZoneList()[tile->getZoneID()].mControllerOnZone;

                auto doorId = find_if(begin(cntrllrOnZone), end(cntrllrOnZone), [&tile](Controller cntrllr)
                {
                    return cntrllr.mTileID == tile->getId();
                });

                auto ppDoorZone = ourMap->getZoneList()[tile->getZoneID()].mDoorOnZone;
                auto ppDoor = find_if(begin(ppDoorZone), end(ppDoorZone), [&doorId](Door d) {return d.mIdDoor == doorId->mIdDoor;});

                if (tile->getId() == ppDoor->mTileId || tile->getId() == ppDoor->mOtherSideTileId) // PP is direct neighbour of door
                {
                    unsigned int target = tile->getId() == ppDoor->mTileId ? ppDoor->mOtherSideTileId : ppDoor->mTileId;
                    ppNpc->setGoal(target);
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
