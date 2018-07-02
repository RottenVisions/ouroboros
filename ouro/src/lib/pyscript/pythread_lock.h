// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_SCRIPT_LOCK_H
#define OUROBOROS_SCRIPT_LOCK_H

#include "Python.h"

namespace Ouroboros{ namespace script{


class PyThreadStateLock
{
public:
    PyThreadStateLock(void)
    {
#ifndef OURO_SINGLE_THREADED
        state = PyGILState_Ensure();
#endif
    }

    ~PyThreadStateLock(void)
    {
#ifndef OURO_SINGLE_THREADED
         PyGILState_Release( state );
 #endif
    }
private:
#ifndef OURO_SINGLE_THREADED
    PyGILState_STATE state;
#endif
};

}
}

#endif // OUROBOROS_SCRIPT_LOCK_H
