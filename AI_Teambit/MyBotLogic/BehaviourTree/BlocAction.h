#ifndef BLOCACTION_H
#define BLOCACTION_H


#include "BaseBloc.h"

#include <algorithm>

namespace BehaviourTree
{
    class BlocAction : public BaseBloc
    {
    public:

        virtual BaseBloc* getChild(size_t index) noexcept
        {
            return nullptr;
        }

        virtual size_t getNbChild() const noexcept
        {
            return 0;
        }

        virtual type getType() const noexcept
        {
            return type::ACTION;
        }

        void connect(BaseBloc& toConnect) {}

        void disconnect(size_t iter) {}

        virtual result operator()() = 0;
    };
}



#endif//!BLOCACTION_H

