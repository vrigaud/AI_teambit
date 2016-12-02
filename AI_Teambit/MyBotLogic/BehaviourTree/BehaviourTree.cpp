#include "BehaviourTree.h"

#include "BlocSequence.h"
#include "BlocSelect.h"

#include "BlocUpdateActionList.h"
#include "BlocHasGoal.h"
#include "BlocFindBGBG.h"
#include "BlocFindBGBNpc.h"
#include "BlocUpdateNpcTiles.h"

using namespace BehaviourTree;

void BehaviourTree::BehaviourTreeObject::initialize(const LevelInfo& levelInfo)
{
	mBlackBoard.init(levelInfo);

	mRoot = new BehaviourTree::BlocSequence{};

#pragma region BasicGoalAssigment_SubTree
	BlocSelect* findGoalSelect = new BlocSelect();
	BlocSequence* sequence = new BlocSequence();
	findGoalSelect->connect(*getBlocFindBGBNpc(mBlackBoard));
	findGoalSelect->connect(*getBlocFindBGBG(mBlackBoard));

    mRoot->connect(*getBlocUpdateNpcTiles(mBlackBoard));
	mRoot->connect(*findGoalSelect);
	mRoot->connect(*getBlocHasGoal(mBlackBoard));
	mRoot->connect(*getBlocUpdateActionList(mBlackBoard));
#pragma endregion

}