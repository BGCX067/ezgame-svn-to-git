//////////////////////////////////////////////////////////////////////
// NTService.cpp 
// Version 0.01
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <io.h>
#include <iostream> 
#include "Logger.h"
#include "NTService.h"


using namespace std;

#define	 MAX_SERVICE_NAME					256
#define	 MAX_SERVICE_DISNAME				512
#define	 MAX_SERVICE_MEMO					1024

//////////////////////////////////////////
//2003 7. 15 레지스트리 정보 읽기
//////////////////////////////////////////

UINT SHRegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault)
{
	HKEY key;
	DWORD dwDisp;
	UINT Result;
	DWORD Size;

	if(RegCreateKeyEx(hKey, lpKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &key, &dwDisp)
		!= ERROR_SUCCESS)
		return 0;

	Size = sizeof(LONG);

	if(RegQueryValueEx(key, lpValue, 0, NULL, (LPBYTE) &Result, &Size)
		!= ERROR_SUCCESS)
		Result = nDefault;

	RegCloseKey(key);

	return Result;

}

UINT SHRegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, LPTSTR lpRet, DWORD nSize)
{
	HKEY key;
	DWORD dwDisp;
	DWORD Size;

	TCHAR  str[256];
	if(RegCreateKeyEx(hKey, lpKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &key, &dwDisp)
		!= ERROR_SUCCESS)
		return 0;

	Size = nSize;

	if(RegQueryValueEx(key, lpValue, 0, NULL, (LPBYTE) str, &Size)
		!= ERROR_SUCCESS)
	{
		strcpy(lpRet, lpDefault);
		return 0;
	}

	strcpy(lpRet, str);

	

	RegCloseKey(key);

	return 1;

}


SERVICE_STATUS_HANDLE		g_hSrv;
DWORD						g_State;
char						g_TCurrentDir[MAX_PATH];
char						g_ServiceExe[MAX_PATH];
char						g_ServiceName[MAX_SERVICE_NAME];
char						g_ServiceDisName[MAX_SERVICE_DISNAME];
char						g_Memo[MAX_SERVICE_MEMO];


void SetServiceHandle(SERVICE_STATUS_HANDLE	hSrv)
{
	g_hSrv = hSrv;
}

void SetServiceExe(char *name)
{
	strcpy(g_ServiceExe, name);
}

void SetServiceName(char *name)
{
	strcpy(g_ServiceName, name);
}


void SetServiceDisName(char *DisName)
{
	strcpy(g_ServiceDisName, DisName);
}

void SetServiceMemo(char *memo)
{
	ZeroMemory(g_Memo, 1024);
	strcpy(g_Memo, memo);
}



DWORD GetStatus()
{
	return g_State;
}

// 서비스의 현재 상태를 변경하는 함수
void ServiceSetStatus(DWORD dwState, DWORD dwAccept /*=SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE */)
{

	if(g_hSrv == NULL)
		return;

	Sleep(1000);

	SERVICE_STATUS ss;
	ss.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState=dwState;
	ss.dwControlsAccepted=dwAccept;
	ss.dwWin32ExitCode=0;
	ss.dwServiceSpecificExitCode=0;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=2000;

	// 현재 상태를 보관해 둔다.
	g_State = dwState;
	SetServiceStatus(g_hSrv,&ss);
}


void Start()
{
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0 ;
	BOOL bRet = 0;
	DWORD dwOldCheck = 0;
	
	SERVICE_STATUS ss;

	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail !\n");
		return;
	}

	hService = OpenService(hSCM, g_ServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
	if(!hService)
	{
		SETLOG(ELL_ERROR, "OpenService Fail !\n");
		CloseServiceHandle(hSCM);
		return;
	}

	if(!StartService(hService, 0, NULL))
	{
		SETLOG(ELL_ERROR, "StartService Fail !\n");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return;
	}
	
	// Optionally, make sure service is running before continuing...
	bRet = QueryServiceStatus(hService, &ss);
	if(!bRet)
	{
		SETLOG(ELL_ERROR, "QueryServiceStatus Fail !\n");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return;
	}

	while(SERVICE_RUNNING != ss.dwCurrentState)
	{
		dwOldCheck = ss.dwCheckPoint;
		Sleep(ss.dwWaitHint);
		
		bRet = QueryServiceStatus(hService, &ss);
		if(!bRet)
		{
			SETLOG(ELL_ERROR, "QueryServiceStatus\n");
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return;
		}

		// Service changed state or did not increment in time, so break
		if(dwOldCheck >= ss.dwCheckPoint)
			break;
	}

	if(SERVICE_RUNNING == ss.dwCurrentState)
	{
	//	cout << g_ServiceName << " started successfully." << endl;
	}
	else
	{
	//	cout << g_ServiceName << _T(" start unsuccessful: ") << endl;
	//	cout << _T("Current State: 0x") << ss.dwCurrentState << endl;
	//	cout << _T("Win32 Exit: ") << ss.dwWin32ExitCode << endl;
	//	cout << "Service Exit: " << ss.dwServiceSpecificExitCode << endl;
	//	cout << "Checkpoint: 0x" << ss.dwCheckPoint << endl;
	//	cout << "WaitHint: 0x" << ss.dwWaitHint << endl;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
}



void Display()
{
	SC_HANDLE hSCM = 0;
	BOOL bRet = FALSE;

	TCHAR lpBuf[MAX_SERVICE_NAME + 1]; //Display name can't be longer than this!
	DWORD cchBuffer = MAX_SERVICE_NAME + 1; 

	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail!");
		return;
	}

	
	bRet = GetServiceDisplayName(hSCM, g_ServiceName, lpBuf, &cchBuffer);
	if(bRet)
		SETLOG(ELL_ERROR, "The display name is: %s\n", lpBuf); 
	else
		SETLOG(ELL_ERROR, "GetServiceDisplayName");

	CloseServiceHandle(hSCM);
	return;
}

