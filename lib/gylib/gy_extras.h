/*
File:   gy_extras.h
Author: Taylor Robbins
Date:   10\07\2022
Description:
	** Serves as a somewhat temporary home for various functions that rely on
	** multiple gy_ files and would cause dependency loops if included in
	** their "proper" file
*/

#ifndef _GY_EXTRAS_H
#define _GY_EXTRAS_H

//TODO: This probably needs some #includes
#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_intrinsics.h"
#include "gy_vectors.h"
#include "gy_rectangles.h"
#include "gy_random.h"

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	v2 GetBlueNoisePosInRec(u64 seed, rec bounds, u64 numOfPositions, u64 index);
	void TwoPassPrint(char* resultPntr, u64 resultLength, u64* currentByteIndex, const char* formatString, ...);
	r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0);
	r32 OscillatePhaseBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0);
	r32 OscillateSawBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0);
	MyStr_t GetElapsedString(MemArena_t* memArena, u64 timespanInSecs);
	r32 AnimateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0);
	r32 AnimateAndPauseBy(u64 timeSource, r32 min, r32 max, u64 animationTime, u64 pauseTime, u64 offset = 0);
	u64 AnimateByU64(u64 timeSource, u64 min, u64 max, u64 periodMs, u64 offset = 0);
	u64 AnimateAndPauseByU64(u64 timeSource, u64 min, u64 max, u64 animationTime, u64 pauseTime, u64 offset = 0, bool useFirstFrameForPause = false);
	u64 TimeSinceBy(u64 programTime, u64 programTimeSnapshot);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
v2 GetBlueNoisePosInRec(u64 seed, rec bounds, u64 numOfPositions, u64 index)
{
	Assert(numOfPositions > 0);
	Assert(numOfPositions <= 0x7FFFFFFF); //TODO: Honestly this just breaks down for really large numbers because numRows*numCols can be bigger than u32 max
	if (bounds.width == 0 || bounds.height == 0) { return bounds.topLeft; }
	if (index >= numOfPositions) { index = (index % numOfPositions); }
	RandomSeries_t tempSeries;
	CreateRandomSeries(&tempSeries);
	SeedRandomSeriesU64(&tempSeries, seed);
	StepRandomSeries(&tempSeries, index*2);
	
	r32 aspectRatio = bounds.width / bounds.height;
	v2i arraySize = Vec2i_Zero;
	if (aspectRatio >= 1.0f) //wide bounds
	{
		u32 numRows = (u32)FloorR32i(SqrtR32((r32)numOfPositions));
		if (numRows < 1) { numRows = 1; }
		while (numRows > 1 && (((r32)numOfPositions / (r32)(numRows-1)) / (r32)(numRows-1)) < aspectRatio)
		{
			numRows--;
		}
		arraySize = NewVec2i(CeilR32i((r32)numOfPositions / (r32)numRows), (i32)numRows);
	}
	else //tall bounds
	{
		u32 numColumns = (u32)FloorR32i(SqrtR32((r32)numOfPositions));
		if (numColumns < 1) { numColumns = 1; }
		while (numColumns > 1 && ((r32)(numColumns-1) / ((r32)numOfPositions / (r32)(numColumns-1))) > aspectRatio)
		{
			numColumns--;
		}
		arraySize = NewVec2i((i32)numColumns, CeilR32i((r32)numOfPositions / (r32)numColumns));
	}
	
	u32 actualIndex = (u32)index;
	i32 numEmptySpots = (arraySize.width * arraySize.height) - (i32)numOfPositions;
	if (numEmptySpots > 0)
	{
		i32 skipPeriod = CeilR32i((r32)(arraySize.width * arraySize.height) / (r32)numEmptySpots);
		// if (index == 0) { MyLibPrintLine_D("%d emptySpots, %d skipPeriod", numEmptySpots, skipPeriod); }
		if (skipPeriod > 0) { actualIndex += (u32)((index + skipPeriod/2) / skipPeriod); }
	}
	
	// MyLibPrintLine_D("Array Size for %.1fx%.1f box and %u points: (%d, %d)", bounds.width, bounds.height, numOfPositions, arraySize.width, arraySize.height);
	// return bounds.topLeft + bounds.size/2;
	
	v2 cellSize = Vec2Divide(bounds.size, ToVec2(arraySize));
	v2i gridPos = NewVec2i((i32)(actualIndex % arraySize.width), (i32)(actualIndex / arraySize.width));
	v2 offset = Vec2Multiply(ToVec2(gridPos), cellSize) + cellSize/2;
	
	v2 jitter = NewVec2(GetRandR32(&tempSeries)*2 - 1.0f, GetRandR32(&tempSeries)*2 - 0.5f);
	jitter = Vec2Multiply(jitter, cellSize/4);
	offset += jitter;
	
	return bounds.topLeft + offset;
}

