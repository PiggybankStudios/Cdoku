/*
File:   gy_time.h
Author: Taylor Robbins
Date:   01\07\2022
Description:
	** Holds functions that help us ask questions about real world time.
	** Mostly timestamps and conversion to human readable values and back
*/

#ifndef _GY_TIME_H
#define _GY_TIME_H

#include "gy_std.h"
#include "gy_assert.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_debug.h"

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define NUM_US_PER_MS         1000
#define NUM_MS_PER_SECOND     1000
#define NUM_SEC_PER_MINUTE    60
#define NUM_MIN_PER_HOUR      60
#define NUM_HOUR_PER_DAY      24
#define NUM_DAYS_PER_WEEK     7
#define NUM_DAYS_PER_YEAR     365
#define NUM_DAYS_PER_4YEARS   (365*4 + 1)
#define UNIX_EPOCH_DATE       1970
#define MAX_DAYS_IN_MONTH     31

#define NUM_US_PER_SECOND ((u64)NUM_MS_PER_SECOND  * (u64)NUM_US_PER_MS)       //     1,000,000 (i32 required)
#define NUM_US_PER_MINUTE ((u64)NUM_SEC_PER_MINUTE * (u64)NUM_US_PER_SECOND)   //    60,000,000 (i32 required)
#define NUM_US_PER_HOUR   ((u64)NUM_MIN_PER_HOUR   * (u64)NUM_US_PER_MINUTE)   // 3,600,000,000 (u32 required)
#define NUM_US_PER_DAY    ((u64)NUM_HOUR_PER_DAY   * (u64)NUM_US_PER_HOUR)     //86,400,000,000 (i64 required)

#define NUM_MS_PER_MINUTE ((u64)NUM_SEC_PER_MINUTE * (u64)NUM_MS_PER_SECOND)   //        60,000 (u16 required)
#define NUM_MS_PER_HOUR   ((u64)NUM_MIN_PER_HOUR   * (u64)NUM_MS_PER_MINUTE)   //     3,600,000 (i32 required)
#define NUM_MS_PER_DAY    ((u64)NUM_HOUR_PER_DAY   * (u64)NUM_MS_PER_HOUR)     //    86,400,000 (i32 required)
#define NUM_MS_PER_WEEK   ((u64)NUM_DAYS_PER_WEEK  * (u64)NUM_MS_PER_DAY)      //   604,800,000 (i32 required)
#define NUM_MS_PER_YEAR   ((u64)NUM_DAYS_PER_YEAR  * (u64)NUM_MS_PER_DAY)      //31,536,000,000 (i64 required)

#define NUM_SEC_PER_HOUR   ((u64)NUM_MIN_PER_HOUR  * (u64)NUM_SEC_PER_MINUTE)  //     3,600 (i16 required)
#define NUM_SEC_PER_DAY    ((u64)NUM_HOUR_PER_DAY  * (u64)NUM_SEC_PER_HOUR)    //    86,400 (i32 required)
#define NUM_SEC_PER_WEEK   ((u64)NUM_DAYS_PER_WEEK * (u64)NUM_SEC_PER_DAY)     //   604,800 (i32 required)
#define NUM_SEC_PER_YEAR   ((u64)NUM_DAYS_PER_YEAR * (u64)NUM_SEC_PER_DAY)     //31,536,000 (i32 required)

#define NUM_MIN_PER_DAY     ((u64)NUM_HOUR_PER_DAY  * (u64)NUM_MIN_PER_HOUR)   //  1,440 (i16 required)
#define NUM_MIN_PER_WEEK    ((u64)NUM_DAYS_PER_WEEK * (u64)NUM_MIN_PER_DAY)    // 10,080 (i16 required)
#define NUM_MIN_PER_YEAR    ((u64)NUM_DAYS_PER_YEAR * (u64)NUM_MIN_PER_DAY)    //525,600 (i32 required)

// +--------------------------------------------------------------+
// |                       Structures/Types                       |
// +--------------------------------------------------------------+
enum Month_t
{
	Month_January = 0,
	Month_February,
	Month_March,
	Month_April,
	Month_May,
	Month_June,
	Month_July,
	Month_August,
	Month_September,
	Month_October,
	Month_November,
	Month_December,
	Month_NumMonths,
};

