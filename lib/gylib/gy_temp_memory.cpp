/*
File:   gy_temp_memory.cpp
Author: Taylor Robbins
Date:   09\25\2021
Description:
	** If you include this file it will define a global TempArena that should be
	** assigned to a memory arena for code to implicitly use as "Temporary Memory"
	
	** Alternatively you can set GetTempArena to your own function that decides where
	** TempArena should be sourced from. This function pointer is intentionally declared
	** static so that it will auto reset everytime the compilation unit (DLL) reloads.
	** If you are using your own function make sure you reset this function pointer
	** after every reload.
*/

#ifndef _GY_TEMP_MEMORY_CPP
#define _GY_TEMP_MEMORY_CPP

#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
#define GYLIB_GET_TEMP_ARENA_DEF(functionName) MemArena_t* functionName()
typedef GYLIB_GET_TEMP_ARENA_DEF(GetTempArena_f);

#if defined(GYLIB_TEMP_MEMORY_STATIC)
	static MemArena_t* TempArena = nullptr;
	static MemArena_t* GetTempArena_Stub()
	{
		return TempArena;
	}
	static GetTempArena_f* GetTempArena = GetTempArena_Stub;
#elif defined(GYLIB_HEADER_ONLY)
	extern MemArena_t* TempArena;
	MemArena_t* GetTempArena_Stub();
	extern GetTempArena_f* GetTempArena;
#else
	MemArena_t* TempArena = nullptr;
	MemArena_t* GetTempArena_Stub()
	{
		return TempArena;
	}
	GetTempArena_f* GetTempArena = GetTempArena_Stub;
#endif

// +--------------------------------------------------------------+
// |                         Alloc Macros                         |
// +--------------------------------------------------------------+
#define TempStruct(structName)          (structName*)AllocMem(GetTempArena(), sizeof(structName))
#define TempArray(structName, numItems) (structName*)AllocMem(GetTempArena(), sizeof(structName) * (numItems))
#define TempBytes(numBytes)             (u8*)AllocMem(GetTempArena(), (numBytes))

#define TempPushMark() PushMemMark(GetTempArena())
#define TempPopMark()  PopMemMark(GetTempArena())

#define TempPushMarkIf(condition) do { if (condition) { TempPushMark(); } } while(0)
#define TempPopMarkIf(condition) do { if (condition) { TempPushMark(); } } while(0)

#define TempPushMarkNotArena(memArenaPntrThatMightBeTempArena) do { if ((memArenaPntrThatMightBeTempArena) != GetTempArena()) { TempPushMark(); } } while(0)
#define TempPopMarkNotArena(memArenaPntrThatMightBeTempArena) do { if ((memArenaPntrThatMightBeTempArena) != GetTempArena()) { TempPopMark(); } } while(0)

#define TempPrint(formatString, ...) PrintInArena(GetTempArena(), (formatString), ##__VA_ARGS__)
#define TempPrintVa(resultName, resultLengthName, formatString) PrintInArenaVa(GetTempArena(), resultName, resultLengthName, formatString)
#define TempPrintStr(formatString, ...) PrintInArenaStr(GetTempArena(), formatString, ##__VA_ARGS__)

// +--------------------------------------------------------------+
// |                        Format Macros                         |
// +--------------------------------------------------------------+
#if defined(_GY_TIME_H ) && defined(_GY_STRING_H)
#ifdef GYLIB_HEADER_ONLY
	MyStr_t TempFormatRealTime(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true);
	const char* TempFormatRealTimeNt(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true);
	MyStr_t TempFormatMilliseconds(u64 milliseconds);
	const char* TempFormatMillisecondsNt(u64 milliseconds);
#else
MyStr_t TempFormatRealTime(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	return FormatRealTime(realTime, TempArena, includeDayOfWeek, includeHourMinuteSecond, includeMonthDayYear);
}
const char* TempFormatRealTimeNt(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	return FormatRealTimeNt(realTime, TempArena, includeDayOfWeek, includeHourMinuteSecond, includeMonthDayYear);
}
MyStr_t TempFormatMilliseconds(u64 milliseconds)
{
	return FormatMilliseconds(milliseconds, TempArena);
}
const char* TempFormatMillisecondsNt(u64 milliseconds)
{
	return FormatMillisecondsNt(milliseconds, TempArena);
}
#endif //GYLIB_HEADER_ONLY
#endif //defined(_GY_TIME_H ) && defined(_GY_STRING_H)

#endif //  _GY_TEMP_MEMORY_CPP

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
GYLIB_TEMP_MEMORY_STATIC
@Types
@Globals
TempArena
@Functions
#define GYLIB_GET_TEMP_ARENA_DEF(functionName)
MemArena_t* GetTempArena()
#define TempStruct(structName)
#define TempArray(structName, numItems)
#define TempBytes(numBytes)
#define TempPushMark()
#define TempPopMark()
#define TempPushMarkIf(condition)
#define TempPopMarkIf(condition)
#define TempPushMarkNotArena(memArenaPntrThatMightBeTempArena)
#define TempPopMarkNotArena(memArenaPntrThatMightBeTempArena)
#define TempPrint(formatString, ...)
#define TempPrintVa(resultName, resultLengthName, formatString)
#define TempPrintStr(formatString, ...)
MyStr_t TempFormatRealTime(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
const char* TempFormatRealTimeNt(const RealTime_t* realTime, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
MyStr_t TempFormatMilliseconds(u64 milliseconds)
const char* TempFormatMillisecondsNt(u64 milliseconds)
*/
