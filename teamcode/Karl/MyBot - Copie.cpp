#include "../MyClasses/AStarState/MyBot.h"
#include "../MyClasses/AStarState/Graph.h"
#include "../MyClasses/AStarState/FromTo.h"
#include "../MyClasses/AStarState/MyNeighbours.h"
#include "../MyClasses/AStarState/Node.h"

#include <random>
#include <future>
#include <map>
#include <vector>

#include <string>
#include <fstream>
#include <sstream>

std::ofstream myLog{ "log.txt" };

class InvalidState {};

MyBot::MyBot(const NPCInfo& npc) : NPCInfo{ npc }, /*currentTileId{ npc.tileID },*/ currentTargetTileId{ npc.tileID }, nextTileId{ npc.tileID }, currentState{ CheckForTargets }, nextState{ CheckForTargets }, myPaths{} {}

void MyBot::evalState(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
	bool stableBotsState{};
	std::stringstream ss;
	ss << "TURN: " << _turnInfo.turnNb << "\n";
	myLog << ss.str();
	ss.str(std::string());
	do 
	{
		stableBotsState = true;

		for (auto& bot : Graph::getInstance().bots)
		{
			ss << "BOT: " << bot.first << "\n\tCSTATE: " << bot.second->currentState << "\n\tNSTATE: " << bot.second->nextState << "\n\tCTILE: " << _turnInfo.npcs[bot.first].tileID  << "\n\tTTILE: " << bot.second->currentTargetTileId << "\n\tNTILE: " << bot.second->nextTileId << "\n";
			myLog << ss.str();
			ss.str(std::string());
			bot.second->currentState = bot.second->nextState;

			switch (bot.second->currentState)
			{
			case CheckForTargets:
				if (Graph::getInstance().targets.size() == _turnInfo.npcs.size() ||bot.second->hasTarget())
				{
					bot.second->nextState = (_turnInfo.npcs[bot.first].tileID == bot.second->currentTargetTileId) && (bot.second->currentTargetTileId != bot.second->npcID) ? ReachedMyTarget : Good;
				}
				else
				{
					//AStar
					bot.second->nextState = bot.second->findTargets(_turnInfo) ? Good : CanTSeeTargets;
				}
				break;
			case Good:
				for (auto otherBot : Graph::getInstance().bots)
				{
					if (otherBot.first == bot.second->npcID)
						continue;

					/*if (_turnInfo.npcs[otherBot.first].tileID == otherBot.second->currentTargetTileId)
						continue;
*/
					if (bot.second->currentTargetTileId == otherBot.second->currentTargetTileId)
					{
						size_t myPathLength = bot.second->myPaths.getPathSizeToTarget(bot.second->currentTargetTileId);
						size_t itsPathLenght = otherBot.second->myPaths.getPathSizeToTarget(otherBot.second->currentTargetTileId);

						if (myPathLength <= itsPathLenght && otherBot.second->nextState != IHaveTheSameTarget)
						{
							bot.second->nextState = IHaveTheSameTarget;
						}

						break;
					}
					
					if (bot.second->nextTileId == otherBot.second->nextTileId)
					{
						bot.second->nextState = IMustWait;
						otherBot.second->nextState = IMustPassFirst;

						if(otherBot.second->hasTarget() && otherBot.second->nextTileId == otherBot.second->currentTargetTileId)
						{
							bot.second->nextState = IMustPassFirst;
							otherBot.second->nextState = IMustWait;
						}	

						break;
					}

					//if (_turnInfo.npcs[bot.first].tileID == bot.second->currentTargetTileId)
					//{
					//	bot.second->nextState = ReachedMyTarget;
					//	
					//	break;
					//}
				}

				bot.second->nextState = bot.second->nextState == Good ? ReadyToMove : bot.second->nextState;
				break;
			case ReadyToMove:
				if (_turnInfo.npcs[bot.first].tileID == bot.second->nextTileId)
				{
					bot.second->nextTileId = bot.second->getNextNodeToCurrentTarget();
					bot.second->nextState = Good;
				}
				else
				{
					EDirection dir = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourDirection(bot.second->nextTileId);
					//bot.second->currentTileId = bot.second->nextTileId;
					_actionList.push_back(new Move{ bot.second->npcID , dir });
					bot.second->nextState = NextTurn;
				}
				break;
			case CanTSeeTargets:
				
				for (auto otherBot : Graph::getInstance().bots)
				{
					if (otherBot.first == bot.second->npcID)
						continue;

					/*if (_turnInfo.npcs[otherBot.first].tileID == otherBot.second->currentTargetTileId)
						continue;*/

					if (bot.second->nextTileId == otherBot.second->nextTileId)
					{
						bot.second->nextState = IMustWait;
						otherBot.second->nextState = IMustPassFirst;

						if (otherBot.second->hasTarget() && otherBot.second->nextTileId == otherBot.second->currentTargetTileId)
						{
							bot.second->nextState = IMustPassFirst;
							otherBot.second->nextState = IMustWait;
						}

						break;
					}
				}
				bot.second->nextState = bot.second->nextState == CanTSeeTargets ? RandomMove : bot.second->nextState;
				break;
			case RandomMove:
				EDirection dir;

				if (_turnInfo.npcs[bot.first].tileID == bot.second->nextTileId)
				{
					EDirection dir;
					
					// GARDER UNE LISTE DES DEJA VISITE
					
					bool validDir{};
					do 
					{
						try
						{
							dir = bot.second->getRandomDirection();
							bot.second->nextTileId = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourId(dir);
							auto cost = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->checkTransition(bot.second->nextTileId, _turnInfo);
							if (cost == INFINITY)
								continue;
							validDir = true;
						}
						catch (...){}
					} while (!validDir);
					bot.second->nextState = CanTSeeTargets;
				}
				else
				{
					EDirection dir = Graph::getInstance().nodes[_turnInfo.npcs[bot.first].tileID]->neighbours->getNeighbourDirection(bot.second->nextTileId);
					//bot.second->currentTileId = bot.second->nextTileId;
					_actionList.push_back(new Move{ bot.second->npcID , dir });
					bot.second->nextState = NextTurn;
				}
				break;
			case IHaveTheSameTarget:
				bot.second->nextState = CheckForTargets;
				bot.second->findNextBestTarget();
				break;
			case IMustPassFirst:
				for (auto otherBot : Graph::getInstance().bots)
				{
					if (otherBot.first == bot.second->npcID)
						continue;

					if (bot.second->nextTileId == otherBot.second->nextTileId && otherBot.second->nextState == IMustPassFirst)
					{
						bot.second->nextState = IMustWait;

						if (otherBot.second->hasTarget() && otherBot.second->nextTileId == otherBot.second->currentTargetTileId)
						{
							bot.second->nextState = IMustPassFirst;
							otherBot.second->nextState = IMustWait;
						}
					}
					else
					{
						if (bot.second->myPaths.hasTarget())
						{
							bot.second->nextState = ReadyToMove;
						}
						else
						{
							bot.second->nextState = RandomMove;
						}
						
					}
				}
				break;
			case IMustWait:
				for (auto otherBot : Graph::getInstance().bots)
				{
					if (otherBot.first == bot.second->npcID)
						continue;

					bot.second->nextState = IMustWait;

					if (bot.second->nextTileId == _turnInfo.npcs[otherBot.first].tileID && _turnInfo.npcs[otherBot.first].tileID != otherBot.second->nextTileId)
					{
						if (bot.second->myPaths.hasTarget())
						{
							bot.second->nextState = ReadyToMove;
						}
						else
						{
							bot.second->nextState = RandomMove;
						}
					}
				}
				break;
			case ReachedMyTarget:
			case NextTurn:
				bot.second->nextState = NextTurn;
				break;
			default:
				throw InvalidState{};
			}

			stableBotsState = stableBotsState && bot.second->currentState == bot.second->nextState;
		}
		ss << "\n";
		myLog << ss.str();
		ss.str(std::string());
	} while (!stableBotsState);

	for (auto& bot : Graph::getInstance().bots)
	{
		bot.second->nextState = CheckForTargets;
	}
}

