#ifndef NPC_H
#define NPC_H

#include "Logger.h"
#include "Objective.h"
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
    //Hierarchical state machine states
    enum State
    {
        EXPLORING = 0b00000, 
            EXPLORE_MAP,
            EXPLORE_H_DOOR,
            EXPLORE_DNPC,
            EXPLORE_WAITING,
            MOVE,
        WAITING = 0b01000,
        MOVING = 0b10000,
            SEARCH_PATH,
            FOLLOW_PATH,
            MOVING_DNPC,
            MOVING_WAITING,
            ARRIVED
    };
    enum StateMask
    {
        STATE_LEVEL_0 = 0b11000,
        STATE_LEVEL_1 = 0b00111
    };

    Objective mObjective;

    State mCurrentState;
    State mNextState;

    unsigned int mId;
    unsigned int mGoal;
    unsigned int mTarget;

    std::vector<unsigned int>mPath;
    
    // Debugger
    unsigned int mTurnCount;
    Logger mLogger;

public:
    Npc(unsigned int a_id, unsigned int a_tileId);
    void update();
    void setObjective(Objective::ObjectiveType aType = Objective::NONE, int tileId = -1)
    {
        mObjective = Objective{aType, tileId};
    }

private:
    void updateState();
    void enterStateMachine();
    // Level 0 function for FSM
    void exploring();
    void moving();
    void waiting();

    // Level 1 function for EXPLORING state cluster
    void exploreMap();
    void exploreHiddenDoor();
    inline void exploreWaiting();// Delete ?
    void exploreDNpc();
    void move();

    // Level 1 function for MOVING state cluster
    void searchPath();
    void followPath();
    void movingDNpc();
    inline void movingWaiting(); // Delete ?
    inline void arrived();
};

#endif // NPC_H
