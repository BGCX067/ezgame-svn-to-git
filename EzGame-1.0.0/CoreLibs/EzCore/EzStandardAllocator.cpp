#include "EzSystem.h"
#include "EzStandardAllocator.h"
#include "EzMemoryDefines.h"

// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net


#include "EzStandardAllocator.h"
#include "EzSmallObjectAllocator.h"


#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
EzSmallObjectAllocator* EzStandardAllocator::ms_pSmallAlloc = NULL; 
#endif

#define IS_POWER_OF_TWO(x)   (((x)&(x-1)) == 0)

// In order to facilitate speedy deallocations, it is necessary to prepend
// the size of the allocation. This total size in bytes allows us to quickly
// determine how to handle the memory.
// 
// The code works by adding "EZ_MEM_ALIGNMENT_DEFAULT" bytes
// to the size of the allocation. This value is expected to be a multiple
// of the compiler's default alignment. The size, in bytes, of the allocation
// is then set in this header. The memory address is advanced to the "real"
// address.
// 
// Upon deallocation, this value is retrieved by jumping back the header size,
// grabbing the allocation size and then forwarding the value onward into
// the memory management system.
//
// This design assumes that some allocator will be able to better handle
// knowing the full size, rather than inserting its own header. 
// 
// Note: Aligned allocation routines are a special case. Since they can
// be of any alignment, we forward them on without any additional tracking.

