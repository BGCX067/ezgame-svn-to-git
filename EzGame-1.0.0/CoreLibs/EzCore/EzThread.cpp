#include "EzThread.h"
#include <process.h>

long EzThread::g_iExitSignal = 0;
long EzThread::g_iMaxThread = 0;

DWORD WINAPI ThreadFunc(LPVOID Param);


EzThread::EzThread(bool bCreateSuspended)
{
	// 멤버 변수들을 초기화한다.
	m_hHandle = NULL;
	m_dwThreadID = 0;
	m_bTerminated = false;
	m_bSuspended  = false;
	g_iMaxThread++;
	
	// 쓰레드를 생성한다.
	if (bCreateSuspended == false)
	{
		m_hHandle = chBEGINTHREADEX(NULL, 0, ThreadFunc, this, 0, &m_dwThreadID);
		m_bSuspended = false;
	}
	else if (bCreateSuspended == true)
	{
		m_hHandle = chBEGINTHREADEX(NULL, 0, ThreadFunc, this, CREATE_SUSPENDED, &m_dwThreadID);
		m_bSuspended = true;
	}
}

EzThread::~EzThread()
{
	if (CloseHandle(m_hHandle) == FALSE)
	{
	
	}
}


bool EzThread::getTerminated()
{
	return m_bTerminated;
}

DWORD WINAPI ThreadFunc(LPVOID Param)
{
	EzThread* thread_class = (EzThread*) Param;

	while (thread_class->getTerminated() == false)
	{
		thread_class->execute();
	}

	thread_class->release();
	InterlockedExchangeAdd(&EzThread::g_iExitSignal, 1);
	return 0;
}

void EzThread::suspend()
{
	if (m_bSuspended == false)
	{
		if (SuspendThread(m_hHandle) == 0xFFFFFFFF)
		{

		}


		m_bSuspended = true;
	}
}

void EzThread::resume()
{
	if (m_bSuspended == true)
	{
		if (ResumeThread(m_hHandle) == 0xFFFFFFFF)
		{
			return;
		}

		m_bSuspended = false;
	}
}

void EzThread::terminate()
{
	if (m_bTerminated == false)
	{
		m_bTerminated = true;
		m_bSuspended = true;
	}
}

