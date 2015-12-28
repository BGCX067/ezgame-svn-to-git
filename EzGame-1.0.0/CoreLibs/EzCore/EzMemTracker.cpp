#include "EzMemManager.h"
#include "EzMemTracker.h"    
#include "EzOSOperator.h"
#include "EzLogger.h"
#include "EzPath.h"
#include "EzCore.h"


size_t EzMemTracker::ms_stBreakOnAllocID = INT_MAX;

EzMemTracker::EzMemTracker(EzAllocator* pActualAllocator,
        bool bWriteToLog,
        unsigned int uiInitialSize, 
        unsigned int uiGrowBy, bool bAlwaysValidateAll,
        bool bCheckArrayOverruns)
: m_pActualAllocator(pActualAllocator), m_stCurrentAllocID(0), m_stActiveAllocationCount(0), m_stActiveMemory(0), m_ucFillChar(0xFF)
{
    memset(m_pActiveMem, 0, ms_uiHashSize*sizeof(EzAllocUnit*));    
    if (uiInitialSize > 0)
    {
        // create initial set of tracking data structures
        m_stReservoirBufferSize = 1;
        m_pReservoir = (EzAllocUnit*)calloc(uiInitialSize, sizeof(EzAllocUnit));

        EZASSERT(m_pReservoir != NULL);
        // throw an exception in this situation
        if (m_pReservoir == NULL) 
            throw "Unable to allocate RAM for internal memory tracking data";

        // initialize the allocation units
        for (unsigned int i = 0; i < uiInitialSize-1; i++)
        {
            m_pReservoir[i].m_pNext = &m_pReservoir[i+1];
        }
        m_pReservoir[uiInitialSize-1].m_pNext = NULL;

        m_ppReservoirBuffer = (EzAllocUnit**) EzExternalMalloc(sizeof(EzAllocUnit*));

        // If you hit this NIMEMASSERT, then the memory manager failed to 
        // allocate internal memory for tracking the allocations
        EZASSERT(m_ppReservoirBuffer != NULL);
        // throw an exception in this situation
        if (m_ppReservoirBuffer == NULL) 
            EZASSERT(!"Unable to allocate RAM for internal memory tracking data");

        m_ppReservoirBuffer[0] = m_pReservoir;

        unsigned int uiInitialBytes = uiInitialSize*sizeof(EzAllocUnit) + sizeof(EzAllocUnit*);
        m_stActiveTrackerOverhead = uiInitialBytes;
        m_stPeakTrackerOverhead = uiInitialBytes;
        m_stAccumulatedTrackerOverhead = uiInitialBytes;
    }
    else
    {
        m_stReservoirBufferSize = 0;
    }

}


EzMemTracker::~EzMemTracker()
{
    // Free all memory used by the hash table of alloc units
    if (m_ppReservoirBuffer)
    {
        for (unsigned int i = 0; i < m_stReservoirBufferSize; i++)
        {
            EzExternalFree(m_ppReservoirBuffer[i]);
        }

        EzExternalFree(m_ppReservoirBuffer);
    }

    EzExternalDelete m_pActualAllocator;

}

void EzMemTracker::initialize()
{

    char acLogPath[MAX_PATH];
    EzPath::getExecutableDirectory(acLogPath, MAX_PATH);

    char acOverviewPath[MAX_PATH];
    char acLeakPath[MAX_PATH];

    EzSprintf(acOverviewPath, MAX_PATH, "%sMemory-Overview.xml", acLogPath);
    EzSprintf(acLeakPath, MAX_PATH, "%sMemory-Leaks.xml", acLogPath);

	EzLogger::openLog(EZMESSAGE_MEMORY_COMPLETE, acOverviewPath, EzLogger::ELOM_OPEN_OVERWRITE);
	EzLogger::openLog(EZMESSAGE_MEMORY_LEAK, acLeakPath, EzLogger::ELOM_OPEN_OVERWRITE);

	EZLOG(EZMESSAGE_MEMORY_COMPLETE, "<?xml version=\"1.0\"?>\n");


	

	m_pActualAllocator->initialize();

}