void TwoPassPrint(char* resultPntr, u64 resultLength, u64* currentByteIndex, const char* formatString, ...)
{
	Assert_(resultPntr == nullptr || resultLength > 0);
	NotNull_(currentByteIndex);
	NotNull_(formatString);
	
	u64 printSize = 0;
	va_list args;
	
	va_start(args, formatString);
	int printResult = PrintVa_Measure(formatString, args);
	va_end(args);
	
	if (printResult >= 0)
	{
		printSize = (u64)printResult;
		if (resultPntr != nullptr)
		{
			Assert_(*currentByteIndex <= resultLength);
			u64 spaceLeft = resultLength - *currentByteIndex;
			Assert_(printSize <= spaceLeft);
			va_start(args, formatString);
			PrintVa_Print(formatString, args, &resultPntr[*currentByteIndex], printResult);
			va_end(args);
		}
	}
	else
	{
		//Print error. Use the formatString as a stand-in to indicate an error has occurred in the print formatting
		printSize = MyStrLength64(formatString);
		if (resultPntr != nullptr)
		{
			Assert_(*currentByteIndex <= resultLength);
			u64 spaceLeft = resultLength - *currentByteIndex;
			Assert_(printSize <= spaceLeft);
			MyMemCopy(&resultPntr[*currentByteIndex], formatString, printSize);
		}
	}
	
	*currentByteIndex += printSize;
}

// +--------------------------------------------------------------+
// |                     Time Related Helpers                     |
// +--------------------------------------------------------------+
r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
{
	r32 lerpValue = (SinR32((((timeSource + offset) % periodMs) / (r32)periodMs) * 2*Pi32) + 1.0f) / 2.0f;
	return min + (max - min) * lerpValue;
}

r32 OscillatePhaseBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
{
	r32 lerpValue = (SawR32((((timeSource + offset) % periodMs) / (r32)periodMs) * 2*Pi32) + 1.0f) / 2.0f;
	lerpValue = Ease(EasingStyle_CubicOut, lerpValue);
	return min + (max - min) * lerpValue;
}

r32 OscillateSawBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
{
	r32 lerpValue = (SawR32((((timeSource + offset) % periodMs) / (r32)periodMs) * 2*Pi32) + 1.0f) / 2.0f;
	return min + (max - min) * lerpValue;
}

MyStr_t GetElapsedString(MemArena_t* memArena, u64 timespanInSecs)
{
	NotNull(memArena);
	MyStr_t result = MyStr_Empty;
	
	u64 numDays = (u64)(timespanInSecs/(60*60*24));
	u64 numHours = (u64)((timespanInSecs/(60*60)) - (numDays*24));
	u64 numMinutes = (u64)((timespanInSecs/60) - (numDays*60*24) - (numHours*60));
	u64 numSeconds = (u64)((timespanInSecs) - (numDays*60*60*24) - (numHours*60*60) - (numMinutes*60));
	if (numDays > 0)
	{
		result = PrintInArenaStr(memArena, "%llud %lluh %llum %llus", numDays, numHours, numMinutes, numSeconds);
	}
	else if (numHours > 0)
	{
		result = PrintInArenaStr(memArena, "%lluh %llum %llus", numHours, numMinutes, numSeconds);
	}
	else if (numMinutes > 0)
	{
		result = PrintInArenaStr(memArena, "%llum %llus", numMinutes, numSeconds);
	}
	else
	{
		result = PrintInArenaStr(memArena, "%llus", numSeconds);
	}
	
	return result;
}

