#include "StdAfx.h"
#include <process.h>
#include ".\thread.h"

long CThread::g_iExitSignal = 0;
long CThread::g_iMaxThread = 0;

DWORD WINAPI ThreadFunc(LPVOID Param);


CThread::CThread(bool CreateSuspended)
{
	// 멤버 변수들을 초기화한다.
	Handle = NULL;
	ThreadID = 0;
	Terminated = false;
	Suspended  = false;
	g_iMaxThread++;
	
	// 쓰레드를 생성한다.
	if (CreateSuspended == false)
	{
		Handle = chBEGINTHREADEX(NULL, 0, ThreadFunc, this, 0, &ThreadID);
		Suspended = false;
	}
	else if (CreateSuspended == true)
	{
		Handle = chBEGINTHREADEX(NULL, 0, ThreadFunc, this, CREATE_SUSPENDED, &ThreadID);
		Suspended = true;
	}
}

CThread::~CThread()
{
	if (CloseHandle(Handle) == FALSE)
	{
	
	}
}


bool CThread::GetTerminated()
{
	return Terminated;
}

DWORD WINAPI ThreadFunc(LPVOID Param)
{
	CThread* thread_class = (CThread*) Param;

	while (thread_class->GetTerminated() == false)
	{
		thread_class->Execute();
	}

	thread_class->Release();
	InterlockedExchangeAdd(&CThread::g_iExitSignal, 1);
	return 0;
}

void CThread::Suspend()
{
	if (Suspended == false)
	{
		if (SuspendThread(Handle) == 0xFFFFFFFF)
		{

		}


		Suspended = true;
	}
}

void CThread::Resume()
{
	if (Suspended == true)
	{
		if (ResumeThread(Handle) == 0xFFFFFFFF)
		{
			return;
		}

		Suspended = false;
	}
}

void CThread::Terminate()
{
	if (Terminated == false)
	{
		Terminated = true;
		Suspended = true;
	}
}

