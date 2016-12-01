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

#include <string>

// For debug only
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
class Npc;

struct Controller;

struct Door
{
    unsigned int mTileId;
    unsigned int mOtherSideTileId;
    EDirection mDoorDirection;
    std::vector<Controller> mControllerId;
    unsigned int mIdDoor;

    Door(unsigned int tileId, unsigned int otherSideId, EDirection dir, unsigned int drId) : mTileId{ tileId }, mOtherSideTileId{ otherSideId }, mDoorDirection{ dir }, mControllerId{}, mIdDoor{ drId }
    {
        mControllerId.reserve(10);
    }

    bool operator==(const Door& d0)
    {
        return mIdDoor == d0.mIdDoor;
    }
};

//bool operator==(const Door& d0, const Door& d1)
//{
//    return d0.mIdDoor == d1.mIdDoor;
//};

struct Controller
{
    unsigned int mControllerId;
    unsigned int mIdDoor;
    unsigned int mTileID;

    bool operator==(const Controller& c0)
    {
        return mControllerId == c0.mControllerId;
    }
};

struct Zone;

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
    std::unordered_map<unsigned int, Zone> mZoneList;
    std::vector<Zone> mZone;

    // Attribute related to doors and devices
    std::vector<Door> mDoorsMap;
    std::vector<Controller> mControllers;


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
	void diffuseZoneRec(const unsigned int currentZoneID, Node*, std::set<Node*, NodeZoneIDComparator>& diffusionOpenNodes);
    void ensureNode(unsigned int zoneId, Node*);
    void ensureDoor(unsigned int zoneId, Door);
    void ensureController(unsigned int zoneId, Controller);

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
	unsigned int getNeighborTileIndex(unsigned int iCurrentNode, const EDirection & dir);
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

    std::vector<Controller> isLocallyLinked(unsigned int zoneId);
   // void findNpcOnTheSameZone(unsigned int zoneID);

};

struct Zone
{
    unsigned int mZoneId;
    std::vector<Door> mDoorOnZone;
    std::vector<Controller> mControllerOnZone;
    std::vector<Node*> mNodeOnZone;
    bool isClosed;

    Zone() : mZoneId{}
    {
        mDoorOnZone.reserve(5);
        mControllerOnZone.reserve(5);

        //Assumes Map is already constructed
        mNodeOnZone.reserve(Map::getInstance()->getWidth()*Map::getInstance()->getHeight());

        isClosed = false;
    }
};

#endif // MAP_HEADER
