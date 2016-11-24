#include "Graph.h"
#include "Heuristic.h"
#include "LevelInfo.h"
#include "Node.h"
#include "NPC.h"
#include "TurnInfo.h"

#include <algorithm>
#include <iterator>
#include <queue>


Graph Graph::singletonGraph;

Graph::Graph() noexcept : mGraph{}, mTargets{}
{}

Graph::~Graph()
{
	for (auto node : mGraph)
		delete node.second;
}

void Graph::addNode(Node* node)
{
	mGraph[node->getID()] = node;
}

void Graph::init(const LevelInfo& lvlInfo)
{
	mLevelInfo = lvlInfo;
	int nbTiles{ lvlInfo.colCount * lvlInfo.rowCount };

	addNode(new Node{ 0 });

	//Add and link the added node with the previous one
	for (int i{ 1 }; i < nbTiles; ++i)
	{
		if (i % lvlInfo.colCount != 0)
			addNode(new Node{ i, mGraph.at(i - 1) });
		else
			addNode(new Node{ i });
	}
	//Link the remaining surrounding nodes between them
	connectSurroundings();

	initNodeFlags();

}

void Graph::initNodeFlags()
{
	for (auto& npc : mLevelInfo.npcs)
	{
		mGraph[npc.second.tileID]->taken() = true;
	}
}

void Graph::connectSurroundings()
{
	int idNeighbour{ 0 };
	std::for_each(mGraph.find(mLevelInfo.colCount),
		mGraph.end(),
		[&idNeighbour, this](std::pair<unsigned int, Node*> currPairNode)
	{
		connectionEvenLinesOnRight(currPairNode.second->getID(), idNeighbour);
		++idNeighbour;
	});
}

/* Links all nodes of a line with their corresponding upper & lower neighbours.
**NOTE: this method is ONLY VALID when the EVEN lines are shifted of one Tile to the RIGHT.
*/
void Graph::connectionEvenLinesOnRight(const unsigned int& idNode, const int& idNeighbour)
{
	if ((idNode / mLevelInfo.colCount) % 2 != 0)    //Ligne paire, indice ligne impaire (OK)
	{
		if (idNode % mLevelInfo.colCount == mLevelInfo.colCount - 1)  //Dernière colonne (OK)
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
		else
		{
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 4, 1); //noeud en haut a gauche
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour + 1], 5, 2); // noeud en haut a droite
		}
	}
	else
		if (idNode % mLevelInfo.colCount == 0)
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
		else
		{
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour - 1], 4, 1); //noeud en haut a gauche
			mGraph[idNode]->setNeighbour(mGraph[idNeighbour], 5, 2); // noeud en haut a droite
		}
}

void Graph::updateMapInfo(TurnInfo& turnInfo)
{
	for (auto& tile : turnInfo.tiles)
	{
		mGraph[tile.first]->updateInfo(tile.second);
		if (std::find(
			std::begin(tile.second.tileAttributes),
			std::end(tile.second.tileAttributes),
			TileAttribute_Target)
			!= std::end(tile.second.tileAttributes)
			)
		{
			mTargets.emplace_back(mGraph[tile.first]);
		}
	}
	for (auto& obj : turnInfo.objects)
	{
		mGraph[obj.second.tileID]->setWalls(obj.second.edgesCost);
	}
}

void Graph::updateNpcGoal(const std::vector<NPC*> &npcs)
{
	if (!Graph::Instance().getTargetList().empty())
	{
		for (auto target : Graph::Instance().getTargetList())
		{
			if (!target->taken())
			{
				unsigned int min_dist = UINT_MAX;
				Heuristic h{ target };
				int npcID{ -1 };
				for (unsigned int i{}; i < npcs.size(); ++i)
				{
					if (!npcs[i]->hasGoal())
					{
						cost_type temp = h.estimate(mGraph[npcs[i]->getTileID()], mLevelInfo);
						if (temp < min_dist)
						{
							min_dist = temp;
							npcID = i;
						}
					}
				}
				if (npcID > -1)
				{
					npcs[npcID]->setGoalID(target->getID());
					npcs[npcID]->setPath();
				}
			}
		}
	}
}