void EzMemTracker::shutdown()
{
    EZLOG(EZMESSAGE_MEMORY_COMPLETE, "\">\n");

	logActiveMemoryReport();

	EzLogger::closeLog(EZMESSAGE_MEMORY_COMPLETE);
	EzLogger::closeLog(EZMESSAGE_MEMORY_LEAK);

	m_pActualAllocator->shutdown();
}

void EzMemTracker::freeAllocUnit(EzAllocUnit* pUnit)
{
    pUnit->m_pNext = m_pReservoir;
    m_pReservoir = pUnit;
}


EzMemTracker::EzAllocUnit* EzMemTracker::getAllocUnit()
{
    if (!m_pReservoir)
        reAllocReservoir();

    // Get an allocation unit from the reservoir
    EzAllocUnit* pUnit = m_pReservoir;
    m_pReservoir = pUnit->m_pNext;

	return pUnit;
}

void* EzMemTracker::allocate(size_t& stSizeInBytes, size_t& stAlignment,EMEM_EVENTTYPE eEventType, 
	bool bProvideAccurateSizeOnDeallocate,
	const char* pcFile, 
	int iLine,
	const char* pcFunction)
{

	m_CriticalSection.lock();

    size_t stSizeOriginal = stSizeInBytes;
    float fTime = getTimeSec();  

	EZASSERT(ms_stBreakOnAllocID != m_stCurrentAllocID);

	void* pvMem = m_pActualAllocator->allocate(stSizeInBytes, stAlignment, eEventType, bProvideAccurateSizeOnDeallocate,
		pcFile, iLine, pcFunction);

	if (pvMem == NULL)
    {
        m_CriticalSection.unlock();
        return NULL;
    }

	setMemoryFillWithPattern(pvMem, stSizeOriginal);


	EZASSERT(findAllocUnit(pvMem) == NULL);

	EzAllocUnit* pUnit = getAllocUnit();

    // fill in the known information
    pUnit->reset();
    pUnit->m_stAllocationID = m_stCurrentAllocID;
    pUnit->m_stAlignment = stAlignment;
    pUnit->m_ulAllocThreadId = getCurrentThreadId();
    pUnit->m_eAllocType = eEventType;
    pUnit->m_fAllocTime = fTime;
    pUnit->m_pvMem = pvMem;
    pUnit->m_stSizeRequested = stSizeOriginal;
    pUnit->m_stSizeAllocated = stSizeInBytes;
	pUnit->setSourceFileInfo(pcFile, iLine, pcFunction);
   


	// Insert the new allocation into the hash table
    insertAllocUnit(pUnit);

	m_stCurrentAllocID++;
	m_stActiveAllocationCount++;
	m_stActiveMemory += pUnit->m_stSizeAllocated;

	m_CriticalSection.unlock();

	return pvMem;


}

void EzMemTracker::deallocate(void* pvMemory, EMEM_EVENTTYPE eEventType, size_t stSizeInBytes)
{
    if (pvMemory)
    {
	    m_CriticalSection.lock();
    
        float fTime = getTimeSec();
		unsigned long ulFreeThreadId = getCurrentThreadId();
		size_t stUnusedSize;

        // Search the tracking unit hash table to find the address
        EzAllocUnit* pUnit = findAllocUnit(pvMemory);

        if (pUnit == NULL)
        {
            EZASSERT(pUnit != NULL);
            m_CriticalSection.unlock();
            return;
        }

		stUnusedSize = getMemoryBytesWithPattern(pvMemory, pUnit->m_stSizeRequested);

		m_pActualAllocator->deallocate(pvMemory, eEventType, stSizeInBytes);

		logAllocUnit(pUnit, EZMESSAGE_MEMORY_COMPLETE, "\t", 
			eEventType, fTime, ulFreeThreadId, stUnusedSize);


	    removeAllocUnit(pUnit);

		freeAllocUnit(pUnit);

        // update summary statistics
		m_stActiveAllocationCount--;
    	m_stActiveMemory -= pUnit->m_stSizeAllocated;



		m_CriticalSection.unlock();
	}
}

