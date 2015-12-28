#pragma once

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




class CThread
{
public:
	CThread(bool CreateSuspended);
	~CThread(void);

protected:
	HANDLE Handle;		// 쓰레드의 핸들
	DWORD ThreadID;		// 쓰레드의 ID


protected:
	void CThread::MakeException();


public:
	bool Terminated;	// 종료된 상태인가
	bool Suspended;		// 중지된 상태인가
	bool GetTerminated();
	void Suspend();
	void Resume();
	void Terminate();
	static long g_iMaxThread;
	static long g_iExitSignal;
	virtual void Execute() = 0;
	virtual void Release() = 0;
};


inline int GetThreadMax()
{
	return CThread::g_iMaxThread;
}

inline int GetExitThreadMax()
{
    return CThread::g_iExitSignal;
}

inline BOOL ThreadAllExit()
{
	if(CThread::g_iExitSignal == CThread::g_iMaxThread)
		return TRUE;

	return FALSE;
}