void Config(BOOL bAutostart)
{
	SC_HANDLE hSCM		= 0;
	SC_HANDLE hService	= 0;
	SC_LOCK hLock		= 0; 
	DWORD dwStartType	= 0;
	BOOL bRet			= FALSE;

	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ | GENERIC_EXECUTE);
	if(!hSCM)
	{
//		ErrorPrinter(_T("OpenSCManager"));
		return;
	}
	
	hLock = LockServiceDatabase(hSCM);
	if(!hLock)
	{
		SETLOG(ELL_ERROR, "Service Configuration could not be changed\n");
		CloseServiceHandle(hSCM);
		return;
	}	
	
	hService = OpenService(hSCM, g_ServiceName, SERVICE_CHANGE_CONFIG);
	if(!hService)
	{
		SETLOG(ELL_ERROR, "OpenService Fail !\n");
		UnlockServiceDatabase(hLock);
		CloseServiceHandle(hSCM);
		return;
	}

	dwStartType = (bAutostart) ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
	bRet = ChangeServiceConfig(hService,
                               SERVICE_NO_CHANGE,
                               dwStartType,
                               SERVICE_NO_CHANGE,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL);
	
	if(bRet)
		SETLOG(ELL_ERROR, "Changed Service Configuration Successfully\n");
	else
		SETLOG(ELL_ERROR, "Changed Service Configuration Fail\n");

	CloseServiceHandle(hService);
	UnlockServiceDatabase(hLock);
	CloseServiceHandle(hSCM);
	return;
}

void Status()
{
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0 ;
	BOOL bRet = FALSE;
	
	SERVICE_STATUS ss;
	
	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail!");
		return;
	}

	hService = OpenService(hSCM, g_ServiceName, SERVICE_QUERY_STATUS);
	if(!hService) 
	{
		SETLOG(ELL_ERROR, "OpenService Fail");
		CloseServiceHandle(hSCM);
		return;
	}

	bRet = QueryServiceStatus(hService, &ss);
	if(bRet)
	{
//		cout << _T("Service Status for ") << g_ServiceName << endl;
//		cout << _T("Type: 0x") << ss.dwServiceType << endl;
//		cout << _T("Current State: 0x") << ss.dwCurrentState << endl;
//		cout << _T("Controls Accepted: ") << ss.dwControlsAccepted << endl;
//		cout << _T("Win32 Exit: ") << ss.dwWin32ExitCode << endl;
//		cout << _T("Service Exit: ") << ss.dwServiceSpecificExitCode << endl;
//		cout << _T("Checkpoint: 0x") << ss.dwCheckPoint << endl;
//		cout << _T("WaitHint: 0x") << ss.dwWaitHint << endl;
	}
	else
		SETLOG(ELL_ERROR, "QueryServiceStatus Fail!");

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
}


