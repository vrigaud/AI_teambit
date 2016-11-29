#ifndef BLOCLOOP_H
#define BLOCLOOP_H

#include "BlocDecorator.h"

namespace BehaviourTree
{
    class BlocLoop : public BlocDecorator
    {
    protected:
        size_t iterationCount;


    public:
        BlocLoop(BaseBloc& child, size_t countLoop) noexcept :
            BlocDecorator{ child },
            iterationCount{ countLoop }
        {}

        size_t getIterationCount() const noexcept
        {
            return iterationCount;
        }

        void setIterationCount(size_t newCount) noexcept
        {
            iterationCount = newCount;
        }

		virtual result operator()(BlackBoard &bboard)
        {
            result state;

            for (size_t iter = 0; iter < iterationCount; ++iter)
            {
				state = (*onlyChild)(bboard);
            }

            return state;
        }
    };

}


#endif //!BLOCLOOP_H

