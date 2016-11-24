#include "Graph.h"
#include "LevelInfo.h"
#include "Node.h"
#include "TurnInfo.h"

#include <algorithm>
#include <iterator>

Graph Graph::singletonGraph;


Graph::Graph() noexcept : mGraph{}, mTargets{}
{}
void Graph::init(LevelInfo& lvlInfo)
{
    mRowCount = lvlInfo.rowCount;
    mColCount = lvlInfo.colCount;
    mGraph.reserve(mRowCount * mColCount);
}

void Graph::createGraph(LevelInfo& levelInfo)
{
    addNode(new Node{ levelInfo.tiles[0] });

    //Horizontal node connections
    int posPrevNode = 0;
    std::for_each(std::next(std::begin(levelInfo.tiles)),
        std::end(levelInfo.tiles),
        [&posPrevNode, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        //Adds & connects both current & previous nodes
        if (currIterTile.second.tileID % mColCount != 0)
            addNode(new Node{ currIterTile.second, mGraph[posPrevNode] });
        //Only adds
        else
            addNode(new Node{ currIterTile.second });

        //Looking for targets / goals
        if (std::find(std::begin(currIterTile.second.tileAttributes),
            std::end(currIterTile.second.tileAttributes),
            TileAttribute_Target)
            != std::end(currIterTile.second.tileAttributes)
            )
        {
            mTargets.emplace_back(new TileInfo{ currIterTile.second });
        }

        ++posPrevNode;
    });

    //Vertical node connections
    connectSurroundings(levelInfo);

}

void Graph::connectSurroundings(LevelInfo& levelInfo)
{
    int posPrevNode = 0;
    std::for_each(levelInfo.tiles.find(mColCount),
        levelInfo.tiles.end(),
        [&posPrevNode, this](std::pair<unsigned int, TileInfo> currIterTile)
    {
        connectionEvenLinesOnRight(currIterTile, posPrevNode);
        ++posPrevNode;
    });
}

/* Links all nodes of a line with their corresponding above & under neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void Graph::connectionEvenLinesOnRight(std::pair<unsigned int, TileInfo> currIterTile, int posPrevNode)
{
    if ((currIterTile.second.tileID / mColCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
    {
        if (currIterTile.second.tileID % mColCount == mColCount - 1)  //Dernière colonne (OK)
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode], 4, 1); //noeud en haut a gauche
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode + 1], 5, 2); // noeud en haut a droite
        }
    }
    else
        if (currIterTile.second.tileID % mColCount == 0)
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode], 5, 2); // noeud en haut a droite
        else
        {
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode - 1], 4, 1); //noeud en haut a gauche
            mGraph[currIterTile.second.tileID]->setNeighbour(*mGraph[posPrevNode], 5, 2); // noeud en haut a droite
        }
}

std::list<Node> Graph::pathFinder(Node& start, Node& goal, const LevelInfo& levelInfo)
{

    std::list<Node>closedNodes{};

    start.setParent(nullptr);
    std::list<Node>openedNode{ start };



    while (!openedNode.empty())
    {

        Node current = (*std::min_element(openedNode.begin(), openedNode.end()));


        openedNode.erase(std::find(openedNode.begin(), openedNode.end(), current));



        if ((current.getX(levelInfo) == goal.getX(levelInfo)) && (current.getY(levelInfo) == goal.getY(levelInfo)))
        {
            std::list<Node> result;
            Node* curr = &current;

            while (curr->getParent() != nullptr)
            {
                result.push_back(*curr);
                curr = curr->getParent();
            }

            result.reverse();
            return result;
        }

        for (int i = 0; i < 6; ++i)
        {
            Node* v = current.getNeighbours()[i];


            if (!v || std::find(v->getAttributes()->begin(), v->getAttributes()->end(), ETileAttribute::TileAttribute_Forbidden) != v->getAttributes()->end())
            {
                continue;
            }

            v->setParent(&current);


            auto testClosed = std::find(closedNodes.begin(), closedNodes.end(), *v);
            auto testOpened = std::find(openedNode.begin(), openedNode.end(), *v);

            if (
                testClosed != closedNodes.end() && ((*testClosed).getCost() < v->getCost()) ||
                testOpened != openedNode.end() && ((*testOpened).getCost() < v->getCost())
                )
            {
                continue;
            }

            v->setCost(current.getCost() + 1);
            v->setHeuristic(v->getCost() + v->manhattanDistance(&goal, levelInfo));
            openedNode.push_back(*v);
        }

        closedNodes.push_back(current);
    }


}

