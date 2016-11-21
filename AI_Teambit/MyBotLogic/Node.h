#ifndef NODE_HEADER
#define NODE_HEADER
#include <cmath>
#include "Globals.h"
#include <list>


struct Position
{
    int x;
    int y;
    Position(int xVal, int yVal)
        :x{ xVal }, y{ yVal }
    {}
};

struct EdgeData
{
    enum EdgeType
    {
        FREE,
        WALL,
        WINDOW,
        DOOR,
        DOOR_W
    } mEdgeType;
    bool mBlocking;
    bool mOpen;

    EdgeData()
        : mEdgeType{ FREE }, mBlocking{ false }, mOpen{ false }
    {}
};

struct InfluenceData
{
    enum InfluenceType {
        INFLUENCE_MAP,
        INFLUENCE_HDOORS
    };

    float mInfluences[2];

    InfluenceData()
        : mInfluences{ 0.0f, 0.0f }
    {}
};

class Node
{
public:
    enum NodeType
    {
        NONE,
        FORBIDDEN,
        GOAL,
        OCCUPIED,
        PRESSURE_PLATE,
        PATH
    };
    
    enum { NBNEIGHBOURS = 8 };

private:


    Position* mPos;
    unsigned int mID;
    NodeType mType;
    EdgeData mEdges[8]{};
    Node* mParent;

    int mCost{ 1 };
    unsigned int mHeuristic{};

    
    Node* mNeighbours[NBNEIGHBOURS] = { nullptr };
    //unsigned int m_npcId = {0};

    InfluenceData mInfluence;

    // TODO - Add zones for tile
    // TODO - Add close or open attributes
    //bool m_knowEverythingAboutIt;

public:

    Node(int xVal, int yVal, unsigned int idVal, NodeType typeVal);


    int getCost() const noexcept
    {
        return mCost;
    }

    void setCost(const int cost)
    {
        mCost = cost;
    }

    unsigned int getHeuristic() const noexcept
    {
        return mHeuristic;
    }

    void setHeuristic(unsigned int heuristic)
    {
        mHeuristic = heuristic;
    }

    NodeType getType() const noexcept
    {
        return mType;
    }

    Position* getPosition() const noexcept
    {
        return mPos;
    }

    unsigned int getId() const noexcept
    {
        return mID;
    }

    void setType(NodeType nType)
    {
        mType = nType;
    }

    void setEdgeType(const EDirection& dir, const EdgeData::EdgeType& value)
    {
        if (value != EdgeData::FREE)
        {
            mEdges[dir].mBlocking = true;
            if (mNeighbours[dir])
            {
                mNeighbours[dir]->mEdges[static_cast<EDirection>((dir + 4) % 8)].mBlocking = true;
            }
        }
        mEdges[dir].mEdgeType = value;
    }

    void setDoorState(const EDirection& dir, bool value)
    {
        mEdges[dir].mOpen = value;
        if (mNeighbours[dir])
        {
            mNeighbours[dir]->mEdges[static_cast<EDirection>((dir + 4) % 8)].mOpen = value;
        }
    }

    bool isDoorOpen(const EDirection& dir)
    {
        return mEdges[dir].mOpen;
    }

    bool isEdgeBlocked(const EDirection& dir) const
    {
        return mEdges[dir].mBlocking && !mEdges[dir].mOpen;
    }

    void setNeighbour(const EDirection& dir, Node* p)
    {
        mNeighbours[dir] = p;
    }

    Node* getNeighbour(const EDirection& dir)
    {
        return mNeighbours[dir];
    }

    EdgeData::EdgeType getEdgeType(const EDirection& dir) const
    {
        return static_cast<EdgeData::EdgeType>(mEdges[dir].mEdgeType);
    }

    float getInfluence(const InfluenceData::InfluenceType& aType = InfluenceData::INFLUENCE_MAP) const
    {
        return mInfluence.mInfluences[aType];
    }

    void setInfluence(float inf, const InfluenceData::InfluenceType& aType = InfluenceData::INFLUENCE_MAP)
    {
        mInfluence.mInfluences[aType] = inf;
    }

    void setParent(Node* parent)
    {
        mParent = parent;
    }

    Node* getParent() const noexcept
    {
        return mParent;
    }
};

struct NodeComparator
{
    bool operator() (const Node* n0, const Node* n1) const
    {
         return n0->getHeuristic() < n1->getHeuristic();
    }
};

#endif // NODE_HEADER

#ifndef NODE_RECORD_H
#define NODE_RECORD_H

struct NodeRecord
{
    using cost_type = unsigned int;

    Node* mNode;
    NodeRecord* mPrevious;
    cost_type mCostSoFar;
    cost_type mEstimatedTotalCost;

    NodeRecord()
        : mNode{ nullptr }, mPrevious{ nullptr }, mCostSoFar{ 0 }, mEstimatedTotalCost{ 0 }
    {}
    NodeRecord(NodeRecord* other)
        : mNode{ other->mNode }, mPrevious{ other->mPrevious }, mCostSoFar{ other->mCostSoFar }, mEstimatedTotalCost{ other->mEstimatedTotalCost }
    {}
    NodeRecord(Node* n, const cost_type& costSoFar, const cost_type& estimatedCost)
        : mNode{ n }, mPrevious{ nullptr }, mCostSoFar{ costSoFar }, mEstimatedTotalCost{ estimatedCost }
    {}
    ~NodeRecord() {}

    bool operator<(NodeRecord* other)
    {
        return mEstimatedTotalCost < other->mEstimatedTotalCost;
    }
    bool operator==(NodeRecord* other)
    {
        return mNode == other->mNode;
    }

    bool empty() { return !mNode; }

    static NodeRecord* findIn(std::list<NodeRecord*> nrList, Node* n)
    {
        for (auto nr : nrList)
        {
            if (nr->mNode == n)
                return nr;
        }

        return nullptr;
    }
};
#endif // !NODE_RECORD_H