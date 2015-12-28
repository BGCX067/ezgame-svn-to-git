
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
	SOCKET					sktNumber;			// ������ Ŭ���̾�Ʈ ���� ��ȣ 
	sockaddr_in				local,				// �����ּ�
							peer;				// Ŭ���̾�Ʈ �ּ�

	CRITICAL_SECTION		csRTcp;
	UINT64					Statue;				// ���� ������ ��ġ��
	SYSTEMTIME				ConnectTime;
	DWORD					ConnectTick;
	DWORD					Reserve;			// ����
	DWORD					ContextType;			// ����

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
	SOCKET					sktNumber;					// ������ Ŭ���̾�Ʈ ���� ��ȣ 
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
