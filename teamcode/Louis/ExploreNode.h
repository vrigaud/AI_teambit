#ifndef EXPLORE_NODE
#define EXPLORE_NODE



#include "TileInfo.h"
#include "LevelInfo.h"

#include <vector>
#include <string>
#include <sstream>


template<class Type>
struct comparator
{
    friend bool operator<=(const Type& A, const Type& B) noexcept
    {
        return !(B < A);
    }

    friend bool operator>=(const Type& A, const Type& B) noexcept
    {
        return !(A < B);
    }

    friend bool operator>(const Type& A, const Type& B) noexcept
    {
        return B < A;
    }
};


class ExploreNode : comparator<ExploreNode>
{
private:
    using attributes_type = std::set<ETileAttribute>;

    unsigned int mID;
    attributes_type mAttributes;
    unsigned int mCost;
    ExploreNode* mFrom;
    std::vector<ExploreNode*> mNeighbours;
    std::vector<ObjectInfo> mObjectsOnNode;

    unsigned int mHeuristic;

    bool mKnown;
    unsigned int mExploredCount;

public:
    ExploreNode(TileInfo& currTileInfo)
        :
        mID{ currTileInfo.tileID },
        mAttributes{ currTileInfo.tileAttributes },
        mFrom{ nullptr },
        mNeighbours{},
        mCost{ 1 },
        mHeuristic{ 0 },
        mObjectsOnNode{},
        mKnown{ false },
        mExploredCount{ 0 }
    {}

    ExploreNode(const ExploreNode& other) :
        mNeighbours{ other.mNeighbours.size() },
        mID{ other.getID() },
        mHeuristic{ other.getHeuristic() },
        mCost{ other.getCost() },
        mAttributes{ other.mAttributes },
        mFrom{ other.mFrom },
        mObjectsOnNode{ other.mObjectsOnNode },
        mKnown{ other.mKnown },
        mExploredCount { other.mExploredCount }
    {
        for (size_t iter = 0; iter < other.mNeighbours.size(); ++iter)
        {
            mNeighbours[iter] = other.mNeighbours[iter];
        }
    }

    ~ExploreNode() = default;

    void update(const ObjectInfo& object)
    {
        if (mObjectsOnNode.empty())
        {
            mObjectsOnNode.push_back(object);
        }

        size_t iter = 0;

        std::for_each(
            std::begin(object.edgesCost),
            std::end(object.edgesCost),
            [this, &iter](unsigned int objectCost)
            {
                mObjectsOnNode[0].edgesCost[iter] &= objectCost;
                ++iter;
            }
        );

        if (!mKnown)
        {
            for (size_t iter = 0; iter < mNeighbours.size(); ++iter)
            {
                if (!mNeighbours[iter]->isKnown())
                {
                    return;
                }
            }

            mKnown = true;
        }
    }

    void update(const TileInfo& tile)
    {
        mAttributes = tile.tileAttributes;
    }

    unsigned int getHeuristic() const noexcept
    {
        return mHeuristic;
    }

    void setHeuristic(unsigned int n) noexcept
    {
        mHeuristic = n;
    }

    static int compare2Nodes(const ExploreNode& n1, const ExploreNode& n2)
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

    void setParent(ExploreNode* n)
    {
        mFrom = (n != nullptr) ? new ExploreNode(*n) : nullptr;
    }

    ExploreNode* getParent()
    {
        return mFrom;
    }

    std::vector<ExploreNode*>& getNeighbours() { return mNeighbours; }

    unsigned int getID() const { return mID; }

    attributes_type* getAttributes() { return &mAttributes; }

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

    bool isTarget() const noexcept
    {
        return mAttributes.find(ETileAttribute::TileAttribute_Target) != mAttributes.end();
    }

    bool isForbidden() const noexcept
    {
        return mAttributes.find(ETileAttribute::TileAttribute_Forbidden) != mAttributes.end();
    }

