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
	HANDLE	m_hHandle;		// �������� �ڵ�
	DWORD	m_dwThreadID;		// �������� ID
	bool	m_bTerminated;	// ����� �����ΰ�
	bool	m_bSuspended;		// ������ �����ΰ�


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


