#pragma once


#include "EzDevice.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>



class EzDeviceWin32 : public EzDevice
{

	EzDeclareRTTI;

public:
	EzDeviceWin32();
	~EzDeviceWin32(void);

	virtual	void closeDevice();
	virtual bool run();
	virtual void sleep(unsigned int timeMs);
	virtual void yield();
	virtual void setWindowCaption(const char* text);

	inline HWND getWinHandle() { return m_hWnd; }

protected:
	void resizeIfNecessary();

	bool createWindows(EzDimension2d WindowSize);
	void getWindowsVersion(EzStringc& out);


	bool					m_bResized;
	bool					m_bFullScreen;
	bool					m_bNonNTWindows;
	EzDimension2d			m_WindowSize;

	HWND					m_hWnd;

};

