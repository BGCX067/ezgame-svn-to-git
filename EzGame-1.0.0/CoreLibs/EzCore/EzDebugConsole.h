// DebugConsole.h: interface for the EzDebugConsole class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "EzCoreLibType.h"
#include <windows.h>
#include <stdio.h>


#define CSP EzDebugConsole::getConsole().print



class EZCORE_ENTRY EzDebugConsole
{
public:
	void print(const char* tszMsg, ...);
	virtual ~EzDebugConsole();
	static EzDebugConsole& getConsole()
	{
		static EzDebugConsole gt;
		return gt;
	}



private:
	EzDebugConsole();
	HANDLE m_hOut;
};



