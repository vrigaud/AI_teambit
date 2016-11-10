#include "graph.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <iterator>
#include <map>
#include "TurnInfo.h"

Graph::Graph(std::vector<unsigned int> nodes, std::vector<std::array<unsigned int, 4>> edges, LevelInfo& lvl)
{
    m_nodes = nodes;
    m_edges = edges;
	m_lvl = lvl;
}

Graph::Graph()
{
}

Graph::~Graph()
{
    m_nodes.clear();
    m_edges.clear();
}

void Graph::updateGraph(TurnInfo& turnInfo, std::vector<unsigned int> &goal, std::vector<unsigned int> &occupiedTiles, LevelInfo levelInfo, Graph& graph)
{
#pragma region Update Graph
	goal.clear();
	occupiedTiles.clear();

	/*creer les tuiles*/

	std::vector<unsigned int> nodes;
	std::vector<std::array<unsigned int, 4>> edges;
	for (std::map<unsigned int, NPCInfo>::iterator it = turnInfo.npcs.begin();it != turnInfo.npcs.end();++it)
	{
		occupiedTiles.push_back(it->second.tileID);
	}

	for (std::map<unsigned int, TileInfo>::iterator it = turnInfo.tiles.begin();it != turnInfo.tiles.end();++it)
	{   /*Building the graph*/

		nodes.push_back(it->first);
		/*creer target*/
		if (it->second.tileAttributes.count(ETileAttribute::TileAttribute_Target) != 0)
		{
			if (std::find(occupiedTiles.begin(), occupiedTiles.end(), it->first) == occupiedTiles.end())
			{
				// If not occupied.
				goal.push_back(it->first);
			}
		}
		/*liaisons*/
		if ((it->first) % (levelInfo.colCount) != 0) //pas 1ere colonne
		{
			std::array<unsigned int, 4> liaison = { it->first,it->first - 1, EDirection::W, EDirection::E };
			edges.push_back(liaison); // OE ?
		}
		if ((it->first) >= levelInfo.colCount &&  /* pas la premiere rangee */
			(it->first %levelInfo.colCount != 0 || (it->first / levelInfo.colCount) % 2 == 1)) // Pas gauche ou ligne impaire
		{
			std::array<unsigned int, 4> liaison;
			if ((it->first / levelInfo.colCount) % 2 == 1)
			{
				liaison = { it->first,it->first - (levelInfo.colCount),EDirection::NW,EDirection::SE };
			}
			else
			{
				liaison = { it->first,it->first - (levelInfo.colCount + 1),EDirection::NW,EDirection::SE };
			}
			edges.push_back(liaison); //NO SE ?
		}
		if ((it->first) >= levelInfo.colCount && /* pas la premiere rangee */
			((it->first %levelInfo.colCount != levelInfo.colCount - 1) ||
			((it->first / levelInfo.colCount) % 2 == 0))) //pas a droite ou ligne paire
		{
			std::array<unsigned int, 4> liaison;
			if ((it->first / levelInfo.colCount) % 2 == 1)
			{
				liaison = { it->first,it->first - (levelInfo.colCount - 1),EDirection::NE,EDirection::SW };
			}
			else
			{
				liaison = { it->first,it->first - (levelInfo.colCount),EDirection::NE,EDirection::SW };
			}
			edges.push_back(liaison); // NESO ?
		}
	}

	std::multimap<unsigned int, ObjectInfo > mmList;
	for (auto o : turnInfo.objects)
	{
		mmList.insert(std::make_pair(o.second.tileID, o.second));
	}
#pragma endregion Update Graph

#pragma region CleanTiles

	/*Retrieve edges with Forbidden Tiles or walls*/
	edges.erase(std::remove_if(edges.begin(), edges.end(), [edges, turnInfo, mmList, this](std::array<unsigned int, 4> arc)
	{
		if (turnInfo.tiles.find(arc[0]) != turnInfo.tiles.end() &&
			turnInfo.tiles.find(arc[1]) != turnInfo.tiles.end())
		{
			TileInfo nodeInfo1 = turnInfo.tiles.at(arc[0]);
			TileInfo nodeInfo2 = turnInfo.tiles.at(arc[1]);

			auto removeEdge = false;
			auto search = mmList.find(arc[0]); // Find tileId
			auto searchReverse = mmList.find(arc[1]);
			if (search != mmList.end())
			{
				auto ret = mmList.equal_range(arc[0]);
				for (auto it = ret.first; it != ret.second; ++it)
				{
					int edgeIt = 0;
					for (int e : it->second.edgesCost)
					{
						if (e == 0 && edgeIt == arc[2]) //Trouve un mur!!
						{
							removeEdge = true;
						}
						edgeIt++;
					}
				}
			}

			// TODO: filter direction to blocked node, but can't at moment since graph doesn't dinstinguish directions
			if (searchReverse != mmList.end()) {
				auto ret = mmList.equal_range(arc[1]);
				for (auto it = ret.first; it != ret.second; ++it)
				{
					int edgeIt = 0;
					for (int e : it->second.edgesCost)
					{
						if (e == 0 && edgeIt == arc[3]) //Trouve un mur!!
						{
							removeEdge = true;
						}
						edgeIt++;
					}
				}
			}

			return (nodeInfo1.tileAttributes.count(ETileAttribute::TileAttribute_Forbidden) != 0 ||
				nodeInfo2.tileAttributes.count(ETileAttribute::TileAttribute_Forbidden) != 0 ||
				nodeInfo1.tileAttributes.count(ETileAttribute::TileAttribute_Obstacle) != 0 ||
				nodeInfo2.tileAttributes.count(ETileAttribute::TileAttribute_Obstacle) != 0 ||
				removeEdge);
		}
		return false;
	}), edges.end());
	graph.setNodes(nodes);
	graph.setEdges(edges);
	graph.setLvl(levelInfo);

#pragma endregion CleanTiles



}

