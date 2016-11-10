#pragma once

#include "BehaviorTree.h"


//
//BT::ITask* actionFactory(Blackboard& bboard)
//{
//	auto lambda = [&]() {
//		//Blabla
//		return true;
//	};
//
//	return new BT::Action<decltype(lambda)>(lambda);
//}
//
//
//class Blackboard
//{
//	int main()
//	{
//		BT::Selector sel();
//		sel.addTask(actionFactory(*this));
//
//		BT::Sequence seq();
//
//		seq.addTask(sel);
//	}
//};
