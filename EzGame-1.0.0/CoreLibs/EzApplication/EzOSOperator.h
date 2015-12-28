#pragma once

#include "../EzCore/EzSingleton.h"


class EzOSOperator : public EzSingleton<EzOSOperator>
{
public:
	enum
	{
		E_WIN32 = 0,
		E_NUM_PLATFORM_IDS
	} EPLATFORMID;

	static void init();
	static void shutdown();

	f32 getPerformanceCounterHz() const;
	bool getSystemMemory(u32* Total, u32* Avail) const;

protected:
	EzOSOperator(void);
	~EzOSOperator(void);

	u32		m_uiNumLogicalProcessors;
	f32		m_fPCCyclesPerSecond;

};

inline f32 EzOSOperator::getPerformanceCounterHz() const
{
	return m_fPCCyclesPerSecond;
}

