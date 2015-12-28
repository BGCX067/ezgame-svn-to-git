#ifndef _EZ_MEMTRACKER_H_
#define _EZ_MEMTRACKER_H_


#include "EzAllocator.h"
#include "EzMap.h"
#include "EzCriticalSection.h"
#include "EzCoreUtility.h"

#define LOG_FILE_NAME_LENGTH			256
#define LOG_FUNCTION_NAME_LENGTH		256


class EZCORE_ENTRY EzMemTracker : public EzAllocator
{
public:

	class EZCORE_ENTRY EzAllocUnit
	{
	public:
    	size_t				m_stAllocationID;
    	void* 				m_pvMem;
    	size_t 				m_stAlignment;
    	size_t 				m_stSizeRequested;
    	size_t 				m_stSizeAllocated;

		EzAllocUnit* 		m_pPrev;
    	EzAllocUnit* 		m_pNext;

    	unsigned long 		m_ulAllocThreadId;
    	float 				m_fAllocTime;
    	EMEM_EVENTTYPE  	m_eAllocType;
		
		void reset();

		void setSourceFileInfo(const char* pcFile, unsigned int uiLine, const char* pcFunc);
		const char* getSourceFile() const;
		const char* getFuncName() const;
		const unsigned int& getSourceLine() const;


	protected:
    	char 				m_pcFile[LOG_FILE_NAME_LENGTH];
    	char 				m_pcFunc[LOG_FUNCTION_NAME_LENGTH];
	    unsigned int 		m_uiLine;

	};

	
    EzMemTracker(EzAllocator* pActualAllocator, bool bWriteToLog = true,
        unsigned int uiInitialSize = 65536, 
        unsigned int uiGrowBy = 4096, bool bAlwaysValidateAll = false,
        bool bCheckArrayOverruns = true);
    virtual ~EzMemTracker();

    virtual void* allocate(
        size_t& stSizeInBytes, 
        size_t& stAlignment,
        EMEM_EVENTTYPE eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        const char* pcFile, 
        int iLine,
        const char* pcFunction);

    virtual void deallocate(
        void* pvMemory, 
        EMEM_EVENTTYPE eEventType, 
        size_t stSizeInBytes);

    virtual void* reallocate(
        void* pvMemory, 
        size_t& stSizeInBytes,
        size_t& stAlignment, 
        EMEM_EVENTTYPE eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        size_t stSizeCurrent,
        const char* pcFile, int iLine,
        const char* pcFunction);

    // Functions to allow tracking of memory. These functions should NOT
    // touch the input addresses in any way.
    virtual bool trackAllocate(
        const void* const pvMemory, 
        size_t stSizeInBytes, 
        EMEM_EVENTTYPE eEventType, 
        const char* pcFile, 
        int iLine,
        const char* pcFunction);

    virtual bool trackDeallocate(
        const void* const pvMemory, 
        EMEM_EVENTTYPE eEventType);

    virtual void initialize();
    virtual void shutdown();

    // Called by the memory manager to verify a memory address
    virtual bool verifyAddress(const void* pvMemory);


	static size_t 		ms_stBreakOnAllocID;

protected:
	EzAllocUnit* getAllocUnit();
	void freeAllocUnit(EzAllocUnit* pUnit);

	size_t getMemoryBytesWithPattern(void* pvMemory, size_t stSizeInBytes) const;
	void setMemoryFillWithPattern(void* pvMemory, size_t stSizeInBytes);


	static const char* formatForXML(const char* pcInString);
	void logAllocUnit(const EzAllocUnit *pkUnit,
    	int iChannel,  const char* pcPrefix = "", EMEM_EVENTTYPE eDeallocType = EMEM_UNKNOWN,   float fDeallocTime = 0.0f,
    	unsigned long ulDeallocThreadId = 0, size_t stSizeUnused = 0) const;
	void logActiveMemoryReport() const;


	void reAllocReservoir();
	void insertAllocUnit(EzAllocUnit* pUnit);
	void removeAllocUnit(EzAllocUnit* pUnit);
	EzAllocUnit* findAllocUnit(const void* pvMem);

	unsigned int getAddressToHashIndex(const void* pvAddress);


    EzAllocator* 		m_pActualAllocator;
	EzCriticalSection 	m_CriticalSection;
	 
	size_t 				m_stCurrentAllocID;
    bool 				m_bCheckArrayOverruns;
    unsigned char 		m_ucFillChar;
    bool 				m_bWriteToLog;


    static const unsigned int ms_uiHashBits = 12;
    static const unsigned int ms_uiHashSize = 1 << ms_uiHashBits;
	
	EzAllocUnit* m_pReservoir;


	size_t m_stActiveMemory;
    size_t m_stActiveAllocationCount;
    size_t m_stPeakAllocationCount;
	size_t m_stActiveTrackerOverhead;
	size_t m_stPeakTrackerOverhead;
	size_t m_stAccumulatedTrackerOverhead;

	size_t m_stReservoirGrowBy;
    EzAllocUnit** m_ppReservoirBuffer;
    size_t m_stReservoirBufferSize;
    EzAllocUnit* m_pActiveMem[ms_uiHashSize];

};


inline const char* EzMemTracker::EzAllocUnit::getSourceFile() const
{
	return m_pcFile;
}

inline const char* EzMemTracker::EzAllocUnit::getFuncName() const
{
	return m_pcFunc;
}

inline const unsigned int& EzMemTracker::EzAllocUnit::getSourceLine() const
{
	return m_uiLine;
}

inline void EzMemTracker::EzAllocUnit::setSourceFileInfo(const char* pcFile, unsigned int uiLine, const char* pcFunc)
{
	const char* pcSlash = strrchr(pcFile, '\\');

	EZASSERT(strlen(pcFunc) < LOG_FUNCTION_NAME_LENGTH)

	EzStringCpy(m_pcFunc, LOG_FUNCTION_NAME_LENGTH, pcFunc);
	m_uiLine = uiLine;


    if (pcSlash) 
	{

		EZASSERT(strlen(pcSlash + 1) < LOG_FILE_NAME_LENGTH)

		EzStringCpy(m_pcFile, LOG_FILE_NAME_LENGTH, pcSlash + 1);
		return;
	}

    pcSlash = strrchr(m_pcFile, '/');
    if (pcSlash) 
	{
		EZASSERT(strlen(pcSlash + 1) < LOG_FILE_NAME_LENGTH)
		EzStringCpy(m_pcFile, LOG_FILE_NAME_LENGTH, pcSlash + 1);
	}
}

inline void EzMemTracker::EzAllocUnit::reset()
{
    m_stAllocationID = (size_t) -1;
    m_stAlignment = 0;
    m_ulAllocThreadId = 0;
    m_pvMem = NULL;
    m_stSizeRequested = 0;
    m_stSizeAllocated = 0;
//    m_kFLF.Reset();
//    m_eAllocType = NI_UNKNOWN;
//    m_fAllocTime = -FLT_MAX;
    m_pPrev = NULL;
    m_pNext = NULL;
}

inline unsigned int EzMemTracker::getAddressToHashIndex(const void* pvAddress)
{
    // Use the address to locate the hash index. Note that we shift off the 
    // lower four bits. This is because most allocated addresses will be on 
    // four-, eight- or even sixteen-byte boundaries. If we didn't do this, 
    // the hash index would not have very good coverage.

    // The size_t may be cast to an unsigned int as long as ms_uiHashSize
    // is less than the largest unsigned int.

    return (unsigned int)(reinterpret_cast<size_t>(pvAddress) >> 4) & 
        (ms_uiHashSize - 1);
}


#endif
