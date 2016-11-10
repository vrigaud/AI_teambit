#include "MyBotLogic.h"
#include "../TurnInfo.h"
#include "Graph.h"
#include "Node.h"

#include <windows.h>

#include <algorithm>



MyBotLogic::MyBotLogic()
{
    //Write Code Here
}

/*virtual*/ MyBotLogic::~MyBotLogic()
{
    //Write Code Here
    delete blackBoard;
}

/*virtual*/ void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
#ifdef BOT_LOGIC_DEBUG
    mLogger.Init(_logpath, "MyBotLogic.log");
#endif

    BOT_LOGIC_LOG(mLogger, "Configure", true);


    //Write Code Here
}

/*virtual*/ void MyBotLogic::Load()
{
    //Write Code Here
    Sleep(8000);
}

/*virtual*/ void MyBotLogic::Init(LevelInfo& _levelInfo)
{
    blackBoard = new BlackBoard(_levelInfo);
}

/*virtual*/ void MyBotLogic::OnBotInitialized()
{
    //Write Code Here
}

/*virtual*/ void MyBotLogic::OnGameStarted()
{
    //Write Code Here
}

/*virtual*/ void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{

    blackBoard->update(_turnInfo);
    blackBoard->giveActionForThisTurn(_actionList);


    BOT_LOGIC_LOG(mLogger, blackBoard->toStdString(), true);
}

/*virtual*/ void MyBotLogic::Exit()
{
    //Write Code Here

    //BOT_LOGIC_LOG(mLogger, blackBoard->getExplorationGraph().toStdString(), true);


    BOT_LOGIC_LOG(mLogger, "*************END***********", true);
}
