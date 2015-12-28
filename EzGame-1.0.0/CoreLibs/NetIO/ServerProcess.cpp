#include "StdAfx.h"
#include "NetIOModule.h"
#include "ServerProcess.h"


CServerProcess::CServerProcess(int iQueueSize, int iProcessNumber) 
	: CThread(true) 
{
	m_pProcessQueue = new CQueue<JOB_MESSAGE*>(iQueueSize);
	m_pPacketPool = new MemPooler<JOB_MESSAGE>(iQueueSize);
	m_iProcessNumber = iProcessNumber;
	m_TimeOut = INFINITE;

    GETPROCESSMGR()->AddProcess(iProcessNumber, this);
}


CServerProcess::~CServerProcess(void)
{
	delete m_pPacketPool;
	delete m_pProcessQueue;

	GETPROCESSMGR()->DelProcess(m_iProcessNumber);

}


void CServerProcess::Execute()
{

	JOB_MESSAGE*				lpJob = NULL;
	BOOL						bResult;
	LPSOCKETCONTEXT				lpSocketContext = NULL;

	bResult = m_pProcessQueue->Blocking_Pop_Timeout(&lpJob, m_TimeOut); 

	if( GetTickCount() - m_dwTimeOutLastTime > m_TimeOut)
	{
		JobTimeOutProcess();
		m_dwTimeOutLastTime = GetTickCount();
	}
	
	if(bResult == TRUE)
	{
		lpSocketContext = lpJob->lpSocketContext;

		JobProcess(lpSocketContext, (char*) lpJob->Packet.Data, lpJob->Packet.Common.Len - 10, lpJob->dwPushTime);

		m_pPacketPool->Free(lpJob);

		if(lpJob->Packet.Common.Len == 0)
            Terminate();
	}

}

void CServerProcess::SetTimeOut(DWORD TimeOut)
{
	m_TimeOut = TimeOut;
	m_dwTimeOutLastTime = GetTickCount();

}

void CServerProcess::Stop(void)
{
	TCommonPacket Common;
	ZeroMemory(&Common, sizeof(TCommonPacket));
	JobMessage(NULL, 0, (char *)&Common, sizeof(TCommonPacket));
}

//void CServerProcess::SendJob(LPSOCKETCONTEXT lpSocketContext, int iCmdID, DWORD uid, BYTE Ctrl, BYTE Cmd, char *pPacket, int iSize)
//{
//	CServerProcess* pSP = m_pProcessManager->FindProcess(iCmdID);
//	pSP->JobMessage(lpSocketContext, uid, Ctrl, Cmd, (char *)pPacket, iSize);
//}


