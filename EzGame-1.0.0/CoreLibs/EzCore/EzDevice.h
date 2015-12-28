#pragma once

#include "EzRefObject.h"
#include "SEvent.h"
#include "EzRTTI.h"
#include "EzApplication.h"
#include "EzCore.h"

class EzDevice : public EzRefObject
{
	EzDeclareRootRTTI(EzDevice);

public:
	EzDevice(void);
	~EzDevice(void);

	virtual bool run() = 0;
	virtual void sleep(u32 timeMs) = 0;
	virtual	void closeDevice() = 0;
	virtual void yield() = 0;

	virtual void* getVideoDriver() { return NULL; }
	virtual void* getRender() { return NULL; }
	
	inline bool postEvent(SEvent event) 
	{ 
		if(m_pApplication) 
		{
			switch(event.eEventType)
			{
			case EET_MOUSE_INPUT_EVENT:
				m_pApplication->OnMouseEvent(event.MouseInput);
				return true;

			case EET_KEY_INPUT_EVENT:
				m_pApplication->OnKeyEvent(event.KeyInput);
				return true;

			case EET_JOYSTICK_INPUT_EVENT:
				return 0;

			case EET_LOG_TEXT_EVENT:
				m_pApplication->OnLogEvent(event.LogEvent);
				return true;

			case EET_USER_EVENT:
				m_pApplication->OnUserEvent(event.UserEvent);
				return true;
			}
				
	
			
		}

		return false;
	}

	void setApplication(EzApplication* pApp);


	static EzDevice* getDevice() { return m_pGlobal; }

	f32 getTimeSec();
	u32 getPerformanceCounter();
	inline f32 GetPerformanceCounterHz() 
	{ 
		return 	m_fPCCyclesPerSecond; 
	}


protected:
	static	EzDevice*				m_pGlobal;
	EzApplication*					m_pApplication;

	f32									m_fPCCyclesPerSecond;

	LARGE_INTEGER						m_freq;
    LARGE_INTEGER						m_initial;

};


EzDevice* createDeviceEx(EzDimension2d<s32> WindowSize);


