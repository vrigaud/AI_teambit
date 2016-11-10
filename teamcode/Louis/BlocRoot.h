#ifndef BLOCROOT_H
#define BLOCROOT_H


#include "BaseBloc.h"


#include <algorithm>


namespace BehaviourTree
{
    /*
    A huge behavior tree object.
    */
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
            return mRoot->child(index);
        }

        virtual size_t nbChild()       const noexcept
        {
            return mRoot->nbChild();
        }

        virtual void connect(BaseBloc& otherModule)
        {
            mRoot->connect(otherModule);
        }

        virtual void disconnect(size_t index)
        {
            mRoot->disconnect(index);
        }


        general::type type() const noexcept
        {
            return mRoot->type();
        }

        general::result operator()()
        {
            return (*mRoot)();
        }

        template<class ... IntegerIndex>
        IModule* operator()(IntegerIndex ... indexes)
        {
            IModule* iteratorMod = &mRoot;

            size_t argument[] = { indexes... };

            std::for_each(std::begin(argument),
                std::end(argument),
                [&iteratorMod](size_t index) {
                    iteratorMod = &iteratorMod->child(index);
                }
            );

            return iteratorMod;
        }
    };

}


#endif //!BLOCROOT_H