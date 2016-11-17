#ifndef MICOMA_H
#define MICOMA_H

#include "Singleton.h"
#include "Objective.h"

#include "Logger.h"
#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_MICOMA
#endif

#ifdef BOT_LOGIC_DEBUG_MICOMA
#define BOT_LOGIC_MICOMA_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_MICOMA_LOG(logger, text, autoEndLine) 0
#endif

#include <vector>
#include <algorithm>



class Npc;
struct LevelInfo;
struct TurnInfo;
class MiCoMa : Singleton
{
    std::vector<Npc*> mNpcs;
    Logger mLogger;

public :
    static MiCoMa* getInstance()
    {
        static MiCoMa instance;
        return &instance;
    }
    void init(const LevelInfo&);

    void update(const TurnInfo&);
};

#endif //MICOMA_H
