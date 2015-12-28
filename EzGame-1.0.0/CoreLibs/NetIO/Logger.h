#pragma once

#define SETLOG					GetLogger()->log

enum enELOG_LEVEL
{
	ELL_INFORMATION = 0,
	ELL_WARNING,
	ELL_ERROR,
	ELL_NONE
};

class CLogger
{
public:
	CLogger(void);
	~CLogger(void);

	static void _cdecl log(enELOG_LEVEL ll, const char* format, ...);
	

protected:
	static void WriteFile(enELOG_LEVEL ll, const char* message);
	static void MonitorSend(const char* message);
    
};


inline CLogger* GetLogger()
{
	static CLogger log;

	return &log;

}