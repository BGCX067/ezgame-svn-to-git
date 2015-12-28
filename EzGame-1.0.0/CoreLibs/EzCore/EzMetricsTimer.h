#ifndef __EZMETRICS_H_INCLUDED__
#define __EZMETRICS_H_INCLUDED__



#include "EzDebugHelper.h"
#include "EzOSOperator.h"


class EZCORE_ENTRY EzMetricsTimer
{
public:
	EzMetricsTimer(const char* pcName) :
	m_pcName(pcName),
    m_fElapsed(0.0f)
	{
		DEBUG_BREAK(!pcName);
	}

	inline const char* getName() const
	{
		return m_pcName;
	}

	inline float getElapsed() const
	{
		return m_fElapsed;
	}



protected:
    const char* m_pcName;
    float m_fElapsed;
};



template<class T> class EzTMetricsTimer : public EzMetricsTimer
{
public:

	EzTMetricsTimer(const char* pcName) : EzMetricsTimer(pcName)
	{
	}

	~EzTMetricsTimer()
	{
	}


	inline void start()
	{
		m_Timer.start();
	}

	inline void stop()
	{
		m_Timer.stop();
		m_fElapsed = m_Timer.getElapsed();
	}


protected:
    T		m_Timer;
};

class EzSystemClockTimer
{
public:
	inline float getElapsed() const
	{
		return m_fElapsed;
	}

	inline void start()
	{
		m_fStart = getTimeSec();
	}

	inline void stop()
	{
		m_fElapsed = getTimeSec() - m_fStart;
	}

protected:
    float m_fStart;
    float m_fElapsed;
};

class EzPerformanceCounterTimer
{
public:


	inline float getElapsed() const
	{
		return m_fElapsed;
	}

	inline void start()
	{
		m_uiStart = getPerformanceCounter();
	}

	inline void stop()
	{
		unsigned int uiEnd = getPerformanceCounter();

		m_fElapsed = (uiEnd - m_uiStart) / EzOSOperator::get().getPerformanceCounterHz();
	}

protected:
    unsigned int m_uiStart;
    float m_fElapsed;
};


// Typedef'd for convenience
typedef EzTMetricsTimer<EzSystemClockTimer>				EzMetricsClockTimer;
typedef EzTMetricsTimer<EzPerformanceCounterTimer>		EzMetricsPCTimer;


#define PerfomanceCheckStart(PerfomanceName, Timer) \
	static GameEasy::Timer* pPerformanceTimer = EzNew GameEasy::Timer(PerfomanceName);\
	pPerformanceTimer->start();


#define PerfomanceCheckEnd() \
	pPerformanceTimer->stop(); \
	information(EI_CODE_PERFOMANCE, "[%s] Total Perfomance time : %f\n", pPerformanceTimer->getName(), pPerformanceTimer->getElapsed()); 


#endif