// 서비스를 설치한다.
bool Install()
{
	SC_HANDLE hScm, hSrv;
	SERVICE_DESCRIPTION lpDes;
	
	TCHAR SrvPath[MAX_PATH];
	TCHAR ExeName[100];
	TCHAR SrvName[100];
	TCHAR DisName[100];
	TCHAR Memo[500];

	memset(ExeName, 0, 100);
	memset(SrvName, 0, 100);
	memset(DisName, 0, 100);
	memset(Memo, 0, 100);

	// SCM을 연다
	hScm=OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm==NULL) 
	{
		SETLOG(ELL_ERROR, "SCManager를 오픈할 수 업습니다.\n");
		return false;
	}

	// 등록할 서비스 파일이 있는지 조사해 보고 경로를 구한다.
	GetCurrentDirectory(MAX_PATH, SrvPath);
	strcat(SrvPath, "\\");
	strcat(SrvPath, g_ServiceExe);
	strcat(ExeName, g_ServiceExe);
	strcat(SrvName, g_ServiceName);
	strcat(DisName, g_ServiceDisName);
	strcat(Memo, g_Memo);

	
	if (access(SrvPath,0) != 0) {
		CloseServiceHandle(hScm);
		SETLOG(ELL_ERROR, "같은 디렉토리에 서비스 파일이 없습니다\n");
		return false;
	}

	// 서비스를 등록한다.
	hSrv=CreateService(hScm, SrvName, DisName, SERVICE_STOP | SERVICE_PAUSE_CONTINUE | SERVICE_CHANGE_CONFIG,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, SrvPath,
		NULL, NULL, NULL, NULL, NULL);

	if (hSrv==NULL)
	{
		SETLOG(ELL_ERROR, "서비스를 등록 하지 못했습니다. %d\n", GetLastError());
		return false;
	}
	else
	{
		// 설명을 등록한다.
		lpDes.lpDescription=Memo;
		ChangeServiceConfig2(hSrv, SERVICE_CONFIG_DESCRIPTION, &lpDes);
		CloseServiceHandle(hSrv);
	}

	CloseServiceHandle(hScm);

	// 이벤트 로그 등록

	return true;
}


void Control(DWORD dwControl)
{
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0 ;
	BOOL bRet = 0;
	
	SERVICE_STATUS ss;

	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail!\n");
		return;
	}

	hService = OpenService(hSCM, g_ServiceName, GENERIC_EXECUTE);
	if(!hService)
	{
		SETLOG(ELL_ERROR, "OpenService Fail !\n");
		CloseServiceHandle(hSCM);
		return;
	}

	bRet = ControlService(hService, dwControl, &ss);
	if(bRet)
	{
		cout << "Service Status for " << g_ServiceName << endl;
		cout << "Type: 0x" << ss.dwServiceType << endl;
		cout << "Current State: 0x" << ss.dwCurrentState << endl;
		cout << "Controls Accepted: " << ss.dwControlsAccepted << endl;
		cout << "Win32 Exit: " << ss.dwWin32ExitCode << endl;
		cout << "Service Exit: " << ss.dwServiceSpecificExitCode << endl;
		cout << "Checkpoint: 0x" << ss.dwCheckPoint << endl;
		cout << "WaitHint: 0x" << ss.dwWaitHint << endl;		
	}
	else
		SETLOG(ELL_ERROR, "ControlService");

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
}

void Dependencies()
{
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0;
	BOOL bRet = 0;
	
	DWORD dwBytesNeeded = 0;
	DWORD dwServicesReturned = 0;
	LPENUM_SERVICE_STATUS pessBuf = {0};

	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail !");
		return;
	}

	hService = OpenService(hSCM, g_ServiceName, SERVICE_ENUMERATE_DEPENDENTS);
	if(!hService)
	{
		SETLOG(ELL_ERROR, "OpenService Fail !");
		CloseServiceHandle(hSCM);
		return;
	}

	// Find out how much to allocate
	bRet = EnumDependentServices(hService,
                                 SERVICE_STATE_ALL,
                                 0,
                                 0,
                                 &dwBytesNeeded,
                                 &dwServicesReturned);

	if(dwBytesNeeded == 0)
	{
		SETLOG(ELL_ERROR, "%s has no dependencies\n", g_ServiceName);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return;
	}

	// Allocate the proper size
	pessBuf = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, dwBytesNeeded);
	if(!pessBuf)
	{
		SETLOG(ELL_ERROR, "LocalAlloc Fail !");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return;
	}

	bRet = EnumDependentServices(hService,
                                 SERVICE_STATE_ALL,
                                 pessBuf,
                                 dwBytesNeeded,
                                 &dwBytesNeeded,
                                 &dwServicesReturned);
	
	if(bRet)
	{
		LPENUM_SERVICE_STATUS pess = 0;
		cout << "Services dependent on " << g_ServiceName << endl;
		for(DWORD i = 0 ; i < dwServicesReturned ; i++)
		{
			pess = &pessBuf[i];
			cout << pess->lpDisplayName << endl;
			cout << "  Current State: 0x";
			cout << pess->ServiceStatus.dwCurrentState << endl;
		}
	}
	else
		SETLOG(ELL_ERROR, "EnumDependentServices Fail !");
	
	// Clean up
	LocalFree(pessBuf);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
}