void* EzMemTracker::reallocate(void* pvMemory, size_t& stSizeInBytes,size_t& stAlignment, 
	EMEM_EVENTTYPE eEventType, 
	bool bProvideAccurateSizeOnDeallocate,
	size_t stSizeCurrent,
	const char* pcFile, int iLine,
	const char* pcFunction)
{
	size_t stSizeOriginal = stSizeInBytes;

    if (pvMemory == NULL)
    {
        return allocate(stSizeInBytes, stAlignment, eEventType,
            bProvideAccurateSizeOnDeallocate,
            pcFile, iLine, pcFunction);
    }

    if (stSizeInBytes== 0)
    {
        deallocate(pvMemory, eEventType, EZ_MEM_DEALLOC_SIZE_DEFAULT);
        return NULL;
    }

    m_CriticalSection.lock();
    
    float fTime = getTimeSec(); 
	unsigned long ulFreeThreadId = getCurrentThreadId();
	size_t stUnusedSize;
	EZASSERT(ms_stBreakOnAllocID != m_stCurrentAllocID);


    EzAllocUnit* pUnit = findAllocUnit(pvMemory);

	EZASSERT(pUnit != NULL);

	stUnusedSize = getMemoryBytesWithPattern(pvMemory, pUnit->m_stSizeRequested);

	void* pvNewMemory = m_pActualAllocator->reallocate(pvMemory, stSizeInBytes, stAlignment, eEventType, 
		bProvideAccurateSizeOnDeallocate, stSizeCurrent, pcFile, iLine, pcFunction);

	logAllocUnit(pUnit, EZMESSAGE_MEMORY_COMPLETE, "\t", 
			eEventType, fTime, ulFreeThreadId, stUnusedSize);

	int iDeltaSize = stSizeInBytes - pUnit->m_stSizeAllocated;
	
	m_stActiveMemory += iDeltaSize;

	removeAllocUnit(pUnit);
	freeAllocUnit(pUnit);

	EzAllocUnit* pUnitNew = getAllocUnit();

    // fill in the known information
    pUnitNew->reset();
    pUnitNew->m_stAllocationID = m_stCurrentAllocID;
    pUnitNew->m_stAlignment = stAlignment;
    pUnitNew->m_ulAllocThreadId = getCurrentThreadId();
    pUnitNew->m_eAllocType = eEventType;
    pUnitNew->m_fAllocTime = fTime;
    pUnitNew->m_pvMem = pvNewMemory;
    pUnitNew->m_stSizeRequested = stSizeOriginal;
    pUnitNew->m_stSizeAllocated = stSizeInBytes;
	pUnitNew->setSourceFileInfo(pcFile, iLine, pcFunction);

	// Insert the new allocation into the hash table
    insertAllocUnit(pUnitNew);

	m_stCurrentAllocID++;
	
	m_CriticalSection.unlock();

	return pvNewMemory;
}


bool EzMemTracker::trackAllocate(const void* const pvMemory, size_t stSizeInBytes, EMEM_EVENTTYPE eEventType, 
	const char* pcFile, 
	int iLine,
	const char* pcFunction)
{
	return false;

}

bool EzMemTracker::trackDeallocate(const void* const pvMemory, EMEM_EVENTTYPE eEventType)
{
	return false;
}



bool EzMemTracker::verifyAddress(const void* pvMemory)
{
	return true;

}


