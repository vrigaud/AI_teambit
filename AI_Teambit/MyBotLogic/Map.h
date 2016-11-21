#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include "Singleton.h"
#include "NPCInfo.h"

#include "Logger.h"
#include <algorithm>
#include <map>
#include <vector>

// For debug only
#include <string>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_MAP
#endif

#ifdef BOT_LOGIC_DEBUG_MAP
#define BOT_LOGIC_MAP_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_MAP_LOG(logger, text, autoEndLine) 0
#endif

struct TileInfo;
struct ObjectInfo;
struct TurnInfo;

class Map : Singleton
{
    static Map mInstance;
    unsigned int mWidth;
    unsigned int mHeight;

    std::vector<Node*> mNodeMap;
    std::vector<unsigned int> mGoalTiles;
	unsigned int mInfluenceRange;

	std::map<unsigned, bool> mKnownTilesAndVisitedStatus;
	std::vector<Node*> mInterestingNodes;

    // Log stuff
    Logger mLogger;
    Logger mLoggerEdges;
    Logger mLoggerInfluence;

private:
    Map() : mWidth(0), mHeight(0)
    {}
    // Graph creation
    void createNode(Node*);
    void connectNodes();
    void updateEdges(TurnInfo&);
    void updateTiles(TurnInfo&);

    // Edges update 
    void processDoorState(ObjectInfo &object, Node* node, int i);

    EDirection inverseDirection(const EDirection& dir) const
    {
        return static_cast<EDirection>((dir + 4) % 8);
    }

public:
    static Map *getInstance() noexcept
    {
        return &mInstance;
    }
    
    void initMap(int, int, int = 0);
    void updateMap(TurnInfo&);
    void addGoalTile(unsigned int number);
    const std::vector<unsigned int>& getGoalIDs() const { return mGoalTiles; }
	void createInfluenceMap(const InfluenceData::InfluenceType& = InfluenceData::INFLUENCE_MAP);

    // Influence methods
	void propagateInfluence();
	void propagate(Node * myNode, unsigned curDist, unsigned maxDist, float initialInfluence) const;
    std::vector<unsigned int> getCloseMostInfluenteTile(unsigned int) const; 

	Node* getNode(unsigned int, unsigned int) const;
    Node* getNode(unsigned int) const;
    void setNodeType(unsigned int, Node::NodeType);
    unsigned int getWidth() const
    {
        return mWidth;
    }
    void setWidth(unsigned int w)
    {
        mWidth = w;
    }
    unsigned int getHeight() const
    {
        return mHeight;
    }
    void setHeight(unsigned int h)
    {
        mHeight = h;
    }
    unsigned int getInfluenceRange() const
    {
        return mInfluenceRange;
    }
    void setInfluenceRange(unsigned int range)
    {
        BOT_LOGIC_MAP_LOG(mLoggerInfluence, "\t Influence Range = " + std::to_string(range), true);
        mInfluenceRange = range;
    }
	void addSeenTile(unsigned tileId);

    unsigned int calculateDistance(int start, int end);
    EDirection getDirection(unsigned int from, unsigned int to);

    void setLoggerPath();
    void logMap(unsigned);
    void logInfluenceMap(unsigned nbTurn);
};

#endif // MAP_HEADER
