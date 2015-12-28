#ifndef __EZMEMORYDEFINES_H_INCLUDED__
#define __EZMEMORYDEFINES_H_INCLUDED__

#include "EzCoreLibType.h"
#include <malloc.h>

#define EZ_MEM_FILE_DEFAULT 0
#define EZ_MEM_LINE_DEFAULT -1
#define EZ_MEM_FUNCTION_DEFAULT 0
#define EZ_MEM_DEALLOC_SIZE_DEFAULT (size_t)-1

#ifdef _XENON
    #define EZ_MEM_ALIGNMENT_DEFAULT  16
#elif defined(_PS3)
    #define EZ_MEM_ALIGNMENT_DEFAULT  16
#else
    #define EZ_MEM_ALIGNMENT_DEFAULT  4
#endif


enum EMEM_EVENTTYPE
{
    EMEM_UNKNOWN = 0,                 // Unknown allocation/deallocation type.
                                    // This type will cause assertions in
                                    // the memory management interfaces.
    EMEM_OPER_NEW,                    // Allocated through NiNew. 
                                    // Should be deallocated with NiDelete
    EMEM_OPER_NEW_ARRAY,              // Allocated through NiNew[]. 
                                    // Should be deallocated with NiDelete[]
    EMEM_OPER_DELETE,                 // Deallocated with NiDelete.

    EMEM_OPER_DELETE_ARRAY,           // Deallocated with NiDelete[]
    
    EMEM_MALLOC,                      // Allocated through NiMalloc.
                                    // Should be deallocated with NiFree
    EMEM_REALLOC,                     // Allocated through NiRealloc.
                                    // Should be deallocated with NiFree.
    EMEM_ALIGNEDMALLOC,               // Allocated through NiAlignedAlloc.
                                    // Should be deallocated with 
                                    // NiAlignedFree
    EMEM_ALIGNEDREALLOC,              // Allocated through NiAlignedRealloc.
                                    // Should be deallocated with 
                                    // NiAlignedFree
    EMEM_FREE,                        // Deallocated with NiFree
    EMEM_ALIGNEDFREE,                 // Deallocated with NiAlignedFree
    EMEM_EXTERNALALLOC,               // Used to track external allocations
    EMEM_EXTERNALFREE                 // Used to track external frees
};



// Build-independent memory management routines
void  _EzFree(void* pvMemory);
void  _EzAlignedFree(void* pvMemory);






#ifdef _MEMORY_DEBUGGER
    EZCORE_ENTRY void* _EzMalloc(size_t stSizeInBytes, 
        const char* pcSourceFile, int iSourceLine, const char* pcFunction);
    EZCORE_ENTRY void* _EzAlignedMalloc(size_t stSizeInBytes, 
        size_t stAlignment, const char* pcSourceFile, int iSourceLine,
        const char* pcFunction);
    EZCORE_ENTRY void* _EzRealloc(void *memblock, size_t stSizeInBytes, 
        const char* pcSourceFile, int iSourceLine, const char* pcFunction); 
    EZCORE_ENTRY void* _EzAlignedRealloc(void *memblock, 
        size_t stSizeInBytes, size_t stAlignment, const char* pcSourceFile,
        int iSourceLine, const char* pcFunction); 
    EZCORE_ENTRY bool EzVerifyAddress(const void* pvMemory);
    EZCORE_ENTRY bool _EzTrackAlloc(const void* pvMemory, 
        size_t stSizeInBytes, const char* pcSourceFile, int iSourceLine, 
        const char* pcFunction); 
    EZCORE_ENTRY bool _EzTrackFree(const void* pvMemory); 

    #define EzNew new(__FILE__,__LINE__,__FUNCTION__)
    #define EzDelete delete
    #define EzAlloc(T, count) ((T*)_EzMalloc(sizeof(T)*(count), \
        __FILE__, __LINE__, __FUNCTION__)) 
    #define EzAlignedAlloc(T, count, alignment) ((T*)_EzAlignedMalloc( \
        sizeof(T)*(count), alignment, __FILE__, __LINE__, __FUNCTION__)) 
    #define EzAlignedMalloc(size, alignment) (_EzAlignedMalloc(size, \
        alignment, __FILE__, __LINE__, __FUNCTION__)) 
    #define EzMalloc(size) (_EzMalloc(size, \
        __FILE__, __LINE__, __FUNCTION__)) 
    #define EzRealloc(memblock, size) (_EzRealloc(memblock, size, \
        __FILE__, __LINE__, __FUNCTION__)) 
    #define EzAlignedRealloc(memblock, size, alignment) (_EzAlignedRealloc( \
        memblock, size, alignment, __FILE__, __LINE__, __FUNCTION__))
    #define EzFree(p) _EzFree(p)
    #define EzAlignedFree(p) _EzAlignedFree(p)
    #define EzTrackAlloc(pointer, sizeInBytes) (_EzTrackAlloc(pointer,  \
        sizeInBytes,__FILE__, __LINE__, __FUNCTION__)) 
    #define EzTrackFree(pointer)  (_EzTrackFree(pointer)) 

	#define EzBreakOnAlliocID(p) 		EzMemTracker::ms_stBreakOnAllocID = p
#else

	EZCORE_ENTRY void* _EzMalloc(size_t stSizeInBytes);
	EZCORE_ENTRY void* _EzAlignedMalloc(size_t stSizeInBytes,size_t stAlignment);
	EZCORE_ENTRY void* _EzRealloc(void *memblock, size_t stSizeInBytes); 
	EZCORE_ENTRY void* _EzAlignedRealloc(void *memblock, size_t stSizeInBytes, size_t stAlignment); 
   
	#define EzNew											new
	#define EzDelete										delete
	#define EzAlloc(T, count)								((T*)_EzMalloc(sizeof(T)*(count))); 
	#define EzAlignedAlloc(T, count, alignment)				((T*)_EzAlignedMalloc(sizeof(T)*(count), alignment))
	#define EzAlignedMalloc(size, alignment)				(_EzAlignedMalloc(size, alignment)) 
	#define EzMalloc(size)									(_EzMalloc(size)) 
	#define EzRealloc(memblock, size)						(_EzRealloc(memblock, size))
	#define EzAlignedRealloc(memblock, size, alignment)		(_EzAlignedRealloc(memblock, size, alignment))
	#define EzFree(p)										_EzFree(p)
	#define EzAlignedFree(p)								_EzAlignedFree(p)
    #define EzTrackAlloc(p, sizeInBytes) false
    #define EzTrackFree(p) false

#endif


#define EzExternalNew				new
#define EzExternalDelete			delete
#define EzExternalAlloc(T, count)	((T*)malloc(sizeof(T)*(count)))
#define EzExternalMalloc			malloc
#define EzExternalRealloc			realloc
#define EzExternalFree				free
#define EzExternalCalloc			calloc


#ifdef WIN32
	#define EzExternalAlignedMalloc		_aligned_malloc
	#define EzExternalAlignedRealloc	_aligned_realloc
	#define EzExternalAlignedFree		_aligned_free
	#define EzAlignOf(T)				__alignof(T)
#endif


#endif // __EZMEMORYDEFINES_H_INCLUDED__
