#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "Map.h"
#include "Npc.h"

#ifdef _DEBUG
   #define BOT_LOGIC_DEBUG
#endif

#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#ifdef BOT_LOGIC_DEBUG
#else
   //#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif

#include <vector>

//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{
    // debug stuff
    unsigned mTurnCount;
    unsigned long long totalTime{};
    std::string mLogPath;

    // Temporary stuff
    std::vector<Npc*> mNpcs; // gonna be in MiCoMa
public:
	MyBotLogic();
	virtual ~MyBotLogic();

	virtual void Configure(int argc, char *argv[], const std::string& _logpath);
	virtual void Load();
    virtual void Start();
	virtual void OnBotInitialized();
	virtual void Init(LevelInfo& _levelInfo);
	virtual void FillActionList(TurnInfo& _turnInfo, std::vector<Action*>& _actionList);  
	virtual void Exit();

protected:
	Logger mLogger;
};

