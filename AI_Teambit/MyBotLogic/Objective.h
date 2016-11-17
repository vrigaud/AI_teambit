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
        : mTileId{ tileId }, mType{ aType }
    {}

    ObjectiveType mType;
    int mTileId;
};

#endif // OBJECTIVE_H