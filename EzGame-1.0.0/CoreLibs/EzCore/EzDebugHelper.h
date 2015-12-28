#ifndef __DEBUG_HELPER_H_INCLUDED__
#define __DEBUG_HELPER_H_INCLUDED__

#include "EzCoreLibType.h"

EZCORE_ENTRY void EzDebugOutput(const char*format,...);


//! define a break macro for debugging.
#if defined(_DEBUG)


#if defined(_WINDOWS_API_) && defined(_MSC_VER) && !defined (_WIN32_WCE)
  #if defined(_WIN64) // using portable common solution for x64 configuration
  #include <crtdbg.h>
  #define DEBUG_BREAK( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
  #else
  #define DEBUG_BREAK( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
  #endif
#else

#include "assert.h"
#define DEBUG_BREAK( _CONDITION_ ) assert( !(_CONDITION_) );



#endif



#if defined(WIN32)
#include <assert.h>
#endif

#if defined(_PS3)
#define DEBUGBREAKPROC() __asm__ volatile("tw 31,1,1")
#elif defined(WIN32)
#define DEBUGBREAKPROC() __debugbreak()
#elif defined(_XENON)
#define DEBUGBREAKPROC() __debugbreak()
#else
#error Platform undefined.
#endif // #ifdef _PS3

#if defined(_DEBUG)
#if defined(WIN32)
#define EZASSERT(value) {assert(value);}
#else
#define EZASSERT(value) {if ((value) == 0) NIDEBUGBREAK()}
#endif
#define EZVERIFY(condition) EZASSERT(!(condition))
#define DEBUGBREAK() {NIDEBUGBREAKPROC();}
#else
#define ASSERT(value)  
#define VERIFY(condition) (condition)
#define DEBUGBREAK() {}
#endif
#endif




#endif
