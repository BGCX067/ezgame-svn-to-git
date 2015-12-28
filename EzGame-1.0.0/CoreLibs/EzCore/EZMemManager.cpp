#include "EZMemManager.h"
#include "EzStandardAllocator.h"
#include "EzMemTracker.h"


template<> EzMemManager* EzSingleton<EzMemManager>::ms_pSingleton = 0;

void EzMemManager::init()
{
    ms_pSingleton = EzExternalNew EzMemManager();

	EzAllocator* pAllocator = EzExternalNew EzStandardAllocator();

#ifdef	_MEMORY_DEBUGGER
	ms_pSingleton->m_pAllocator = EzExternalNew EzMemTracker(pAllocator);
#else
    ms_pSingleton->m_pAllocator = pAllocator;
#endif

    ms_pSingleton->m_pAllocator->initialize();
}

void EzMemManager::shutdown()
{
    EZASSERT(ms_pSingleton->m_pAllocator);
    ms_pSingleton->m_pAllocator->shutdown(); 
    EzExternalDelete ms_pSingleton;
    ms_pSingleton = NULL;
}

bool EzMemManager::verifyAddress(const void* pvMemory)
{
    EZASSERT(ms_pSingleton->m_pAllocator);
    return ms_pSingleton->m_pAllocator->verifyAddress(pvMemory);
}

