#include "EzDeviceWin32.h"
#include "EzEvent.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
	#endif
	#ifndef WHEEL_DELTA
	#define WHEEL_DELTA 120
	#endif



	EzEvent event;
	static int nClickCount=0;
	EzDeviceWin32* dev = NULL;

	if (GetCapture() != hWnd && nClickCount > 0)
		nClickCount = 0;

	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SETCURSOR:
		break;

	case WM_MOUSEWHEEL:
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.fWheel = (float)((short)HIWORD(wParam)) / (float)WHEEL_DELTA;
		event.MouseInput.Event = EMIE_MOUSE_WHEEL;

		POINT p; // fixed by jox
		p.x = 0; p.y = 0;
		ClientToScreen(hWnd, &p);
		event.MouseInput.X = LOWORD(lParam) - p.x;
		event.MouseInput.Y = HIWORD(lParam) - p.y;

		EzApplication::postEvent(event);
		break;

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_LMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		
		EzApplication::postEvent(event);
		
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_LMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);

		EzApplication::postEvent(event);

		return 0;

	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_RMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		
		EzApplication::postEvent(event);

		return 0;

	case WM_RBUTTONUP:
		ReleaseCapture();
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_RMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);
		
		EzApplication::postEvent(event);		
		
		return 0;

	case WM_MBUTTONDOWN:
		SetCapture(hWnd);
		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_MMOUSE_PRESSED_DOWN;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);

		EzApplication::postEvent(event);		
		return 0;

	case WM_MBUTTONUP:
		ReleaseCapture();

		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.Event = EMIE_MMOUSE_LEFT_UP;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);

		EzApplication::postEvent(event);		

		return 0;

	case WM_MOUSEMOVE:

		if(GetCapture() == hWnd)
		{
			event.MouseInput.Event = EMIE_MOUSE_DOWN_MOVED;
		}
		else
		{
			event.MouseInput.Event = EMIE_MOUSE_MOVED;
		}

		event.eEventType = EET_MOUSE_INPUT_EVENT;
		event.MouseInput.X = (short)LOWORD(lParam);
		event.MouseInput.Y = (short)HIWORD(lParam);

		EzApplication::postEvent(event);		

		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			BYTE allKeys[256];

			event.eEventType = EET_KEY_INPUT_EVENT;
			event.KeyInput.eKey = (EKEY_CODE)wParam;
			event.KeyInput.bPressedDown = (message==WM_KEYDOWN);

			WORD KeyAsc=0;
			GetKeyboardState(allKeys);
			ToAscii((UINT)wParam,(UINT)lParam,allKeys,&KeyAsc,0);

			event.KeyInput.bShift = ((allKeys[VK_SHIFT] & 0x80)!=0);
			event.KeyInput.bControl = ((allKeys[VK_CONTROL] & 0x80)!=0);
			event.KeyInput.Char = (KeyAsc & 0x00ff); //KeyAsc >= 0 ? KeyAsc : 0;
		
			EzApplication::postEvent(event);		

		}

		return 0;

	case WM_SIZE:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER)
			return 0;
		break;

	case WM_USER:
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


EzImplementRTTI(EzDeviceWin32, EzDevice);



EzDeviceWin32::EzDeviceWin32(const SCreationParameters3D& parameters)
: m_bNonNTWindows(true), m_bFullScreen(parameters.bFullscreen)
{
	EzStringc winversion;

	getWindowsVersion(winversion);

	if (parameters.WindowID)
	{
		m_hWnd = static_cast<HWND>(parameters.WindowID);
		// attach external window
		RECT r;
		GetWindowRect(m_hWnd, &r);

		m_WindowSize.Width = r.right - r.left;
		m_WindowSize.Height = r.bottom - r.top;
	
	}
	else
	{
		if(!createWindows(parameters.WindowSize))
		{
			log(0, "WindowId is not valid.");
			return;
		}
	}


	// register environment
	//// set this as active window
	SetActiveWindow(m_hWnd);
	SetForegroundWindow(m_hWnd);
}

EzDeviceWin32::~EzDeviceWin32(void)
{
}



