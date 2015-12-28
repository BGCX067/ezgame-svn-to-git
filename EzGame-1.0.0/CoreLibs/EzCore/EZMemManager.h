#pragma once

#include "EzAllocator.h"
#include "EzCriticalSection.h"
#include "EzSingleton.h"


class EZCORE_ENTRY EzMemManager : public EzSingleton<EzMemManager>
{
public:
    // Singleton access
    static EzMemManager& get();

    // 3 funnel points for all memory operations:
    void* allocate(
        size_t stSize, 
        size_t stAlignment, 
        EMEM_EVENTTYPE eEventType,
        bool bProvideAccurateSizeOnDeallocate = false,
        const char *pcSourceFile = EZ_MEM_FILE_DEFAULT,
        int iSourceLine = EZ_MEM_LINE_DEFAULT,
        const char* pcFunction = EZ_MEM_FUNCTION_DEFAULT);

    void* reallocate(
        void* pvMem, 
        size_t stSize, 
        size_t stAlignment, 
        EMEM_EVENTTYPE eEventType, 
        bool bProvideAccurateSizeOnDeallocate = false,
        size_t stSizeCurrent = EZ_MEM_DEALLOC_SIZE_DEFAULT,
        const char *pcSourceFile = EZ_MEM_FILE_DEFAULT,
        int iSourceLine = EZ_MEM_LINE_DEFAULT,
        const char* pcFunction = EZ_MEM_FUNCTION_DEFAULT);

    void deallocate(
        void* pvMem, 
        EMEM_EVENTTYPE eEventType,
        size_t stSizeinBytes= EZ_MEM_DEALLOC_SIZE_DEFAULT);

    bool trackAllocate(
        const void* const pvMemory, 
        size_t stSizeInBytes, 
        EMEM_EVENTTYPE eEventType, 
        const char *pcSourceFile = EZ_MEM_FILE_DEFAULT,
        int iSourceLine = EZ_MEM_LINE_DEFAULT,
        const char* pcFunction = EZ_MEM_FUNCTION_DEFAULT);

    bool trackDeallocate(
        const void* const pvMemory, 
        EMEM_EVENTTYPE eEventType);

    static void init();
    static void shutdown();

    static bool verifyAddress(const void* pvMemory);

    // Utility functions
    static const char* EzMemEventTypeToString(EMEM_EVENTTYPE eEventType);

protected:
    EzMemManager()
	: m_pAllocator(NULL)
	{
	}
    EzMemManager(const EzMemManager&) {};

    EzAllocator* m_pAllocator;
};

inline EzMemManager& EzMemManager::get()
{
    return *(ms_pSingleton);
}
//---------------------------------------------------------------------------
inline void* EzMemManager::allocate(
    size_t stSizeInBytes, 
    size_t stAlignment, 
    EMEM_EVENTTYPE eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    const char *pcSourceFile,
    int iSourceLine, 
    const char* pcFunction)
{
    EZASSERT(m_pAllocator);

    void* pvMem = m_pAllocator->allocate(stSizeInBytes, stAlignment, 
        eEventType, bProvideAccurateSizeOnDeallocate, 
        pcSourceFile, iSourceLine, pcFunction);
    
    return pvMem;
}
//---------------------------------------------------------------------------
inline void* EzMemManager::reallocate(
    void* pvMem, 
    size_t stSizeInBytes, 
    size_t stAlignment, 
    EMEM_EVENTTYPE eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    size_t stSizeCurrent,
    const char *pcSourceFile,
    int iSourceLine, 
    const char* pcFunction)
{
    EZASSERT(m_pAllocator);

    void* pvNewMem = m_pAllocator->reallocate(pvMem, stSizeInBytes,
        stAlignment, eEventType, bProvideAccurateSizeOnDeallocate,
        stSizeCurrent, pcSourceFile, iSourceLine, 
        pcFunction);
    return pvNewMem;
}
//---------------------------------------------------------------------------
inline  void EzMemManager::deallocate(void* pvMem,
    EMEM_EVENTTYPE eEventType, size_t stSizeinBytes)
{
    EZASSERT(m_pAllocator);

    m_pAllocator->deallocate(pvMem, eEventType, stSizeinBytes);
 
}
//---------------------------------------------------------------------------
inline bool EzMemManager::trackAllocate(const void* const pvMem,  
    size_t stSizeInBytes, EMEM_EVENTTYPE eEventType, 
    const char *pcSourceFile, int iSourceLine, const char* pcFunction)
{
    EZASSERT(m_pAllocator);
    return m_pAllocator->trackAllocate(pvMem, stSizeInBytes, eEventType,
        pcSourceFile, iSourceLine, pcFunction);
}
//---------------------------------------------------------------------------
inline  bool EzMemManager::trackDeallocate(
    const void* const pvMem, EMEM_EVENTTYPE eEventType)
{
    EZASSERT(m_pAllocator);
    return m_pAllocator->trackDeallocate(pvMem, eEventType);
}
//---------------------------------------------------------------------------
inline const char* EzMemManager::EzMemEventTypeToString(
    EMEM_EVENTTYPE eEventType)
{
    switch (eEventType)
    {
    case EMEM_UNKNOWN:
        return "EMEM_UNKNOWN";
    case EMEM_OPER_NEW:
        return "EMEM_OPER_NEW";
    case EMEM_OPER_NEW_ARRAY:
        return "EMEM_OPER_NEW_ARRAY";
    case EMEM_OPER_DELETE:
        return "EMEM_OPER_DELETE";
    case EMEM_OPER_DELETE_ARRAY:
        return "EMEM_OPER_DELETE_ARRAY";
    case EMEM_MALLOC:
        return "EMEM_MALLOC";
    case EMEM_REALLOC:
        return "EMEM_REALLOC";
    case EMEM_ALIGNEDMALLOC:
        return "EMEM_ALIGNEDMALLOC";
    case EMEM_ALIGNEDREALLOC:
        return "EMEM_ALIGNEDREALLOC";
    case EMEM_FREE:
        return "EMEM_FREE";
    case EMEM_ALIGNEDFREE:
        return "EMEM_ALIGNEDFREE";
    case EMEM_EXTERNALALLOC:
        return "EMEM_EXTERNALALLOC";
    case EMEM_EXTERNALFREE:
        return "EMEM_EXTERNALFREE";
    default:
        EZASSERT(!"Unknown EMEM_EVENTTYPE  value!");
    };

    return NULL;
}

