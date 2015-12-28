#ifndef __EZSYSTEM_H_INCLUDE__
#define __EZSYSTEM_H_INCLUDE__


#include <string.h>
#include "EzDebugHelper.h"


inline int EzMemoryCopy(void* pvDest, size_t stDestSize, const void* pvSrc, 
    size_t stCount)
{
    int iRet = 0;

#if _MSC_VER >= 1400
    iRet = memcpy_s(pvDest, stDestSize, pvSrc, stCount);
#else // #if _MSC_VER >= 1400
    if (stDestSize < stCount)
        iRet = -1;
    else
        memcpy(pvDest, pvSrc, stCount);
#endif // #if _MSC_VER >= 1400

    EZASSERT(iRet != -1);
    return iRet;
}

inline int EzMemoryCopy(void* pvDest, const void* pvSrc, size_t stCount)
{
    return EzMemoryCopy(pvDest, stCount, pvSrc, stCount);
}

#endif