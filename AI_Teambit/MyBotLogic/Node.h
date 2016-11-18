#ifndef NODE_HEADER
#define NODE_HEADER
#include <cmath>
#include <limits>
#include "Globals.h"

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

    EdgeData()
        : mEdgeType{ FREE }, mBlocking{ false }
    {}
};

struct InfluenceData
{
    enum InfluenceType {
        INFLUENCE_MAP,
        INFLUENCE_HDOORS
    };

    float mInfluences[2];
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

private:
    Position* mPos;
    unsigned int mID;
    NodeType mType;
    EdgeData mEdges[8]{};
    Node* mParent;

    int mCost{ std::numeric_limits<int>::max() };
    unsigned int mHeuristic{};

    Node* mNeighbours[8] = { nullptr };
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

    void setEdgeCost(const EDirection& dir, const EdgeData::EdgeType& value)
    {
        mEdges[dir].mEdgeType = value;
    }

    bool isEdgeBlocked(const EDirection& dir) const
    {
        return mEdges[dir].mBlocking;
    }

    void setNeighboor(const EDirection& dir, Node* p)
    {
        mNeighbours[dir] = p;
    }

    Node* getNeighboor(const EDirection& dir)
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
    bool operator() (const Node* ft0, const Node* ft1) const
    {

        /*
        TODO
        Compare costs, not ids :P
        */
        return ft0->getId() < ft1->getId();
    }
};

#endif // NODE_HEADER
