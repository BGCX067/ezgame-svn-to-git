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
	HANDLE Handle;		// �������� �ڵ�
	DWORD ThreadID;		// �������� ID


protected:
	void CThread::MakeException();


public:
	bool Terminated;	// ����� �����ΰ�
	bool Suspended;		// ������ �����ΰ�
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

