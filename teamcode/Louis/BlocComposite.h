#ifndef BLOCCOMPOSITE_H
#define BLOCCOMPOSITE_H


#include "BaseBloc.h"

#include <algorithm>
#include <vector>

namespace BehaviourTree
{
    class BlocComposite : public BaseBloc
    {
    protected:

        std::vector<BaseBloc*> children;
    public:

        virtual ~BlocComposite()
        {
            std::for_each(children.begin(),
                children.end(),
                [](BaseBloc* bloc){
                    delete bloc;
                }
            );
        }

        virtual BaseBloc* child(size_t index) noexcept
        {
            return children[index];
        }

        virtual size_t nbChild() const noexcept
        {
            return children.size();
        }

        virtual general::type type() const noexcept
        {
            return general::type::COMPOSITE;
        }

        virtual void connect(BaseBloc& toConnect)
        {
            this->children.push_back(&toConnect);
            toConnect.conn(this);
        }

        virtual void disconnect(size_t iter)
        {
            children.erase(children.begin() + iter);
        }

        virtual general::result operator()() = 0;

    };
}


#endif//!BLOCCOMPOSITE_H