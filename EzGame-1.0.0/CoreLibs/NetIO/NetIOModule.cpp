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
	// 윈속 초기화
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
// 리슨소켓과 포트를 바인딩 한다.
//////////////////////////////////////////////////////////////////////
BOOL CNetIOModule::IPBind(SOCKET sktListen, int nPort, int nBacklog)
{
	int zero = 0;
	struct sockaddr_in sockaddr;

	// 아이피주소 셋팅
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	sockaddr.sin_port = htons(nPort);

	// 소켓주소와 포트를 바인딩
	if(bind(sktListen,(struct sockaddr *)&sockaddr, sizeof(sockaddr))
		==	SOCKET_ERROR) {
		return FALSE;
	}

	// 클라이언트 접속 대기 
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
		CLogger::log(ELL_ERROR, "메모리가 부족하여 서버를 실핼 할 수 없습니다.");
		return FALSE;
	}

	m_sktListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if(m_sktListen == INVALID_SOCKET)
	{
		CLogger::log(ELL_ERROR, "소켓을 여는데 실패함.");
		return FALSE;
	}

	// 서버 포트 개설 및 포트 바인딩
	if(!IPBind(m_sktListen, m_iPort, 0))
	{
		CLogger::log(ELL_ERROR, "이미 같은 포트로 서버가 실행 중입니다.");
		return FALSE;
	}

	if(!CreateIOCP(m_iConcurrent))
	{
		CLogger::log(ELL_ERROR, "I/O 이벤트 핸들러 생성 실패");
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
// 소켓 초기화 
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

	//PrintEx("핸들 초기화\n");


}
// 소켓 초기화
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
			printf("접속수 초과\n");
            CloseNetWork( lpSockContext); 
			AddClientSocket(m_sktListen, lpSockContext->index);
		}
		else
		{
			printf("접속  성공\n");
            ClientAccept(lpSockContext);
			m_ConnectCount++;
		}

		break;

	case enReading:
		lpSockContext = (LPSOCKETCONTEXT) lpEov;

		// 클라이언트 패킷 리시브 수신 처리 
		if(dwTransBytes == 0)
		{
            CloseNetWork(lpSockContext);
			break;
		}

        // 리시브 버퍼 포인터 계산
		RecvTcpBufEnqueue( lpSockContext, dwTransBytes);

#ifdef _RECV_STREAM_MODE
		if(StreamRecvToGameQueue( lpSockContext ))
			
#else
        // 게임 프로세스로 전달
		if(RecvToGameQueue( lpSockContext ))
#endif
		{
			// 리시브 IO 걸기
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
	case 1221:			// 접속 시도 오류 네트웍이 거부 당했음
	case 64: // 이미 종료된 네트워크
		ClientClose((LPSOCKETCONTEXT) lpEov);
		break;
	}

	

}



//////////////////////////////////////////////////////////////////////////////
// IOCP 응답 대기 처리 
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
	} // 서버 프로세스를 초기화 한다.

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

		// 대기 상태 처리 
		// 이미 접속한 사람은 프로세스 하고 새로운 접속을 못하게 만든다.
	

		// 이것은 서버 접속 중지를 내리는 명령이다.
		if(m_ServerState)
		{
			 if(lpEov->mode == enAccepting)
			{
				continue;
			}
		}

		if ( result == FALSE ) // IOCP 에러이다.
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

	// 생성한것들 해제 
	for(int i = 0 ; i < m_iMaxConnect; i++)
	{
		::DeleteCriticalSection(&m_lpSendContext[i].csSTcp);
		::DeleteCriticalSection(&m_lpClientContext[i].csRTcp);

		// 접속된 상태 이면...
		if(m_lpClientContext[i].eovRecvTcp.mode == enReading)
		{
            OnClose(&m_lpClientContext[i]);
			Sleep(10);		// 약간의 딜레이가 필요
		}


		closesocket(m_lpSendContext[i].sktNumber);
	}

	CloseHandle(m_hIOCP);
	closesocket(m_sktListen);
	m_bStop			= false;

	OnStop();	// 서버가 종료되었다.


}




