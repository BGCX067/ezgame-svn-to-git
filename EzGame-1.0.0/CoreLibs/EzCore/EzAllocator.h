#ifndef __EZALLOCATOR_H_INCLUDED__
#define __EZALLOCATOR_H_INCLUDED__

#include "EzCoreLibType.h"
#include <new>
#include <memory.h>


#include "EzMemoryDefines.h"


class EZCORE_ENTRY EzAllocator
{
public:
    virtual ~EzAllocator(){}
    // Encapsulate all memory management through a 
    // single set of API calls.

    // Note that the size and alignment are passed-by-reference. 
    // This allows the allocator to adjust the values
    // internally and pass the results back to the caller.
    virtual void* allocate(
        size_t& stSizeInBytes, 
        size_t& stAlignment,
        EMEM_EVENTTYPE eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        const char* pcFile, 
        int iLine,
        const char* pcFunction) = 0;

    virtual void deallocate(
        void* pvMemory, 
        EMEM_EVENTTYPE eEventType, 
        size_t stSizeInBytes) = 0;

    virtual void* reallocate(
        void* pvMemory, 
        size_t& stSizeInBytes,
        size_t& stAlignment, 
        EMEM_EVENTTYPE eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        size_t stSizeCurrent,
        const char* pcFile, int iLine,
        const char* pcFunction) = 0;

    // Functions to allow tracking of memory. These functions should NOT
    // touch the input addresses in any way.
    virtual bool trackAllocate(
        const void* const pvMemory, 
        size_t stSizeInBytes, 
        EMEM_EVENTTYPE eEventType, 
        const char* pcFile, 
        int iLine,
        const char* pcFunction) = 0;

    virtual bool trackDeallocate(
        const void* const pvMemory, 
        EMEM_EVENTTYPE eEventType) = 0;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    // Called by the memory manager to verify a memory address
    virtual bool verifyAddress(const void* pvMemory) = 0;
};


template<typename T>
class EzAllocatorInterface 
{
public:

	//! Destructor
	virtual ~EzAllocatorInterface() {}

	//! Allocate memory for an array of objects
	static T* allocate(size_t cnt)
	{
		return EzAlloc(T, cnt* sizeof(T));
	}

	//! Deallocate memory for an array of objects
	static void deallocate(T* ptr)
	{
		EzFree(ptr);
	}

	//! Construct an element
	static void construct(T* ptr, const T&e)
	{
		EzExternalNew ((void*)ptr) T(e);
	}

	//! Destruct an element
	static void destruct(T* ptr)
	{
		ptr->~T();
	}
};


#endif