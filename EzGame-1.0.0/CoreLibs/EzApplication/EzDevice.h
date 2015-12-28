#pragma once

#include "EzRefObject.h"
#include "EzCore.h"



class EzDevice : public EzRefObject
{
	EzDeclareRootRTTI(EzDevice);

public:
	EzDevice();
	~EzDevice(void);

	
	virtual bool run() = 0;
	virtual void sleep(unsigned int timeMs) = 0;
	virtual	void closeDevice() = 0;
	virtual void yield() = 0;

	virtual void setWindowCaption(const char* text) = 0;
	virtual void* getVideoDriver() { return NULL; }
	virtual void* getRender() { return NULL; }

};


float getTimeSec();
unsigned int getPerformanceCounter();




