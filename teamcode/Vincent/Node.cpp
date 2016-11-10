#include "Node.h"

Node::Node(int xVal, int yVal, unsigned int idVal, NodeType typeVal): m_ID(idVal), m_type(typeVal)
{
    m_pos = new Position(xVal, yVal);
}
