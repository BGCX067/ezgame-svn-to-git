#include "EzMemoryDefines.h"
#include "EzMemManager.h"

#ifdef _MEMORY_DEBUGGER
void* _EzMalloc(size_t stSizeInBytes, const char* pcSourceFile,
    int iSourceLine, const char* pcFunction)
#else
void* _EzMalloc(size_t stSizeInBytes)
#endif
{
    EZASSERT(EzMemManager::isInitialized());

    if (stSizeInBytes == 0)
        stSizeInBytes = 1;

    void* pvMem = EzMemManager::get().allocate(stSizeInBytes, EZ_MEM_ALIGNMENT_DEFAULT, EMEM_MALLOC, false
#ifdef _MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
		);
    
    EZASSERT(pvMem != NULL);

    return pvMem;
}

#ifdef _MEMORY_DEBUGGER
void* _EzAlignedMalloc(
    size_t stSizeInBytes, 
    size_t stAlignment, 
    const char* pcSourceFile, 
    int iSourceLine, 
    const char* pcFunction)
#else
void* _EzAlignedMalloc(size_t stSizeInBytes, size_t stAlignment)
#endif 
{
    EZASSERT(EzMemManager::isInitialized());

	if (stSizeInBytes == 0)
        stSizeInBytes = 1;

    return EzMemManager::get().allocate(stSizeInBytes, stAlignment, EMEM_ALIGNEDMALLOC, false
#ifdef _MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
		);
}

#ifdef _MEMORY_DEBUGGER
void* _EzRealloc(
    void *pvMem, 
    size_t stSizeInBytes, 
    const char* pcSourceFile,
    int iSourceLine, 
    const char* pcFunction)
#else
void* _EzRealloc(void *pvMem, size_t stSizeInBytes)
#endif
{
    EZASSERT(EzMemManager::isInitialized());

    if (stSizeInBytes == 0 && pvMem != 0)
    {
        EzFree(pvMem);
        return NULL;
    }
    else if (pvMem == 0)
    {
        return _EzMalloc(stSizeInBytes
#ifdef _MEMORY_DEBUGGER
            , pcSourceFile, iSourceLine, pcFunction
#endif
			);
    }

    pvMem = EzMemManager::get().reallocate(pvMem, stSizeInBytes,  EZ_MEM_ALIGNMENT_DEFAULT, EMEM_REALLOC, false, 
        EZ_MEM_DEALLOC_SIZE_DEFAULT
#ifdef _MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
		);

    return pvMem; 
}

#ifdef _MEMORY_DEBUGGER
void* _EzAlignedRealloc(
    void *pvMem, 
    size_t stSizeInBytes, 
    size_t stAlignment, 
    const char* pcSourceFile, 
    int iSourceLine, 
    const char* pcFunction)
#else
void* _EzAlignedRealloc(void *pvMem, size_t stSizeInBytes, size_t stAlignment)
#endif
{
    EZASSERT(EzMemManager::isInitialized());

    if (stSizeInBytes == 0 && pvMem != 0)
    {
        EzAlignedFree(pvMem);
        return NULL;
    }
    else if (pvMem == 0)
    {
        return _EzAlignedMalloc(stSizeInBytes, stAlignment
#ifdef _MEMORY_DEBUGGER
            , pcSourceFile, iSourceLine, pcFunction
#endif
			);
    }

    return EzMemManager::get().reallocate(pvMem, stSizeInBytes, 
        stAlignment, EMEM_ALIGNEDREALLOC, false, EZ_MEM_DEALLOC_SIZE_DEFAULT
#ifdef _MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
		);

}

void _EzFree(void* pvMem)
{
    if (pvMem == 0)
        return;

    EzMemManager::get().deallocate(pvMem, EMEM_FREE);
}

void _EzAlignedFree(void* pvMem)
{
    if (pvMem == 0)
        return;

	EzMemManager::get().deallocate(pvMem, EMEM_ALIGNEDFREE);
}



#ifdef _MEMORY_DEBUGGER
bool _EzTrackAlloc(const void* const pvMemory, size_t stSizeInBytes, 
        const char* pcSourceFile, int iSourceLine, const char* pcFunction)
{
    if (pvMemory == NULL)
        return true;

    EZASSERT(EzMemManager::isInitialized());
    return EzMemManager::get().trackAllocate(pvMemory, stSizeInBytes, 
        EMEM_EXTERNALALLOC, pcSourceFile, iSourceLine, pcFunction);
}
bool _EzTrackFree(const void* const pvMemory)
{
    if (pvMemory == NULL)
        return true;

    EZASSERT(EzMemManager::isInitialized());
    return EzMemManager::get().trackDeallocate(pvMemory, EMEM_EXTERNALFREE);
}
#endif