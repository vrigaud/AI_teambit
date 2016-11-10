#ifndef NODE_H
#define NODE_H

#include "TurnInfo.h"
#include <vector>

#include <cmath>


class Node {
public:
    enum { NB_NEIGHBOURS = 6 };

private:
    using attributes_type = std::set<ETileAttribute>;
    unsigned int mID;
    attributes_type* mAttributes;
    unsigned int mCost;
    Node* mFrom;
    std::vector<Node*> mNeighbours;    //CW - 0 on right side

    unsigned int mHeuristic = 0;

public:
    
    Node(TileInfo& currTileInfo)
        : mID{ currTileInfo.tileID }, mAttributes{ new attributes_type{ currTileInfo.tileAttributes } }, mFrom{ nullptr }, mNeighbours{ NB_NEIGHBOURS }, mCost{0}, mHeuristic{0}
    {}

    Node(TileInfo& currTileInfo, Node* from)
        : mID{ currTileInfo.tileID }, mAttributes{ new attributes_type{ currTileInfo.tileAttributes } }, mFrom{ from }, mNeighbours{ NB_NEIGHBOURS }, mCost{ 0 }, mHeuristic{ 0 }
    {
        mNeighbours[0] = nullptr;
        mNeighbours[1] = nullptr;
        mNeighbours[2] = nullptr;
        mNeighbours[3] = from;
        mNeighbours[4] = nullptr;
        mNeighbours[5] = nullptr;

        from->mNeighbours[0] = this;
    }


    ~Node() = default;

    

    unsigned int getHeuristic() const noexcept
    {
        return mHeuristic;
    }

    void setHeuristic(unsigned int n) noexcept
    {
        mHeuristic = n;
    }

    static int compare2Nodes(const Node& n1, const Node& n2)
    {
        if (n1.getHeuristic() < n2.getHeuristic())
        {
            return 1;
        }
        else if (n1.getHeuristic() == n2.getHeuristic())
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    friend bool operator<(const Node& n1, const Node& n2)
    {
        return n1.getHeuristic() < n2.getHeuristic();
    }

    void setParent(Node* n)
    {
        mFrom = (n != nullptr) ? new Node(*n) : nullptr;
    }

    Node* getParent()
    {
        return mFrom;
    }

    std::vector<Node*> getNeighbours() const { return mNeighbours; }
    unsigned int getID() const { return mID; }
    attributes_type* getAttributes() const { return mAttributes; }
    void setNeighbour(Node& neighbour, unsigned int idxNeighb, unsigned int idxCurr)
    {
        mNeighbours[idxNeighb] = &neighbour;
        neighbour.mNeighbours[idxCurr] = this;
    }

    unsigned int getX(const LevelInfo& level) const noexcept
    {
        return this->mID % level.colCount;
    }

    unsigned int getY(const LevelInfo& level) const noexcept
    {
        return this->mID / level.colCount;
    }

    unsigned int getCost() const noexcept
    {
        return mCost;
    }

    void setCost(unsigned int n) noexcept
    {
        mCost = n;
    }

    static unsigned int manhattanDistance(unsigned int startId, unsigned int goalId, LevelInfo& level) noexcept
    {
        return absUnsigned(goalId % level.colCount, startId % level.colCount) + absUnsigned(goalId / level.colCount, startId / level.colCount);
    }

    unsigned int manhattanDistance(Node* goal, const LevelInfo& level) const noexcept
    {
        unsigned int xStart = this->mID % level.colCount;
        unsigned int yStart = this->mID / level.colCount;

        unsigned int xEnd = goal->mID % level.colCount;
        unsigned int yEnd = goal->mID / level.colCount;

        return absUnsigned(xEnd, xStart) + absUnsigned(yEnd, yStart);


    }



    static unsigned int absUnsigned(unsigned int x1, unsigned int x2) noexcept
    {
        return ((x1 < x2) ? (x2 - x1) : (x1 - x2));
    }



    bool operator==(const Node& other) const noexcept
    {
        return mID == other.mID && mCost == other.mCost && mHeuristic == other.mHeuristic;
    }

    bool operator!=(const Node& other) const noexcept
    {
        return !(*this == other);
    }

    Node(const Node& other) :
        mNeighbours{ other.getNeighbours().size() },
        mID{ other.getID() },
        mHeuristic{ other.getHeuristic() },
        mCost { other.getCost() },
        mAttributes{ other.mAttributes },
        mFrom{ other.mFrom }
    {

        for (size_t iter = 0; iter < other.getNeighbours().size(); ++iter)
        {
            mNeighbours[iter] = other.getNeighbours()[iter];
        }
    }

    Node& operator=(const Node& other)
    {
        mNeighbours = std::vector<Node*>{ other.getNeighbours().size() };
        mHeuristic = other.getHeuristic();
        mID = other.getID();

        mNeighbours.clear();

        for (size_t iter = 0; iter < other.getNeighbours().size(); ++iter)
        {
            mNeighbours.push_back(other.getNeighbours()[iter]);
        }
        mCost = other.getCost();
        
        mFrom = other.mFrom;

        mAttributes = other.mAttributes;
        
        return *this;
    }
};

#endif // 
