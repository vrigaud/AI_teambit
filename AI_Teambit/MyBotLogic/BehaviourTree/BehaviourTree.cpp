#include "BehaviourTree.h"

#include "BlocSequence.h"
#include "BlocSelect.h"

#include "BlocUpdateActionList.h"
#include "BlocHasGoal.h"
#include "BlocFindBGBG.h"
#include "BlocFindBGBNpc.h"
#include "BlocDoor.h"

using namespace BehaviourTree;

void BehaviourTree::BehaviourTreeObject::initialize(const LevelInfo& levelInfo)
{
	mBlackBoard.init(levelInfo);

	mRoot = new BehaviourTree::BlocSelect{};

#pragma region BasicGoalAssigment_SubTree
    
    BlocSequence* hasDevice = new BlocSequence{};
    hasDevice->connect(*getBlocDoor(mBlackBoard));
    //HasDevice->connect();

    BlocSequence* mNormal = new BlocSequence{};

	BlocSelect* findGoalSelect = new BlocSelect();
	BlocSequence* sequence = new BlocSequence();
	findGoalSelect->connect(*getBlocFindBGBNpc(mBlackBoard));
	findGoalSelect->connect(*getBlocFindBGBG(mBlackBoard));

    mNormal->connect(*findGoalSelect);
    mNormal->connect(*getBlocHasGoal(mBlackBoard));
    mNormal->connect(*getBlocUpdateActionList(mBlackBoard));

    mRoot->connect(*hasDevice);
    mRoot->connect(*mNormal);

#pragma endregion

}