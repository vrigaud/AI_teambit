/*
	Karl-Etienne Perron
	Alexandre Sendra
*/

#include "../MyClasses/AStarState/MyBot.h"
#include "../MyClasses/AStarState/Graph.h"
#include "../MyClasses/AStarState/FromTo.h"
#include "../MyClasses/AStarState/MyNeighbours.h"
#include "../MyClasses/AStarState/Node.h"

#include <random>
#include <future>
#include <vector>
#include <algorithm>

#include <string>
#include <fstream>
#include <sstream>

std::ofstream myLog{ "log.txt" };

//Static attributes
std::unordered_map<Node::id_value_type, unsigned int> MyBot::nodesVisit{};
unsigned int MyBot::maxNodesVisit{};
unsigned int MyBot::minNodesVisit{};

//In case of a state machine malfunction
class InvalidState {};

MyBot::MyBot(unsigned int id, Node::id_value_type startTile) : npcID{id}, currentTargetTileId{ startTile }, nextTileId{ startTile }, currentState{ TurnStart }, nextState{ TurnStart }, myPaths{}
{
	visitedNode(startTile);
}

void MyBot::evalState(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
	bool stableBotsState{};

	//Log
	std::stringstream ss;
	ss << "TURN: " << _turnInfo.turnNb << "\n";
	myLog << ss.str();
	ss.str(std::string());

	//State machine loop
	do 
	{
		stableBotsState = true;

		for (auto& bot : Graph::getInstance().bots)
		{
			//Log
			ss << "BOT: " << bot.first << "\n\tCSTATE: " << bot.second->currentState << "\n\tNSTATE: " << bot.second->nextState << "\n\tCTILE: " << _turnInfo.npcs[bot.first].tileID  << "\n\tTTILE: " << bot.second->currentTargetTileId << "\n\tNTILE: " << bot.second->nextTileId << "\n";
			myLog << ss.str();
			ss.str(std::string());
			bot.second->currentState = bot.second->nextState;

			switch (bot.second->currentState)
			{
			case TurnStart:
				if (bot.second->hasTarget())
				{
					bot.second->nextState = IHaveTargets;
				}
				else
				{
					bot.second->nextState = IHaveNoTargets;
				}
				break;
			case IHaveTargets:
				for (auto otherBot : Graph::getInstance().bots)
				{
					if (otherBot.first == bot.second->npcID)
						continue;

					//If the bots have the same target, one must change its current target.
					if (bot.second->currentTargetTileId == otherBot.second->currentTargetTileId)
					{
						size_t myPathLength = bot.second->myPaths.getPathSizeToTarget(bot.second->currentTargetTileId);
						size_t itsPathLenght = otherBot.second->myPaths.getPathSizeToTarget(otherBot.second->currentTargetTileId);

						if (myPathLength >= itsPathLenght && otherBot.second->nextState != NeedAnotherTarget)
						{
							bot.second->nextState = NeedAnotherTarget;
						}

						break;
					}
				}

				bot.second->nextState = bot.second->nextState == IHaveTargets ? IHaveSpecificTarget : bot.second->nextState;

				break;
			case IHaveNoTargets:
				//A*
				bot.second->nextState = bot.second->findTargets(_turnInfo) ? IHaveTargets : RandomMove;
				break;
			case NeedAnotherTarget:
				if (bot.second->findNextBestTarget())
				{
					bot.second->nextState = IHaveTargets;
				}
				// In case the npc had to move randomly because its target was already taken, it needs to search for a new one
				else
				{
					auto oldTargetSize = bot.second->myPaths.targetNb();

					// AStar 
					bot.second->findTargets(_turnInfo);
					bot.second->nextState = oldTargetSize == bot.second->myPaths.targetNb() ? bot.second->nextState = RandomMove : bot.second->nextState = IHaveTargets;
				}
				break;
			case IHaveSpecificTarget:
				if ((_turnInfo.npcs[bot.first].tileID == bot.second->currentTargetTileId) && (bot.second->currentTargetTileId != bot.second->npcID))
				{
					bot.second->nextState = ReachedTarget;
				}
				else
				{
					bot.second->nextState = SpecificMove;
				}
				break;
			case SpecificMove:
				if (_turnInfo.npcs[bot.first].tileID == bot.second->nextTileId)
				{
					bot.second->nextTileId = bot.second->getNextNodeToCurrentTarget();
					bot.second->visitedNode(bot.second->nextTileId);
					bot.second->nextState = TurnStart;
				}
				else
				{
					EDirection dir = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourDirection(bot.second->nextTileId);
					_actionList.push_back(new Move{ bot.second->npcID , dir });
					bot.second->nextState = TurnEnd;
				}
				break;
			case RandomMove:
				if (_turnInfo.npcs[bot.first].tileID == bot.second->nextTileId)
				{		
					//We get a "semi-random" move where we only consider the least visited neighbours
					auto leastV = bot.second->getLeastVisitedNeighbours(bot.first, _turnInfo);
					auto nextNeighbourId = leastV[static_cast<unsigned int>(bot.second->getRandomDirection()) % leastV.size()];
					EDirection dir{ Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourDirection(nextNeighbourId) };
					bot.second->nextTileId = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourId(dir);
					bot.second->visitedNode(bot.second->nextTileId);

					bot.second->nextState = TurnStart;
				}
				else
				{
					EDirection dir{ Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourDirection(bot.second->nextTileId) };
					_actionList.push_back(new Move{ bot.second->npcID , dir });
					bot.second->nextState = TurnEnd;
				}
				break;
			case ReachedTarget:
				bot.second->visitedNode(_turnInfo.npcs[bot.first].tileID, true);
			case TurnEnd:
				bot.second->nextState = TurnEnd;
				break;
			default:
				throw InvalidState{};
			}

			stableBotsState = stableBotsState && bot.second->currentState == bot.second->nextState;
		}

		//Log
		ss << "\n";
		myLog << ss.str();
		ss.str(std::string());
	} while (!stableBotsState);

	for (auto& bot : Graph::getInstance().bots)
	{
		bot.second->nextState = TurnStart;
	}
}

