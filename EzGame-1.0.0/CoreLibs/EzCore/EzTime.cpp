#include "EzTime.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


const char *_days[] = 
{
  "Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"
};

const char *_days_abbrev[] = 
{
  "Sun", "Mon", "Tue", "Wed", 
  "Thu", "Fri", "Sat"
};

const char *_months[] = 
{
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};

const char *_months_abbrev[] = 
{
  "Jan", "Feb", "Mar",
  "Apr", "May", "Jun",
  "Jul", "Aug", "Sep",
  "Oct", "Nov", "Dec"
};

const int _ytab[2][12] = 
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};



EzTime::EzTime()
{
 
	TIME_ZONE_INFORMATION tzi;

	// get Greenwich mean time of this system 
	GetTimeZoneInformation(&tzi);
	
	m_luTimezone = (-tzi.Bias) * 60;

	struct tm tmbuf3;

	ZeroMemory(&tmbuf3, sizeof(tm));
	tmbuf3.tm_mday = 1;
	tmbuf3.tm_year = START_YEAR - 1900;

	m_uDaylight = 0;
	m_luDstbias = 0;

	m_start_time = mktime(&tmbuf3);
}

EzTime::~EzTime(void)
{
}

struct tm *EzTime::gmtime(const time_t *timer, struct tm *tmbuf)
{
  time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long) time % SECS_DAY;
  dayno = (unsigned long) time / SECS_DAY;

  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long) YEARSIZE(year)) 
  {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) 
  {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0;

  return tmbuf;
}



void EzTime::gettime(const time_t *timer, struct tm *tmbuf)
{
	struct tm tmbuf3;
	time_t t;

	ZeroMemory(&tmbuf3, sizeof(tm));
	tmbuf3.tm_mday = 1;
	tmbuf3.tm_year = START_YEAR - 1900;

	t = *timer + mktime(&tmbuf3); 

	gmtime(&t, tmbuf);

}

unsigned int EzTime::getSecond()
{
	time_t timer;

	return (unsigned int) (time(&timer) - m_start_time)  + m_luTimezone; 
}

EzStringc EzTime::localtime()
{
	EzStringc temp;
	struct tm tmbuf;

	localtime(&tmbuf);

	temp.format("%d %d-%d %d:%d:%d", 1900 + tmbuf.tm_year, tmbuf.tm_mon, tmbuf.tm_mday, 
		tmbuf.tm_hour, tmbuf.tm_min, tmbuf.tm_sec);

	return temp;


}
 
void EzTime::localtime(struct tm *tmbuf)
{
	time_t t, timer;

  	time(&timer); 

  	t = timer + m_luTimezone;
  	gmtime(&t, tmbuf);
}

time_t EzTime::mktime(struct tm *tmbuf)
{
    long day, year;
	int tm_year;
	int yday, month;
	/*unsigned*/ long seconds;
	int overflow;
	long dst;

	tmbuf->tm_min += tmbuf->tm_sec / 60;
	tmbuf->tm_sec %= 60;
	if (tmbuf->tm_sec < 0) 
	{
		tmbuf->tm_sec += 60;
		tmbuf->tm_min--;
	}
	
	tmbuf->tm_hour += tmbuf->tm_min / 60;
	tmbuf->tm_min = tmbuf->tm_min % 60;

	if (tmbuf->tm_min < 0) 
	{
		tmbuf->tm_min += 60;
		tmbuf->tm_hour--;
	}
	
	day = tmbuf->tm_hour / 24;
	tmbuf->tm_hour= tmbuf->tm_hour % 24;
  
	if (tmbuf->tm_hour < 0) 
	{
        tmbuf->tm_hour += 24;
		day--;
	}

	tmbuf->tm_year += tmbuf->tm_mon / 12;
	tmbuf->tm_mon %= 12;

	if (tmbuf->tm_mon < 0) 
	{
		tmbuf->tm_mon += 12;
		tmbuf->tm_year--;
	}
    
	day += (tmbuf->tm_mday - 1);
	
	while (day < 0) 
	{
        if(--tmbuf->tm_mon < 0) 
        {
            tmbuf->tm_year--;
			tmbuf->tm_mon = 11;
		}
    
		day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
	}
  
	while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) 
	{
		day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
		if (++(tmbuf->tm_mon) == 12) 
		{
            tmbuf->tm_mon = 0;
			tmbuf->tm_year++;
        }
    }
    
	tmbuf->tm_mday = day + 1;
	year = EPOCH_YR;
	
	if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
  
	seconds = 0;
	day = 0;                      // Means days since day 0 now
	overflow = 0;

  // Assume that when day becomes negative, there will certainly
  // be overflow on seconds.
  // The check for overflow needs not to be done for leapyears
  // divisible by 400.
  // The code only works when year (1970) is not a leapyear.
	tm_year = tmbuf->tm_year + YEAR0;

	if (TIME_MAX / 365 < tm_year - year) overflow++;
	
	day = (tm_year - year) * 365;

    if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon)
	{
        yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}

    yday += (tmbuf->tm_mday - 1);
  
	if (day + yday < 0) overflow++;
    
	day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)

	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

	if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
	seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	if (((m_luTimezone > 0) && (TIME_MAX - m_luTimezone <(unsigned int) seconds))
		  || ((m_luTimezone < 0) && ( seconds < -m_luTimezone)))
			  overflow++;
	//  seconds += m_luTimezone;

	if (tmbuf->tm_isdst)
		dst = m_luDstbias;
	else 
	    dst = 0;

	if (dst > seconds) overflow++;        // dst is always non-negative
	
	seconds -= dst;

	if(overflow) return (time_t) -1;

	if ((time_t) seconds != seconds) return (time_t) -1;
	
	return (time_t) seconds;

}
