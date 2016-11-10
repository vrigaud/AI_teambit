#ifndef NPC_H
#define NPC_H

#include "MyBotLogic.h"
#include <algorithm>
#include <list>

struct NPCInfo;

class NPC
{
public:
	using cost_type = MyBotLogic::cost_type;
	using distance_id_pair_type = MyBotLogic::distance_id_pair_type;
	using id_count_pair_type = std::map<unsigned int, unsigned int>;

private:
	NPCInfo mInfos;
	int mGoalID;
	unsigned int mNbTurnBlocked;
	unsigned int mNextTileID;

	std::list<unsigned int> mPathToGoal;    //Path of Tile IDs to goal
	id_count_pair_type mExploredTiles;

public:
	NPC(const NPCInfo& infos)
		: mPathToGoal{},
		mNextTileID{ infos.tileID }, mNbTurnBlocked{ 0 }, mGoalID{ -1 }
	{
		mInfos = infos;
	}
	NPC(const NPC& other)
	{
		mInfos = other.mInfos;
		mNextTileID = other.mNextTileID;
		mNbTurnBlocked = other.mNbTurnBlocked;
		mPathToGoal = other.mPathToGoal;
	}
	~NPC() {}

	bool isArrived()
	{
		return mPathToGoal.empty();
	}
	bool isBlocked()
	{
		if (Graph::Instance().getNode(mPathToGoal.front())->taken())
		{
			return true;
		}
		return false;
	}
	bool hasGoal() { return mGoalID > -1; }

	void update(std::vector<Action*>& actionList);
	void updateInfo(const NPCInfo& npcInfo);

	unsigned int getTileID() { return mInfos.tileID; }
	void setGoalID(unsigned int goalID)
	{
		mGoalID = goalID;
	}
	void setPath();

private:
	EDirection chooseDirection(const unsigned int&, const unsigned int&);
	void explore(std::vector<Action*>& actionList);
	void findNewPath();
	void followPath(std::vector<Action *> &_actionList);
	std::vector<Node*> getAccessibleNeighbours(const std::vector<Node*> &neighbourTiles, Node * currNode);
	unsigned int getLessVisitedTileId(std::vector<Node*> accessibleNodes);
	std::list<unsigned int> pathFinderAStar(const Graph& graph, const unsigned int& startID, const unsigned int& goalID, Heuristic&);
	void updatePathToGoal()
	{
		Graph::Instance().getNode(mInfos.tileID)->taken() = false;
		mNextTileID = mPathToGoal.front();
		mPathToGoal.pop_front();
		Graph::Instance().getNode(mNextTileID)->taken() = true;
	}
};

#endif //!NPC8_H
