#ifndef BASEBLOC_H
#define BASEBLOC_H


namespace BehaviourTree
{
    namespace general
    {
        enum result
        {
            FAIL = 0,
            SUCCESS = 1,
            INPROGRESS = 2
        };

        enum type
        {
            COMPOSITE = 0,
            DECORATOR = 1,
            ACTION = 2

        };
    }
    

    class BaseBloc
    {
    protected:
        size_t mLvlId;
        size_t mIdNode;

        BaseBloc* mParent;


    public :

        void conn(BaseBloc* toConn) noexcept { mParent = toConn; }

        BaseBloc* parent() const noexcept { return mParent; }

        virtual BaseBloc* child(size_t index) noexcept = 0;
        virtual size_t nbChild() const noexcept = 0;
        
        size_t level() const noexcept { return mLvlId; }
        size_t identity() const noexcept { return mIdNode; }

        virtual general::type type() const noexcept = 0;
        virtual void connect(BaseBloc& toConnect) = 0;
        virtual void disconnect(size_t iter) = 0;

        virtual general::result operator()() = 0;


    };
}


#endif//!BASEBLOC_H