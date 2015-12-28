// Application3D.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SampleAPP3D.h"


int _tmain(int argc, _TCHAR* argv[])
{

	EzInit();
	//
	SCreationParameters3D  parameters;

	
	CSampleAPP3D test(create3DDeviceEx(parameters));

	if(test.initialize())
	{

		test.excute();


	}


	EzShutdown();
	return 0;
}

