#pragma once

#include "EzAllocator.h"


#define EZ_USE_SMALL_OBJECT_ALLOCATOR
class EzSmallObjectAllocator;

class EZCORE_ENTRY EzStandardAllocator : public EzAllocator
{
public:
    virtual void* allocate(size_t& stSize,
        size_t& stAlignment,
        EMEM_EVENTTYPE eEventType,
        bool bProvideAccurateSizeOnDeallocate,
        const char* pcFile,
        int iLine,
        const char* pcFunction);

    virtual void deallocate(void* pvMemory,
        EMEM_EVENTTYPE eEventType,
        size_t stSizeinBytes);

    virtual void* reallocate(void* pvMemory,
        size_t& stSize,
        size_t& stAlignment,
        EMEM_EVENTTYPE eEventType,
        bool bProvideAccurateSizeOnDeallocate,
        size_t stSizeCurrent,
        const char* pcFile,
        int iLine,
        const char* pcFunction);

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

    virtual bool verifyAddress(const void* pvMemory);


#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    static EzSmallObjectAllocator* getSmallAllocator();
#endif

protected:
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    static EzSmallObjectAllocator* ms_pSmallAlloc; 
#endif
    static size_t getSizeFromAddress(void* pMemory);
    static void setSizeToAddress(void* pMemory, size_t stSize);
};


