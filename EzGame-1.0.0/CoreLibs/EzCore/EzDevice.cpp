#include "EzDevice.h"

namespace GameEasy
{

EzImplementRootRTTI(EzDevice);


EzDevice* EzDevice::m_pGlobal = NULL;

EzDevice::EzDevice(void)
: m_pApplication(0)
{
	m_pGlobal = this;

	QueryPerformanceFrequency(&m_freq);
    QueryPerformanceCounter(&m_initial);

    m_fPCCyclesPerSecond = (float)m_freq.QuadPart;


}

EzDevice::~EzDevice(void)
{
}

f32 EzDevice::getTimeSec()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (f32)((long double)
        (counter.QuadPart - m_initial.QuadPart) / 
        (long double) m_freq.QuadPart);
}

u32 EzDevice::getPerformanceCounter()
{
    LARGE_INTEGER counter;

    QueryPerformanceCounter(&counter);
    return counter.LowPart;
}

void EzDevice::setApplication(EzApplication* pApp)
{
	m_pApplication = pApp;

}

void EzDevice::setErrorLog(const u32 uErrcode, const c8* text)
{
	SEvent event;
	
	event.eEventType = EET_LOG_TEXT_EVENT;
	event.LogEvent.Level = LOG_ERROR;
	event.LogEvent.Text = text;
	event.LogEvent.uErrCode = uErrcode;

	postEvent(event);
}
void EzDevice::setErrorInfomation(const u32 uErrcode, const c8* text)
{
	SEvent event;
	
	event.eEventType = EET_LOG_TEXT_EVENT;
	event.LogEvent.Level = LOG_INFORMATION;
	event.LogEvent.Text = text;
	event.LogEvent.uErrCode = uErrcode;

	postEvent(event);

}

void EzDevice::setErrorWarn(const u32 uErrcode, const c8* text)
{
	SEvent event;
	
	event.eEventType = EET_LOG_TEXT_EVENT;
	event.LogEvent.Level = LOG_WARNING;
	event.LogEvent.Text = text;
	event.LogEvent.uErrCode = uErrcode;

	postEvent(event);

}


}