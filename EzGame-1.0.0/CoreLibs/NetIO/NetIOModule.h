/*=========== (C) Copyright 2003 SUNWOO Entertainment All rights reserved. ===========
//
// Purpose  : 네트웍 서버기능을 가지는 클레스
//
// $Workfile: ServerBase.h
// $Date    : 2003. 6. 21
// $NoKeywords: Singleton Patters
//
// 2004. 9 : 패킷 분기 루틴 디비 사용 프로세싱과 디비 사용안하는 프로세싱 분리 				

=============================================================================*/

#pragma once


#include "Context.h"
#include "MemPooler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "ServerProcess.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
 


class CUdpSocket;

class CNetIOModule
{
public:
	CNetIOModule(void);
	~CNetIOModule(void);
	BOOL InitNetIO(void);
	void Start(void);
	BOOL Create(int iPort, int iMaxConnect, int iJobMemPoolSize, int iConcurrent);
	bool CreateUDP(int nPort);

	void SendUDP(const char* IP, int nPort, const char *cpPacket, int nSize);

	void SendPacket(LPSOCKETCONTEXT lpSockContext, BYTE Ctrl, const char *cpPacket, int iSize);
	void BrocastTcpSend(char *cpPacket, int iPacketSize );
	void PostTcpSend( LPSOCKETCONTEXT lpSockContext, int iTransferred);
	void Stop(void);
	void Pause(void);
	void Continue(void);
	int	WakeUpIOCP(LPSOCKETCONTEXT lpSockContext, ULONG pkey);
	bool GetServerStatue(void);
    LPSOCKETCONTEXT Connect(const char* Addr, int iPort);

	void ConnectClose( LPSOCKETCONTEXT lpSockContext);
	void CloseNetWork( LPSOCKETCONTEXT lpSockContext );
	inline static CNetIOModule* GetNet() { return m_pGlobalNetIO; }
	inline int GetMaxConnet() { return m_iMaxConnect; }
	inline int GetConcurrent()	{ return 	m_iConcurrent; }
	inline int GetPort() { return m_iPort; }
	inline int GetQueueSize() { return m_pGameJobMemPool->GetCount(); }
	inline LPSOCKETCONTEXT GetSocketContext(int iIndex)
	{ 
		if(iIndex >= 0 && iIndex <= m_iMaxConnect)
		{
            return &m_lpClientContext[iIndex]; 
		}

		return NULL;
	}
	inline bool bServerStart() { return m_bServerStart; }
	void ReuseSocket( LPSOCKETCONTEXT lpSockContext);

	static CProcessManager* GetProcessMgr() { return m_pProcessManager; }

	virtual void OnRecvUDP(const char* pPacket)  = 0;

protected:

	void DoIOFail(LPEOVERLAPPED lpEov);
	void DoIO(LPEOVERLAPPED lpEov, DWORD dwTransBytes);
	BOOL IPBind(SOCKET sktListen, int nPort, int nBacklog);
	BOOL CreateIOCP(int concurrent);
	HANDLE AddIOCompletionPort(HANDLE h, unsigned long completionKey);
	void RecvTcpBufEnqueue( LPSOCKETCONTEXT lpSockContext, int iPacketSize );
	BOOL RecvToGameQueue( LPSOCKETCONTEXT lpSockContext );
	void PostTcpRecv( LPSOCKETCONTEXT lpSockContext );
	void PostTcpReSend( LPSENDSOCKETCONTEXT lpSendContext, int iTransferred );
	void ClientAccept( LPSOCKETCONTEXT lpEov );

	SOCKET AddClientSocket(SOCKET sktListen, UINT iAddCount);

	void InitSocketContext(LPSOCKETCONTEXT  lpSocketContext,SOCKET sktClient, int iAddCount, int iContextType);

	CRITICAL_SECTION		m_csRecv;

