#ifndef GENERALACTION_H
#define GENERALACTION_H

#include "BlocAction.h"


namespace BehaviourTree
{
    template<class Foncteur>
    class GeneralAction : public BlocAction
    {
    private:
        Foncteur mFunctor;

    public:
        GeneralAction() = delete;

        GeneralAction(const Foncteur& functor) :
            mFunctor{ functor }
        {}

        result operator()()
        {
            return mFunctor();
        }
    };

    template<class LambdaAction>
    static BaseBloc* createAction(LambdaAction& actionBehavior)
    {
        return new GeneralAction<LambdaAction>(actionBehavior);
    }
}


#endif //!GENERALACTION_H