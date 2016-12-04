//#ifndef BLOCDOORS_H
//#define BLOCDOORS_H
//
//#include <algorithm>
//
//#include "BehaviourTree\BehaviourTree.h"
//#include "MiCoMa.h"
//#include "Npc.h"
//#include "Map.h"
//
//
//BehaviourTree::BaseBloc* getBlocDoor(BlackBoard &bboard)
//{
//    Map* mapRef = Map::getInstance();
//    Npc* currentNpc = *MiCoMa::getInstance()->getNpcs().begin();
//    auto currentNode = mapRef->getNode(currentNpc->getID());
//    std::vector<Node*> npcsOnOurZone{};
//    npcsOnOurZone.reserve(MiCoMa::getInstance()->getNpcs().size());
//
//    //function to find brothers/npcs we have on the same zone as our
//    currentNpc->findNpcOnTheSameZone(currentNode, npcsOnOurZone);
//
//    if ((mapRef->getGoalIDs)->size() < npcsOnOurZone.size()) //TODO modify
//    {
//        if ( ( mapRef->isLocallyLinked( currentNode->getZoneID() ) ).empty() )
//        {
//
//        }
//    }
//
//    auto blocDoorLambda = [&bboard]()
//    {
//        
//        return BehaviourTree::result::SUCCESS;
//    };
//
//    return BehaviourTree::createAction(blocDoorLambda);
//}
//
//#endif //!BLOCDOORS_H
//
//////////////////////////////////////////////////////////////////////////// DO NOT FORGET RETURN SUCCESS OR FAIL