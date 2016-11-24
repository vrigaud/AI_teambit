#include "ExplorationGraph.h"
#include "ExploreNode.h"
#include "LevelInfo.h"
#include "TurnInfo.h"

#include <sstream>

using namespace ExplorationPart;

void ExplorationGraph::createExplorationGraph() 
{
    TileInfo intermediary{};

    addNode(new ExploreNode{ intermediary });

    //Horizontal node connections
    int posPrevNode = 0;
    size_t size = mColCount * mRowCount;

    for (size_t i = 1; i < size; ++i)
    {
        ++intermediary.tileID;

        addNode(new ExploreNode{ intermediary });

        //Adds & connects both current & previous nodes
        if (intermediary.tileID % mColCount != 0)
            mGraph.back()->connectTo(*mGraph[posPrevNode]);

        ++posPrevNode;
    }

    connectSurroundings();
}


void ExplorationGraph::init(LevelInfo& lvl)
{
    mRowCount = lvl.rowCount;
    mColCount = lvl.colCount;
    mGraph.reserve(mRowCount * mColCount);
    mTargetArray = new TargetArray(lvl);
}




void ExplorationGraph::connectSurroundings()
{
    size_t posPrevNode = 0;
    
    for (size_t iter = mColCount; iter < mGraph.size(); ++iter)
    {
        connectionEvenLinesOnRight(iter, posPrevNode);
        ++posPrevNode;
    }
}

/* Links all nodes of a line with their corresponding above & under neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void ExplorationGraph::connectionEvenLinesOnRight(size_t tileId, size_t posPrevNode)
{
    if ((tileId / mColCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
    {
        if (tileId % mColCount == mColCount - 1)  //Dernière colonne (OK)
        {
            mGraph[tileId]->connectTo(*mGraph[posPrevNode]); //noeud en haut a gauche
        }
        else
        {
            mGraph[tileId]->connectTo(*mGraph[posPrevNode]); //noeud en haut a gauche
            mGraph[tileId]->connectTo(*mGraph[posPrevNode + 1]); // noeud en haut a droite
        }
    }
    else
    {
        if (tileId % mColCount == 0)
        {
            mGraph[tileId]->connectTo(*mGraph[posPrevNode]); // noeud en haut a droite
        }          
        else       
        {          
            mGraph[tileId]->connectTo(*mGraph[posPrevNode - 1]); //noeud en haut a gauche
            mGraph[tileId]->connectTo(*mGraph[posPrevNode]); // noeud en haut a droite
        }
    }
}


std::string ExplorationGraph::toStdString() const noexcept
{
    std::stringstream stream;

    std::for_each(
        mGraph.begin(),
        mGraph.end(),
        [&stream, this](ExploreNode* node) {
            stream << node->toStdString() << "   ";

            if ((node->getID() % mColCount) == 0 && (node->getID() != 0))
            {
                if ((node->getID() / mColCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
                {
                    stream << "\n\n\t";
                }
                else
                {
                    stream << "\n\n";
                }
            }
        }
    );

    return stream.str();
}

void ExplorationGraph::updateExploreNode(const TurnInfo &turnInfo)
{
    std::for_each(turnInfo.objects.begin(),
        turnInfo.objects.end(),
        [this](std::pair<unsigned int, ObjectInfo> tileObjects)
    {
        mGraph[tileObjects.second.tileID]->update(tileObjects.second); //update objectInfo (nos edgeCost)

        ExploreNode* toDisconnect;

        for (unsigned int iter = 0; iter < 8; ++iter)
        {
            if (tileObjects.second.edgesCost[iter] == 0)
            {
                toDisconnect = chooseTile(tileObjects.second.tileID, static_cast<EDirection>(iter));

                if (toDisconnect)
                {
                    mGraph[tileObjects.second.tileID]->disconnect(toDisconnect);
                    toDisconnect->disconnect(mGraph[tileObjects.second.tileID]);
                }
                
            }
        }
    });


    std::for_each(turnInfo.tiles.begin(),
        turnInfo.tiles.end(),
        [this](std::pair<unsigned int, TileInfo> tileInfo)
    {
        mGraph[tileInfo.first]->update(tileInfo.second); //update tileInfo (nos tileAttributes)

        if (mGraph[tileInfo.first]->isTarget())
        {
            mTargetArray->addTarget(Target{ mGraph[tileInfo.first] });
        }
    });
}


ExploreNode* ExplorationGraph::chooseTile(unsigned int current, EDirection wallDirection)
{
    unsigned int rowCount = current / mColCount;
    unsigned int colCount = current % mColCount;

    if (rowCount == 0) //premiere ligne
    {
        switch (wallDirection)
        {
        case N:
        case NE:
        case NW:
            return nullptr;
        }
    }
    else if (rowCount == (mRowCount - 1)) //derniere ligne
    {
        switch (wallDirection)
        {
        case SE:
        case S:
        case SW:
            return nullptr;
        }
    }

    if (colCount == 0) //premiere colonne
    {
        if (rowCount % 2) // ligne impaire
        {
            switch (wallDirection)
            {
            case W:
                return nullptr;
            }
        }
        else
        {
            switch (wallDirection)
            {
            case NW:
            case W:
            case SW:
                return nullptr;
            }
        }
    }
    else if (colCount == (mColCount - 1)) //derniere colonne
    {
        if (rowCount % 2) // ligne impaire
        {
            switch (wallDirection)
            {
            case SE:
            case E:
            case NE:
                return nullptr;
            }
        }
        else
        {
            switch (wallDirection)
            {
            case E:
                return nullptr;
            }
        }
    }

    if (rowCount % 2) // ligne impaire
    {
        switch (wallDirection)
        {
        case N:
            return nullptr;

        case NE:
            return mGraph[current - mColCount + 1];

        case E:
            return mGraph[current + 1];

        case SE:
            return mGraph[current + mColCount + 1];

        case S:
            return nullptr;

        case SW:
            return mGraph[current + mColCount];

        case W:
            return mGraph[current - 1];

        case NW:
            return mGraph[current - mColCount];

        default:
            return nullptr;
        }
    }
    else // ligne paire
    {
        switch (wallDirection)
        {
        case N:
            return nullptr;

        case NE:
            return mGraph[current - mColCount];

        case E:
            return mGraph[current + 1];

        case SE:
            return mGraph[current + mColCount];

        case S:
            return nullptr;

        case SW:
            return mGraph[current + mColCount - 1];

        case W:
            return mGraph[current - 1];

        case NW:
            return mGraph[current - mColCount - 1];

        default:
            return nullptr;
        }
    }
}