//////////////////////////////////////////////////////////////////
// 다른 서버와 연결을 위한 소켓 개설
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

	// 아이피주소 셋팅
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr =  htonl(0);
	sockaddr.sin_port = htons(0);

	// 소켓주소와 포트를 바인딩
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
// IOCP에 리시브 등록
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
// 클라이언트 접속 처리 
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


	// 받은 사이즈가 헤더 사이즈 보다. 작으면
	if( iRestSize < sizeof( TCommon ) ) return TRUE;

	int iCount = 0;

	while( 1 )
	{
		iExtra = lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE - lpSockContext->cpRTMark;
		
		// 헤더가 잘려 있음
		if( iExtra < sizeof( TCommon ) )
		{
			iDivideHader = (UINT) (sizeof( TCommon ) - iExtra);

           CopyMemory(lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE, lpSockContext->cRecvTcpRingBuf, iDivideHader);
		}

		pCommonPacket = (TCommon*) lpSockContext->cpRTMark;

		// 바디가 잘려 있음
		if(iExtra < pCommonPacket->Len)
		{
			iDivideBody =(UINT) (lpSockContext->cpRTEnd - lpSockContext->cRecvTcpRingBuf);
			CopyMemory(lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE, lpSockContext->cRecvTcpRingBuf, iDivideBody);
		}

		// 패킷이 아직 다안들어 왔다 
		if( iRestSize < pCommonPacket->Len ) return TRUE;

		if(pCommonPacket->Len <= 0) return FALSE;
		
		OnRead(lpSockContext, pCommonPacket->Ctrl, lpSockContext->cpRTMark + sizeof(TCommon), pCommonPacket->Len);

		lpSockContext->cpRTMark += pCommonPacket->Len;

		// 일버퍼를 다 돌 았다.
		if( lpSockContext->cpRTMark >= lpSockContext->cRecvTcpRingBuf + MAX_BUFF_SIZE )
			lpSockContext->cpRTMark -= MAX_BUFF_SIZE;

		iRestSize -= pCommonPacket->Len;

		if( iRestSize < sizeof( TCommon ) ) return TRUE;
 	}
}

void CNetIOModule::RecvTcpBufEnqueue( LPSOCKETCONTEXT lpSockContext, int iPacketSize )
{
	INT64						iExtra;

	// 패킷이 버퍼를 초과 했는가 ? 
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

	// 센드를 할 수 있으면 지금 당장 보낸다.
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
			// 버퍼를 초과 했다. 
			SETLOG(ELL_INFORMATION,"%d, 버퍼 초과...................\n", lpSendContext->index );
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
// 연결된 서버에게 신호를 보낸다.
/////////////////////////////////////////////////////////////////////////////////////////
void CNetIOModule::ClientClose( LPSOCKETCONTEXT lpSockContext)
{

	linger li = {0, 0};
	li.l_onoff = 1; // SO_LINGER, timeout = 0
	setsockopt( lpSockContext->sktNumber, SOL_SOCKET, SO_LINGER, (char *) &li, sizeof li );
	closesocket( lpSockContext->sktNumber );

	if(lpSockContext->eovRecvTcp.ContextType == enConnectContext)
	{
		printf("접속 핸틀 삭제 \n");
		OnClose(lpSockContext);
	}
	else
	{
		printf("수락 핸들 삭제 재 할당 \n");
		OnClose(lpSockContext);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////
// 서버대 서버 접속용 소켓이 해제됨
/////////////////////////////////////////////////////////////////////////////////////////
void CNetIOModule::ConnectClose( LPSOCKETCONTEXT lpSockContext)
{
	closesocket(lpSockContext->sktNumber);
	delete lpSockContext->lpeovSendTcp;
	delete lpSockContext;
}
