#ifndef BLOCHASGOAL_H
#define BLOCHASGOAL_H

#include "GeneralAction.h"

BehaviourTree::BaseBloc* getBlocHasGoal()
{
    auto forHasGoalLambda = [&]()
    {
        
    };

    return BehaviourTree::createAction(forHasGoalLambda);
}

#endif //!BLOCHASGOAL_H
