#ifndef BLOCSELECT_H
#define BLOCSELECT_H

#include "BlocComposite.h"

namespace BehaviourTree
{
    class BlocSelect : public BlocComposite
    {
    public:

        virtual general::result operator()()
        {

            for (int i = 0; i < nbChild(); ++i)
            {
                if (((*children[i])()) == general::result::SUCCESS)
                {
                    return general::result::SUCCESS;
                }
            }

            return general::result::FAIL;
        }

    };
}


#endif //!BLOCSELECT_H