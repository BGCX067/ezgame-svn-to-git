#include "EzMemObject.h"
#include "EzMemManager.h"
#include "EzDebugHelper.h"
#include <exception> // for std::bad_alloc
#include<new>



#ifdef _MEMORY_DEBUGGER
    void* EzMemObject::operator new(size_t stSize)
    {
        throw std::bad_alloc();
    }

    void* EzMemObject::operator new[](size_t stSize)
    {
        throw std::bad_alloc();
    }

    void* EzMemObject::operator new(
        size_t stSizeInBytes, 
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
        EZASSERT(EzMemManager::isInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

        void* p = EzMemManager::get().allocate(stSizeInBytes, 
            EZ_MEM_ALIGNMENT_DEFAULT, EMEM_OPER_NEW, true, pcSourceFile, 
            iSourceLine, pcFunction);

        if (p == 0)
            throw std::bad_alloc();

        return p;
    }

    void* EzMemObject::operator new[](
        size_t stSizeInBytes, 
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
       
        EZASSERT(EzMemManager::isInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

        void* p =  EzMemManager::get().allocate(stSizeInBytes, 
            EZ_MEM_ALIGNMENT_DEFAULT, EMEM_OPER_NEW_ARRAY, false,
            pcSourceFile, iSourceLine, pcFunction);        
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
#else
    void* EzMemObject::operator new(size_t stSizeInBytes)
    {
        EZASSERT(EzMemManager::isInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;
        
        void* p =  EzMemManager::get().allocate(stSizeInBytes, 
            EZ_MEM_ALIGNMENT_DEFAULT, EMEM_OPER_NEW, true);
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
    void* EzMemObject::operator new[](size_t stSizeInBytes)
    {
        EZASSERT(EzMemManager::isInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

        void* p =  EzMemManager::get().allocate(stSizeInBytes, 
            EZ_MEM_ALIGNMENT_DEFAULT, EMEM_OPER_NEW_ARRAY, false);
        
        if (p == 0)
            throw std::bad_alloc();

        return p;
    }
#endif

void EzMemObject::operator delete(void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        EZASSERT(EzMemManager::isInitialized());
        EzMemManager::get().deallocate(pvMem, EMEM_OPER_DELETE, 
            stElementSize);
    }
}

void EzMemObject::operator delete[](void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        EZASSERT(EzMemManager::isInitialized());       
        EzMemManager::get().deallocate(pvMem, EMEM_OPER_DELETE_ARRAY);
    } 
}

