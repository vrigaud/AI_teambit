#ifndef BLOCROOT_H
#define BLOCROOT_H


#include "BaseBloc.h"


#include <algorithm>

namespace BehaviourTree
{
    class BehaviorTreeObject : public BaseBloc
    {
    private:
        BaseBloc* mRoot;


    public:
        BehaviorTreeObject(BaseBloc& root) noexcept :
            mRoot{ &root }
        {}

        ~BehaviorTreeObject()
        {
            delete mRoot;
        }

        BaseBloc& getRoot() noexcept
        {
            return *mRoot;
        }


        virtual BaseBloc* child(size_t index)   noexcept
        {
            return mRoot->getChild(index);
        }

        virtual size_t nbChild()       const noexcept
        {
            return mRoot->getNbChild();
        }

        virtual void connect(BaseBloc& otherModule)
        {
            mRoot->connect(otherModule);
        }

        virtual void disconnect(size_t index)
        {
            mRoot->disconnect(index);
        }


        type type() const noexcept
        {
            return mRoot->getType();
        }

        result operator()()
        {
            return (*mRoot)();
        }

    };
}



#endif //!BLOCROOT_H