//---------------------------------------------------------------------------
void* EzStandardAllocator::allocate(
    size_t& stSizeInBytes,
    size_t& stAlignment,
    EMEM_EVENTTYPE eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    EZASSERT(IS_POWER_OF_TWO(stAlignment));
    
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    EZASSERT(ms_pSmallAlloc != NULL);
    bool bPrependSize = false;
    void* pvMemory = NULL;

    if (stAlignment == EZ_MEM_ALIGNMENT_DEFAULT && 
        bProvideAccurateSizeOnDeallocate == false &&
        eEventType != EMEM_ALIGNEDMALLOC &&
        eEventType != EMEM_ALIGNEDREALLOC)
    {
        stSizeInBytes += EZ_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (eEventType != EMEM_ALIGNEDMALLOC &&
        eEventType != EMEM_ALIGNEDREALLOC && 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE && 
        stAlignment == EZ_MEM_ALIGNMENT_DEFAULT)
    {
        pvMemory = ms_pSmallAlloc->allocate(stSizeInBytes);
    }
    else
    {
        pvMemory = EzExternalAlignedMalloc(stSizeInBytes, stAlignment);
    }

    if (pvMemory && bPrependSize)
    {
        EZASSERT(EZ_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvMemory) = stSizeInBytes;
        setSizeToAddress(pvMemory, stSizeInBytes);
        pvMemory = ((char*)pvMemory) + EZ_MEM_ALIGNMENT_DEFAULT;
    }

    return pvMemory;
#else
    return EzExternalAlignedMalloc(stSizeInBytes, stAlignment);
#endif
}
//---------------------------------------------------------------------------
void EzStandardAllocator::deallocate(
    void* pvMemory, 
    EMEM_EVENTTYPE eEventType, 
    size_t stSizeInBytes)
{
    if (pvMemory == NULL)
        return;

#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    EZASSERT(ms_pSmallAlloc != NULL);

    if (eEventType != EMEM_ALIGNEDFREE && eEventType != EMEM_ALIGNEDREALLOC && 
        EZ_MEM_DEALLOC_SIZE_DEFAULT == stSizeInBytes)
    {
        pvMemory = ((char*)pvMemory) - EZ_MEM_ALIGNMENT_DEFAULT;
        EZASSERT(EZ_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        stSizeInBytes = getSizeFromAddress(pvMemory);
    }

    if (stSizeInBytes <= MAX_SMALL_OBJECT_SIZE)
    {
        ms_pSmallAlloc->deallocate(pvMemory, stSizeInBytes);
        return;
    }
#endif

    EzExternalAlignedFree(pvMemory);
}
//---------------------------------------------------------------------------
void* EzStandardAllocator::reallocate(
    void* pvMemory,
    size_t& stSizeInBytes,
    size_t& stAlignment,
    EMEM_EVENTTYPE eEventType,
    bool bProvideAccurateSizeOnDeallocate,
    size_t stSizeCurrent,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    EZASSERT(IS_POWER_OF_TWO(stAlignment));

    // The deallocation case should have been caught by us before in
    // the allocation functions.
    EZASSERT(stSizeInBytes != 0);
    
    // The standard allocation case should have be caught by us before 
    // in the realloc functions.
    
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    // Determine the current size for the address
    if (eEventType != EMEM_ALIGNEDFREE && eEventType != EMEM_ALIGNEDREALLOC && 
        EZ_MEM_DEALLOC_SIZE_DEFAULT == stSizeCurrent && 
        bProvideAccurateSizeOnDeallocate == false)
    {
        pvMemory = ((char*)pvMemory) - EZ_MEM_ALIGNMENT_DEFAULT;
        EZASSERT(EZ_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //stSizeCurrent = *((size_t*) pvMemory); 
        stSizeCurrent = getSizeFromAddress(pvMemory);
    }

    // pad the allocation for the new current size
    bool bPrependSize = false;
    if (stAlignment == EZ_MEM_ALIGNMENT_DEFAULT && 
        bProvideAccurateSizeOnDeallocate == false &&
        eEventType != EMEM_ALIGNEDMALLOC &&
        eEventType != EMEM_ALIGNEDREALLOC)
    {
        stSizeInBytes += EZ_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (stSizeCurrent != EZ_MEM_DEALLOC_SIZE_DEFAULT && 
        stSizeInBytes <= stSizeCurrent && bPrependSize)
    {
        stSizeInBytes = stSizeCurrent;
        pvMemory = ((char*)pvMemory) + EZ_MEM_ALIGNMENT_DEFAULT;
        return pvMemory;
    }

    void* pvNewMemory = NULL;

    EZASSERT(ms_pSmallAlloc != NULL);
    if (eEventType != EMEM_ALIGNEDMALLOC &&
        eEventType != EMEM_ALIGNEDREALLOC &&
        (stSizeCurrent <= MAX_SMALL_OBJECT_SIZE || 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE))
    {
        EZASSERT(stAlignment ==  EZ_MEM_ALIGNMENT_DEFAULT);
        EZASSERT(stSizeCurrent != EZ_MEM_DEALLOC_SIZE_DEFAULT);

        // Since the small object allocator does not support reallocation
        // we must manually reallocate.

        // Remove any header information that we have added
        // in this call.
        stSizeInBytes -= EZ_MEM_ALIGNMENT_DEFAULT;

        // Allow the allocation call to do its job, which
        // should mean that the size will be embedded just before the
        // address that we receive.
        pvNewMemory = allocate(stSizeInBytes, stAlignment, eEventType,
            bProvideAccurateSizeOnDeallocate, pcFile, iLine, pcFunction);
        bPrependSize = false;

        if (stSizeCurrent != EZ_MEM_DEALLOC_SIZE_DEFAULT)
        {
			int test = getSizeFromAddress(((char*)pvNewMemory - EZ_MEM_ALIGNMENT_DEFAULT));
            char* pvMemoryToCopy = ((char*)pvMemory) + 
                EZ_MEM_ALIGNMENT_DEFAULT;
            size_t stSizeToCopy = stSizeCurrent - EZ_MEM_ALIGNMENT_DEFAULT;

            EzMemoryCopy(pvNewMemory, stSizeInBytes, pvMemoryToCopy, stSizeToCopy);
            deallocate(pvMemory, eEventType, stSizeCurrent);
        }
    }
    else
    {
        pvNewMemory = EzExternalAlignedRealloc(pvMemory, stSizeInBytes, 
            stAlignment);
    }

    if (pvNewMemory && bPrependSize)
    {
        EZASSERT(EZ_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvNewMemory) = stSizeInBytes;
        setSizeToAddress(pvNewMemory, stSizeInBytes);
        pvNewMemory = ((char*)pvNewMemory) + EZ_MEM_ALIGNMENT_DEFAULT;
    }

    return pvNewMemory;
#else
    return EzExternalAlignedRealloc(pvMemory, stSizeInBytes, stAlignment);
#endif   

}
//---------------------------------------------------------------------------
bool EzStandardAllocator::trackAllocate(
    const void* const pvMemory, 
    size_t stSizeInBytes, 
    EMEM_EVENTTYPE eEventType, 
    const char* pcFile, 
    int iLine,
    const char* pcFunction)
{
    return false;
}
//---------------------------------------------------------------------------
bool EzStandardAllocator::trackDeallocate(
    const void* const pvMemory, 
    EMEM_EVENTTYPE eEventType)
{
    return false;
}
//---------------------------------------------------------------------------
void EzStandardAllocator::initialize()
{
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    ms_pSmallAlloc = EzExternalNew EzSmallObjectAllocator();
#endif
}
//---------------------------------------------------------------------------
void EzStandardAllocator::shutdown()
{
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
    EzExternalDelete ms_pSmallAlloc;
    ms_pSmallAlloc = NULL;
#endif
}
//---------------------------------------------------------------------------
bool EzStandardAllocator::verifyAddress(const void* pvMemory)
{
    return true;
}
//---------------------------------------------------------------------------
#ifdef EZ_USE_SMALL_OBJECT_ALLOCATOR
EzSmallObjectAllocator* EzStandardAllocator::getSmallAllocator()
{
    return ms_pSmallAlloc;
}
#endif
//---------------------------------------------------------------------------
size_t EzStandardAllocator::getSizeFromAddress(void* pMemory)
{
    size_t stSize;
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    EZASSERT(sizeof(size_t) == 4);
    pcSize[0] = pcMemory[0];
    pcSize[1] = pcMemory[1];
    pcSize[2] = pcMemory[2];
    pcSize[3] = pcMemory[3];
    
    /*
    for (size_t i = 0; i < sizeof(size_t); i++)
    {
        pcSize[i] = pcMemory[i];
    }
    */

    return stSize;
}
//---------------------------------------------------------------------------
void EzStandardAllocator::setSizeToAddress(void* pMemory, size_t stSize)
{
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    
    EZASSERT(sizeof(size_t) == 4);
    pcMemory[0] = pcSize[0];
    pcMemory[1] = pcSize[1];
    pcMemory[2] = pcSize[2];
    pcMemory[3] = pcSize[3];

    /*
    for (size_t i = 0; i < sizeof(size_t); i++)
    {
        pcMemory[i] = pcSize[i];
    }
    */
}
