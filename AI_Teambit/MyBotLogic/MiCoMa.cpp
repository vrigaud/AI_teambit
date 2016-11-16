#include "MiCoMa.h"

#include "LoggerPath.h"
#include "LevelInfo.h"
#include "Npc.h"

void MiCoMa::init(const LevelInfo& levelInfos)
{
#ifdef BOT_LOGIC_DEBUG_MICOMA
    mLogger.Init(LoggerPath::getInstance()->getPath(), "MiCoMa.log");
#endif

    BOT_LOGIC_MICOMA_LOG(mLogger, "Configure", true);
    BOT_LOGIC_MICOMA_LOG(mLogger, "MiCoMa", true);

    for (auto npc : levelInfos.npcs)
    {
        mNpcs.push_back(new Npc(npc.second.npcID, npc.second.tileID));
    }
}

void MiCoMa::update(const TurnInfo& turnInfo)
{
    for (Npc* npc : mNpcs)
    {
        npc->update();
    }
}