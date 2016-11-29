#ifndef BLOCROOT_H
#define BLOCROOT_H


#include "BaseBloc.h"

#include <algorithm>

namespace BehaviourTree
{
	class BehaviourTreeObject
	{
	private:
		class BTreeResultFailed {};

		BaseBloc* mRoot;
		BlackBoard mBlackBoard;


	public:
		BehaviourTreeObject() : mRoot{}
		{}
		~BehaviourTreeObject()
		{
			delete mRoot;
		}

		void initialize(const LevelInfo& levelInfo);

		BaseBloc& getRoot() noexcept
		{
			return *mRoot;
		}

		BaseBloc* getChhild(size_t index) noexcept
		{
			return mRoot->getChild(index);
		}

		size_t getNbChild() const noexcept
		{
			return mRoot->getNbChild();
		}

		type getType() const noexcept
		{
			return mRoot->getType();
		}

		void run(const TurnInfo &turnInfo, std::vector<Action*> &actions)
		{
			mBlackBoard.update(turnInfo);
			if ((*mRoot)(mBlackBoard) != SUCCESS)
			{
				throw BTreeResultFailed{};
			}
			actions = mBlackBoard.getActionList();
		}

	};
}



#endif //!BLOCROOT_H