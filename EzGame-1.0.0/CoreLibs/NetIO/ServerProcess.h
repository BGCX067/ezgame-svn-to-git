#pragma once

#include "Thread.h"
#include "Context.h"
#include "CQueue.h"
#include "MemPooler.h"
#include "ProcessManager.h"

class CServerProcess : public CThread
{
public:
	CServerProcess(int iQueueSize, int iProcessNumber);
	~CServerProcess(void);
	void Execute();
	void Stop(void);

	inline BOOL JobMessage(LPSOCKETCONTEXT lpSocketContext, BYTE Ctrl, char *pPacket, int iSize) 
	{ 


		JOB_MESSAGE* pJobMsg = m_pPacketPool->Alloc();


		if(pJobMsg == NULL)
		{
			return FALSE;
		}

		pJobMsg->lpSocketContext = lpSocketContext;
		pJobMsg->dwPushTime = GetTickCount();

		pJobMsg->Packet.Common.Len = iSize + sizeof(TCommon);
		CopyMemory(&pJobMsg->Packet.Data, pPacket, iSize);
		pJobMsg->Packet.Data[iSize] = 0;
       
		return m_pProcessQueue->NonBlocking_Push(pJobMsg); 
	}


	inline int GetQueueSize() { return m_pProcessQueue->GetQueueSize(); }

	//void SendJob(LPSOCKETCONTEXT lpSocketContext, int iCmdID, DWORD uid, BYTE Ctrl, BYTE Cmd, char *pPacket, int iSize);
	void SetTimeOut(DWORD TimeOut);

protected:
	CQueue<JOB_MESSAGE*>*	m_pProcessQueue; 
	int							m_iProcessNumber;
	DWORD						m_TimeOut;
	DWORD						m_dwTimeOutLastTime;

	virtual void Release() { };
	virtual void JobProcess(LPSOCKETCONTEXT lpSocketContext, char* pData, DWORD iSize, DWORD dwTime) = 0;
	virtual void JobTimeOutProcess() { };


	MemPooler<JOB_MESSAGE>*	m_pPacketPool;
	bool m_bStop;

};
