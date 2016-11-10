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

class SearchMap;
class TileInfo;

class Map : Singleton
{

    static Map m_instance;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_influenceRange;
    std::map<unsigned int, Node*> m_nodeMap;
    std::vector<unsigned int> m_goalTiles;
    std::map<unsigned, bool> m_seenTiles;
    std::vector<Node*> m_interestingNodes;
    //std::map<unsigned int, SearchMap*> m_searchMap;

    // Log stuff
    Logger m_logger;
    Logger m_loggerInfluence;

private:
    Map() : m_width(0), m_height(0)
    {}
    float calculateDistance(int start, int end);
    std::string getStringDirection(unsigned int, unsigned int);
    void testAddTile(std::vector<unsigned>& v, unsigned int, unsigned int tileId);
public:
    void setLoggerPath(const std::string &a_path);
    void setNodeType(unsigned int, Node::NodeType);
    void createNode(Node*);
    void connectNodes();
    Node* getNode(unsigned int, unsigned int);
    Node* getNode(unsigned int);
    std::map<unsigned, unsigned> getBestGoalTile(std::map<unsigned, NPCInfo> npcInfo);
    EDirection getNextDirection(unsigned int a_start, unsigned int a_end);
    static Map *get() noexcept
    {
        return &m_instance;
    }

    unsigned int getWidth() const
    {
        return m_width;
    }
    void setWidth(unsigned int w)
    {
        m_width = w;
    }
    unsigned int getHeight() const
    {
        return m_height;
    }
    void setHeight(unsigned int h)
    {
        m_height = h;
    }
    unsigned int getInfluenceRange() const
    {
        return m_influenceRange;
    }
    void setInfluenceRange(unsigned int range)
    {
        m_influenceRange = range;
    }

    void addSeenTile(unsigned tileId)
    {
        if(m_seenTiles[tileId])
        {
            return;
        }
        m_seenTiles[tileId] = false;
    }

    void visitTile(unsigned tileId)
    {
        m_seenTiles[tileId] = true;
    }

    bool isVisited(unsigned tileId)
    {
        return m_seenTiles[tileId];
    }

    std::vector<unsigned> getNonVisitedTile()
    {
        std::vector<unsigned> v;
        for(auto seenTile : m_seenTiles)
        {
            if(!seenTile.second)
            {
                v.push_back(seenTile.first);
            }
        }
        return v;
    }

    std::vector<unsigned> getMostInfluencedTile();

    void addGoalTile(unsigned int number);
    void createInfluenceMap();
    void propagateInfluence();
    void propage(Node* myNode, unsigned curDist, unsigned maxDist, float initialInfluence) const;

    std::vector<unsigned int> getNpcPath(unsigned int a_start, unsigned int a_end, std::set<Node::NodeType> forbiddenType = {Node::FORBIDDEN});
    std::vector<unsigned int> getNearUnVisitedTile(unsigned int a_currentId);

    bool canMoveOnTile(unsigned int a_fromTileId, unsigned int a_toTileId);

    std::vector<unsigned int> getNearInfluencedTile(unsigned int a_currentId);
    bool isAllNeighboorHaveSameInfluence(Node* node);
    void logMap(unsigned);
    void logInfluenceMap(unsigned nbTurn);
};

#endif // MAP_HEADER
