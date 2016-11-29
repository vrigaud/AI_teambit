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
                [](BaseBloc* bloc) {
                delete bloc;
            }
            );
        }

        virtual BaseBloc* getChild(size_t index) noexcept
        {
            return children[index];
        }

        virtual size_t getNbChild() const noexcept
        {
            return children.size();
        }

        virtual type getType() const noexcept
        {
            return type::COMPOSITE;
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

        virtual result operator()(BlackBoard &bboard) = 0;

    };
}



#endif//!BLOCCOMPOSITE_H