void EzMemTracker::reAllocReservoir()
{
    EZASSERT(!m_pReservoir);

    m_pReservoir =EzExternalAlloc(EzAllocUnit, m_stReservoirGrowBy);

    EZASSERT(m_pReservoir != NULL);

    m_stActiveTrackerOverhead += sizeof(EzAllocUnit) * m_stReservoirGrowBy + sizeof(EzAllocUnit*);

    if (m_stActiveTrackerOverhead > m_stPeakTrackerOverhead)
    {
		m_stPeakTrackerOverhead = m_stActiveTrackerOverhead;
    }
    
	m_stAccumulatedTrackerOverhead += sizeof(EzAllocUnit) * m_stReservoirGrowBy + sizeof(EzAllocUnit*);

    // Build a linked-list of the elements in the reservoir
    // Initialize the allocation units
    for (unsigned int i = 0; i < m_stReservoirGrowBy-1; i++)
    {
       m_pReservoir[i].m_pNext = &m_pReservoir[i+1];
    }

    m_pReservoir[m_stReservoirGrowBy-1].m_pNext = NULL;

    // Add this address to the reservoir buffer so it can be freed later
    EzAllocUnit **pTemp = (EzAllocUnit**) EzExternalRealloc(
        m_ppReservoirBuffer, 
        (m_stReservoirBufferSize + 1) * sizeof(EzAllocUnit*));

    EZASSERT(pTemp != NULL);

	m_ppReservoirBuffer = pTemp;
    m_ppReservoirBuffer[m_stReservoirBufferSize] = m_pReservoir;
    m_stReservoirBufferSize++;
}

EzMemTracker::EzAllocUnit* EzMemTracker::findAllocUnit(const void* pvMem)
{
    // Just in case...
    EZASSERT(pvMem != NULL);

    unsigned int uiHashIndex = getAddressToHashIndex(pvMem);

    EzAllocUnit* pUnit = m_pActiveMem[uiHashIndex];
    while(pUnit)
    {
        if (pUnit->m_pvMem == pvMem) 
            return pUnit;

        pUnit = pUnit->m_pNext;
    }

    return NULL;
}

void EzMemTracker::insertAllocUnit(EzAllocUnit* pUnit)
{
    EZASSERT(pUnit != NULL && pUnit->m_pvMem != NULL);
    unsigned int uiHashIndex = getAddressToHashIndex(pUnit->m_pvMem);

    // Remap the new allocation unit to the head of the hash entry
    if (m_pActiveMem[uiHashIndex])
    {
        m_pActiveMem[uiHashIndex]->m_pPrev = pUnit;
    }

    pUnit->m_pNext = m_pActiveMem[uiHashIndex];
    pUnit->m_pPrev = NULL;
    m_pActiveMem[uiHashIndex] = pUnit;
}

void EzMemTracker::removeAllocUnit(EzAllocUnit* pUnit)
{
    EZASSERT(pUnit != NULL && pUnit->m_pvMem != NULL);
    unsigned int uiHashIndex = getAddressToHashIndex(pUnit->m_pvMem);

    if (m_pActiveMem[uiHashIndex] == pUnit)
    {
        m_pActiveMem[uiHashIndex] = pUnit->m_pNext;
        
        if (m_pActiveMem[uiHashIndex])
            m_pActiveMem[uiHashIndex]->m_pPrev = NULL;
    }
    else
    {      
        if (pUnit->m_pPrev)
        {
            pUnit->m_pPrev->m_pNext = pUnit->m_pNext;
        }
        if (pUnit->m_pNext)
        {
            pUnit->m_pNext->m_pPrev = pUnit->m_pPrev;
        }
    }
}


size_t EzMemTracker::getMemoryBytesWithPattern(void* pvMemory, size_t stSizeInBytes) const
{
    unsigned char* pcMemArray = (unsigned char*) pvMemory;
    size_t numBytes = 0;
    for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    {
        if (pcMemArray[ui] == m_ucFillChar)
        {
            numBytes++;
        }
    }

    return numBytes;
}

void EzMemTracker::setMemoryFillWithPattern(void* pvMemory, size_t stSizeInBytes)
{
    unsigned char* pcMemArray = (unsigned char*) pvMemory;
    for (unsigned int ui = 0; ui < stSizeInBytes; ui++)
    {
        pcMemArray[ui] = m_ucFillChar;
    }
}


