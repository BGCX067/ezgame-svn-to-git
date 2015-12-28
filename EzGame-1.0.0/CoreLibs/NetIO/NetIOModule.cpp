#include "StdAfx.h"
#include <mswsock.h>
#include "NetIOModule.h"
#include "CUdpSocket.h"

CNetIOModule* CNetIOModule::m_pGlobalNetIO   = NULL;
CProcessManager* CNetIOModule::m_pProcessManager = NULL;


LPFN_CONNECTEX lpfnConnectEx = NULL;              // a pointer to the 'ConnectEx()' function
GUID           GuidConnectEx = WSAID_CONNECTEX;   // The Guid


CNetIOModule::CNetIOModule(void)
{

	m_pGlobalNetIO	= this;
	
	m_hIOCP			= NULL;
	m_sktListen		= INVALID_SOCKET;
	m_iMaxConnect	= 0;
	m_ServerState	= false;
	m_bStop			= false;

	m_pGameJobMemPool = NULL;
	m_lpClientContext = NULL;
	m_lpSendContext   = NULL;

	m_pProcessManager = new CProcessManager;

}

BOOL CNetIOModule::InitNetIO(void)
{
	// ���� �ʱ�ȭ
	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 

	wVersionRequested = MAKEWORD( 2, 2 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	
	
	if ( err != 0 ) 
	{
		return FALSE;
	} 

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return FALSE;
	} 

	return TRUE;
}


CNetIOModule::~CNetIOModule(void)
{

	//if(m_pUDP)
	//{
	//	delete m_pUDP;
	//}

	delete m_pGameJobMemPool;
	delete m_lpClientContext;
	delete m_lpSendContext;	
}

void CNetIOModule::SendUDP(const char* IP, int nPort, const char *cpPacket, int nSize)
{
	//m_pUDP->Send(IP, nPort, cpPacket, nSize);
}


///////////////////////////////////////////////////////////////////////
// �������ϰ� ��Ʈ�� ���ε� �Ѵ�.
//////////////////////////////////////////////////////////////////////
BOOL CNetIOModule::IPBind(SOCKET sktListen, int nPort, int nBacklog)
{
	int zero = 0;
	struct sockaddr_in sockaddr;

	// �������ּ� ����
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	sockaddr.sin_port = htons(nPort);

	// �����ּҿ� ��Ʈ�� ���ε�
	if(bind(sktListen,(struct sockaddr *)&sockaddr, sizeof(sockaddr))
		==	SOCKET_ERROR) {
		return FALSE;
	}

	// Ŭ���̾�Ʈ ���� ��� 
	if(listen(sktListen, nBacklog) != 0) 
	{
		return FALSE;
	}

	return TRUE;
}


int CNetIOModule::WakeUpIOCP(LPSOCKETCONTEXT lpSockContext, ULONG pkey)
{

	BOOL ret = PostQueuedCompletionStatus(m_hIOCP, 0, pkey, (OVERLAPPED *) &lpSockContext->eovRecvTcp);
	return ret;
}


BOOL CNetIOModule::CreateIOCP(int concurrent)
{
	m_concurrent = concurrent;
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, m_concurrent );
	
	if( INVALID_HANDLE_VALUE == m_hIOCP ) 
	{
		return FALSE;
	}

	return TRUE;
}

//bool CNetIOModule::CreateUDP(int nPort)
//{
//	m_pUDP = new CUdpSocket;
//
//	m_pUDP->Init(nPort);
//
//
//	return true;
//
//}

