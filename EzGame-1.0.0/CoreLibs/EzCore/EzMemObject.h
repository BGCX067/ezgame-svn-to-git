#pragma once

#include "EzCoreLibType.h"
#include "EzMemoryDefines.h"
#include "EzDebugHelper.h"


class EZCORE_ENTRY EzMemObject
{
#ifdef _MEMORY_DEBUGGER
    private:
        // The memory debugger uses the file, line, function 
        // routines. Therefore new and new[] should be 
        // unavailable to the outer application.
        static void* operator new(size_t stSize);
        static void* operator new[](size_t stSize);
    public:
        static void *operator new(
            size_t stSize, 
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction);

        static void *operator new[](
            size_t stSize, 
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction);

        // Required for exception handling in the compiler. These 
        // should not be used in practice.
        static void operator delete(
            void *pvMem, 
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction){}

        static void operator delete[](
            void *pvMem, 
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction){}
#else
    public:
        static void* operator new(size_t stSize);
        static void* operator new[](size_t stSize);
#endif
    public:
        static void operator delete(void *pvMem, size_t stElementSize);
        static void operator delete[](void *pvMem, size_t stElementSize);

        static void* operator new( size_t stSize, void* p ) { return p; }
        static void* operator new[]( size_t stSize, void* p ) { return p; }

        static void operator delete( void *, void* ) {}
        static void operator delete[]( void *, void* ) {}
};

