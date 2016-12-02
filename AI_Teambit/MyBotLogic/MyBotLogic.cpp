#include "MyBotLogic.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "Map.h"
#include "MiCoMa.h"
#include "LoggerPath.h"
#include <windows.h>
#include <algorithm>
#include <chrono>


using namespace std::chrono;

MyBotLogic::MyBotLogic()
{
    //Write Code Here
}

MyBotLogic::~MyBotLogic()
{
    //Write Code Here
}

void MyBotLogic::Configure(int argc, char *argv[], const std::string& _logpath)
{
    mLogger.Init(_logpath, "MyBotLogic.log");
#ifdef BOT_LOGIC_DEBUG
#endif

    BOT_LOGIC_LOG(mLogger, "Configure", true);
    LoggerPath::getInstance()->setPath(_logpath);
    Map::getInstance()->setLoggerPath();
    mTurnCount = 0;
}

void MyBotLogic::Load()
{
    //Write Code Here
#ifdef BOT_LOGIC_DEBUG
    Sleep(5000);
#endif
}

void MyBotLogic::Init(LevelInfo& _levelInfo)
{
    Map::getInstance()->initMap(_levelInfo);
    MiCoMa::getInstance()->init(_levelInfo);
}

void MyBotLogic::OnBotInitialized()
{
    //Write Code Here
}

void MyBotLogic::Start()
{
    //Write Code Here
#ifdef BOT_LOGIC_DEBUG
    Sleep(5000);
#endif
}

void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{
    BOT_LOGIC_LOG(mLogger, "\nTURN #" + std::to_string(++mTurnCount), true);
    auto avant = system_clock::now();

    // Update graph
    Map::getInstance()->updateMap(_turnInfo);
    Map::getInstance()->logMap(mTurnCount);

    MiCoMa::getInstance()->update(_turnInfo, _actionList);
    auto apres = system_clock::now();
    BOT_LOGIC_LOG(mLogger, "turn duration : " + std::to_string(duration_cast<microseconds>(apres - avant).count()) + "us", true);    
}

void MyBotLogic::Exit()
{
    //Write Code Here
}