#include "MyBotLogic.h"
#include "TurnInfo.h"
#include "LevelInfo.h"
#include "Map.h"
#include "LoggerPath.h"
#include <windows.h>
#include <algorithm>

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
#ifdef BOT_LOGIC_DEBUG
    mLogger.Init(_logpath, "MyBotLogic.log");
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
    Map::getInstance()->initMap(_levelInfo.rowCount, _levelInfo.colCount, _levelInfo.visionRange);
    for (auto npc : _levelInfo.npcs)
    {
        mNpcs.push_back(new Npc(npc.second.npcID, npc.second.tileID));
    }
}

void MyBotLogic::OnBotInitialized()
{
    //Write Code Here
}

void MyBotLogic::OnGameStarted()
{
    //Write Code Here
}

void MyBotLogic::FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList)
{

    BOT_LOGIC_LOG(mLogger, "\nTURN #" + std::to_string(++mTurnCount), true);
    
    // Update graph
    Map::getInstance()->updateMap(_turnInfo);

    Map::getInstance()->logMap(mTurnCount);
}

void MyBotLogic::Exit()
{
    //Write Code Here
}