#include <windows.h>

#include "../EzCore/EzSingtonGroup.h"
#include "EzApplication.h"

//---------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	EzInit();
	//

	EzApplication* pApp = EzApplication::create();

	if(pApp->initialize())
	{
		pApp->excute();

	}

	pApp->destory();


	EzShutdown();

	return 0;
}
//---------------------------------------------------------------------------
