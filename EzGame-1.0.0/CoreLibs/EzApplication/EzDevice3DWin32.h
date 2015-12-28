#pragma once

#include "../EzRenderer/EzVideoDriver.h"
#include "../EzRenderer/EzRender.h"
#include "SCreationParameters3D.h"
#include "EzDeviceWin32.h"

class EzDevice3DWin32 : public EzDeviceWin32
{
	EzDeclareRTTI;


public:
	EzDevice3DWin32(const SCreationParameters3D& parameters);
	~EzDevice3DWin32(void);


	void* getVideoDriver() { return m_pVideoDriver; }
	void* getRender() 
	{ 
		return m_pVideoDriver->getRenderer();
	}
	void createDriver(HWND hWnd, const SCreationParameters3D& parameters);

protected:


	EzVideoDriver*		m_pVideoDriver;
	EzRender*				m_pRender;

};

