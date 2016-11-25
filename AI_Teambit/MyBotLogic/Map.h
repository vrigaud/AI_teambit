#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include "Singleton.h"
#include "Logger.h"
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
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

struct LevelInfo;
struct TileInfo;
struct ObjectInfo;
struct TurnInfo;
struct NPCInfo;

class Map : Singleton
{
    static Map mInstance;
	static unsigned int mGreatestZoneID;

    unsigned int mWidth;
    unsigned int mHeight;

    std::vector<Node*> mNodeMap;
    std::vector<unsigned int> mGoalTiles;
	unsigned int mInfluenceRange;
    int mVisionRange;

	std::map<unsigned, bool> mKnownTilesAndVisitedStatus;
	std::vector<Node*> mInterestingNodes;

    // Zone related attribute
    std::vector<bool> mWasDiffused;
    std::unordered_map<unsigned int, bool> mZonesClosedStatus;

    // Log stuff
    Logger mLogger;
    Logger mLoggerEdges;
    Logger mLoggerInfluence;
	Logger mLoggerGalacticZones;

private:
	Map() : mWidth(0), mHeight(0)
    {}

    // Graph creation
    void createNode(Node*);
    void connectNodes();
	void initZones(std::map<unsigned int, NPCInfo> npcsInfo);
    void updateEdges(TurnInfo&);
    void updateTiles(TurnInfo&);
	void updateZones(TurnInfo&);

    // Edges update 
    void processDoorState(ObjectInfo &object, Node* node, int i);

    EDirection inverseDirection(const EDirection& dir) const
    {
        return static_cast<EDirection>((dir + 4) % 8);
    }

	// Zone diffusion and management
	void diffuseZone(const unsigned int startTileID);
	void diffuseZoneRec(const unsigned int currentZoneID, const Node*, std::set<Node*, NodeZoneIDComparator>& diffusionOpenNodes);

    // Influence methods
	void propagateInfluence();
	void propagate(Node * myNode, unsigned curDist, unsigned maxDist, float initialInfluence) const;

public:
    static Map *getInstance() noexcept
    {
        return &mInstance;
    }
    
    void initMap(const LevelInfo&);
    void updateMap(TurnInfo&);
    void addGoalTile(unsigned int number);
    const std::vector<unsigned int>& getGoalIDs() const { return mGoalTiles; }
	void createInfluenceMap(const InfluenceData::InfluenceType& = InfluenceData::INFLUENCE_MAP);
    std::vector<unsigned int> getCloseMostInfluenteTile(unsigned int) const; 

    // Getter and Setter
	Node* getNode(unsigned int, unsigned int) const;
    Node* getNode(unsigned int) const;
    void setNodeType(unsigned int, Node::NodeType);
    unsigned int getWidth() const
    {
        return mWidth;
    }
    void setWidth(const unsigned int w)
    {
        mWidth = w;
    }
    unsigned int getHeight() const
    {
        return mHeight;
    }
    void setHeight(const unsigned int h)
    {
        mHeight = h;
    }
    unsigned int getInfluenceRange() const
    {
        return mInfluenceRange;
    }
    void setInfluenceRange(const unsigned int range)
    {
        BOT_LOGIC_MAP_LOG(mLoggerInfluence, "\t Influence Range = " + std::to_string(range), true);
        mInfluenceRange = range;
    }
	void addSeenTile(unsigned tileId);

    unsigned int calculateDistance(int start, int end) const;
    EDirection getDirection(unsigned int from, unsigned int to);
    bool canMoveOnTile(unsigned int, unsigned int);

    void setLoggerPath();
    void logMap(const unsigned int);
	void logZones(const unsigned int);
    void logInfluenceMap(const unsigned int nbTurn);
};

#endif // MAP_HEADER
