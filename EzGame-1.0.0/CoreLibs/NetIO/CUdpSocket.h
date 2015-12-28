#pragma once

#include <windows.h>
#include <winsock2.h>
#include "NetIOModule.h"


class CUdpSocket
{
public:
	CUdpSocket();
	virtual ~CUdpSocket();
	virtual bool start(int nPort);	
	virtual void send(const char* IP, int Port, const char * pPacket, int iSize);

private:
	bool BeginMainThread();
	void CloseMainThread();
		
	void run();

	virtual void OnRecv(const char *pPacket, int iSize) = 0;
	

protected:
	static unsigned int __stdcall MainThreadProc(void *lpParam); // 클래스 멤버 함수로 할때는 앞에 static을 붙인다.
	bool m_bRunThread;	
	HANDLE	m_hThread;
	DWORD	m_dwThreadID;
	int m_nServerPort;	

	SOCKET m_hSock;
};
