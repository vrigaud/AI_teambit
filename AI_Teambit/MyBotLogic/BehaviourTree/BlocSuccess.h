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

        virtual result operator()()
        {
            (*onlyChild)();
            return result::SUCCESS;
        }
    };
}



#endif //!BLOCSUCCESS_H
