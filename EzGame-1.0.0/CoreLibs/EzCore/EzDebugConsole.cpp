// DebugConsole.cpp: implementation of the EzDebugConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "EzDebugConsole.h"

EzDebugConsole::EzDebugConsole()
{
	AllocConsole();
	m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

EzDebugConsole::~EzDebugConsole()
{
	FreeConsole();
}

void EzDebugConsole::print(const char* format ...)
{

	char* tmp;
    va_list args; 

	va_start( args, format );
	int len = 4096;
	tmp = (char*)malloc( len );

#if( _MSC_VER > 1399 )
	vsprintf_s( tmp, len, format, args );
#else // if( _MSC_VER > 1399 )
	vsprintf( tmp, format, args );
#endif 


	DWORD uWrite;

	WriteConsole(m_hOut, tmp, (DWORD) strlen(tmp), &uWrite, NULL);

	free(tmp);
	

}
