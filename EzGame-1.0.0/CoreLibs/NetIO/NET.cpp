/*=========== (C) Copyright 2003 SUNWOO Entertainment All rights reserved. ===========
//
// Purpose: 클라이언트 네트웍 라이브러리
//
// $Workfile: NET.cpp
// $Date:         $
// $NoKeywords: $
=============================================================================*/

#include "stdafx.h"
#include "NET.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <assert.h>


// 시간 정보와 로그 메세지를 출력
void OutputDebugLogFileA(const char *FileName,const char *Message)
{

	FILE *fp;
	
	fp = fopen(FileName,"a+");

	if(fp == NULL)
	{
		fp = fopen(FileName,"w");
	}
	
	fwrite(Message,sizeof(char),strlen(Message),fp);
	
	fclose(fp);
}

void _cdecl LogFile(char* filename, const char *format,...)
{

    char Buffer[1024];
	
	ZeroMemory(Buffer, 1024);
	va_list ArgPtr;

	va_start( ArgPtr, format );

	
	_vsnprintf( Buffer, 1024, format, ArgPtr );

    va_end( ArgPtr );
	
	OutputDebugLogFileA(filename,Buffer);


}

bool InitWinsock();
int Recv(NETHANDLE hNet, char *Packet);

int CryptoIncodePacket(BYTE* inData, BYTE* OutData, int nSize)
{
	int		i;
	
	UINT16	temp = 0;
	
	for (i = 0; i < nSize; i++)
	{
        OutData[i] = ~inData[i];
	}


	return 0;

}

void CryptoIncodePacket(BYTE* inData, int nSize)
{
	int		i;
	
	UINT16	temp = 0;
	
	for (i = 0; i < nSize; i++)
	{
        inData[i] = ~inData[i];
	}


}


int CryptoDecodePacket(BYTE*inData, BYTE* OutData, int nSize)
{

	int		i;
	
	UINT16	temp = 0;
	
	for (i = 0; i < nSize; i++)
	{
        OutData[i] = ~inData[i];
	}


	return 0;
}

void CryptoDecodePacket(BYTE*inData, int nSize)
{

	int		i;
	
	UINT16	temp = 0;
	
	for (i = 0; i < nSize; i++)
	{
        inData[i] = ~inData[i];
	}

}



// 버퍼에 보낼 자료가 남아 있다면 보낸다.
bool PostReSend( NETHANDLE hNet, int iTransferred)
{
    int						iExtra;

	NETPROSET*				pNetProset = (NETPROSET*) hNet;
	SendEnQueueTcp*			lpSendBuff = &pNetProset->m_SendBuff;

	
	iExtra = lpSendBuff->cpSTMark + iTransferred - lpSendBuff->lpRingBuff - MAX_BUFF_SIZE;
	if( iExtra >= 0 ) 
	{
		lpSendBuff->cpSTMark = lpSendBuff->lpRingBuff + iExtra;
	}
	else 
	{
		lpSendBuff->cpSTMark += iTransferred;
	}


	lpSendBuff->iSTRestCnt -= iTransferred;

	return true;

}
//////////////////////////////////////////////////////////////////////////////
// 윈속을 초기화 
/////////////////////////////////////////////////////////////////////////////
bool InitWinsock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 

	wVersionRequested = MAKEWORD( 2, 2 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	
	
	if ( err != 0 ) 
	{
		return false;
	} 

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return false;
	} 
	return true;
}


// 체크섬

DWORD GetChkSum(BYTE *ptr, WORD size)
{
    DWORD sum = 0;
    WORD  *wPtr = (WORD *)ptr;
	WORD  i;

    for(i=0; i<size/2; ++i)
       sum += wPtr[i];

    sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}