    bool isAttribute(ETileAttribute attribute) const noexcept
    {
        return mAttributes.find(attribute) != mAttributes.end();
    }

    bool isKnown() const
    {
        return mKnown;
    }

    void setKnown(bool known)
    {
        mKnown = known;
    }

    unsigned int getExploredCount() const
    {
        return mExploredCount;
    }

    void setExploredCount(unsigned int newValue)
    {
        mExploredCount = newValue;
    }

    void incrementExploredCount()
    {
        ++mExploredCount;
    }

    //estimation with ponderation
    static unsigned int estimate(ExploreNode& start, ExploreNode& goal, const LevelInfo& lvlInfo)
    {
        return goal.getCost() + manhattanDistance(start.getID(), goal.getID(), lvlInfo);
    }



    static unsigned int manhattanDistance(unsigned int startId, unsigned int goalId, const LevelInfo& level) noexcept
    {
        return absUnsigned(goalId % level.colCount, startId % level.colCount) + absUnsigned(goalId / level.colCount, startId / level.colCount);
    }

    unsigned int manhattanDistance(ExploreNode* goal, const LevelInfo& level) const noexcept
    {
        return absUnsigned(goal->mID % level.colCount, this->mID % level.colCount) + absUnsigned(goal->mID / level.colCount, this->mID / level.colCount);
    }

    
    void connectTo(ExploreNode& other)
    {
        if (isNotAlreadyConnected(&other))
        {
            mNeighbours.push_back(&other);
            other.connectTo(*this);
        }
    }

    void disconnect(ExploreNode* node)
    {
        auto toDisconnect = std::find(mNeighbours.begin(), mNeighbours.end(), node);

        if (toDisconnect != mNeighbours.end())
        {
            mNeighbours.erase(toDisconnect);
        }
    }


    friend bool operator<(const ExploreNode& n1, const ExploreNode& n2)
    {
        return n1.getHeuristic() < n2.getHeuristic();
    }

    bool operator==(const ExploreNode& other) const noexcept
    {
        return mID == other.mID;
    }

    bool operator!=(const ExploreNode& other) const noexcept
    {
        return !(*this == other);
    }

    ExploreNode& operator=(const ExploreNode& other)
    {
        mNeighbours = std::vector<ExploreNode*>{ other.mNeighbours.size() };
        mHeuristic = other.getHeuristic();
        mID = other.getID();

        mNeighbours.clear();

        for (size_t iter = 0; iter < other.mNeighbours.size(); ++iter)
        {
            mNeighbours.push_back(other.mNeighbours[iter]);
        }
        mCost = other.getCost();

        mFrom = other.mFrom;

        mAttributes = other.mAttributes;

        return *this;
    }

    std::string toStdString() const noexcept
    {
        std::stringstream stream;

        stream << (mKnown ? "o" : "x");
        stream << mID << "-" << mExploredCount << " (";
        
        if (!mObjectsOnNode.empty())
        {
            std::for_each(
                std::begin(mObjectsOnNode[0].edgesCost),
                std::end(mObjectsOnNode[0].edgesCost),
                [&stream](unsigned int cost) {
                stream << cost << ",";
            }
            );
        }
        

        stream << ") {";
        std::for_each(
            mNeighbours.begin(),
            mNeighbours.end(),
            [&stream](ExploreNode* node) {
            stream << node->getID() << ",";
        }
        );

        stream << "}";

        return stream.str();
    }

private:
    static unsigned int absUnsigned(unsigned int x1, unsigned int x2) noexcept
    {
        return ((x1 < x2) ? (x2 - x1) : (x1 - x2));
    }

    bool isNotAlreadyConnected(ExploreNode* toCheck) const noexcept
    {
        return std::find(mNeighbours.begin(), mNeighbours.end(), toCheck) == mNeighbours.end();
    }
};

#endif // !EXPLORE_NODE
