#pragma once

#include "EzCoreLibType.h"
#include "EzString.h"
#include <time.h>

#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

#define TIME_MAX                2147483647L
	
#define START_YEAR				2004


class EZCORE_ENTRY EzTime
{
public:
	EzTime();
	~EzTime(void);

	struct tm *gmtime(const time_t *timer, struct tm *tmbuf);
	void localtime(struct tm *tmbuf);
	EzStringc localtime();
	time_t mktime(struct tm *tmbuf);
	unsigned int  getSecond();
	void gettime(const time_t *timer, struct tm *tmbuf);

private:
	unsigned short			m_uDaylight;                  // Non-zero if daylight savings time is used
	unsigned int			m_luDstbias;                  // Offset for Daylight Saving Time
	unsigned int			m_luTimezone;                 // Difference in seconds between GMT and local time
	time_t		m_start_time;
};
