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

		virtual result operator()(BlackBoard &bboard)
        {
			(*onlyChild)(bboard);
            return result::SUCCESS;
        }
    };
}



#endif //!BLOCSUCCESS_H