void MyBot::findNextBestTarget()
{
	try
	{
		currentTargetTileId = myPaths.findNextBestTarget(currentTargetTileId);
		nextTileId = myPaths.peekNextTileToTarget(currentTargetTileId);
	}
	catch (...) 
	{
		nextState = CanTSeeTargets;
	}
}

bool MyBot::hasTarget() const
{
	return myPaths.hasTarget();
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
	std::map<Node::id_value_type, std::future<std::vector<Node::id_value_type>>> targetsAStar;
	for (auto t : Graph::getInstance().targets)
	{
		Node::id_value_type initial{ _turnInfo.npcs[npcID].tileID };

		//FAIR EDE ASTAR UNE METHOD

		targetsAStar[t] = std::async
		(
			[initial, &_turnInfo](const Node::id_value_type target)
			{
				FromTo n( initial,initial, Graph::getInstance().nodes[initial]->getHeuristicTo(target) );

				std::multiset<FromTo, FromToComparator> open;
				std::vector<FromTo> closed;

				open.insert(n);
				while (true)
				{
					if (open.empty())
					{
						closed = std::vector<FromTo>{ FromTo{ initial,initial } };
						break;
					}

					n = *(open.begin());
					open.erase(open.begin());
					closed.push_back(n);

					if (n.to == target)
					{
						break;
					}

					for (auto neighbour : Graph::getInstance().nodes[n.to]->neighbours->getNeighboursList())
					{
						FromTo nP{ n.to, neighbour, Graph::getInstance().nodes[n.to]->checkTransition(Graph::getInstance().nodes[neighbour]->id, _turnInfo) };

						if (neighbour == n.from || nP.cost == INFINITY)
							continue;

						auto gPrime = (n.cost - Graph::getInstance().nodes[n.to]->getHeuristicTo(target));
						auto C = Graph::getInstance().nodes[n.to]->neighbours->neighbourCost(neighbour);
						auto H = Graph::getInstance().nodes[neighbour]->getHeuristicTo(target);

						nP.cost = gPrime + C + H;

						bool isInOpenClosed{};

						for (auto o{ open.begin() }; o != open.end(); ++o)
						{
							if ((*o).from == nP.from && (*o).from != (*o).to)
							{
								isInOpenClosed = true;
								if ((*o).cost >= nP.cost)
								{
									open.erase(o);
									open.insert(nP);
								}
								break; // Dois-je breaker si ya encore des noeuds du vieux chemin?
							}
						}

						for (auto c{ closed.begin() }; c != closed.end(); ++c)
						{
							if ((*c).from == nP.from && (*c).from != (*c).to)
							{
								isInOpenClosed = true;
								if ((*c).cost >= nP.cost)
								{
									closed.erase(c);
									open.insert(nP);
								}
								break; // Dois-je breaker si ya encore des noeuds du vieux chemin?
							}
						}

						if (!isInOpenClosed)
						{
							open.insert(nP);
						}
					}
				}

				std::vector<Node::id_value_type> path{};
				for_each(closed.begin(), closed.end(), [&path](FromTo& id) {path.push_back(id.to);});
				return path;
			}
			, t
		);
	}
	for (auto t : Graph::getInstance().targets) // FOR_EACH
	{
		auto path = targetsAStar[t].get();
		for (auto id : path)
		{
			if (id != _turnInfo.npcs[npcID].tileID) // Open empty flag
			{
				addNodeToTargetPath(t, id);
			}
		}
	}

	bool aStarSuccessful{};

	if (hasTarget())
	{
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

//void MyBot::setTurnAction(const TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
//{
//	if (currentState != IHaveNoTarget) // A enlever?
//	{
//		switch (currentState)
//		{
//		case Good:
//		case IMustPassFirst:
//		{
//			auto nextTileId = getNextNodeToCurrentTarget();
//			EDirection dir = Graph::getInstance().nodes[currentTileId]->neighbours->getNeighbourDirection(nextTileId);
//			currentTileId = nextTileId;
//			_actionList.push_back(new Move{ npcID , dir });
//			nextState = Good;
//		}
//			break;
//		case CanTSeeTargets:
//		{			
//			EDirection dir;
//
//			// GARDER UNE LISTE DES DEJA VISITE
//
//			bool validDir{};
//			do 
//			{
//				try
//				{
//					dir = getRandomDirection();
//					auto nextTileId = Graph::getInstance().nodes[currentTileId]->neighbours->getNeighbourId(dir);
//					auto cost = Graph::getInstance().nodes[currentTileId]->checkTransition(nextTileId, _turnInfo);
//					if (cost == INFINITY)
//						continue;
//					currentTileId = nextTileId;
//					validDir = true;
//				}
//				catch (...){}
//			} while (!validDir);
//			_actionList.push_back(new Move{ npcID , dir });
//			nextState = IHaveNoTarget;
//		}
//			break;
//		default:
//			nextState = Good;
//		}
//	}
//}