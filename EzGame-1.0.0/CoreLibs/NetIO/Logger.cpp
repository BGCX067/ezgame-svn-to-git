#include "stdafx.h"
#include ".\logger.h"

CLogger::CLogger(void)
{
}

CLogger::~CLogger(void)
{

}




void CLogger::log(enELOG_LEVEL ll, const char* format, ...)
{
   char Buffer[1024];
	
	ZeroMemory(Buffer, 1024);

	va_list ArgPtr;
	va_start( ArgPtr, format );

	_vsnprintf( Buffer, 1024, format, ArgPtr );

    va_end( ArgPtr );

	printf("%s\r\n", Buffer);

	// 파일로 로그를 적는다.
	WriteFile(ll, Buffer);

}

void CLogger::WriteFile(enELOG_LEVEL ll, const char* message)
{
	FILE* fp;
	char FileName[500];
	std::string strTime;

	SYSTEMTIME  time;

	GetLocalTime(&time);

	CreateDirectory("log", NULL); 


	sprintf(FileName, "log\\%d_%d_%d.log", time.wYear, time.wMonth, time.wDay);


	fp = fopen(FileName, "a+");

	if(fp == NULL)
	{
        fp = fopen(FileName, "w");

		if(fp == NULL)
			return;
	}
	
	fprintf(fp, "%d\t%2d:%2d:%2d\t%s\r\n", ll, time.wHour, time.wMinute, time.wSecond, message);

	fclose(fp);

}

void CLogger::MonitorSend(const char* message)
{

}