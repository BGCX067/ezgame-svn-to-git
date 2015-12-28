#include "EzOSOperator.h"
#include "EzMemoryDefines.h"

#ifdef WIN32
#include <Windows.h>
#endif


template<> EzOSOperator* EzSingleton<EzOSOperator>::ms_pSingleton = 0;

#ifdef WIN32
LARGE_INTEGER	EzOSOperator::m_freq;
LARGE_INTEGER	EzOSOperator::m_initial;
#endif

EzOSOperator::EzOSOperator(void)
{
	SYSTEM_INFO kInfo;
	GetSystemInfo(&kInfo);
	m_uiNumLogicalProcessors = kInfo.dwNumberOfProcessors;



#ifdef WIN32
	QueryPerformanceFrequency(&m_freq);
	QueryPerformanceCounter(&m_initial);

	m_fPCCyclesPerSecond = (float)m_freq.QuadPart;

#endif

}

float EzOSOperator::getTimeSec()
{
#ifdef WIN32
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (float)((long double)(counter.QuadPart - m_initial.QuadPart) / (long double) m_freq.QuadPart);
#endif
}

EzOSOperator::~EzOSOperator(void)
{
}

unsigned int EzOSOperator::getPerformanceCounter()
{
#ifdef WIN32
	LARGE_INTEGER counter;

	QueryPerformanceCounter(&counter);
	return counter.LowPart;
#endif
}

bool EzOSOperator::getSystemMemory(unsigned int* Total, unsigned int* Avail) const
{
#ifdef WIN32
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (unsigned int)(MemoryStatus.dwTotalPhys>>10);
	if (Avail)
		*Avail = (unsigned int)(MemoryStatus.dwAvailPhys>>10);

	return true;

#elif defined(_EZ_POSIX_API_)
#if defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
	long ps = sysconf(_SC_PAGESIZE);
	long pp = sysconf(_SC_PHYS_PAGES);
	long ap = sysconf(_SC_AVPHYS_PAGES);

	if ((ps==-1)||(pp==-1)||(ap==-1))
		return false;

	if (Total)
		*Total = (unsigned int)((ps*(long long)pp)>>10);
	if (Avail)
		*Avail = (unsigned int)((ps*(long long)ap)>>10);
	return true;
#else
	// TODO: implement for non-availablity of symbols/features
	return false;
#endif
#else
	// TODO: implement for OSX 
	return false;
#endif
}

void EzOSOperator::init()
{
	EzExternalNew EzOSOperator;
}

void EzOSOperator::shutdown()
{
	EZASSERT(ms_pSingleton);
	EzExternalDelete ms_pSingleton;
	ms_pSingleton = NULL;
}
