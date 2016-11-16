#include "Npc.h"
#include "LoggerPath.h"
Npc::Npc(unsigned int a_id, unsigned int a_tileId)
    : mCurrentState{}, mNextState{}, mId{ a_id }, mTurnCount{ 0 }, mPath{}
{
#ifdef BOT_LOGIC_DEBUG_NPC
    mLogger.Init(LoggerPath::getInstance()->getPath(), "Npc_" + std::to_string(mId) + ".log");
#endif

    BOT_LOGIC_NPC_LOG(mLogger, "Configure", true);

};

void Npc::update()
{
    ++mTurnCount;
    BOT_LOGIC_NPC_LOG(mLogger, "\nTurn #" + std::to_string(mTurnCount), true);
    BOT_LOGIC_NPC_LOG(mLogger, "\tEntering State Machine : ...", true);


    do 
    {


    } while (mCurrentState != mNextState);
}
