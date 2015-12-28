#include "StdAfx.h"
#include ".\processmanager.h"
#include "ServerProcess.h"


CProcessManager::CProcessManager(void)
{
   ::InitializeCriticalSection(&m_CS);

}

CProcessManager::~CProcessManager(void)
{
	m_MapProcess.clear();
}

/*
BOOL CProcessManager::SetCommand(JOB_MESSAGE* lpGameJob)
{

	CServerProcess* pProcess = 0;
	std::map<int, CServerProcess*>::iterator it;
	LPSOCKETCONTEXT	lpSocketContext = lpGameJob->lpSocketContext;

	
	it = m_MapProcess.find(lpGameJob->Packet.Ctrl);

	if(it != m_MapProcess.end())
	{
		pProcess = it->second;
	}
	else
	{
		return FALSE;
	}



//	return pProcess->JobMessage(lpGameJob);

	return 0;

}

*/

CServerProcess* CProcessManager::FindProcess(int iCmdID)
{
	std::map<int, CServerProcess*>::iterator it;

    it = m_MapProcess.find(iCmdID);

	if(it == m_MapProcess.end())
	{
		return NULL;
	}

	return it->second;

}

BOOL CProcessManager::DelProcess(int iCmdID)
{


	std::map<int, CServerProcess*>::iterator it;

    it = m_MapProcess.find(iCmdID);

	if(it == m_MapProcess.end())
	{
		return FALSE;
	}

	m_MapProcess.erase(it);

	return TRUE;
}


BOOL CProcessManager::AddProcess(int iCmdID, CServerProcess* pProcess)
{
	std::map<int, CServerProcess*>::iterator it;

    it = m_MapProcess.find(iCmdID);

	if(it != m_MapProcess.end())
	{
		return FALSE;
	}

	m_MapProcess.insert(std::map<int, CServerProcess*>::value_type(iCmdID, pProcess));

	return TRUE;
}