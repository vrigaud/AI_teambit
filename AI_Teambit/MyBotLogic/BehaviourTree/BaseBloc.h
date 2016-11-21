#ifndef BASEBLOC_H
#define BASEBLOC_H


namespace BehaviourTree
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



    class BaseBloc
    {
    protected:
        size_t mLvlID;
        size_t mNodeID;

        BaseBloc* mParent;


    public:

        void conn(BaseBloc* toConn) noexcept { mParent = toConn; }

        BaseBloc* getParent() const noexcept { return mParent; }

        virtual BaseBloc* getChild(size_t index) noexcept = 0;
        virtual size_t getNbChild() const noexcept = 0;

        size_t getLevel() const noexcept { return mLvlID; }
        size_t getNodeID() const noexcept { return mNodeID; }

        virtual type getType() const noexcept = 0;
        virtual void connect(BaseBloc& toConnect) = 0;
        virtual void disconnect(size_t iter) = 0;

        virtual result operator()() = 0;
    };
}


#endif//!BASEBLOC_H