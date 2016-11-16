#ifndef MAP_HEADER
#define MAP_HEADER

#include "Node.h"
#include "Globals.h"
#include <map>
#include <vector>
#include "Singleton.h"
#include "NPCInfo.h"

#include "Logger.h"
#include <algorithm>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_MAP
#endif

#ifdef BOT_LOGIC_DEBUG_MAP
#define BOT_LOGIC_MAP_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_MAP_LOG(logger, text, autoEndLine) 0
#endif

struct TileInfo;
struct TurnInfo;

class Map : Singleton
{
    static Map mInstance;
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mInfluenceRange;
    std::vector<Node*> mNodeMap;
    std::vector<unsigned int> mGoalTiles;

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

    float calculateDistance(int start, int end);

public:
    static Map *getInstance() noexcept
    {
        return &mInstance;
    }
    
    void initMap(int, int, int = 0);
    void updateMap(TurnInfo&);
    void addGoalTile(unsigned int number);
    
    Node* getNode(unsigned int, unsigned int);
    Node* getNode(unsigned int);
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
        mInfluenceRange = range;
    }
    void setLoggerPath();

    void logMap(unsigned);
    void logInfluenceMap(unsigned nbTurn);
};

#endif // MAP_HEADER
