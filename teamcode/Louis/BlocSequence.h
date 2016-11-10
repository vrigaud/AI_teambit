#ifndef BLOCSEQUENCE_H
#define BLOCSEQUENCE_H

#include "BlocComposite.h"

namespace BehaviourTree
{
    class BlocSequence : public BlocComposite
    {
    public:

        virtual general::result operator()()
        {
            
            for (int i = 0; i < nbChild() ; ++i)
            {
                if (((*children[i])()) == general::result::FAIL)
                {
                    return general::result::FAIL;
                }
            }
            return general::result::SUCCESS;
        }

    };
}


#endif //!BLOCSEQUENCE_H