void Graph::setEdges(std::vector<std::array<unsigned int, 4>> edges)
{
    m_edges = edges;
}
void Graph::setNodes(std::vector<unsigned int> nodes)
{
    m_nodes = nodes;
}

void Graph::setLvl(LevelInfo& lvl)
{
	m_lvl = lvl;
}
/*Compute the smallest distance between two nodes*/
unsigned int Graph::length(unsigned int firstNode, unsigned int sndNode)
{
	unsigned int firstX = firstNode%m_lvl.colCount;
	unsigned int firstY = firstNode / m_lvl.colCount;
	unsigned int sndX = sndNode%m_lvl.colCount;
	unsigned int sndY = sndNode / m_lvl.colCount;

	int deltaX = static_cast<int>(sndX - firstX);
	int deltaY = static_cast<int>(sndY - firstY);
	int deltas = deltaY - deltaX;
	return std::max(abs(deltaX), std::max(abs(deltaY), abs(deltas)));		
}

/* Retourne index voisin et enum direction */
std::vector<std::array<unsigned int, 2>> Graph::neighbours(unsigned int otherNode)
{
	std::vector<std::array<unsigned int, 2>> result;

    for (int i =0;i<m_edges.size();i++)
    {
        if (otherNode == m_edges.at(i)[0])
        {
			std::array<unsigned int, 2> neigh = { m_edges.at(i)[1],m_edges.at(i)[2] };
           result.push_back(neigh);
        }
        else if (otherNode == m_edges.at(i)[1])
        {
			std::array<unsigned int, 2> neigh = { m_edges.at(i)[0],m_edges.at(i)[3] };
            result.push_back(neigh);
        }
    }
	return(result);
}

std::vector<EDirection> Graph::bestPath(unsigned int firstNode, unsigned int sndNode, TurnInfo& turnInfo, unsigned int playedId)
{
#pragma region setup
	std::map<unsigned int, NODE_INFO> nodeInfo;
#pragma endregion setup

	std::vector<EDirection> path;
	std::vector<unsigned int> closedNodes; // The set of nodes already evaluated.
	std::vector<unsigned int> openedNodes; // The set of currently discovered nodes still to be evaluated.

	NODE_INFO ni;
	if (!m_lvl.bOmnicientMode)
	{
		for (unsigned int i = 0; i < neighbours(firstNode).size(); i++)
		{
			ni.id = neighbours(firstNode)[i][0];
			nodeInfo.insert(std::pair<unsigned int, NODE_INFO>(i, ni));
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_nodes.size(); i++)
		{
			ni.id = i;
			nodeInfo.insert(std::pair<unsigned int, NODE_INFO>(i, ni));
		}
	}

	// The set of currently discovered nodes still to be evaluated.
	// Initially, only the start node is known.
	openedNodes.push_back(firstNode);

	unsigned int currentNode = firstNode;
	while (openedNodes.size()) // while openSet is not empty // while openSet is not empty
	{
		std::sort(openedNodes.begin(), openedNodes.end(), [nodeInfo](int a, int b) {
			return(nodeInfo.at(a).fScore > nodeInfo.at(b).fScore);
		});

		currentNode = openedNodes.back(); //the node in openSet having the lowest fScore[] value
		
		openedNodes.pop_back(); 
		if (currentNode == sndNode)
		{
			return reversePath(firstNode, sndNode, nodeInfo);
		}

		for (auto neighbor : neighbours(currentNode))
		{
			if (std::count(closedNodes.begin(), closedNodes.end(), neighbor[0]) != 0)
			{
				continue;
			}

			unsigned int gScoreAttempt = nodeInfo[neighbor[0]].gScore + length(currentNode, neighbor[0]); /* implemenert length*/

			if (std::count(openedNodes.begin(), openedNodes.end(), neighbor[0]) == 0)
			{
				openedNodes.push_back(neighbor[0]);

			}
			else if (gScoreAttempt >= nodeInfo[neighbor[0]].gScore)
			{
				continue;
			}
			path.push_back(static_cast<EDirection>(neighbor[1])); 
			nodeInfo[neighbor[0]].parent = currentNode;

			nodeInfo[neighbor[0]].gScore = gScoreAttempt;
			nodeInfo[neighbor[0]].fScore = nodeInfo[neighbor[0]].gScore + length(neighbor[0], sndNode);

		}
		closedNodes.push_back(currentNode);
	} 

	// TODO returne FAIL!
    return reversePath(firstNode, sndNode, nodeInfo);
}

std::vector<EDirection> Graph::reversePath(unsigned int firstNode, unsigned int sndNode, std::map<unsigned int, NODE_INFO> ni)
{
	std::vector<EDirection> reversePath;
	std::vector<unsigned int> reverseNodes{ sndNode };
	unsigned int currentNode = sndNode;
	while (currentNode != firstNode)
	{
		auto it = reverseNodes.begin();
		reverseNodes.insert(it, ni[currentNode].parent);
		currentNode = ni[currentNode].parent;
	}
	unsigned int j = 0;
	for (auto n : reverseNodes)
	{
		std::vector<std::array<unsigned int, 2>> neig = neighbours(n);
		if (n != sndNode)
		{
			for (int i = 0;i < neig.size();i++)
			{
				if (neig.at(i)[0] == reverseNodes.at(j + 1))
				{
					reversePath.push_back(static_cast<EDirection>(neig.at(i)[1]));
					
				}
			}
		}
		j++;
	}
	return reversePath;
}