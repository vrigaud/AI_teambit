#ifndef SEARCHNODE_HEADER
#define SEARCHNODE_HEADER

class SearchNode
{
    unsigned int m_xpos;
    unsigned int m_ypos;
    unsigned int m_id;
    SearchNode* m_parent;
    unsigned int m_H;
    unsigned int m_G;
public:
    SearchNode(unsigned xpos, unsigned ypos, unsigned id, SearchNode* search_node)
        : m_xpos(xpos), m_ypos(ypos), m_id(id),
        m_parent(search_node), m_H(0), m_G(0)
    {}

    SearchNode(unsigned xpos, unsigned ypos, unsigned id)
        : m_xpos(xpos), m_ypos(ypos), m_id(id),
        m_parent(nullptr), m_H(0), m_G(0)
    {}

    unsigned int getX() const
    {
        return m_xpos;
    }
    unsigned int getY() const
    {
        return m_ypos;
    }
    unsigned int getId() const
    {
        return m_id;
    }
    unsigned int getH() const
    {
        return m_H;
    }
    unsigned int getG() const
    {
        return m_G;
    }
    unsigned int getF() const
    {
        return m_G + m_H;
    }
    SearchNode* getParent() const
    {
        return m_parent;
    }
    void setX(unsigned int x)
    {
        m_xpos = x;
    }
    void setY(unsigned int y)
    {
        m_ypos = y;
    }
    void setH(const  unsigned int H)
    {
        m_H = H;
    }
    void setG(const  unsigned int G)
    {
        m_G = G;
    }
    void setParent(SearchNode* parent)
    {
        m_parent = parent;
    }
};

#endif // SEARCHNODE_HEADER