	virtual void OnConnect(LPSOCKETCONTEXT lpSocketContext)  = 0;
	virtual void OnAccept(LPSOCKETCONTEXT lpSocketContext)   = 0;
	virtual void OnRead(LPSOCKETCONTEXT lpSocketContext, BYTE Ctrl, char* pPacket, int iSize) = 0;
	virtual void OnClose(LPSOCKETCONTEXT lpSocketContext)    = 0;
	virtual BOOL OnCreate()   = 0;
	virtual void OnStop()  = 0;

	void ClientClose( LPSOCKETCONTEXT lpSockContext);



protected:
	static	CNetIOModule*		m_pGlobalNetIO;
	LPSOCKETCONTEXT				m_lpClientContext;
	LPSENDSOCKETCONTEXT			m_lpSendContext;
	bool						m_bStop;
	bool						m_ServerState;
	int							m_iMaxConnect;
	HANDLE						m_hIOCP;
	int							m_concurrent;
	SOCKET						m_sktListen;
	int							m_iConcurrent;
	int							m_iPort;
	int							m_ConnectCount;			// 접속자 카운트
	bool						m_bServerStart;

	MemPooler<JOB_MESSAGE>*		m_pGameJobMemPool;

	static CProcessManager*		m_pProcessManager;
	//CUdpSocket*					m_pUDP;


};


inline CNetIOModule* Network()
{
	return CNetIOModule::GetNet();
}

inline LPSOCKETCONTEXT Connect(const char* Addr, int iPort)
{
	return CNetIOModule::GetNet()->Connect(Addr, iPort);
}



inline int SendUDP(char *pPacket, int iSize, char* IP, int iPort)
{
	SOCKADDR_IN servAddr;
	int iResult;

	SOCKET hSocket;

	hSocket=socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(IP);
	servAddr.sin_port=htons(iPort);

	iResult = sendto(hSocket, pPacket, iSize , 0,
			(struct sockaddr*)&servAddr, sizeof(servAddr));

	closesocket(hSocket);

	return iResult;


}


inline void SendPacket(LPSOCKETCONTEXT lpSockContext, BYTE Ctrl, const char *cpPacket, int iSize)
{
		if(lpSockContext != NULL)
            CNetIOModule::GetNet()->SendPacket(lpSockContext, Ctrl, cpPacket, iSize);
}

inline void SendUDP(const char* IP, int nPort, const char *cpPacket, int nSize)
{
	CNetIOModule::GetNet()->SendUDP(IP, nPort, cpPacket, nSize);
}




////////////////////////////////////////////////////////////
// 방송용 패킷을 보낸다.
////////////////////////////////////////////////////////////
inline void BrocastTcpSend(char *cpPacket, int iPacketSize )
{
	CNetIOModule::GetNet()->BrocastTcpSend(cpPacket, iPacketSize);
}

inline void ReuseSocket(LPSOCKETCONTEXT lpSockContext )
{
	CNetIOModule::GetNet()->ReuseSocket(lpSockContext);

}

inline void NetClose(LPSOCKETCONTEXT lpSockContext )
{
	if(lpSockContext != NULL)
	{
		//if(lpSockContext->eovRecvTcp.ContextType == enAcceptContext)
			CNetIOModule::GetNet()->CloseNetWork(lpSockContext);
	}
}


inline int IsPacket(const char *p)
{
	UINT16 iResult = 0;

	assert(p);
	CopyMemory(&iResult, p, 2);
	return iResult;

}

inline CProcessManager*	GETPROCESSMGR()
{
	return CNetIOModule::GetProcessMgr();

}


inline void	SENDJOB(LPSOCKETCONTEXT lpSocketContext, BYTE Ctrl, const char *pPacket, int iSize)
{
	CServerProcess* pSP = CNetIOModule::GetProcessMgr()->FindProcess(Ctrl);

	if(pSP)
		pSP->JobMessage(lpSocketContext, Ctrl, (char *)pPacket, iSize);


}

