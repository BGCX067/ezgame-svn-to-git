
#pragma once

#define MAX_SENDPACKET_SIZE							10000
#define MAX_BUFF_SIZE								20000

#define MAX_QUEUE_SIZE								10000
#define PACKET_DATA_SIZE_MAX						8192		



#pragma pack(1)


typedef struct tagTCommon 
{
	DWORD		Len;
	BYTE		Ctrl;
	UINT32		key;
} TCommon;

typedef struct tagTCommonPacket 
{
	TCommon Common;
	BYTE Data [PACKET_DATA_SIZE_MAX];
} TCommonPacket;


#pragma pack()


#define INITRECVSIZE				0
#define NOT_PROCESSS				-1

enum enContextType { enAcceptContext, enConnectContext };
enum enIOMode {enConnecting, enAccepting, enReading, enWriting, enClose, enStop };

typedef struct 
{
	OVERLAPPED				ovl;
	enIOMode				mode;
	short					ContextType;
	bool					bConnect;

} EOVERLAPPED, *LPEOVERLAPPED;

typedef struct
{
	EOVERLAPPED				eovRecvTcp;
	LPEOVERLAPPED			lpeovSendTcp;

	char					cRecvTcpRingBuf[MAX_BUFF_SIZE * 2],
							*cpRTBegin,
							*cpRTEnd,
							*cpRTMark;
	
	int						index;			
	SOCKET					sktNumber;			// 접속한 클라이언트 소켓 번호 
	sockaddr_in				local,				// 서버주소
							peer;				// 클라이언트 주소

	CRITICAL_SECTION		csRTcp;
	UINT64					Statue;				// 지금 현재의 위치값
	SYSTEMTIME				ConnectTime;
	DWORD					ConnectTick;
	DWORD					Reserve;			// 예약
	DWORD					ContextType;			// 예약

	void*					pParam;

} SOCKETCONTEXT, *LPSOCKETCONTEXT;

typedef struct
{
	EOVERLAPPED				eovSendTcp;
	LPEOVERLAPPED			lpeovRecvTcp;
	HANDLE					hIOCP;


	char					cSendTcpRingBuf[MAX_BUFF_SIZE * 2],
							*cpSTBegin,
							*cpSTEnd;
	
	int						index;
	int						iSTRestCnt;
	SOCKET					sktNumber;					// 접속한 클라이언트 소켓 번호 
	CRITICAL_SECTION		csSTcp;

} SENDSOCKETCONTEXT, *LPSENDSOCKETCONTEXT;


struct sProtocol 
{
	DWORD  ConnectionID;
	int    Ctrl;
	char   pData[MAX_BUFF_SIZE];

};

struct JOB_MESSAGE
{
	LPSOCKETCONTEXT		lpSocketContext;
	TCommonPacket		Packet;
	DWORD				dwPushTime;
};
