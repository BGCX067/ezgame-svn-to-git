#pragma once

#include "../EzCore/EzRTTI.h"
#include "../EzCore/EzMemoryDefines.h"
#include "EzLogReceiver.h"
#include "SEvent.h"


class EzDevice;


class EzApplication
{
	EzDeclareRootRTTI(EzApplication);

public:

	static EzApplication* ms_pApplication;
	static EzApplication* create();
	static bool postEvent(SEvent event); 

	virtual bool preinitialize(){ return true; }
	virtual	bool initialize() = 0;
	virtual	void excute() = 0;
	
	inline EzDevice* getDevice() { return m_pDevice; }
	inline void setDevice(EzDevice* pDevice) { m_pDevice = pDevice; }

	virtual	void destory() { EzDelete this; }

protected:
	virtual	void OnUserEvent(const SEvent::SUserEvent &UserEvent) = 0;
	virtual	void OnLogEvent(const SEvent::SLogEvent &LogEvent) = 0;
	virtual	void OnKeyEvent(const SEvent::SKeyInput &KeyEvent) = 0;
	virtual	void OnMouseEvent(const SEvent::SMouseInput &MouseEvent) = 0;



	EzApplication();
	~EzApplication(void);
	

	EzDevice* m_pDevice;


};

