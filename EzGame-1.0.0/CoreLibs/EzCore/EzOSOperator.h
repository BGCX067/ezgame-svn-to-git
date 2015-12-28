#pragma once

#include "../EzCore/EzSingleton.h"




class EZCORE_ENTRY EzOSOperator : public EzSingleton<EzOSOperator>
{
public:
	static void init();
	static void shutdown();

	float getPerformanceCounterHz() const;
	bool getSystemMemory(unsigned int* Total, unsigned int* Avail) const;
	static float getTimeSec();
	static unsigned int getPerformanceCounter();
	static unsigned long getCurrentThreadId();
protected:
	EzOSOperator(void);
	~EzOSOperator(void);

	unsigned int		m_uiNumLogicalProcessors;
	float	m_fPCCyclesPerSecond;

#ifdef WIN32
	static LARGE_INTEGER	m_freq;
	static LARGE_INTEGER	m_initial;
#endif

};

inline float EzOSOperator::getPerformanceCounterHz() const
{
	return m_fPCCyclesPerSecond;
}

inline unsigned long EzOSOperator::getCurrentThreadId()
{
	 return (unsigned long)GetCurrentThreadId();
}


inline unsigned long getCurrentThreadId()
{
    return EzOSOperator::getCurrentThreadId();
}


inline float getTimeSec()
{
	return EzOSOperator::getTimeSec();
}


inline unsigned int getPerformanceCounter()
{

}
