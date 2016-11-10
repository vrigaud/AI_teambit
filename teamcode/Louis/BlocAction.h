#ifndef BLOCACTION_H
#define BLOCACTION_H


#include "BaseBloc.h"

#include <algorithm>

namespace BehaviourTree
{
    class BlocAction : public BaseBloc
    {
    public:

        virtual BaseBloc* child(size_t index) noexcept
        {
            return nullptr;
        }

        virtual size_t nbChild() const noexcept
        {
            return 0;
        }

        virtual general::type type() const noexcept
        {
            return general::type::ACTION;
        }

        void connect(BaseBloc& toConnect) {}

        void disconnect(size_t iter) {}

        virtual general::result operator()() = 0;


    };
}


#endif//!BLOCACTION_H