const char* EzMemTracker::formatForXML(const char* pcInString)
{
    static char acOutString[1024];
    EZASSERT(pcInString != NULL);
    int iLen = strlen(pcInString);
    EZASSERT(iLen < 1024);
    unsigned int uiIdx = 0;
    for (int i = 0; i < iLen; i++)
    {
        acOutString[uiIdx] = pcInString[i];
        
        if (acOutString[uiIdx] == '<')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'l';
            acOutString[uiIdx++] = 't';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '>')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'g';
            acOutString[uiIdx++] = 't';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '&')
        {
            acOutString[uiIdx++] = '&';
            acOutString[uiIdx++] = 'a';
            acOutString[uiIdx++] = 'm';
            acOutString[uiIdx++] = 'p';
            acOutString[uiIdx++] = ';';
        }
        else if (acOutString[uiIdx] == '`')
        {
            acOutString[uiIdx++] = ' ';
        }
        else if (acOutString[uiIdx] == '\'')
        {
            acOutString[uiIdx++] = ' ';
        }
        else
        {
            uiIdx++;
        }
        
        EZASSERT(uiIdx < 1024);
    }
    EZASSERT(uiIdx < 1024);
    acOutString[uiIdx] = '\0';
    return acOutString;
}


void EzMemTracker::logAllocUnit(
    const EzAllocUnit *pUnit,
    int iChannel,
    const char* pcPrefix,
    EMEM_EVENTTYPE eDeallocType,
    float fDeallocTime,
    unsigned long ulDeallocThreadId,
    size_t stSizeUnused) const
{
    if (!m_bWriteToLog)
        return;

	EZLOG(iChannel, pcPrefix);
    EZLOG(iChannel, "<alloc_unit ");
    EZLOG(iChannel, "id='%d' ", 
        pUnit->m_stAllocationID);
    EZLOG(iChannel, "alloc_thread_id='%lX' ", 
        pUnit->m_ulAllocThreadId);
    EZLOG(iChannel, "dealloc_thread_id='%lX' ", 
        ulDeallocThreadId);
    EZLOG(iChannel, "alloc_time='%08f' ", 
        pUnit->m_fAllocTime);
    EZLOG(iChannel, "dealloc_time='%08f' ", 
        fDeallocTime);
    EZLOG(iChannel, "life_span='%08f' " , 
        fDeallocTime - pUnit->m_fAllocTime);
    EZLOG(iChannel, "addr='0x%p' ", 
        pUnit->m_pvMem);
    EZLOG(iChannel, "size='%d' ", 
        pUnit->m_stSizeAllocated);
    EZLOG(iChannel, "size_requested='%d' ", 
        pUnit->m_stSizeRequested);
    EZLOG(iChannel, "size_unused='%d' ", 
        stSizeUnused);
    EZLOG(iChannel, "alignment='%d' ", 
        pUnit->m_stAlignment);
    EZLOG(iChannel, "alloc_type='%s' ", 
        EzMemManager::EzMemEventTypeToString(pUnit->m_eAllocType));
    EZLOG(iChannel, "dealloc_type='%s' ", 
        EzMemManager::EzMemEventTypeToString(eDeallocType));
    EZLOG(iChannel, "file='%s' ", 
		pUnit->getSourceFile());
    EZLOG(iChannel, "line='%d' ", 
		pUnit->getSourceLine());
    EZLOG(iChannel, "func='%s' ", 
		formatForXML(pUnit->getFuncName()));
    EZLOG(iChannel, "></alloc_unit>\n");
}

void EzMemTracker::logActiveMemoryReport() const
{
    if (!m_bWriteToLog)
        return;

    EZLOG(EZMESSAGE_MEMORY_LEAK, "<active_memory_dump timestamp='%f' >\n", getTimeSec());

    for (unsigned int uiHashIndex = 0; uiHashIndex < ms_uiHashSize; 
        uiHashIndex++)
    {
        EzAllocUnit* pUnit = m_pActiveMem[uiHashIndex];
        while(pUnit)
        {
            logAllocUnit(pUnit, EZMESSAGE_MEMORY_LEAK, "\t");

            // continue to the next unit
            pUnit = pUnit->m_pNext;
        }
    }
    
    EZLOG(EZMESSAGE_MEMORY_LEAK, "</active_memory_dump>\n");
}