void GetConfig()
{
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0 ;
	BOOL bRet = FALSE;

	LPQUERY_SERVICE_CONFIG pqscBuf = {0};
	DWORD dwBytesNeeded = 0;
	
	hSCM = OpenSCManager(NULL, NULL, GENERIC_READ);
	if(!hSCM)
	{
		SETLOG(ELL_ERROR, "OpenSCManager Fail !\n");
		return;
	}

	hService = OpenService(hSCM, g_ServiceName, SERVICE_QUERY_CONFIG);
	if(!hService)
	{
		SETLOG(ELL_ERROR, "OpenService Fail !");
		CloseServiceHandle(hSCM);
		return;
	}

	// Obtain the number of bytes needed
	bRet = QueryServiceConfig(hService, 0, 0, &dwBytesNeeded);

	// Allocate the proper size
	pqscBuf = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, dwBytesNeeded);
	if(!pqscBuf)
	{
		SETLOG(ELL_ERROR, "LocalAlloc Fail !");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return;
	}
	
	// Call with the right amount
	bRet = QueryServiceConfig(hService, pqscBuf, dwBytesNeeded, &dwBytesNeeded);
	if(bRet)
	{
//		cout << _T("Service Configuration for ") << g_ServiceName << endl;
//		cout << _T("Display Name: ") << pqscBuf->lpDisplayName << endl;
//		cout << _T("Type: 0x") << pqscBuf->dwServiceType << endl;
//		cout << _T("Start Type: 0x") << pqscBuf->dwStartType << endl;
//		cout << _T("Error Level: 0x") << pqscBuf->dwErrorControl << endl;
//		cout << _T("Binary path: ") << pqscBuf->lpBinaryPathName << endl;
//		cout << _T("Load Order Group: ") << pqscBuf->lpLoadOrderGroup << endl;
//		cout << _T("Tag ID: ") << pqscBuf->dwTagId << endl;
//		cout << _T("Dependencies: ") << endl;

		if(pqscBuf->lpDependencies)
		{
			char* pszDepend = 0;
			int i = 0;
			pszDepend = &pqscBuf->lpDependencies[i];
			while(*pszDepend != 0) 
			{
				cout << pszDepend << endl;
				i += (int) strlen(pszDepend) + 1;
				pszDepend = &pqscBuf->lpDependencies[i];
			}
		}
//		cout << _T("Login Under: ") << pqscBuf->lpServiceStartName << endl;
	}
	else
		SETLOG(ELL_ERROR, "QueryServiceConfig Fail !");

	LocalFree(pqscBuf);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);
	return;
}

// 서비스를 제거한다.
void Remove()
{
	SC_HANDLE hScm, hSrv;
	SERVICE_STATUS ss;

	// SCM을 연다
	hScm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
	if (hScm==NULL) {
		SETLOG(ELL_ERROR, "SCManager을 열 수 없습니다.\n");
		return;
	}

	// 서비스의 핸들을 구한다.
	hSrv=OpenService(hScm, g_ServiceName, SERVICE_ALL_ACCESS);
	if (hSrv==NULL) 
	{
		CloseServiceHandle(hScm);
		SETLOG(ELL_ERROR, "서비스가 설치되어 있지 않습니다.");
		return;
	}

	// 실행중이면 중지시킨다.
	QueryServiceStatus(hSrv,&ss);
	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(2000);
	}

	// 서비스 제거
	if (DeleteService(hSrv)) {
		SETLOG(ELL_ERROR, "서비스를 제거했습니다.");
	}
	else
	{
		SETLOG(ELL_ERROR, "서비스를 제거하지 못했습니다. %d", GetLastError());
	}
	CloseServiceHandle(hSrv);

}

// 서비스 설치 경로를 찿는다.
std::string GetServiceDir(char* ServiceName)
{
	char str[MAX_PATH];
	char dir[MAX_PATH];
	char drive[MAX_PATH];
	char Key[2048];
	string servicePath;

	UINT iResult;
    
	sprintf(Key, "SYSTEM\\CurrentControlSet\\Services\\%s", ServiceName);

	iResult = SHRegReadString(HKEY_LOCAL_MACHINE, Key, "ImagePath", "", str, MAX_PATH);

	if(iResult > 0)
	{
        _splitpath(str, drive, dir, NULL, NULL);
				
		servicePath = drive;
		servicePath += dir;
		

	}
	else
	{
        servicePath = "null";
	}

	return servicePath;

}



