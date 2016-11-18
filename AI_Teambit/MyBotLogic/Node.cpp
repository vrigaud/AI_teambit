#include "Node.h"

Node::Node(int xVal, int yVal, unsigned int idVal, NodeType typeVal) : mID(idVal), mType(typeVal), mParent{}
{
    mPos = new Position(xVal, yVal);
}