BOOL CNetIOModule::Create(int iPort, int iMaxConnect, int iJobMemPoolSize, int iConcurrent)
{
	m_iConcurrent = iConcurrent;
	m_iPort = iPort;
	m_iMaxConnect = iMaxConnect;
	m_ConnectCount = 0;

	if(!InitNetIO())
		return FALSE;

	m_pGameJobMemPool    = new MemPooler<JOB_MESSAGE>(iJobMemPoolSize);
	m_lpClientContext	 = new SOCKETCONTEXT[m_iMaxConnect + 1];
	m_lpSendContext		 = new SENDSOCKETCONTEXT[m_iMaxConnect + 1];

	if(m_lpClientContext == NULL)
	{
		CLogger::log(ELL_ERROR, "�޸𸮰� �����Ͽ� ������ ���� �� �� �����ϴ�.");
		return FALSE;
	}

	m_sktListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(m_sktListen == INVALID_SOCKET)
	{
		CLogger::log(ELL_ERROR, "������ ���µ� ������.");
		return FALSE;
	}

	// ���� ��Ʈ ���� �� ��Ʈ ���ε�
	if(!IPBind(m_sktListen, m_iPort, 0))
	{
		CLogger::log(ELL_ERROR, "�̹� ���� ��Ʈ�� ������ ���� ���Դϴ�.");
		return FALSE;
	}

	if(!CreateIOCP(m_iConcurrent))
	{
		CLogger::log(ELL_ERROR, "I/O �̺�Ʈ �ڵ鷯 ���� ����");
		return FALSE;
	}

	for(int i = 0 ; i < m_iMaxConnect + 1; i++)
	{
		::InitializeCriticalSection(&m_lpSendContext[i].csSTcp);
		::InitializeCriticalSection(&m_lpClientContext[i].csRTcp);
        AddClientSocket(m_sktListen, i);
	}

	AddIOCompletionPort((HANDLE) m_sktListen, 0);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ���� �ʱ�ȭ 
////////////////////////////////////////////////////////////////////////////////////////////////
void CNetIOModule::InitSocketContext(LPSOCKETCONTEXT  lpSocketContext, SOCKET sktClient, int iAddCount, int iContextType)
{
	int					zero = 0;
	LPSENDSOCKETCONTEXT lpSendContext =(LPSENDSOCKETCONTEXT) lpSocketContext->lpeovSendTcp;

	ZeroMemory(&lpSocketContext->eovRecvTcp, sizeof(EOVERLAPPED));
	ZeroMemory(&lpSendContext->eovSendTcp, sizeof(EOVERLAPPED));

	lpSocketContext->index					= iAddCount;
	lpSocketContext->eovRecvTcp.ContextType	= iContextType;
	lpSocketContext->sktNumber				= sktClient;
	lpSendContext->index					= iAddCount;
	lpSendContext->sktNumber				= sktClient;
	lpSendContext->iSTRestCnt				= 0;
	lpSocketContext->eovRecvTcp.mode = enAccepting;
	lpSendContext->lpeovRecvTcp		  = &lpSocketContext->eovRecvTcp;
	lpSendContext->eovSendTcp.mode   = enWriting;


	lpSendContext->cpSTBegin			= &lpSendContext->cSendTcpRingBuf[0];
	lpSendContext->cpSTEnd				= &lpSendContext->cSendTcpRingBuf[0];
	lpSocketContext->cpRTMark			= &lpSocketContext->cRecvTcpRingBuf[0];
	lpSocketContext->cpRTBegin			= &lpSocketContext->cRecvTcpRingBuf[0];
	lpSocketContext->cpRTEnd			= &lpSocketContext->cRecvTcpRingBuf[0];
	lpSocketContext->Statue				= 0;
	lpSocketContext->Reserve			= 0;
	lpSocketContext->pParam				= NULL;
	lpSocketContext->ContextType			= 0;


	setsockopt( sktClient, SOL_SOCKET, SO_SNDBUF, (char *) &zero, sizeof zero );
	setsockopt( sktClient, SOL_SOCKET, SO_RCVBUF, (char *) &zero, sizeof zero );

	//PrintEx("�ڵ� �ʱ�ȭ\n");


}
// ���� �ʱ�ȭ
SOCKET CNetIOModule::AddClientSocket(SOCKET sktListen, UINT iAddCount)
{

	SOCKET				sktClient;
	LPSOCKETCONTEXT		lpContext		= NULL;
	LPSENDSOCKETCONTEXT lpSendContext   = NULL;
	DWORD				dwRecvSize;



    lpContext = &m_lpClientContext[iAddCount];
	lpContext->lpeovSendTcp = (LPEOVERLAPPED) &m_lpSendContext[iAddCount];
	lpSendContext = &m_lpSendContext[iAddCount];


	sktClient = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	
	if ( sktClient != INVALID_SOCKET )
	{
		InitSocketContext(lpContext, sktClient, iAddCount, enAcceptContext);

    			
		if ( ! AcceptEx( sktListen, sktClient, lpContext->cpRTEnd,
			INITRECVSIZE, sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16, (LPDWORD) &dwRecvSize, (LPOVERLAPPED) lpContext) )
		{
			if ( GetLastError() != ERROR_IO_PENDING )
				return INVALID_SOCKET;
			
		}
	}

	else
		return INVALID_SOCKET;


	return sktClient;
}


HANDLE CNetIOModule::AddIOCompletionPort(HANDLE h, unsigned long completionKey)
{
	HANDLE hResult;

	if(h == INVALID_HANDLE_VALUE || h == 0)
		return INVALID_HANDLE_VALUE;

	hResult = CreateIoCompletionPort( h, m_hIOCP, completionKey, m_concurrent);

	if (INVALID_HANDLE_VALUE == hResult ) 
	{
		return INVALID_HANDLE_VALUE;
	}

	return 	hResult;
}




void CNetIOModule::DoIO(LPEOVERLAPPED lpEov, DWORD dwTransBytes)
{

	LPSOCKETCONTEXT			lpSockContext;
    LPSENDSOCKETCONTEXT		lpSendContext;

    switch(lpEov->mode)
	{
	case enClose:
		{
			lpSockContext = (LPSOCKETCONTEXT) lpEov;
			ClientClose(lpSockContext); 
		}
		break;

	case enConnecting:
		lpSockContext = (LPSOCKETCONTEXT) lpEov;
		lpSockContext->eovRecvTcp.bConnect = true;
		OnConnect(lpSockContext);
		PostTcpRecv(lpSockContext);
		
		break;

	case enAccepting:
        lpSockContext = (LPSOCKETCONTEXT) lpEov;

		if(m_iMaxConnect <= m_ConnectCount)
		{
			printf("���Ӽ� �ʰ�\n");
            CloseNetWork( lpSockContext); 
			AddClientSocket(m_sktListen, lpSockContext->index);
		}
		else
		{
			printf("����  ����\n");
            ClientAccept(lpSockContext);
			m_ConnectCount++;
		}

		break;

	case enReading:
		lpSockContext = (LPSOCKETCONTEXT) lpEov;

		// Ŭ���̾�Ʈ ��Ŷ ���ú� ���� ó�� 
		if(dwTransBytes == 0)
		{
            CloseNetWork(lpSockContext);
			break;
		}

        // ���ú� ���� ������ ���
		RecvTcpBufEnqueue( lpSockContext, dwTransBytes);

#ifdef _RECV_STREAM_MODE
		if(StreamRecvToGameQueue( lpSockContext ))
			
#else
        // ���� ���μ����� ����
		if(RecvToGameQueue( lpSockContext ))
#endif
		{
			// ���ú� IO �ɱ�
			PostTcpRecv( lpSockContext );
		}
		else
		{
            CloseNetWork(lpSockContext);
		}
		break;

	case enWriting:
		lpSendContext = (LPSENDSOCKETCONTEXT) lpEov;
		PostTcpReSend(lpSendContext, dwTransBytes);
		break;

	}
}


void CNetIOModule::DoIOFail(LPEOVERLAPPED lpEov)
{
    DWORD dwError;
			
	dwError = GetLastError();

	printf("IO Fail ddd = %ld\n", dwError);

	switch(dwError)
	{
	case 1221:			// ���� �õ� ���� ��Ʈ���� �ź� ������
	case 64: // �̹� ����� ��Ʈ��ũ
		ClientClose((LPSOCKETCONTEXT) lpEov);
		break;
	}

	

}



//////////////////////////////////////////////////////////////////////////////
// IOCP ���� ��� ó�� 
//////////////////////////////////////////////////////////////////////////////
void CNetIOModule::Start(void)
{
	DWORD					dwTransBytes = 0;
	ULONG					key = 0;
	LPEOVERLAPPED			lpEov;
	BOOL					result;

	m_bServerStart = false;

	if(OnCreate() == FALSE)
	{
		goto STOP;
	} // ���� ���μ����� �ʱ�ȭ �Ѵ�.

	m_bServerStart = true;

	SETLOG(ELL_INFORMATION, "Server is start");

	while(!m_bStop)
	{
        lpEov = NULL;
		key = 0;

		result = GetQueuedCompletionStatus( m_hIOCP, &dwTransBytes, (LPDWORD) &key, 
 			(LPOVERLAPPED *)&lpEov, INFINITE);

		if(key == enStop)
		{
			continue;
		}

		// ��� ���� ó�� 
		// �̹� ������ ����� ���μ��� �ϰ� ���ο� ������ ���ϰ� �����.
	

		// �̰��� ���� ���� ������ ������ ����̴�.
		if(m_ServerState)
		{
			 if(lpEov->mode == enAccepting)
			{
				continue;
			}
		}

		if ( result == FALSE ) // IOCP �����̴�.
		{
			if ( lpEov == 0 ) // 
			{

			}
			else
			{
				DoIOFail(lpEov);
			}
		}
		else
		{
			DoIO(lpEov, dwTransBytes);
		}



	} // while end

STOP:

	// �����Ѱ͵� ���� 
	for(int i = 0 ; i < m_iMaxConnect; i++)
	{
		::DeleteCriticalSection(&m_lpSendContext[i].csSTcp);
		::DeleteCriticalSection(&m_lpClientContext[i].csRTcp);

		// ���ӵ� ���� �̸�...
		if(m_lpClientContext[i].eovRecvTcp.mode == enReading)
		{
            OnClose(&m_lpClientContext[i]);
			Sleep(10);		// �ణ�� �����̰� �ʿ�
		}


		closesocket(m_lpSendContext[i].sktNumber);
	}

	CloseHandle(m_hIOCP);
	closesocket(m_sktListen);
	m_bStop			= false;

	OnStop();	// ������ ����Ǿ���.


}




//////////////////////////////////////////////////////////////////
// �ٸ� ������ ������ ���� ���� ����
/////////////////////////////////////////////////////////////////
LPSOCKETCONTEXT CNetIOModule::Connect(const char* Addr, int iPort)
{
	int							iRet;
	struct sockaddr_in			Addres;
	SOCKET						skt;
	LPSOCKETCONTEXT				lpSocketContext = NULL;
	LPSENDSOCKETCONTEXT			lpSendContext = NULL;

	DWORD dwErr;
	DWORD dwBytes;


	skt = 	WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (skt == INVALID_SOCKET)
	{
		return NULL;
	}

    Addres.sin_family = AF_INET;
	Addres.sin_addr.s_addr = inet_addr(Addr);
	Addres.sin_port = htons(iPort);

	//unsigned long nNonBlock = 1; // 1 -> non block, 0 -> block

	//ioctlsocket(skt, FIONBIO, &nNonBlock);

	DWORD dwSent;

	dwErr = WSAIoctl(skt,
                 SIO_GET_EXTENSION_FUNCTION_POINTER,
                 &GuidConnectEx,
                 sizeof(GuidConnectEx),
                 &lpfnConnectEx,
                 sizeof(lpfnConnectEx),
                 &dwBytes,
                 NULL,
                 NULL); 
	if(dwErr == SOCKET_ERROR)
	{
		return NULL;

	}


	
//	iRet = connect(skt, , sizeof(struct sockaddr));
	
	lpSocketContext = new SOCKETCONTEXT;

	if(lpSocketContext == NULL)
		return NULL;

	lpSendContext = new SENDSOCKETCONTEXT;
	
	if(lpSendContext == NULL)
	{
		delete lpSocketContext;
        return NULL;
	}
	

	::InitializeCriticalSection(&lpSendContext->csSTcp);
	::InitializeCriticalSection(&lpSocketContext->csRTcp);

	lpSocketContext->lpeovSendTcp = (LPEOVERLAPPED) lpSendContext;

	InitSocketContext(lpSocketContext, skt, NULL, enConnectContext);
	lpSocketContext->eovRecvTcp.mode = enConnecting;	

	AddIOCompletionPort((HANDLE) skt, 0);

	int zero = 0;
	struct sockaddr_in sockaddr;

	// �������ּ� ����
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr =  htonl(0);
	sockaddr.sin_port = htons(0);

	// �����ּҿ� ��Ʈ�� ���ε�
	if(bind(skt,(struct sockaddr *)&sockaddr, sizeof(sockaddr))
		==	SOCKET_ERROR) {
		return FALSE;
	}



	iRet = lpfnConnectEx(skt, (struct sockaddr FAR *)&Addres, sizeof(struct sockaddr), lpSocketContext->cpRTEnd, INITRECVSIZE, 
		&dwSent, (LPOVERLAPPED) lpSocketContext);



	


	
	//PostTcpRecv(lpSocketContext);

	return lpSocketContext;
}




/////////////////////////////////////////////////////////////////////
// IOCP�� ���ú� ���
/////////////////////////////////////////////////////////////////////
void CNetIOModule::PostTcpRecv( LPSOCKETCONTEXT lpSockContext )
{
	WSABUF					wsaBuf;
	DWORD					dwFlags;
	DWORD					dwTransBytes;
	int						iResult;

	dwFlags = 0;
	dwTransBytes = 0;
	lpSockContext->eovRecvTcp.mode = enReading;
	wsaBuf.buf = lpSockContext->cpRTEnd;
	wsaBuf.len = PACKET_DATA_SIZE_MAX;
		
	iResult = WSARecv( lpSockContext->sktNumber, &wsaBuf, 1, 
		&dwTransBytes, (LPDWORD)lpSockContext, 
		(OVERLAPPED *)&lpSockContext->eovRecvTcp, NULL );
	
	if( iResult == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
	{
        OnClose(lpSockContext);
	}
}

/////////////////////////////////////////////////////////////////////
// Ŭ���̾�Ʈ ���� ó�� 
/////////////////////////////////////////////////////////////////////
void CNetIOModule::ClientAccept(LPSOCKETCONTEXT lpSockContext)
{
	sockaddr_in					*plocal = 0,
										*premote = 0;
	int									locallen, 
  	 									remotelen;


	GetAcceptExSockaddrs( lpSockContext->cpRTEnd, INITRECVSIZE,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, (sockaddr **) &plocal, &locallen,
		(sockaddr **) &premote, &remotelen );

	
	CopyMemory( &lpSockContext->local, plocal, sizeof(sockaddr_in) );
	CopyMemory( &lpSockContext->peer, premote, sizeof(sockaddr_in) );

	GetLocalTime(&lpSockContext->ConnectTime);
	lpSockContext->ConnectTick = GetTickCount();

	setsockopt( lpSockContext->sktNumber, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(char *) &m_sktListen, sizeof(m_sktListen) );

	AddIOCompletionPort((HANDLE) lpSockContext->sktNumber, 0);
        
	PostTcpRecv(lpSockContext);
	
	OnAccept(lpSockContext);
	
}


BOOL CNetIOModule::RecvToGameQueue( LPSOCKETCONTEXT lpSockContext )
{
	INT64					iRestSize,
							iExtra;
	UINT					iDivideHader,
							iDivideBody;

	TCommon*				pCommonPacket;



	iRestSize = lpSockContext->cpRTEnd - lpSockContext->cpRTMark;
	if( iRestSize < 0 ) iRestSize += MAX_BUFF_SIZE;


	// ���� ����� ��� ������ ����. ������
	if( iRestSize < sizeof( TCommon ) ) return TRUE;

	int iCount = 0;

	while( 1 )
	{
		iExtra = lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE - lpSockContext->cpRTMark;
		
		// ����� �߷� ����
		if( iExtra < sizeof( TCommon ) )
		{
			iDivideHader = (UINT) (sizeof( TCommon ) - iExtra);

           CopyMemory(lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE, lpSockContext->cRecvTcpRingBuf, iDivideHader);
		}

		pCommonPacket = (TCommon*) lpSockContext->cpRTMark;

		// �ٵ� �߷� ����
		if(iExtra < pCommonPacket->Len)
		{
			iDivideBody =(UINT) (lpSockContext->cpRTEnd - lpSockContext->cRecvTcpRingBuf);
			CopyMemory(lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE, lpSockContext->cRecvTcpRingBuf, iDivideBody);
		}

		// ��Ŷ�� ���� �پȵ�� �Դ� 
		if( iRestSize < pCommonPacket->Len ) return TRUE;

		if(pCommonPacket->Len <= 0) return FALSE;
		
		OnRead(lpSockContext, pCommonPacket->Ctrl, lpSockContext->cpRTMark + sizeof(TCommon), pCommonPacket->Len);

		lpSockContext->cpRTMark += pCommonPacket->Len;

		// �Ϲ��۸� �� �� �Ҵ�.
		if( lpSockContext->cpRTMark >= lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE )
			lpSockContext->cpRTMark -= MAX_BUFF_SIZE;

		iRestSize -= pCommonPacket->Len;

		if( iRestSize < sizeof( TCommon ) ) return TRUE;
 	}
}

void CNetIOModule::RecvTcpBufEnqueue( LPSOCKETCONTEXT lpSockContext, int iPacketSize )
{
	INT64						iExtra;

	// ��Ŷ�� ���۸� �ʰ� �ߴ°� ? 
	iExtra = lpSockContext->cpRTEnd + iPacketSize - lpSockContext->cRecvTcpRingBuf - MAX_BUFF_SIZE;
	if( iExtra >= 0 )
	{
		CopyMemory( lpSockContext->cRecvTcpRingBuf, lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE, (UINT) iExtra );
		lpSockContext->cpRTEnd = lpSockContext->cRecvTcpRingBuf + iExtra;

	}
	else
	{
		lpSockContext->cpRTEnd += iPacketSize;

	}

}

void CNetIOModule::PostTcpReSend( LPSENDSOCKETCONTEXT lpSendContext, int iTransferred )
{
	WSABUF			wsaBuf;
	DWORD			dwSent;
	int				iResult, iRestSize;
	INT64			iExtra;

	
	EnterCriticalSection( &lpSendContext->csSTcp );

	// dequeue completion size 
	iExtra = ((lpSendContext->cpSTBegin + iTransferred) - lpSendContext->cSendTcpRingBuf) - MAX_BUFF_SIZE;
	if( iExtra >= 0 ) 
	{
		lpSendContext->cpSTBegin = lpSendContext->cSendTcpRingBuf + iExtra;
	}
	else 
	{
		lpSendContext->cpSTBegin += iTransferred;
	}

	// set rest size, and get "is rest exist" 
	lpSendContext->iSTRestCnt -= iTransferred;

	iRestSize = lpSendContext->iSTRestCnt;

	if( iRestSize > 0 )
	{
		if( iRestSize > MAX_SENDPACKET_SIZE ) iRestSize = MAX_SENDPACKET_SIZE;

		iExtra = lpSendContext->cSendTcpRingBuf + MAX_BUFF_SIZE - lpSendContext->cpSTBegin;

		if( iExtra < iRestSize )
		{
			CopyMemory( lpSendContext->cSendTcpRingBuf + MAX_BUFF_SIZE, lpSendContext->cSendTcpRingBuf,
				(size_t) (iRestSize - iExtra));
		}

//		 send packet 
		wsaBuf.buf = lpSendContext->cpSTBegin;
		wsaBuf.len = iRestSize;

		iResult = WSASend( lpSendContext->sktNumber, &wsaBuf, 1, &dwSent,
			0, (OVERLAPPED *) &lpSendContext->eovSendTcp, NULL);
		if( iResult == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
		{
            NetClose(( LPSOCKETCONTEXT) lpSendContext->lpeovRecvTcp);
		}

	}

	LeaveCriticalSection( &lpSendContext->csSTcp );
}



void CNetIOModule::SendPacket(LPSOCKETCONTEXT lpSockContext, BYTE Ctrl, const char *cpPacket, int iSize)
{
	LPSENDSOCKETCONTEXT			lpSendContext;
	WSABUF						wsaBuf;
	DWORD						dwSent;
	int							iResult, iExtra, iSendNow;

	lpSendContext = (LPSENDSOCKETCONTEXT) lpSockContext->lpeovSendTcp;
	
	TCommonPacket SendPacket;
	SendPacket.Common.Ctrl= Ctrl; 
	SendPacket.Common.Len = iSize + sizeof(TCommon);

	EnterCriticalSection( &lpSendContext->csSTcp );

	CopyMemory(lpSendContext->cpSTEnd, &SendPacket, sizeof(TCommon));
	CopyMemory(&lpSendContext->cpSTEnd[sizeof(TCommon)], cpPacket, iSize);

	iExtra = (int) ((lpSendContext->cpSTEnd + SendPacket.Common.Len) - lpSendContext->cSendTcpRingBuf) - MAX_BUFF_SIZE;
	if( iExtra >= 0 )
	{
		CopyMemory( lpSendContext->cSendTcpRingBuf, lpSendContext->cSendTcpRingBuf + MAX_BUFF_SIZE, iExtra );
		lpSendContext->cpSTEnd = lpSendContext->cSendTcpRingBuf + iExtra;
	}
	else 
	{
		lpSendContext->cpSTEnd += SendPacket.Common.Len;
	}


	lpSendContext->iSTRestCnt += SendPacket.Common.Len;

	iSendNow = ( lpSendContext->iSTRestCnt == SendPacket.Common.Len ) ? TRUE : FALSE;

	LeaveCriticalSection( &lpSendContext->csSTcp );

	// ���带 �� �� ������ ���� ���� ������.
	if( iSendNow )
	{
		wsaBuf.buf = lpSendContext->cpSTBegin;
		wsaBuf.len = SendPacket.Common.Len;
		iResult = WSASend( lpSockContext->sktNumber, &wsaBuf, 1, &dwSent,
				0, (OVERLAPPED *) &lpSendContext->eovSendTcp, NULL );
		if( iResult == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
		{
			NetClose(lpSockContext);
		}

	}
	else
	{
		if( lpSendContext->iSTRestCnt > MAX_BUFF_SIZE )
		{
			// ���۸� �ʰ� �ߴ�. 
			SETLOG(ELL_INFORMATION,"%d, ���� �ʰ�...................\n", lpSendContext->index );
			NetClose(lpSockContext);
		}
	}
}



void CNetIOModule::Stop(void)
{
	ULONG pkey = enStop;
	m_bStop = true;
	WakeUpIOCP(NULL, pkey);
}

void CNetIOModule::Pause(void)
{
	m_ServerState = true;
}

void CNetIOModule::Continue(void)
{
	m_ServerState = false;
}


bool CNetIOModule::GetServerStatue(void)
{
	return m_ServerState;
}


void CNetIOModule::CloseNetWork( LPSOCKETCONTEXT lpSockContext )
{
	lpSockContext->eovRecvTcp.mode = enClose;
	WakeUpIOCP(lpSockContext, 0xFFFFFFF);
}



void CNetIOModule::ReuseSocket( LPSOCKETCONTEXT lpSockContext)
{
    AddClientSocket(m_sktListen, lpSockContext->index);
	m_ConnectCount--;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ����� �������� ��ȣ�� ������.
/////////////////////////////////////////////////////////////////////////////////////////
void CNetIOModule::ClientClose( LPSOCKETCONTEXT lpSockContext)
{

	linger li = {0, 0};
	li.l_onoff = 1; // SO_LINGER, timeout = 0
	setsockopt( lpSockContext->sktNumber, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof li );
	closesocket( lpSockContext->sktNumber );

	if(lpSockContext->eovRecvTcp.ContextType == enConnectContext)
	{
		printf("���� ��Ʋ ���� \n");
		OnClose(lpSockContext);
	}
	else
	{
		printf("���� �ڵ� ���� �� �Ҵ� \n");
		OnClose(lpSockContext);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////
// ������ ���� ���ӿ� ������ ������
/////////////////////////////////////////////////////////////////////////////////////////
void CNetIOModule::ConnectClose( LPSOCKETCONTEXT lpSockContext)
{
	closesocket(lpSockContext->sktNumber);
	delete lpSockContext->lpeovSendTcp;
	delete lpSockContext;
}
