#ifndef NPC_H
#define NPC_H

#include "Logger.h"
#ifdef _DEBUG
#define BOT_LOGIC_DEBUG_NPC
#endif

#ifdef BOT_LOGIC_DEBUG_NPC
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_NPC_LOG(logger, text, autoEndLine) 0
#endif

#include <string>
#include <vector>
class Npc
{
    //State machine states
    enum State
    {
        TurnStart,			//State machine loop entry point
        IHaveTargets,		//The npc currently knows of targets on the map
        IHaveNoTargets,		//The npcs does not know about any target
        NeedAnotherTarget,	//A npc has the same target as another
        IHaveSpecificTarget,//The npc will absolutely move towards a target
        SpecificMove,		//The npc will fill the action list with a move towards its target
        RandomMove,			//The npc will fill the action list with a semi-random move
        ReachedTarget,		//The npc reached its target
        TurnEnd				//State machine loop exit point
    };

    State mCurrentState;
    State mNextState;

    unsigned int mId;
    unsigned int mGoal;
    unsigned int mTarget;

    std::vector<unsigned int>mPath;
    
    // Debugger
    unsigned int mTurnCount;
    Logger mLogger;

public :
    Npc(unsigned int a_id, unsigned int a_tileId);
    void update();
};

#endif // NPC_H
