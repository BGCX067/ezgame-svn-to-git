#include "EzOSOperator.h"
#include "../EzCore/EzMemoryDefines.h"
#include <Windows.h>


template<> EzOSOperator* EzSingleton<EzOSOperator>::ms_pSingleton = 0;

EzOSOperator::EzOSOperator(void)
{
	SYSTEM_INFO kInfo;
	GetSystemInfo(&kInfo);
	m_uiNumLogicalProcessors = kInfo.dwNumberOfProcessors;

	LARGE_INTEGER ticksPerSec;
	QueryPerformanceFrequency(&ticksPerSec);
	m_fPCCyclesPerSecond = (float)ticksPerSec.QuadPart;

}

EzOSOperator::~EzOSOperator(void)
{
}

bool EzOSOperator::getSystemMemory(u32* Total, u32* Avail) const
{
#if defined(_EZ_WINDOWS_API_)
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (u32)(MemoryStatus.dwTotalPhys>>10);
	if (Avail)
		*Avail = (u32)(MemoryStatus.dwAvailPhys>>10);

	return true;

#elif defined(_EZ_POSIX_API_)
#if defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
	long ps = sysconf(_SC_PAGESIZE);
	long pp = sysconf(_SC_PHYS_PAGES);
	long ap = sysconf(_SC_AVPHYS_PAGES);

	if ((ps==-1)||(pp==-1)||(ap==-1))
		return false;

	if (Total)
		*Total = (u32)((ps*(long long)pp)>>10);
	if (Avail)
		*Avail = (u32)((ps*(long long)ap)>>10);
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
