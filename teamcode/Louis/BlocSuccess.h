#ifndef BLOCSUCCESS_H
#define BLOCSUCCESS_H

#include "BlocDecorator.h"

namespace BehaviourTree
{
    class BlocSuccess : public BlocDecorator
    {
    public:
        BlocSuccess(BaseBloc& child) :
            BlocDecorator{ child }
        {}

        virtual general::result operator()()
        {
            (*onlyChild)();
            return general::result::SUCCESS;
        }
    };
}


#endif //!BLOCSUCCESS_H
