#ifndef OBJECTIVE_H
#define OBJECTIVE_H

struct Objective
{
    enum ObjectiveType {
        GO_TO,
        SEARCH_MAP,
        SEARCH_H_DOOR,
        NONE
    };

    Objective(ObjectiveType aType = NONE, int tileId = -1)
        : mTileId{ tileId }, mType{ aType }, mIsAchieved{false}
    {}

    ObjectiveType mType;
    int mTileId;
    bool mIsAchieved;
};

#endif // OBJECTIVE_H