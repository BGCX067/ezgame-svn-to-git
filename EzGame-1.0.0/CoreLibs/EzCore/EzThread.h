#pragma once


#include "EzCoreLibType.h"
#include <windows.h>
#include <process.h>
#include <exception>

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                      \
         (void *)        (psa),                     \
         (unsigned)      (cbStack),                 \
         (PTHREAD_START) (pfnStartAddr),            \
         (void *)        (pvParam),                 \
         (unsigned)      (fdwCreate),               \
         (unsigned *)    (pdwThreadId)))




class EZCORE_ENTRY EzThread
{
public:
	EzThread(bool bCreateSuspended);
	~EzThread(void);

protected:
	HANDLE	m_hHandle;		// 쓰레드의 핸들
	DWORD	m_dwThreadID;		// 쓰레드의 ID
	bool	m_bTerminated;	// 종료된 상태인가
	bool	m_bSuspended;		// 중지된 상태인가


public:
	bool getTerminated();
	void suspend();
	void resume();
	void terminate();
	static long g_iMaxThread;
	static long g_iExitSignal;
	virtual void execute() = 0;
	virtual void release() = 0;
};


inline EZCORE_ENTRY int GetThreadMax()
{
	return EzThread::g_iMaxThread;
}

inline EZCORE_ENTRY int GetExitThreadMax()
{
    return EzThread::g_iExitSignal;
}


