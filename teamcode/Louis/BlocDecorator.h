#ifndef BLOCDECORATOR_H
#define BLOCDECORATOR_H


#include "BaseBloc.h"

#include <algorithm>

namespace BehaviourTree
{
    class BlocDecorator : public BaseBloc
    {
    protected:
        BaseBloc* onlyChild;

    public:
        BlocDecorator() = delete;

        BlocDecorator(BaseBloc& child) noexcept :
            onlyChild{ &child }
        {}

        virtual ~BlocDecorator()
        {
            delete onlyChild;
        }

        virtual BaseBloc* child(size_t index) noexcept
        {
            return onlyChild;
        }

        virtual size_t nbChild() const noexcept
        {
            return 1;
        }

        virtual general::type type() const noexcept
        {
            return general::type::DECORATOR;
        }

        void connect(BaseBloc& toConnect){}

        void disconnect(size_t iter) {}

        virtual general::result operator()() = 0;
    };
}


#endif//!BLOCDECORATOR_H
