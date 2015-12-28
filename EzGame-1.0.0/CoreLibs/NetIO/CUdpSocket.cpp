#include "StdAfx.h"
#include <process.h>
#include "Context.h"
#include "CUdpSocket.h"


CUdpSocket::CUdpSocket()
{
	m_hThread = NULL;
}

CUdpSocket::~CUdpSocket()
{	
	CloseMainThread();	
}

bool CUdpSocket::start(int nPort)
{	
	m_nServerPort = nPort;
	
	return (BeginMainThread());	
}

void CUdpSocket::send(const char* IP, int iPort, const char * pPacket, int iSize)
{
	SOCKADDR_IN servAddr;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(IP);
	servAddr.sin_port=htons(iPort);

	SOCKET hSock=socket(PF_INET, SOCK_DGRAM, 0);

	if(hSock == INVALID_SOCKET)
	{
		return;
	}

	sendto(hSock, pPacket, iSize , 0,
		(struct sockaddr*)&servAddr, sizeof(servAddr));


	closesocket(hSock);
}

bool CUdpSocket::BeginMainThread()
{
	if (m_bRunThread == true) return false;

	m_hThread = (HANDLE) _beginthreadex(NULL, NULL, MainThreadProc, this, NULL, (UINT *)&m_dwThreadID);
	if (m_hThread == INVALID_HANDLE_VALUE) return false;

	Sleep(1);
	return true;
}

unsigned int __stdcall CUdpSocket::MainThreadProc(void *lpParam)
{
	CUdpSocket * pThis = (CUdpSocket *)lpParam;
	pThis->run();

	return 0;
}

void CUdpSocket::run()
{	
	SOCKADDR_IN servAddr;
	SOCKADDR_IN clntAddr;	
	int clntAddrSize;
	int strLen;
	char message[MAX_BUFF_SIZE];

	m_hSock=socket(PF_INET, SOCK_DGRAM, 0);

	if(m_hSock == INVALID_SOCKET)
	{
		return;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(m_nServerPort);

	if(bind(m_hSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
	{
		return;
	}

	m_bRunThread = true;

	while(m_bRunThread == true)
	{

		clntAddrSize=sizeof(clntAddr);
		strLen=recvfrom(m_hSock, message, MAX_BUFF_SIZE , 0,
			(struct sockaddr*)&clntAddr, &clntAddrSize);

		OnRecv(message, strLen);

	}
	closesocket(m_hSock);
	Sleep(1);
}





void CUdpSocket::CloseMainThread()
{
	if (m_bRunThread == false) return;

	m_bRunThread = false;

	closesocket(m_hSock);
	if(m_hThread != NULL) CloseHandle(m_hThread);
}
