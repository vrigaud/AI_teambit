/*
Karl-Etienne Perron
Alexandre Sendra
*/

#include "../MyClasses/AStarState/Graph.h"
#include "../MyClasses/AStarState/Node.h"
#include "../MyClasses/AStarState/MyNeighbours.h"

#include <algorithm>

Graph::~Graph() { deleteGraph(); }

void Graph::deleteGraph()
{
	for (auto n : nodes)
	{
		delete n.second;
	}
	for (auto b : bots)
	{
		delete b.second;
	}
}

void Graph::build(const size_t x, const size_t y)
{
	deleteGraph();

	width = x;
	height = y;

	for (Node::id_value_type i{}; i < (x*y); i++)
	{
		Node* node = new Node{ i };
		try
		{
			node->findNeighbours(x, y);
			nodes[i] = node;
		}
		catch (...)
		{
			delete node;
			throw;
		}
	}
}

void Graph::refreshBots(const TurnInfo& _turnInfo)
{
	if (_turnInfo.npcs.size() > bots.size())
	{
		for (auto npc : _turnInfo.npcs)
		{
			if (bots.find(npc.first) == bots.end())
			{
				bots[npc.first] = new MyBot{ npc.second.npcID, npc.second.tileID };
			}
		}
	}
}

void Graph::findVisibleTargets(const TurnInfo& _turnInfo)
{
	for (auto t : _turnInfo.tiles)
	{
		bool isNotInGraph{ targets.find(t.second.tileID) == targets.end() };
		bool isTarget{ t.second.tileAttributes.find(ETileAttribute::TileAttribute_Target) != t.second.tileAttributes.end() };

		if (isTarget && isNotInGraph)
			targets.insert(t.first);
	}
}

/*
	This method ensures we have all the information pertaining to the walls, forbidden tiles and other objects...
*/
void Graph::refreshVisibleInfo(const TurnInfo& _turnInfo)
{
	for (auto n : _turnInfo.tiles)
	{
		//If forbidden tile
		if (_turnInfo.tiles.find(n.second.tileID) != _turnInfo.tiles.end() && _turnInfo.tiles.at(n.second.tileID).tileAttributes.find(ETileAttribute::TileAttribute_Forbidden) != _turnInfo.tiles.at(n.second.tileID).tileAttributes.end())
		{
			//We add "walls" around it
			for (size_t i{}; i < Node::NB_WALLS; i++)
			{
				if (nodes[n.second.tileID]->isTowardsTip(static_cast<EDirection>(i)))
					continue;

				nodes[n.second.tileID]->peekedAtSchrodingersWall(static_cast<EDirection>(i));

				//In case there is no neighbour on the other side...
				try
				{
					auto neighbourId = nodes[n.second.tileID]->neighbours->getNeighbourId(static_cast<EDirection>(i));
					auto neighbourDirToNPC = nodes[neighbourId]->neighbours->getNeighbourDirection(n.second.tileID);
					nodes[neighbourId]->peekedAtSchrodingersWall(neighbourDirToNPC);
				}
				catch (...) {}
			}
		}

		//TODO
		//We do check ALL the objects, which we should not, but this will do for now...
		for (auto o : _turnInfo.objects)
		{
			if (!nodes[o.second.tileID]->allInfo())
			{
				auto dir = std::find(std::begin(o.second.edgesCost), std::end(o.second.edgesCost), 0);
				if (dir != std::end(o.second.edgesCost))
				{
					nodes[o.second.tileID]->peekedAtSchrodingersWall(static_cast<EDirection>(dir - std::begin(o.second.edgesCost)));

					//In case there is no neighbour on the other side of the wall...
					try
					{
						auto neighbourId = nodes[o.second.tileID]->neighbours->getNeighbourId(static_cast<EDirection>(dir - std::begin(o.second.edgesCost)));
						auto neighbourDirToNPC = nodes[neighbourId]->neighbours->getNeighbourDirection(o.second.tileID);
						nodes[neighbourId]->peekedAtSchrodingersWall(neighbourDirToNPC);
					}
					catch (...)
					{
					}
				}
			}
		}
	}

	//We complete the information around each npc
	for (auto n : _turnInfo.npcs)
	{
		//If we don't have all the information on the npc's current tile, it's because there are no "walls"
		if (!nodes[n.second.tileID]->allInfo())
		{
			for (size_t i{}; i < Node::NB_WALLS; i++)
			{
				if (!nodes[n.second.tileID]->isThereAWall(static_cast<EDirection>(i)))
				{
					nodes[n.second.tileID]->peekedAtSchrodingersWall(static_cast<EDirection>(i), false);
					auto neighbourId = nodes[n.second.tileID]->neighbours->getNeighbourId(static_cast<EDirection>(i));
					auto neighbourDirToNPC = nodes[neighbourId]->neighbours->getNeighbourDirection(n.second.tileID);
					nodes[neighbourId]->peekedAtSchrodingersWall(neighbourDirToNPC, false);
				}

			}
		}
	}
}