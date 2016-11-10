#ifndef BLOCFAIL_H
#define BLOCFAIL_H

#include "BlocDecorator.h"

namespace BehaviourTree
{
    class BlocFail : public BlocDecorator
    {
    public:
        BlocFail(BaseBloc& child) noexcept :
            BlocDecorator{ child }
        {}

        virtual general::result operator()()
        {
            (*onlyChild)();
            return general::result::FAIL;
        }
    };
}


#endif //!BLOCFAIL_H
