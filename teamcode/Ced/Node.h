#ifndef NODE_H
#define NODE_H

#include "TileInfo.h"
#include <vector>


class Node
{
public:
	enum { NB_NEIGHBOURS = 6 };

private:
	TileInfo mTileInfo;

	std::vector<Node*> mNeighbours{ NB_NEIGHBOURS };	//CW - 0 on right side
	bool mWalls[NB_NEIGHBOURS]{};
	struct Flags
	{
		bool taken;
		bool obstacle;
		bool forbidden;
	} mFlags{};

public:
	Node(Node* other)
		: mTileInfo(other->mTileInfo), mNeighbours{ other->mNeighbours }
	{}
	Node(const int& id, Node* from = nullptr)
	{
		mTileInfo.tileID = id;
		if (from)
			linkWith(from);
	}
	Node(TileInfo& currTileInfo)
		: mTileInfo(currTileInfo)
	{}
	Node(TileInfo& currTileInfo, Node* from)
		: mTileInfo(currTileInfo)
	{
		linkWith(from);
	}


	~Node()
	{ }


	unsigned int getID() const { return mTileInfo.tileID; }
	std::set<ETileAttribute>& getTileAttributes() { return mTileInfo.tileAttributes; }
	std::vector<Node*> getNeighbours() const { return mNeighbours; }
	Node* getNeighbour(int i) const { return mNeighbours[i]; }

	void setNeighbour(Node* neighbour, unsigned int idxNeighb, unsigned int idxCurr)
	{
		mNeighbours[idxNeighb] = neighbour;
		neighbour->mNeighbours[idxCurr] = this;
	}
	void setWalls(unsigned int edgecost[])
	{
		unsigned int id{};
		for (unsigned int* i = edgecost; i < edgecost + 8; ++i, ++id)
		{
			unsigned int eCost = *i;
			if (!*i)
			{
				unsigned int idWall{};
				if (id <= 1)
				{
					idWall = 5;
				}
				else if (id <= 3)
				{
					idWall = id - 2;
				}
				else if (id <= 7)
				{
					idWall = id - 3;
				}
				mWalls[idWall] = true;

				if (mNeighbours[idWall])
				{
					if (idWall <= 2)
						mNeighbours[idWall]->mWalls[idWall + 3] = true;

					else //idWall : [3;5]
						mNeighbours[idWall]->mWalls[idWall - 3] = true;
				}

			}
		}
	}

	void updateInfo(const TileInfo& tileInfo)
	{
		mTileInfo = tileInfo;
		setFlags(tileInfo.tileAttributes);
	}

	bool isNotAvailable() const
	{
		return mFlags.obstacle || mFlags.forbidden
			|| mFlags.taken;
	}
	bool hasWall(const unsigned int& idNeighbour) const
	{
		return mWalls[idNeighbour];
	}
	bool& taken() { return mFlags.taken; }
	bool& obstacle() { return mFlags.obstacle; }

private:
	void linkWith(Node* n)
	{
		mNeighbours[3] = n;
		n->mNeighbours[0] = this;
	}
	void setFlags(const std::set<ETileAttribute>& tileAttributes)
	{
		if (std::find(
			std::begin(tileAttributes),
			std::end(tileAttributes),
			TileAttribute_Obstacle)
			!= std::end(tileAttributes)
			)
		{
			mFlags.obstacle = true;
		}
		if (std::find(
			std::begin(tileAttributes),
			std::end(tileAttributes),
			TileAttribute_Forbidden)
			!= std::end(tileAttributes)
			)
		{
			mFlags.forbidden = true;
		}
	}
};

#endif // !NODE_H
