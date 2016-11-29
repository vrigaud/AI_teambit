#ifndef BLOCINVERT_H
#define BLOCINVERT_H

#include "BlocDecorator.h"

namespace BehaviourTree
{
    class BlocInvert : public BlocDecorator
    {
    public:
        BlocInvert(BaseBloc& child) noexcept :
            BlocDecorator{ child }
        {}

		virtual result operator()(BlackBoard &bboard)
        {
			result res = (*onlyChild)(bboard);

            switch (res)
            {
            case FAIL:
                return SUCCESS;

            case SUCCESS:
                return FAIL;

            default:
                return res;
            }
        }
    };
}



#endif //!BLOCINVERT_H
