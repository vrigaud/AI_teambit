#ifndef NPCTARGET_H
#define NPCTARGET_H

#include "LevelInfo.h"

#include <vector>
#include <algorithm>

class ExploreNode;

namespace ExplorationPart
{
    struct Target
    {
        bool isTargeted;
        ExploreNode* targetNode;

        Target(ExploreNode* targetNode) :
            isTargeted{ false },
            targetNode{ targetNode }
        {}

        bool operator==(const Target& other)const noexcept
        {
            return targetNode == other.targetNode;
        }

        bool operator!=(const Target& other)const noexcept
        {
            return !(*this == other);
        }
    };


    class NPCTarget
    {
    private:
        unsigned int npc;
        Target* target;

    public:
        NPCTarget(unsigned int npc) :
            npc{ npc },
            target{ nullptr }
        {}

        NPCTarget(unsigned int npc, Target* targetNode) :
            npc{ npc },
            target{ targetNode }
        {
            target->isTargeted = true;
        }

    public:
        unsigned int getNpcId() const noexcept
        {
            return npc;
        }

        Target* getTarget() const noexcept
        {
            return target;
        }

        bool hasTarget() const noexcept
        {
            return target != nullptr;
        }

    public:
        void changeNpc(unsigned int newNpcId) noexcept
        {
            npc = newNpcId;
        }

        void changeTarget(Target* newTarget) noexcept
        {
            if (hasTarget())
            {
                target->isTargeted = false;
            }
            
            target = newTarget;

            if (hasTarget())
            {
                target->isTargeted = true;
            }
        }

        void swapTarget(NPCTarget& other)
        {
            std::swap(target, other.target);
        }

        bool operator==(unsigned int otherNpc) const noexcept
        {
            return npc == otherNpc;
        }

        bool operator!=(unsigned int otherNpc) const noexcept
        {
            return npc != otherNpc;
        }
    };


    class TargetArray
    {
    private:
        std::vector<Target> mTargets;
        std::vector<NPCTarget*> mNPCTarget;

    public:
        TargetArray(const LevelInfo& levelInfo)
        {
            std::for_each(
                levelInfo.npcs.begin(),
                levelInfo.npcs.end(),
                [this](std::pair<unsigned int, NPCInfo> pair){
                    mNPCTarget.push_back(new NPCTarget{ pair.second.npcID });
                }
            );

            mTargets.reserve(levelInfo.rowCount * levelInfo.colCount);
        }

        ~TargetArray()
        {
            for (; mNPCTarget.size();)
            {
                delete mNPCTarget[0];
                std::swap(mNPCTarget[0], mNPCTarget[mNPCTarget.size() - 1]);
                mNPCTarget.pop_back();
            }
        }


    public:
        std::vector<Target>& getAllTarget() noexcept
        {
            return mTargets;
        }

        std::vector<NPCTarget*>& getTargetMapped() noexcept
        {
            return mNPCTarget;
        }

        bool changeTarget(unsigned int npc, Target* target)
        {
            if (target)
            {
                auto iterTarget = std::find(mTargets.begin(), mTargets.end(), *target);

                if (iterTarget == mTargets.end())
                {
                    return false;
                }

                auto iterNpc = mNPCTarget.begin();

                for (; iterNpc != mNPCTarget.end(); ++iterNpc)
                {
                    if ((*iterNpc)->getNpcId() == npc)
                    {
                        break;
                    }
                }

                if (iterNpc == mNPCTarget.end())
                {

                    return false;
                }
                else
                {
                    (*iterNpc)->changeTarget(target);
                }
            }
            else
            {
                auto iterNpc = mNPCTarget.begin();

                for (; iterNpc != mNPCTarget.end(); ++iterNpc)
                {
                    if ((*iterNpc)->getNpcId() == npc)
                    {
                        break;
                    }
                }

                if (iterNpc == mNPCTarget.end())
                {

                    return false;
                }
                else
                {
                    (*iterNpc)->changeTarget(target);
                }
            }

            return true;
        }

        void addTarget(Target& target)
        {
            if (std::find(mTargets.begin(), mTargets.end(), target) == mTargets.end())
            {
                mTargets.emplace_back(target);
            }
        }

        NPCTarget* find(unsigned int npcId)
        {
            auto iter = mNPCTarget.begin();

            for (; iter != mNPCTarget.end(); ++iter)
            {
                if ((*iter)->getNpcId() == npcId)
                {
                    break;
                }
            }

            if (iter == mNPCTarget.end())
            {
                return nullptr;
            }

            return *iter;
        }
    };
}

#endif //!NPCTARGET_H