bool MyBot::findNextBestTarget()
{
	bool founddNextBestTarget{};

	try
	{
		currentTargetTileId = myPaths.findNextBestTarget(currentTargetTileId);
		nextTileId = myPaths.peekNextTileToTarget(currentTargetTileId);
		founddNextBestTarget = true;
	}
	catch (...) {}
	
	return founddNextBestTarget;
}

bool MyBot::hasTarget() const
{
	return myPaths.hasTarget();
}

void MyBot::clearNodesToTargets()
{
	myPaths.clearNodesToTargets();
}

Node::id_value_type MyBot::getNextNodeToCurrentTarget()
{
	return myPaths.getNextNodeToTarget(currentTargetTileId);
}

void MyBot::addNodeToTargetPath(Node::id_value_type t, Node::id_value_type n)
{
	myPaths.addNodeToTargetPath(t, n);
}

//AStar
bool MyBot::findTargets(TurnInfo& _turnInfo)
{
	std::unordered_map<Node::id_value_type, std::future<std::vector<Node::id_value_type>>> targetsAStar;
	for (auto t : Graph::getInstance().targets)
	{
		Node::id_value_type initial{ _turnInfo.npcs[npcID].tileID };

		//Parallel A* for each target
		targetsAStar[t] = std::async
		(
			[initial, &_turnInfo](const Node::id_value_type target)
			{
				FromTo n( initial,initial, Graph::getInstance().nodes.at(initial)->getHeuristicTo(target) );
				
				std::multiset<FromTo, FromToComparator> open;
				std::multimap<Node::id_value_type,FromTo> closed;

				open.insert(n);
				while (true)
				{
					if (open.empty())
					{
						closed.insert(std::pair<Node::id_value_type, FromTo>(initial,FromTo(initial,initial, 0.0)));
						break;
					}

					n = *(open.begin());
					open.erase(open.begin());
					closed.insert(std::pair<Node::id_value_type, FromTo>(n.to, n));

					if (n.to == target)
					{
						break;
					}

					size_t nCount{};

					for (auto neighbour : Graph::getInstance().nodes.at(n.to)->neighbours->getNeighboursList())
					{
						FromTo nP{ n.to, neighbour, Graph::getInstance().nodes.at(n.to)->checkTransition(Graph::getInstance().nodes.at(neighbour)->id, _turnInfo) };

						if (neighbour == n.from || nP.cost == INFINITY)
							continue;

						++nCount;

						auto gPrime = (n.cost - Graph::getInstance().nodes.at(n.to)->getHeuristicTo(target));
						auto C = Graph::getInstance().nodes.at(n.to)->neighbours->neighbourCost(neighbour);
						auto H = Graph::getInstance().nodes.at(neighbour)->getHeuristicTo(target);

						nP.cost = gPrime + C + H;

						bool isInOpenClosed{};

						for (auto o{ open.begin() }; o != open.end(); ++o)
						{
							if ((*o).to == nP.to && (*o).from != (*o).to)
							{
								isInOpenClosed = true;
								if ((*o).cost >= nP.cost)
								{
									open.erase(o);
									open.insert(nP);
								}
								break;
							}
						}

						auto foundIt = closed.find(nP.to);
						if (foundIt != closed.end())
						{
							isInOpenClosed = true;
							if (foundIt->second.cost >= nP.cost)
							{
								closed.erase(foundIt);
								open.insert(nP);
							}
						}

						if (!isInOpenClosed)
						{
							open.insert(nP);
						}
					}
				}

				//We build the path back
				std::vector<Node::id_value_type> path{};
				auto ft = closed.find(target);
				if (ft != closed.end())
				{
					auto to = target;
					auto from = ft->second.from;

					path.push_back(to);
					while (from != to)
					{
						path.insert(path.begin(), from);
						ft = closed.find(from);
						to = ft->second.to;
						from = ft->second.from;
					}
				}

				return path;
			}
			, t
		);
	}
	for (auto t : Graph::getInstance().targets)
	{
		auto path = targetsAStar[t].get();
		for (auto id : path)
		{
			if (id != _turnInfo.npcs[npcID].tileID) // If not "Open was empty" flag
			{
				addNodeToTargetPath(t, id);
			}
		}
	}

	bool aStarSuccessful{};

	if (hasTarget())
	{
		//We set the current target to the first available target so that the npcs manage their paths on their own during the state machine loop
		currentTargetTileId = *Graph::getInstance().targets.begin();
		aStarSuccessful = true;
	}

	return aStarSuccessful;
}

