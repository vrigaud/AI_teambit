#ifndef OBJECTIVE_H
#define OBJECTIVE_H

struct Objective
{
    enum ObjectiveType {
        GO_TO,
        GO_TO_BUTTON_DOOR,
        GO_TO_EXPLORE,
        SEARCH_MAP,
        SEARCH_H_DOOR,
        NONE
    };

    Objective(ObjectiveType aType = NONE, int tileId = -1)
        : mTileId{ tileId }, mType{ aType }, mIsAchieved{ false }, mDoorId{}
    {}

    ObjectiveType mType;
    int mTileId;
    bool mIsAchieved;
    unsigned int mDoorId;
};

#endif // OBJECTIVE_H