r32 AnimateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
{
	r32 lerpValue = ((timeSource + offset) % periodMs) / (r32)periodMs;
	return min + (max - min) * lerpValue;
}

r32 AnimateAndPauseBy(u64 timeSource, r32 min, r32 max, u64 animationTime, u64 pauseTime, u64 offset = 0)
{
	u64 lerpIntValue = (u64)((timeSource + offset) % (animationTime + pauseTime));
	if (lerpIntValue >= animationTime) { lerpIntValue = animationTime; }
	return min + (max - min) * (lerpIntValue / (r32)animationTime);
}

u64 AnimateByU64(u64 timeSource, u64 min, u64 max, u64 periodMs, u64 offset = 0)
{
	Assert(periodMs > 0);
	if (min == max) { return min; }
	bool reversed = false;
	if (max < min) { SWAP_VARIABLES(u64, min, max); reversed = true; }
	u64 bucketTime = periodMs / (max-min);
	u64 inLoopTime = ((timeSource + offset) % periodMs);
	if (reversed) { inLoopTime = periodMs-1 - inLoopTime; }
	u64 result = ClampU64(inLoopTime / bucketTime, min, max-1);
	return result;
}

u64 AnimateAndPauseByU64(u64 timeSource, u64 min, u64 max, u64 animationTime, u64 pauseTime, u64 offset = 0, bool useFirstFrameForPause = false)
{
	Assert(animationTime > 0);
	if (min == max) { return min; }
	bool reversed = false;
	if (max < min) { SWAP_VARIABLES(u64, min, max); reversed = true; }
	u64 bucketTime = animationTime / (max - min);
	u64 inLoopTime = ((timeSource + offset) % (animationTime + pauseTime));
	if (inLoopTime > animationTime) { inLoopTime = (useFirstFrameForPause ? 0 : animationTime-1); }
	if (reversed) { inLoopTime = animationTime-1 - inLoopTime; }
	u64 result = ClampU64(inLoopTime / bucketTime, min, max-1);
	return result;
}

u64 TimeSinceBy(u64 programTime, u64 programTimeSnapshot)
{
	if (programTimeSnapshot <= programTime)
	{
		return programTime - programTimeSnapshot;
	}
	else 
	{
		return 0;
	}
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_EXTRAS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
v2 GetBlueNoisePosInRec(u64 seed, rec bounds, u64 numOfPositions, u64 index)
void TwoPassPrint(char* resultPntr, u64 resultLength, u64* currentByteIndex, const char* formatString, ...)
r32 OscillateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
r32 OscillatePhaseBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
r32 OscillateSawBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
MyStr_t GetElapsedString(MemArena_t* memArena, u64 timespanInSecs)
r32 AnimateBy(u64 timeSource, r32 min, r32 max, u64 periodMs, u64 offset = 0)
r32 AnimateAndPauseBy(u64 timeSource, r32 min, r32 max, u64 animationTime, u64 pauseTime, u64 offset = 0)
u64 AnimateByU64(u64 timeSource, u64 min, u64 max, u64 periodMs, u64 offset = 0)
u64 AnimateAndPauseByU64(u64 timeSource, u64 min, u64 max, u64 animationTime, u64 pauseTime, u64 offset = 0, bool useFirstFrameForPause = false)
u64 TimeSinceBy(u64 programTime, u64 programTimeSnapshot)
*/
