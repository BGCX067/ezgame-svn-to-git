#pragma once

#define SERVICE_CONTROL_NEWFILE					128


void SetServiceHandle(SERVICE_STATUS_HANDLE	hSrv);
void SetServiceExe(TCHAR *name);
void SetServiceName(TCHAR *name);
void SetServiceMemo(TCHAR *memo);
void SetServiceDisName(TCHAR *DisName);
bool Install();
void Remove();
void Config(BOOL bAutostart);
void Display();
void Status();
void GetConfig();
void Dependencies();
void Start();
void Control(DWORD dwControl);
void ServiceSetStatus(DWORD dwState, DWORD dwAccept=SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE);
DWORD GetStatus();
std::string GetServiceDir(TCHAR* ServiceName);

UINT SHRegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault);
UINT SHRegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, LPTSTR lpRet, DWORD nSize);




