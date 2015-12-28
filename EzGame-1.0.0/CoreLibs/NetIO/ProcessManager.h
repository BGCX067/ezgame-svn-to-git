#pragma once

#include <map>

class CServerProcess;

class CProcessManager
{
public:
	CProcessManager(void);
	~CProcessManager(void);
//	BOOL SetCommand(JOB_MESSAGE* lpGameJob);
	BOOL AddProcess(int iCmdID, CServerProcess* pProcess);
	BOOL DelProcess(int iCmdID);
	CServerProcess* FindProcess(int iCmdID);
	inline int Size() { return (int) m_MapProcess.size(); }

protected:
	std::map<int, CServerProcess*>	m_MapProcess;

	CRITICAL_SECTION			m_CS;


};