bool EzDeviceWin32::createWindows(EzDimension2d WindowSize)
{
	const c8* ClassName = "EzDeviceWin32";

	// get handle to exe file
	HINSTANCE hInstance = GetModuleHandle(0);

	// Register Class
	WNDCLASSEX wcex;
	wcex.cbSize		= sizeof(WNDCLASSEX);
	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= ClassName;
	wcex.hIconSm		= 0;

	// if there is an icon, load it
	wcex.hIcon = (HICON)LoadImage(hInstance, "irrlicht.ico", IMAGE_ICON, 0,0, LR_LOADFROMFILE); 

	RegisterClassEx(&wcex);

	// calculate client size

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = WindowSize.Width;
	clientSize.bottom = WindowSize.Height;

	DWORD style = WS_POPUP;

	if (!m_bFullScreen)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	AdjustWindowRect(&clientSize, style, FALSE);

	const int realWidth = clientSize.right - clientSize.left;
	const int realHeight = clientSize.bottom - clientSize.top;

	int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if (m_bFullScreen)
	{
		windowLeft = 0;
		windowTop = 0;
	}

	// create window
	HWND hWnd;

	hWnd = CreateWindow( ClassName, "", style, windowLeft, windowTop,
				realWidth, realHeight, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// fix ugly ATI driver bugs. Thanks to ariaci
	MoveWindow(hWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	m_hWnd = hWnd;

	return true;
}


//! runs the device. Returns false if device wants to be deleted
bool EzDeviceWin32::run()
{

	bool bQuit = false;

	MSG Winmsg;

	while (PeekMessage(&Winmsg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Winmsg);

		if (Winmsg.hwnd == m_hWnd)
			WndProc(m_hWnd, Winmsg.message, Winmsg.wParam, Winmsg.lParam);
		else
			DispatchMessage(&Winmsg);

		if (Winmsg.message == WM_QUIT)
			bQuit = true;
	}

	if (!bQuit)
		resizeIfNecessary();

	//if(!quit)
	//	pollJoysticks();

	return !bQuit;
}

void EzDeviceWin32::resizeIfNecessary()
{
	if (!m_bResized)
		return;

	RECT r;
	GetClientRect(m_hWnd, &r);

	//char tmp[255];

	//if (r.right < 2 || r.bottom < 2)
	//{
	//	//sprintf(tmp, "Ignoring resize operation to (%ld %ld)", r.right, r.bottom);
	//	//os::Printer::log(tmp);
	//}
	//else
	//{
	//	//sprintf(tmp, "Resizing window (%ld %ld)", r.right, r.bottom);
	//	//os::Printer::log(tmp);

	//	//getVideoDriver()->OnResize(irr::EzDimension2d<irr::s32>(r.right, r.bottom));
	//}

	m_bResized = false;
}

void EzDeviceWin32::sleep(unsigned int timeMs)
{
	Sleep(timeMs);
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void EzDeviceWin32::yield()
{
	Sleep(1);
	
}

//! sets the caption of the window
void EzDeviceWin32::setWindowCaption(const char* text)
{
	DWORD dwResult;
	//if (m_bNonNTWindows)
	//{

		SendMessageTimeout(m_hWnd, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(text),
				SMTO_ABORTIFHUNG, 2000, &dwResult);
	//}
	//else
	//	SendMessageTimeoutW(m_hWnd, WM_SETTEXT, 0,
	//			reinterpret_cast<LPARAM>(text),
	//			SMTO_ABORTIFHUNG, 2000, &dwResult);
}


void EzDeviceWin32::getWindowsVersion(EzStringc& out)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx((OSVERSIONINFO *) &osvi))
			return;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4)
			out.append("Microsoft Windows NT ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows 2000 ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			out.append("Microsoft Windows XP ");
		else
		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows Vista ");

		if (bOsVersionInfoEx)
		{
			#ifdef VER_SUITE_ENTERPRISE
			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
#ifndef __BORLANDC__
				if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					out.append("Personal ");
				else
					out.append("Professional ");
#endif
			}
			else if (osvi.wProductType == VER_NT_SERVER)
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					out.append("DataCenter Server ");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					out.append("Advanced Server ");
				else
					out.append("Server ");
			}
			#endif
		}
		else
		{
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if (lstrcmpi( "WINNT", szProductType) == 0 )
				out.append("Professional ");
			if (lstrcmpi( "LANMANNT", szProductType) == 0)
				out.append("Server ");
			if (lstrcmpi( "SERVERNT", szProductType) == 0)
				out.append("Advanced Server ");
		}

		// Display version, service pack (if any), and build number.

		char tmp[255];

		if (osvi.dwMajorVersion <= 4 )
		{
			sprintf_s(tmp, 255, "version %ld.%ld %s (Build %ld)",
				osvi.dwMajorVersion,
				osvi.dwMinorVersion,
				osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}
		else
		{
			sprintf_s(tmp, 255, "%s (Build %ld)", osvi.szCSDVersion,
				osvi.dwBuildNumber & 0xFFFF);
		}

		out.append(tmp);
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		m_bNonNTWindows = true;

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			out.append("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				out.append("OSR2 " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			out.append("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				out.append( "SE " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			out.append("Microsoft Windows Me ");

		break;

	case VER_PLATFORM_WIN32s:

		m_bNonNTWindows = true;
		out.append("Microsoft Win32s ");
		break;
	}
}

//! notifies the device that it should close itself
void EzDeviceWin32::closeDevice()
{
	MSG Winmsg;
	PeekMessage(&Winmsg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	PostQuitMessage(0);
	PeekMessage(&Winmsg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	DestroyWindow(m_hWnd);
}

EzDevice* createDeviceEx(const SCreationParameters3D& parameters)
{
	return new EzDeviceWin32(parameters);
}


