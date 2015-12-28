#ifndef __EZCRITICALSECTION_H_INCLUDE__
#define __EZCRITICALSECTION_H_INCLUDE__

#include "EzCoreLibType.h"
#include "EzDebugHelper.h"
#include <windows.h>


class EZCORE_ENTRY EzCriticalSection
{
public:
    
	EzCriticalSection();
    ~EzCriticalSection();

    void lock();
    void unlock();

    unsigned int getOwnerThreadID() const;
    unsigned int getCurrentLockCount() const;

protected:
#if defined (EZ_MULTITHREADED)
#if defined (WIN32) || defined(_XENON)
    CRITICAL_SECTION m_CriticalSection;
    unsigned int m_ulThreadOwner;

#endif // #ifdef for platforms

	unsigned int m_uiLockCount;
#endif // #if defined (NI_MULTITHREADED)


};

#if defined(EZ_MULTITHREADED)
inline EzCriticalSection::EzCriticalSection() : 
    m_ulThreadOwner(0), m_uiLockCount(0)
{
    InitializeCriticalSection(&m_CriticalSection);
}
#else 
inline EzCriticalSection::EzCriticalSection()
{
}
#endif

inline EzCriticalSection::~EzCriticalSection()
{
#if defined(EZ_MULTITHREADED)
    DeleteCriticalSection(&m_CriticalSection);
#endif // #if !defined(NI_MULTITHREADED)
}

inline void EzCriticalSection::lock()
{

#if defined(EZ_MULTITHREADED)
    EnterCriticalSection(&m_CriticalSection);

#if defined(_DEBUG)
    m_ulThreadOwner = GetCurrentThreadId();
    m_uiLockCount++;
#endif

#endif // #if !defined(NI_MULTITHREADED)

}

inline void EzCriticalSection::unlock()
{
#if defined(EZ_MULTITHREADED)

#if defined(_DEBUG)
    EZASSERT(m_uiLockCount > 0);
    m_uiLockCount--;
    if (m_uiLockCount == 0)
        m_ulThreadOwner = 0;

#endif
    LeaveCriticalSection(&m_CriticalSection);
#endif // #if !defined(EZ_MULTITHREADED)

}

inline unsigned int EzCriticalSection::getOwnerThreadID() const
{
#if defined(EZ_MULTITHREADED)
    return m_ulThreadOwner;
#else
    return 0;
#endif // #if !defined(NI_MULTITHREADED)
}

inline unsigned int EzCriticalSection::getCurrentLockCount() const
{
#if defined(EZ_MULTITHREADED)
    return m_uiLockCount;
#else
    return 0;
#endif // #if !defined(EZ_MULTITHREADED)

}




class EZCORE_ENTRY EzFastCriticalSection
{
public:
    /// Default constructor.
    EzFastCriticalSection();
    /// Destructor.
    ~EzFastCriticalSection();

    /// Locks the critical section.
    void lock();

    /// Unlocks the critical section.
    void unlock();
    
protected:
#if defined (EZ_MULTITHREADED)
#if defined (WIN32) || defined(_XENON)
    CRITICAL_SECTION m_kCriticalSection;
#elif defined (_PS3)
    sys_mutex_t m_kCriticalSection;
#endif // #ifdef for platforms

#if defined (_DEBUG)
    bool m_bLocked;
#endif // #if defined (_DEBUG)

#endif // #if defined (EZ_MULTITHREADED)
};




inline EzFastCriticalSection::EzFastCriticalSection()
{
#if defined(EZ_MULTITHREADED)
#if defined(_DEBUG)
    m_bLocked = false;
#endif  //#if defined(_DEBUG)

    InitializeCriticalSection(&m_kCriticalSection);
#endif // #if !defined(EZ_MULTITHREADED)
}

inline EzFastCriticalSection::~EzFastCriticalSection()
{ 
#if defined(EZ_MULTITHREADED)
    DeleteCriticalSection(&m_kCriticalSection);
#endif // #if !defined(EZ_MULTITHREADED)
}

inline void EzFastCriticalSection::lock()
{
#if defined(EZ_MULTITHREADED)
    EnterCriticalSection(&m_kCriticalSection);
#if defined(_DEBUG)
    NIASSERT(m_bLocked == false);
    m_bLocked = true;
#endif  //#if defined(_DEBUG)
#endif // #if !defined(NI_MULTITHREADED)
}

inline void EzFastCriticalSection::unlock()
{ 
#if defined(EZ_MULTITHREADED)
#if defined(_DEBUG)
    m_bLocked = false;
#endif  //#if defined(_DEBUG)

    LeaveCriticalSection(&m_kCriticalSection);
#endif // #if !defined(EZ_MULTITHREADED)
}

#endif
