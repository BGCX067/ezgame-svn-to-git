#pragma once

#include "EzCoreLibType.h"
#include "EzDebugHelper.h"
#include "EzLogger.h"
#include <stdio.h>
#include <float.h>



#define MAKE_ID(c0, c1, c2, c3) \
	((unsigned int)(unsigned char)(c0) | ((unsigned int)(unsigned char)(c1) << 8) | \
	((unsigned int)(unsigned char)(c2) << 16) | ((unsigned int)(unsigned char)(c3) << 24 ))


EZCORE_ENTRY int EzSprintf(char* pcDest, size_t stDestSize, const char* pcFormat, ...);
EZCORE_ENTRY int EzVsprintf(char* pcDest, size_t stDestSize, const char* pcFormat,va_list Args);
EZCORE_ENTRY int EzSnprintf(char* pcDest, size_t stDestSize, size_t stCount, const char* pcFormat, ...);
EZCORE_ENTRY int EzVsnprintf(char* pcDest, size_t stDestSize, size_t stCount, const char* pcFormat, va_list Args);


EZCORE_ENTRY char* EzStringCpy(char* pcDest, size_t stDestSize, const char* pcSrc);
EZCORE_ENTRY char* EzStringNCpy(char* pcDest, size_t stDestSize, const char* pcSrc, size_t stCount);
EZCORE_ENTRY char* EzStringCat(char* pcDest, size_t stDestSize, const char* pcSrc);







inline int EzSprintf(char* pcDest, size_t stDestSize, const char* pcFormat, ...)
{
    EZASSERT(pcFormat);

    va_list	Args;
    va_start(Args, pcFormat);
    int iRet = EzVsprintf(pcDest, stDestSize, pcFormat, Args);
    va_end(Args);

    return iRet;
}

inline int EzVsprintf(char* pcDest, size_t stDestSize, const char* pcFormat,va_list Args)
{
    return EzVsnprintf(pcDest, stDestSize, _TRUNCATE, pcFormat, Args);
}

inline int EzSnprintf(char* pcDest, size_t stDestSize, size_t stCount,
    const char* pcFormat, ...)
{
    EZASSERT(pcFormat);

    va_list Args;
    va_start(Args, pcFormat);
    int iRet = EzVsnprintf(pcDest, stDestSize, stCount, pcFormat, Args);
    va_end(Args);

    return iRet;
}

inline int EzVsnprintf(char* pcDest, size_t stDestSize, size_t stCount, const char* pcFormat, va_list Args)
{
    if (stDestSize == 0)
    {
        return 0;
    }

    EZASSERT(pcDest);
    EZASSERT(stCount < stDestSize || stCount == _TRUNCATE);
   	EZASSERT(pcFormat);

    // Ensure that input buffer is cleared out.
    pcDest[0] = '\0';

    bool bTruncate = (stCount == _TRUNCATE);

#if _MSC_VER >= 1400
    int iRet = vsnprintf_s(pcDest, stDestSize, stCount, pcFormat, Args);
#else   // _MSC_VER >= 1400
    if (bTruncate)
    {
        stCount = stDestSize - 1;
    }
    int iRet = _vsnprintf(pcDest, stCount, pcFormat, kArgs);
#endif  // _MSC_VER >= 1400

    if (iRet == -1 && !bTruncate)
    {
        iRet = stCount;
    }

#if _MSC_VER < 1400
    // Ensure that the string ends in a null character.
    if (iRet == -1)
    {
        pcDest[stDestSize - 1] = '\0';
    }
    else
    {
        pcDest[iRet] = '\0';
    }
#endif  // #if _MSC_VER < 1400

    return iRet;
}




inline char* EzStringCpy(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcpy_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    NIASSERT(stDestSize != 0);

    size_t stSrcLen = strlen(pcSrc);
    NIASSERT(stDestSize > stSrcLen); // > because need null character

    size_t stWrite;
    
    if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
        stWrite = stDestSize;
    else 
        stWrite = stSrcLen + 1;

    char *pcResult = strncpy(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* EzStringNCpy(char* pcDest, size_t stDestSize, const char* pcSrc, 
    size_t stCount)
{
#if _MSC_VER >= 1400
    strncpy_s(pcDest, stDestSize, pcSrc, stCount);
    return pcDest;
#else // #if _MSC_VER >= 1400
    
    NIASSERT(pcDest != 0 && stDestSize != 0);
    NIASSERT(stCount < stDestSize || stCount == NI_TRUNCATE);

    if (stCount >= stDestSize)
    {
        if (stCount != NI_TRUNCATE)
        {
            pcDest[0] = '\0';
            return pcDest;
        }
        else
        {
            stCount = stDestSize - 1;
        }
    }

    char* pcResult = strncpy(pcDest, pcSrc, stCount);
    pcResult[stCount] = '\0';

    return pcResult;
#endif // #if _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
inline char* EzStringCat(char* pcDest, size_t stDestSize, const char* pcSrc)
{
#if _MSC_VER >= 1400
    strcat_s(pcDest, stDestSize, pcSrc);
    return pcDest;
#else // #if _MSC_VER >= 1400

    size_t stSrcLen = strlen(pcSrc);
    size_t stDestLen = strlen(pcDest);
    NIASSERT(stSrcLen + stDestLen <= stDestSize - 1);

    size_t stWrite = stDestSize - 1 - stDestLen;  

    char* pcResult = strncat(pcDest, pcSrc, stWrite);
    pcResult[stDestSize - 1] = '\0';
    return pcResult;

#endif // #if _MSC_VER >= 1400
}