void RecvTcpBufEnqueue( NETHANDLE hNet, int iPacketSize )
{
	int						iExtra;
	NETPROSET*				pNetProset		= (NETPROSET*) hNet;
	BufEnQueueTcp*			lpEnQueueTcp	= &pNetProset->m_RingBuff;


	/* packet is already stored */
	iExtra = lpEnQueueTcp->cpRTEnd + iPacketSize - lpEnQueueTcp->lpRingBuff - MAX_BUFF_SIZE;

	if( iExtra >= 0 )
	{
		CopyMemory( lpEnQueueTcp->lpRingBuff, lpEnQueueTcp->lpRingBuff + MAX_BUFF_SIZE, iExtra );
		lpEnQueueTcp->cpRTEnd = lpEnQueueTcp->lpRingBuff + iExtra;
	}
	else
	{
		lpEnQueueTcp->cpRTEnd += iPacketSize;
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////////////
bool InitNetwork()
{
    if(!InitWinsock())
	{
		return false;
	}

	return true;
 
}

void CloseNetwork()
{
	WSACleanup();
}



////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////
NETHANDLE Connect(char *Addr, int port)
{
	int							nRet;
	struct sockaddr_in			Addres;
	SOCKET						skt;
	NETPROSET*					pNetProsetConnect;


	Addres.sin_family = AF_INET;
	Addres.sin_addr.s_addr = inet_addr(Addr);
	Addres.sin_port = htons(port);


	skt = 	WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (skt == INVALID_SOCKET)
	{
		return NULL;
	}

	nRet = connect(skt, (struct sockaddr FAR *)&Addres, sizeof(struct sockaddr));

	if(nRet == SOCKET_ERROR) 
	{
        return NULL;
	}	

	pNetProsetConnect = new NETPROSET;


	if(pNetProsetConnect == NULL)
		return NULL;


	pNetProsetConnect->m_RingBuff.cpRTEnd = pNetProsetConnect->m_RingBuff.lpRingBuff;
	pNetProsetConnect->m_RingBuff.cpRTMark = pNetProsetConnect->m_RingBuff.cpRTEnd;
	
	pNetProsetConnect->m_SendBuff.cpSTEnd = pNetProsetConnect->m_SendBuff.lpRingBuff;
	pNetProsetConnect->m_SendBuff.cpSTMark = pNetProsetConnect->m_SendBuff.cpSTEnd;
	pNetProsetConnect->m_SendBuff.iSTRestCnt = 0;
	pNetProsetConnect->socket	= skt;

	return pNetProsetConnect;
}


void Disconnet(NETHANDLE hNet)
{

	LINGER LingerStruct;
	LingerStruct.l_onoff  = 1;
	LingerStruct.l_linger = 100;


	NETPROSET*  pNetProset = (NETPROSET*) hNet;

	if(pNetProset == NULL)
		return;

	setsockopt(pNetProset->socket, SOL_SOCKET, SO_LINGER,
					(char*)&LingerStruct, sizeof(LingerStruct));


	shutdown(pNetProset->socket, SD_SEND);
	
	closesocket(pNetProset->socket);

	delete pNetProset;
}


////////////////////////////////////////////////////////////////
// 패킷을 보낸다.
//////////////////////////////////////////////////////////////
bool Send(NETHANDLE hNet, char *data, int size)
{
	int			nResult = 0;
	PACKET		Packet;
	int			iPacketSize = size + PACKET_OTHER_SIZE;
	int			iSendCount = 0;
	int			iExtra;

	NETPROSET*  pNetProset = (NETPROSET*) hNet;
	SendEnQueueTcp*  lpSendBuff = &pNetProset->m_SendBuff;




	if(lpSendBuff->iSTRestCnt + size > MAX_BUFF_SIZE)
    	return false;

	// 전송할 패킷을 만든다.
	Packet.stx = STX;
	Packet.key = GetChkSum((BYTE*) data, size);
	Packet.len = size;
	memcpy(Packet.Data, data,size);
	Packet.Data[size] = ETX;

	CryptoIncodePacket((BYTE *) &Packet, (BYTE *)lpSendBuff->cpSTEnd, size + PACKET_OTHER_SIZE);

	iExtra = lpSendBuff->cpSTEnd + iPacketSize - lpSendBuff->lpRingBuff - MAX_BUFF_SIZE;
	
	// 버퍼를 초과 했다면 앞쪽으로 복사 한다.
	if( iExtra >= 0 )
	{
		CopyMemory( lpSendBuff->lpRingBuff, lpSendBuff->lpRingBuff + MAX_BUFF_SIZE, iExtra );
		lpSendBuff->cpSTEnd = lpSendBuff->lpRingBuff + iExtra;
	}
	else 
	{
		lpSendBuff->cpSTEnd += iPacketSize;
	}

	lpSendBuff->iSTRestCnt += iPacketSize;

	return true;
}


// 현재 수신버퍼의 크기를 알아온다. (랜카드)
long GetRecvsize(NETHANDLE hNet)
{
	u_long tmp_long = 0;
	fd_set  fdRead;
	NETPROSET*		pNetProset = (NETPROSET*) hNet;

	timeval time;
	time.tv_sec  = 0;
	time.tv_usec = 1;

	FD_ZERO(&fdRead);
	FD_SET(pNetProset->socket, &fdRead);


	if (ioctlsocket(pNetProset->socket, FIONREAD, &tmp_long) == SOCKET_ERROR ) 
	{
		return NETMSG_SOCKETERR;
	}

	FD_CLR(pNetProset->socket, &fdRead);

	return tmp_long;
}



int Getpacket(NETHANDLE hNet, char *Packet)
{
	short					iBodySize;
	int						iExtra,
							iRestSize;
	NETPROSET*				pNetProset		= (NETPROSET*) hNet;
	BufEnQueueTcp*			lpEnQueueTcp	= &pNetProset->m_RingBuff;


	// 이전에 표시된 위치와 전송받은 데이터 실제 크기를 구한다.
	iRestSize = lpEnQueueTcp->cpRTEnd - lpEnQueueTcp->cpRTMark;

	if(iRestSize < 0)		// 바디가 잘린경우
	{
		int iDivideBody = lpEnQueueTcp->cpRTEnd - lpEnQueueTcp->lpRingBuff;
		CopyMemory(lpEnQueueTcp->lpRingBuff + MAX_BUFF_SIZE, lpEnQueueTcp->lpRingBuff, iDivideBody);
		iRestSize += MAX_BUFF_SIZE;
	}


	// 헤더를 도달하지 않았다.
	if(iRestSize < HEADERSIZE)
	{
		return 0;
	}

	iExtra = lpEnQueueTcp->lpRingBuff + MAX_BUFF_SIZE - lpEnQueueTcp->cpRTMark;

	if( iExtra < sizeof( HEADERSIZE ) )
	{
		for(int i = 0; i < iExtra; i++)
            *(lpEnQueueTcp->lpRingBuff + MAX_BUFF_SIZE + i) = *lpEnQueueTcp->lpRingBuff + i;
	}


	/* 잘못된 패킷이다.*/
	if(lpEnQueueTcp->cpRTMark[0] != STX)
		return -1;

	// 패킷의 길이를 구한다.
	CopyMemory( &iBodySize, &lpEnQueueTcp->cpRTMark[1], sizeof( short ) );

	// 디버그
	UINT16  PacketType;

	CopyMemory(&PacketType, &lpEnQueueTcp->cpRTMark[7], sizeof(UINT16));
	

	////////////////

	// 패킷이 아직 다 들어 오지 않았다.
	if(iRestSize < iBodySize + HEADERSIZE + 1)
	{
		return 0;
	}


	CopyMemory(Packet, &lpEnQueueTcp->cpRTMark[7], iBodySize);

	if( iExtra <= iBodySize + HEADERSIZE + 1)
	{
		/* recursive at body */
		CopyMemory( lpEnQueueTcp->lpRingBuff + MAX_BUFF_SIZE, lpEnQueueTcp->lpRingBuff, 
			(iBodySize + HEADERSIZE + 1) - iExtra );
		lpEnQueueTcp->cpRTMark = lpEnQueueTcp->lpRingBuff + iBodySize + HEADERSIZE + 1 - iExtra;
	}
	else
	{
		lpEnQueueTcp->cpRTMark = lpEnQueueTcp->cpRTMark + iBodySize + HEADERSIZE + 1;
	}

	return iBodySize;
}

int CopyBuffFromNetBuff(NETHANDLE hNet)
{
	timeval				timeout;
	int					iRecvBuffSize = 0;
	NETPROSET*			pNetProset	= (NETPROSET*) hNet;
	fd_set				readfds;
	int					iResult;
	int					iRecvSize;

	if(pNetProset->m_RingBuff.cpRTEnd < pNetProset->m_RingBuff.cpRTMark)
	{
		iRecvBuffSize = (pNetProset->m_RingBuff.cpRTMark - pNetProset->m_RingBuff.cpRTEnd) - 1;
	}
	else
	{
		// 받을 사이즈계산
		iRecvBuffSize = pNetProset->m_RingBuff.lpRingBuff + MAX_BUFF_SIZE - pNetProset->m_RingBuff.cpRTEnd;
		iRecvBuffSize += pNetProset->m_RingBuff.cpRTMark - pNetProset->m_RingBuff.lpRingBuff;
		iRecvBuffSize -= 1;
	}

	if(iRecvBuffSize > MAX_RECV_SIZE) iRecvBuffSize = MAX_RECV_SIZE;

	if(iRecvBuffSize == 0)
		return 1;


	
	
	ZeroMemory(&timeout, sizeof(timeval));
	timeout.tv_usec = 1;
	FD_ZERO(&readfds);

	FD_SET(pNetProset->socket, &readfds);
	

	iResult = select(0, &readfds, (fd_set *)0, (fd_set *)0, &timeout);	

	if(iResult)
	{
        if(FD_ISSET(pNetProset->socket, &readfds))					//  넷트워크로 부터 패킷을 받음
		{
			iRecvSize = recv(pNetProset->socket, pNetProset->m_RingBuff.cpRTEnd, iRecvBuffSize, 0);

			if(iRecvSize == -1)
			{
				return -1;
			}
	
			if(iRecvSize == 0)
			{
				return -1;
			}
			CryptoDecodePacket((BYTE *)pNetProset->m_RingBuff.cpRTEnd, iRecvSize);
			RecvTcpBufEnqueue( hNet, iRecvSize );
		}
	}

	else if(iResult < 0)			// 네트웍이 종료됨
	{
		FD_CLR(pNetProset->socket, &readfds);
		return -1;
	}

	FD_CLR(pNetProset->socket, &readfds);

	return iResult;
}

///////////////////////////////////////////////////////////////////////////////////////
// 네트웍크의 상태를 점검한다.
//////////////////////////////////////////////////////////////////////////////////////
int NetState(NETHANDLE hNet)
{

	int							irecvsize		= 0;
	int							iPacketSize		= 0;
	int							iResult;

	NETPROSET*			pNetProset	= (NETPROSET*) hNet;
	SendEnQueueTcp*		lpSendBuff  = &pNetProset->m_SendBuff;

	int iRestSize;


	timeval				timeout;
	fd_set				writefds;
	FD_ZERO(&writefds);
	FD_SET(pNetProset->socket, &writefds);

	ZeroMemory(&timeout, sizeof(timeval));
	timeout.tv_usec = 1;

	iResult = select(0, (fd_set *)0, &writefds, (fd_set *)0, &timeout);	

	if(FD_ISSET(pNetProset->socket, &writefds))					//  접속 상태인지 판단
	{
        
        // 보내기 버퍼 점검
		// 보내기 버퍼에 보낼 자료가 있는가 ?
		int iSendNow;

		iSendNow = ( lpSendBuff->iSTRestCnt > 0 ) ? TRUE : FALSE;

		// 보내기 버퍼에 자료가 있어 보내야 한다.
		if(iSendNow)
		{
            iResult = send(pNetProset->socket, (const char*) lpSendBuff->cpSTMark, lpSendBuff->iSTRestCnt, 0);

			if(iResult == SOCKET_ERROR)
			{
                return -1;
			}

			PostReSend(hNet, iResult);
		}
	}

    // 리시브 버퍼 점검
	// 이전에 표시된 위치와 전송받은 데이터 실제 크기를 구한다.
	if(CopyBuffFromNetBuff(hNet) == -1)
	{
		return -1;
	}

	iRestSize = pNetProset->m_RingBuff.cpRTEnd - pNetProset->m_RingBuff.cpRTMark;

	if(iRestSize < 0) iRestSize += MAX_BUFF_SIZE;

	return iRestSize;

}