enum DayOfWeek_t
{
	DayOfWeek_Sunday = 0,
	DayOfWeek_Monday,
	DayOfWeek_Tuesday,
	DayOfWeek_Wednesday,
	DayOfWeek_Thursday,
	DayOfWeek_Friday,
	DayOfWeek_Saturday,
	DayOfWeek_NumDays,
};

enum TimeUnit_t
{
	TimeUnit_Milliseconds = 0,
	TimeUnit_Seconds,
	TimeUnit_Minutes,
	TimeUnit_Hours,
	TimeUnit_Days,
	TimeUnit_Months,
	TimeUnit_Years,
	TimeUnit_NumUnits,
};

struct RealTime_t
{
	u64 timestamp;
	
	u16 year;
	Month_t month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	
	bool isLeapYear;
	bool isDstActive;
	DayOfWeek_t dayOfWeek;
	u16 dayOfYear;
	u16 minuteOfDay;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsPostMeridian(u16 twentyFourHourValue);
	u16 Convert24HourTo12Hour(u16 twentyFourHourValue);
	u8 NumDaysInMonth(Month_t month, bool isLeapYear);
	const char* GetMonthStr(Month_t month);
	const char* GetDayOfWeekStr(DayOfWeek_t dayOfWeek, bool shortened = false);
	const char* GetTimeUnitStr(TimeUnit_t timeUnit, bool shortened = false);
	const char* GetDayOfMonthString(u16 dayOfMonth);
	bool IsDstActive(Month_t month, u8 day, u8 hour, DayOfWeek_t dayOfWeek);
	void ConvertTimestampToRealTime(u64 timestamp, RealTime_t* realTimeOut, bool applyDst = true);
#else

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
bool IsPostMeridian(u16 twentyFourHourValue)
{
	return (twentyFourHourValue >= 12);
}

u16 Convert24HourTo12Hour(u16 twentyFourHourValue)
{
	if (twentyFourHourValue == 0)       { return 12; }
	else if (twentyFourHourValue <= 12) { return twentyFourHourValue; }
	else                                { return (twentyFourHourValue - 12); }
}

u8 NumDaysInMonth(Month_t month, bool isLeapYear)
{
	switch (month)
	{
		case Month_January:   return 31;
		case Month_February:  return (u8)(isLeapYear ? 29 : 28);
		case Month_March:     return 31;
		case Month_April:     return 30;
		case Month_May:       return 31;
		case Month_June:      return 30;
		case Month_July:      return 31;
		case Month_August:    return 31;
		case Month_September: return 30;
		case Month_October:   return 31;
		case Month_November:  return 30;
		case Month_December:  return 31;
		default: return 30; //shouldn't happen
	}
}

// +--------------------------------------------------------------+
// |                    String Const Functions                    |
// +--------------------------------------------------------------+
const char* GetMonthStr(Month_t month)
{
	switch (month)
	{
		case Month_January:   return "January";
		case Month_February:  return "February";
		case Month_March:     return "March";
		case Month_April:     return "April";
		case Month_May:       return "May";
		case Month_June:      return "June";
		case Month_July:      return "July";
		case Month_August:    return "August";
		case Month_September: return "September";
		case Month_October:   return "October";
		case Month_November:  return "November";
		case Month_December:  return "December";
		default: return "Unknown";
	}
}

const char* GetDayOfWeekStr(DayOfWeek_t dayOfWeek, bool shortened = false)
{
	switch (dayOfWeek)
	{
		case DayOfWeek_Sunday:    return (shortened ? "Sun" : "Sunday");
		case DayOfWeek_Monday:    return (shortened ? "Mon" : "Monday");
		case DayOfWeek_Tuesday:   return (shortened ? "Tue" : "Tuesday");
		case DayOfWeek_Wednesday: return (shortened ? "Wed" : "Wednesday");
		case DayOfWeek_Thursday:  return (shortened ? "Thu" : "Thursday");
		case DayOfWeek_Friday:    return (shortened ? "Fri" : "Friday");
		case DayOfWeek_Saturday:  return (shortened ? "Sat" : "Saturday");
		default: return "Unknown";
	}
}

const char* GetTimeUnitStr(TimeUnit_t timeUnit, bool shortened = false)
{
	switch (timeUnit)
	{
		case TimeUnit_Milliseconds: return (shortened ? "ms"  : "Milliseconds");
		case TimeUnit_Seconds:      return (shortened ? "sec" : "Seconds");
		case TimeUnit_Minutes:      return (shortened ? "min" : "Minutes");
		case TimeUnit_Hours:        return (shortened ? "hr"  : "Hours");
		case TimeUnit_Days:         return (shortened ? "day" : "Days");
		case TimeUnit_Months:       return (shortened ? "mth" : "Months");
		case TimeUnit_Years:        return (shortened ? "yr"  : "Years");
		default: return "Unknown";
	}
}

const char* GetDayOfMonthString(u16 dayOfMonth)
{
	switch (dayOfMonth)
	{
		case 0:  return "1st";
		case 1:  return "2nd";
		case 2:  return "3rd";
		case 3:  return "4th";
		case 4:  return "5th";
		case 5:  return "6th";
		case 6:  return "7th";
		case 7:  return "8th";
		case 8:  return "9th";
		case 9:  return "10th";
		case 10: return "11th";
		case 11: return "12th";
		case 12: return "13th";
		case 13: return "14th";
		case 14: return "15th";
		case 15: return "16th";
		case 16: return "17th";
		case 17: return "18th";
		case 18: return "19th";
		case 19: return "20th";
		case 20: return "21st";
		case 21: return "22nd";
		case 22: return "23rd";
		case 23: return "24th";
		case 24: return "25th";
		case 25: return "26th";
		case 26: return "27th";
		case 27: return "28th";
		case 28: return "29th";
		case 29: return "30th";
		case 30: return "31st";
		default: return "Unk";
	};
}

// +--------------------------------------------------------------+
// |                    Complicated Functions                     |
// +--------------------------------------------------------------+
//DST = Daylight Savings Time
bool IsDstActive(Month_t month, u8 day, u8 hour, DayOfWeek_t dayOfWeek)
{
	//NOTE: This function is only useful for the United States or countries that start and stop DST at the same time as the US.
	bool result = false;
	
	//DST starts on the second Sunday in March at 2:00am (so jump from 2:00am to 3:00am)
	if (month == Month_March)
	{
		u8 numSundays = 0;
		u8 dow = (u8)dayOfWeek;
		i32 dIndex = day;
		while (dIndex >= 0)
		{
			if (dow == DayOfWeek_Sunday)
			{
				numSundays++;
			}
			if (dow == DayOfWeek_Sunday) { dow = DayOfWeek_Saturday; }
			else { dow--; }
			dIndex--;
		}
		
		if (numSundays >= 2)
		{
			if (numSundays == 2 && dayOfWeek == DayOfWeek_Sunday)
			{
				result = (hour >= 2); //Starts at 2:00am on this Sunday
			}
			else
			{
				result = true;
			}
		}
	}
	//DST ends on the first Sunday in November at 1:00am original time (so jump from 2:00am back to 1:00am)
	else if (month == Month_November)
	{
		result = true;
		
		u8 numSundays = 0;
		u8 dow = (u8)dayOfWeek;
		i32 dIndex = day;
		while (dIndex >= 0)
		{
			if (dow == DayOfWeek_Sunday)
			{
				numSundays++;
			}
			if (dow == DayOfWeek_Sunday) { dow = DayOfWeek_Saturday; }
			else { dow--; }
			dIndex--;
		}
		
		if (numSundays >= 1)
		{
			if (numSundays == 1 && dayOfWeek == DayOfWeek_Sunday)
			{
				result = (hour < 1); //Ends at 1:00am on this Sunday
			}
			else
			{
				result = false;
			}
		}
	}
	//All the months in between March and November are DST active months
	else if (month > Month_March && month < Month_November)
	{
		result = true;
	}
	
	//All other months are normal time
	return result;
}

//timestamp is number of seconds since the unix epoch (Jan 1st 1970 at 0:00.00)
void ConvertTimestampToRealTime(u64 timestamp, RealTime_t* realTimeOut, bool applyDst = true)
{
	NotNull(realTimeOut);
	ClearPointer(realTimeOut);
	realTimeOut->timestamp = timestamp;
	u64 numSecondsLeft = timestamp;
	
	// if (TimezoneDoesDst && IsDstActive(timestamp, false))
	// {
	// 	numSecondsLeft += NUM_SEC_PER_HOUR;
	// }
	
	//TODO: This doesn't properly take into account the full logic for which years
	//      are and aren't leap years (every 100? years is not a leap year I think)
	//      Every year divisible by 100 is NOT a leap year EXCEPT years divisible by 400 which ARE leap years
	//TODO: This u16 overflows in 216,000 years after 1970. Maybe we should fix that?
	u16 numFourYears = (u16)(numSecondsLeft / (NUM_SEC_PER_DAY*NUM_DAYS_PER_4YEARS));
	realTimeOut->year = (u16)(UNIX_EPOCH_DATE + 4*numFourYears);
	numSecondsLeft -= numFourYears * (NUM_SEC_PER_DAY*NUM_DAYS_PER_4YEARS);
	
	u16 numDaysLeft = (u16)(numSecondsLeft / NUM_SEC_PER_DAY);
	numSecondsLeft -= numDaysLeft * NUM_SEC_PER_DAY;
	
	if (numDaysLeft < 365)              { realTimeOut->year += 0; numDaysLeft -= 0;           realTimeOut->isLeapYear = false; }
	else if (numDaysLeft < 365+365)     { realTimeOut->year += 1; numDaysLeft -= 365;         realTimeOut->isLeapYear = false; }
	else if (numDaysLeft < 365+365+366) { realTimeOut->year += 2; numDaysLeft -= 365+365;     realTimeOut->isLeapYear = true;  }
	else                                { realTimeOut->year += 3; numDaysLeft -= 365+365+366; realTimeOut->isLeapYear = false; }
	
	Assert(numDaysLeft <= 365);
	realTimeOut->dayOfYear = (u16)numDaysLeft;
	
	for (realTimeOut->month = Month_January; realTimeOut->month <= Month_NumMonths; realTimeOut->month = (Month_t)((u8)realTimeOut->month + 1))
	{
		Assert(realTimeOut->month < Month_NumMonths);
		u8 numMonthDays = NumDaysInMonth(realTimeOut->month, realTimeOut->isLeapYear);
		if (numDaysLeft < numMonthDays)
		{
			break;
		}
		else
		{
			numDaysLeft -= numMonthDays;
		}
	}
	
	Assert(numDaysLeft < MAX_DAYS_IN_MONTH);
	realTimeOut->day = (u8)numDaysLeft;
	
	realTimeOut->hour = (u8)(numSecondsLeft / NUM_SEC_PER_HOUR);
	numSecondsLeft -= realTimeOut->hour * NUM_SEC_PER_HOUR;
	
	realTimeOut->minute = (u8)(numSecondsLeft / NUM_SEC_PER_MINUTE);
	numSecondsLeft -= realTimeOut->minute * NUM_SEC_PER_MINUTE;
	
	realTimeOut->second = (u8)numSecondsLeft;
	
	realTimeOut->minuteOfDay = (u16)((realTimeOut->hour*NUM_MIN_PER_HOUR) + realTimeOut->minute);
	Assert(realTimeOut->minuteOfDay < NUM_MIN_PER_DAY);
	
	//The dayOfWeek calculation operates indipendantly on the original
	//timestamp. Jan 1st 1970 was a Thursday so we have to offset the
	//timestamp by 4 days before taking the modulas
	realTimeOut->dayOfWeek = (DayOfWeek_t)(((timestamp + NUM_SEC_PER_DAY*DayOfWeek_Thursday) % NUM_SEC_PER_WEEK) / NUM_SEC_PER_DAY);
	
	// +==============================+
	// |          Apply DST           |
	// +==============================+
	realTimeOut->isDstActive = false;
	if (applyDst)
	{
		realTimeOut->isDstActive = IsDstActive(realTimeOut->month, realTimeOut->day, realTimeOut->hour, realTimeOut->dayOfWeek);
		if (realTimeOut->isDstActive)
		{
			realTimeOut->hour++;
			realTimeOut->minuteOfDay += NUM_MIN_PER_HOUR;
			if (realTimeOut->hour >= NUM_HOUR_PER_DAY)
			{
				realTimeOut->hour -= NUM_HOUR_PER_DAY;
				realTimeOut->day++;
				
				u8 numDaysInMonth = NumDaysInMonth(realTimeOut->month, realTimeOut->isLeapYear);
				if (realTimeOut->day >= numDaysInMonth)
				{
					realTimeOut->day -= numDaysInMonth;
					realTimeOut->month = (Month_t)((u8)realTimeOut->month + 1);
					if (realTimeOut->month >= Month_NumMonths) //this shouldn't really happen
					{
						DebugAssert(false);
						realTimeOut->month = (Month_t)((u8)realTimeOut->month - Month_NumMonths);
						realTimeOut->year++;
						//TODO: realTimeOut->isLeapYear might be wrong now
					}
				}
				
				realTimeOut->dayOfWeek = (DayOfWeek_t)(((u8)realTimeOut->dayOfWeek + 1) % DayOfWeek_NumDays);
				
				realTimeOut->dayOfYear++;
				u16 numDaysInYear = (u16)(realTimeOut->isLeapYear ? (NUM_DAYS_PER_YEAR+1) : NUM_DAYS_PER_YEAR);
				if (realTimeOut->dayOfYear >= numDaysInYear)
				{
					realTimeOut->dayOfYear -= (u16)numDaysInYear;
				}
			}
		}
	}
}

//TODO: Make a function that takes a year, moth, day, hour, minute, second and produces a timestamp

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_TIME_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
NUM_US_PER_MS
NUM_MS_PER_SECOND
NUM_SEC_PER_MINUTE
NUM_MIN_PER_HOUR
NUM_HOUR_PER_DAY
NUM_DAYS_PER_WEEK
NUM_DAYS_PER_YEAR
NUM_DAYS_PER_4YEARS
UNIX_EPOCH_DATE
MAX_DAYS_IN_MONTH
NUM_US_PER_SECOND
NUM_US_PER_MINUTE
NUM_US_PER_HOUR
NUM_US_PER_DAY
NUM_MS_PER_MINUTE
NUM_MS_PER_HOUR
NUM_MS_PER_DAY
NUM_MS_PER_WEEK
NUM_MS_PER_YEAR
NUM_SEC_PER_HOUR
NUM_SEC_PER_DAY
NUM_SEC_PER_WEEK
NUM_SEC_PER_YEAR
NUM_MIN_PER_DAY
NUM_MIN_PER_WEEK
NUM_MIN_PER_YEAR
Month_January
Month_February
Month_March
Month_April
Month_May
Month_June
Month_July
Month_August
Month_September
Month_October
Month_November
Month_December
Month_NumMonths
DayOfWeek_Sunday
DayOfWeek_Monday
DayOfWeek_Tuesday
DayOfWeek_Wednesday
DayOfWeek_Thursday
DayOfWeek_Friday
DayOfWeek_Saturday
DayOfWeek_NumDays
TimeUnit_Milliseconds
TimeUnit_Seconds
TimeUnit_Minutes
TimeUnit_Hours
TimeUnit_Days
TimeUnit_Months
TimeUnit_Years
TimeUnit_NumUnits
@Types
Month_t
DayOfWeek_t
TimeUnit_t
RealTime_t
@Functions
bool IsPostMeridian(u16 twentyFourHourValue)
u16 Convert24HourTo12Hour(u16 twentyFourHourValue)
u8 NumDaysInMonth(Month_t month, bool isLeapYear)
const char* GetMonthStr(Month_t month)
const char* GetDayOfWeekStr(DayOfWeek_t dayOfWeek, bool shortened = false)
const char* GetTimeUnitStr(TimeUnit_t timeUnit, bool shortened = false)
const char* GetDayOfMonthString(u16 dayOfMonth)
bool IsDstActive(Month_t month, u8 day, u8 hour, DayOfWeek_t dayOfWeek)
void ConvertTimestampToRealTime(u64 timestamp, RealTime_t* realTimeOut, bool applyDst = true)
*/
