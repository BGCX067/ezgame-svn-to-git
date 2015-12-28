//// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 NET_EXPORTS 기호로
// 컴파일되며, 동일한 DLL을 사용하는 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// NET_API 함수를 DLL에서 가져오는 것으로 보고,
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#include <list>
#include <winsock2.h>
#include <windows.h>

#define NETMSG_DISCONNET			-1
#define NETMSG_RECVFAIL				-2
#define NETMSG_SOCKETERR			-3


#define MAX_RECV_SIZE				1024
#define MAX_SENDPACKET_SIZE			1024
#define MAX_BUFF_SIZE				10000
#define HEADERSIZE					7
#define STX							0x2A
#define ETX							0xA2
#define PACKET_OTHER_SIZE			   8


typedef HANDLE				NETHANDLE;

#pragma pack(1)
typedef struct tagPACKET
{
	UINT8		stx;					// STX
	UINT16		len;					// 패킷의 길이
	UINT32		key;					// 올바른 패킷인가 ? 
	UINT8       Data[MAX_BUFF_SIZE];    // 패킷의 데이터

} PACKET;


typedef struct TagPacketHader
{
	UINT8		m_ui1RTMark_stx;							// 리시브버퍼의 유효성 확인을 위함
	UINT16		m_ui2RTMark_len;
	UINT32		m_ui4pRTMark_key;
} PacketHader;

#pragma pack(8)



typedef struct TagBufEnQueueTcp
{
    char		lpRingBuff[MAX_BUFF_SIZE * 2];			// 리스브 링버퍼
	char		*cpRTEnd;									// 리시브 버퍼의 마지막 포인터
	char		*cpRTMark;

} BufEnQueueTcp;

typedef struct TagSendEnQueueTcp
{
    char		lpRingBuff[MAX_BUFF_SIZE * 2];			// 리스브 링버퍼
	char		*cpSTEnd;									// 리시브 버퍼의 마지막 포인터
	char		*cpSTMark;
	int			iSTRestCnt;

} SendEnQueueTcp;

typedef struct tagNETPROSET
{
	int						iAlloc;
	SOCKET					socket;
	BufEnQueueTcp			m_RingBuff;
	SendEnQueueTcp			m_SendBuff;
	struct  sockaddr_in		servAddr;

} NETPROSET;


// 수정
bool Send(NETHANDLE hNet, char *data, int size);
int NetState(NETHANDLE hNet);
NETHANDLE Connect(char *Addr, int port);
int GetQueueSize(NETHANDLE hNet);
int Getpacket(NETHANDLE hNet, char *Packet);
void Disconnet(NETHANDLE hNet);
long GetRecvsize(NETHANDLE hNet);
void CloseNetwork();
bool InitNetwork();


