//// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� NET_EXPORTS ��ȣ��
// �����ϵǸ�, ������ DLL�� ����ϴ� �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// NET_API �Լ��� DLL���� �������� ������ ����,
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
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
	UINT16		len;					// ��Ŷ�� ����
	UINT32		key;					// �ùٸ� ��Ŷ�ΰ� ? 
	UINT8       Data[MAX_BUFF_SIZE];    // ��Ŷ�� ������

} PACKET;


typedef struct TagPacketHader
{
	UINT8		m_ui1RTMark_stx;							// ���ú������ ��ȿ�� Ȯ���� ����
	UINT16		m_ui2RTMark_len;
	UINT32		m_ui4pRTMark_key;
} PacketHader;

#pragma pack(8)



typedef struct TagBufEnQueueTcp
{
    char		lpRingBuff[MAX_BUFF_SIZE * 2];			// ������ ������
	char		*cpRTEnd;									// ���ú� ������ ������ ������
	char		*cpRTMark;

} BufEnQueueTcp;

typedef struct TagSendEnQueueTcp
{
    char		lpRingBuff[MAX_BUFF_SIZE * 2];			// ������ ������
	char		*cpSTEnd;									// ���ú� ������ ������ ������
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


// ����
bool Send(NETHANDLE hNet, char *data, int size);
int NetState(NETHANDLE hNet);
NETHANDLE Connect(char *Addr, int port);
int GetQueueSize(NETHANDLE hNet);
int Getpacket(NETHANDLE hNet, char *Packet);
void Disconnet(NETHANDLE hNet);
long GetRecvsize(NETHANDLE hNet);
void CloseNetwork();
bool InitNetwork();