EDirection MyBot::getRandomDirection() const
{
	static std::mt19937 prng{ std::random_device{}() };
	static std::uniform_int_distribution<int> randDir{ 0,7 };

	return static_cast<EDirection>(randDir(prng));
}

void MyBot::visitedNode(Node::id_value_type id, bool isTarget)
{
	if (isTarget)
	{
		//A visited target has little purpose of being visited again
		nodesVisit[id] = std::numeric_limits<unsigned int>::max();
	}
	else
	{
		auto visitCount = ++nodesVisit[id];
		maxNodesVisit = visitCount > maxNodesVisit ? visitCount : maxNodesVisit;
		minNodesVisit = visitCount < minNodesVisit ? visitCount : minNodesVisit;
	}
}

std::vector<unsigned int> MyBot::getLeastVisitedNeighbours(Node::id_value_type npcID, TurnInfo& _turnInfo)
{
	std::vector<unsigned int> temp{};
	std::vector<unsigned int> nV{};

	for (unsigned int i = 0; i < 8; i++)
	{
		try
		{
			auto n = Graph::getInstance().nodes[_turnInfo.npcs[npcID].tileID]->neighbours->getNeighbourId(static_cast<EDirection>(i));
			auto cost = Graph::getInstance().nodes[_turnInfo.npcs[npcID].tileID]->checkTransition(n, _turnInfo);
			if (cost == INFINITY)
				continue;
			temp.push_back(nodesVisit[n]);
		}
		catch (...){}
	}
	auto min = *std::min_element(temp.begin(), temp.end());
	for (int j = 0; j < 8; j++)
	{
		try
		{
			auto n = Graph::getInstance().nodes[_turnInfo.npcs[npcID].tileID]->neighbours->getNeighbourId(static_cast<EDirection>(j));
			auto cost = Graph::getInstance().nodes[_turnInfo.npcs[npcID].tileID]->checkTransition(n, _turnInfo);
			if (cost == INFINITY)
				continue;

			if(nodesVisit[n] == min)
				nV.push_back(n);
		}
		catch (...) {}
	}

	return nV;
}