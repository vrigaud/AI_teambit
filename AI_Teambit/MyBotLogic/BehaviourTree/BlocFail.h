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

        virtual result operator()()
        {
            (*onlyChild)();
            return result::FAIL;
        }
    };
}



#endif //!BLOCFAIL_H
