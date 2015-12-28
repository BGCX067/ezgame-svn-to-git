#ifndef __EZCORELIBTYPE_H_INCLUDED__
#define __EZCORELIBTYPE_H_INCLUDED__


#include <windows.h>

#ifdef EZCORE_EXPORT
    // DLL library project uses this
    #define EZCORE_ENTRY __declspec(dllexport)
#else
#ifdef EZCORE_IMPORT
    // client of DLL uses this
    #define EZCORE_ENTRY __declspec(dllimport)
#else
    // static library project uses this
    #define EZCORE_ENTRY
#endif
#endif

#ifdef WIN32
    #pragma warning( disable : 4251 )
#endif            
                          
#endif