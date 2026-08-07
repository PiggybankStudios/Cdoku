/*
File:   gy_math.h
Author: Taylor Robbins
Date:   09\14\2021
Description:
	** Contains a bunch of functions that don't really belong to any other file but are useful things to have around
	** Usually this are common mathematical operations so we all this file gy_math
	
	NOTE: Include gy_lookup.h BEFORE this file to make some operations (like ReduceRatioU32) faster
*/

#ifndef _GY_MATH_H
#define _GY_MATH_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsTriangleClockwise(v2 p0, v2 p1, v2 p2);
	bool IsInsideTriangle(v2 p0, v2 p1, v2 p2, v2 test);
	bool MinNoInfinitiesR32(r32 value1, r32 value2, r32* outValue, u8* whichIsMaxOut = nullptr);
	bool MinNoInfinitiesR64(r64 value1, r64 value2, r64* outValue, u8* whichIsMaxOut = nullptr);
	bool MinNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr);
	bool MinNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr);
	bool MaxNoInfinitiesR32(r32 value1, r32 value2, r32* outValue, u8* whichIsMaxOut = nullptr);
	bool MaxNoInfinitiesR64(r64 value1, r64 value2, r64* outValue, u8* whichIsMaxOut = nullptr);
	bool MaxNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr);
	bool MaxNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr);
	u8 NumDecimalDigitsU32(u32 number);
	u8 NumDecimalDigitsU64(u64 number);
	void ReduceRatioU32(u32* num1, u32* num2);
	void ReduceRatioU64(u64* num1, u64* num2);
	v2 ClosestPointOnLine(v2 lineStart, v2 lineEnd, v2 point);
	v3 ClosestPointOnLine(v3 lineStart, v3 lineEnd, v3 point);
	r32 AngleFromVec2(v2 vector);
	r32 AngleFromVec2(r32 x, r32 y);
	u64 PowerOfTwoGreaterThanOrEqualTo(u64 value, u8* powerOut = nullptr);
#else

// +--------------------------------------------------------------+
// |                  Triangle Related Functions                  |
// +--------------------------------------------------------------+
bool IsTriangleClockwise(v2 p0, v2 p1, v2 p2)
{
	return (Vec2Dot(p2 - p0, Vec2PerpRight(p1 - p0)) >= 0);
}
bool IsInsideTriangle(v2 p0, v2 p1, v2 p2, v2 test)
{
	v2 perp0 = Vec2PerpRight(p1 - p0);
	v2 perp1 = Vec2PerpRight(p2 - p1);
	v2 perp2 = Vec2PerpRight(p0 - p2);
	if (p0.x == p1.x && test.x == p0.x && test.y >= MinR32(p0.y, p1.y) && test.y <= MaxR32(p0.y, p1.y)) { return true; }
	if (p1.x == p2.x && test.x == p1.x && test.y >= MinR32(p1.y, p2.y) && test.y <= MaxR32(p1.y, p2.y)) { return true; }
	if (p2.x == p0.x && test.x == p2.x && test.y >= MinR32(p2.y, p0.y) && test.y <= MaxR32(p2.y, p0.y)) { return true; }
	if (p0.y == p1.y && test.y == p0.y && test.x >= MinR32(p0.x, p1.x) && test.x <= MaxR32(p0.x, p1.x)) { return true; }
	if (p1.y == p2.y && test.y == p1.y && test.x >= MinR32(p1.x, p2.x) && test.x <= MaxR32(p1.x, p2.x)) { return true; }
	if (p2.y == p0.y && test.y == p2.y && test.x >= MinR32(p2.x, p0.x) && test.x <= MaxR32(p2.x, p0.x)) { return true; }
	return (
		SignOfR32(Vec2Dot(p2 - p0, perp0)) == SignOfR32(Vec2Dot(test - p0, perp0)) &&
		SignOfR32(Vec2Dot(p0 - p1, perp1)) == SignOfR32(Vec2Dot(test - p1, perp1)) &&
		SignOfR32(Vec2Dot(p1 - p2, perp2)) == SignOfR32(Vec2Dot(test - p2, perp2))
	);
}

// +--------------------------------------------------------------+
// |                   Miscellaneous Functions                    |
// +--------------------------------------------------------------+
bool MinNoInfinitiesR32(r32 value1, r32 value2, r32* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR32(value1) && IsInfiniteR32(value2)) { return false; }
	
	if (!IsInfiniteR32(value1) && (IsInfiniteR32(value2) || value1 <= value2))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR32(value2) && (IsInfiniteR32(value1) || value2 <= value1))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else { Assert(false); return false; }
}
bool MinNoInfinitiesR64(r64 value1, r64 value2, r64* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR64(value1) && IsInfiniteR64(value2)) { return false; }
	
	if (!IsInfiniteR64(value1) && (IsInfiniteR64(value2) || value1 <= value2))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR64(value2) && (IsInfiniteR64(value1) || value2 <= value1))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else { Assert(false); return false; }
}
bool MinNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR32(value1) && IsInfiniteR32(value2) && IsInfiniteR32(value3)) { return false; }
	
	if (!IsInfiniteR32(value1) && (IsInfiniteR32(value2) || value1 <= value2) && (IsInfiniteR32(value3) || value1 <= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR32(value2) && (IsInfiniteR32(value1) || value2 <= value1) && (IsInfiniteR32(value3) || value2 <= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR32(value3) && (IsInfiniteR32(value1) || value3 <= value1) && (IsInfiniteR32(value2) || value3 <= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else { Assert(false); return false; }
}
bool MinNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR64(value1) && IsInfiniteR64(value2) && IsInfiniteR64(value3)) { return false; }
	
	if (!IsInfiniteR64(value1) && (IsInfiniteR64(value2) || value1 <= value2) && (IsInfiniteR64(value3) || value1 <= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR64(value2) && (IsInfiniteR64(value1) || value2 <= value1) && (IsInfiniteR64(value3) || value2 <= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR64(value3) && (IsInfiniteR64(value1) || value3 <= value1) && (IsInfiniteR64(value2) || value3 <= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else { Assert(false); return false; }
}

bool MaxNoInfinitiesR32(r32 value1, r32 value2, r32* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR32(value1) && IsInfiniteR32(value2)) { return false; }
	
	if (!IsInfiniteR32(value1) && (IsInfiniteR32(value2) || value1 >= value2))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR32(value2) && (IsInfiniteR32(value1) || value2 >= value1))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else { Assert(false); return false; }
}
bool MaxNoInfinitiesR64(r64 value1, r64 value2, r64* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR64(value1) && IsInfiniteR64(value2)) { return false; }
	
	if (!IsInfiniteR64(value1) && (IsInfiniteR64(value2) || value1 >= value2))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR64(value2) && (IsInfiniteR64(value1) || value2 >= value1))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else { Assert(false); return false; }
}
bool MaxNoInfinitiesR32(r32 value1, r32 value2, r32 value3, r32* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR32(value1) && IsInfiniteR32(value2) && IsInfiniteR32(value3)) { return false; }
	
	if (!IsInfiniteR32(value1) && (IsInfiniteR32(value2) || value1 >= value2) && (IsInfiniteR32(value3) || value1 >= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR32(value2) && (IsInfiniteR32(value1) || value2 >= value1) && (IsInfiniteR32(value3) || value2 >= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR32(value3) && (IsInfiniteR32(value1) || value3 >= value1) && (IsInfiniteR32(value2) || value3 >= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else { Assert(false); return false; }
}
bool MaxNoInfinitiesR64(r64 value1, r64 value2, r64 value3, r64* outValue, u8* whichIsMaxOut = nullptr)
{
	if (IsInfiniteR64(value1) && IsInfiniteR64(value2) && IsInfiniteR64(value3)) { return false; }
	
	if (!IsInfiniteR64(value1) && (IsInfiniteR64(value2) || value1 >= value2) && (IsInfiniteR64(value3) || value1 >= value3))
	{
		*outValue = value1;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 0;
		return true;
	}
	else if (!IsInfiniteR64(value2) && (IsInfiniteR64(value1) || value2 >= value1) && (IsInfiniteR64(value3) || value2 >= value3))
	{
		*outValue = value2;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 1;
		return true;
	}
	else if (!IsInfiniteR64(value3) && (IsInfiniteR64(value1) || value3 >= value1) && (IsInfiniteR64(value2) || value3 >= value2))
	{
		*outValue = value3;
		if (whichIsMaxOut != nullptr) *whichIsMaxOut = 2;
		return true;
	}
	else { Assert(false); return false; }
}

u8 NumDecimalDigitsU32(u32 number)
{
	//max is 4,294,967,295 or 4294967295
	if (number <         10UL) { return 1; }
	if (number <        100UL) { return 2; }
	if (number <       1000UL) { return 3; } //thousand
	if (number <      10000UL) { return 4; }
	if (number <     100000UL) { return 5; }
	if (number <    1000000UL) { return 6; } //million
	if (number <   10000000UL) { return 7; }
	if (number <  100000000UL) { return 8; }
	if (number < 1000000000UL) { return 9; } //billion
	return 10;
}
u8 NumDecimalDigitsU64(u64 number)
{
	//max is 18,446,744,073,709,551,616 or 18446744073709551616
	if (number <                   10ULL) { return  1; }
	if (number <                  100ULL) { return  2; }
	if (number <                 1000ULL) { return  3; } //thousand
	if (number <                10000ULL) { return  4; }
	if (number <               100000ULL) { return  5; }
	if (number <              1000000ULL) { return  6; } //million
	if (number <             10000000ULL) { return  7; }
	if (number <            100000000ULL) { return  8; }
	if (number <           1000000000ULL) { return  9; } //billion
	if (number <          10000000000ULL) { return 10; }
	if (number <         100000000000ULL) { return 11; }
	if (number <        1000000000000ULL) { return 12; } //trillion
	if (number <       10000000000000ULL) { return 13; }
	if (number <      100000000000000ULL) { return 14; }
	if (number <     1000000000000000ULL) { return 15; } //quadrillion
	if (number <    10000000000000000ULL) { return 16; }
	if (number <   100000000000000000ULL) { return 17; }
	if (number <  1000000000000000000ULL) { return 18; } //quintillion
	if (number < 10000000000000000000ULL) { return 19; }
	return 20;
}

void ReduceRatioU32(u32* num1, u32* num2)
{
	NotNull2(num1, num2);
	u32 result1 = *num1;
	u32 result2 = *num2;
	
	while (result1 > 1 && result2 > 1)
	{
		bool foundDivisor = false;
		#if defined(_GY_LOOKUP_H)
		u32 pIndex = 0;
		for (pIndex = 0; pIndex < PrimeNumbersU32_Count && PrimeNumbersU32[pIndex] < result1 && PrimeNumbersU32[pIndex] < result2; pIndex++)
		{
			u32 divisor = PrimeNumbersU32[pIndex];
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		if (!foundDivisor && pIndex >= PrimeNumbersU32_Count)
		{
			//fallback to regular testing
			u32 lastPrimeTested = PrimeNumbersU32[PrimeNumbersU32_Count-1];
			for (u32 divisor = lastPrimeTested+1; divisor < result1 && divisor < result2; divisor++)
			{
				if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
				{
					result1 /= divisor;
					result2 /= divisor;
					foundDivisor = true;
					break;
				}
			}
		}
		#else //!defined(_GY_LOOKUP_H)
		for (u32 divisor = 2; divisor < result1 && divisor < result2; divisor++)
		{
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		#endif
		
		if (!foundDivisor) { break; }
	}
	
	*num1 = result1;
	*num2 = result2;
}
void ReduceRatioU64(u64* num1, u64* num2)
{
	NotNull2(num1, num2);
	u64 result1 = *num1;
	u64 result2 = *num2;
	
	while (result1 > 1 && result2 > 1)
	{
		bool foundDivisor = false;
		#if defined(_GY_LOOKUP_H)
		u32 pIndex = 0;
		for (pIndex = 0; pIndex < PrimeNumbersU32_Count && PrimeNumbersU32[pIndex] < result1 && PrimeNumbersU32[pIndex] < result2; pIndex++)
		{
			u64 divisor = PrimeNumbersU32[pIndex];
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		if (!foundDivisor && pIndex >= PrimeNumbersU32_Count)
		{
			//fallback to regular testing
			u32 lastPrimeTested = PrimeNumbersU32[PrimeNumbersU32_Count-1];
			for (u64 divisor = lastPrimeTested+1; divisor < result1 && divisor < result2; divisor++)
			{
				if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
				{
					result1 /= divisor;
					result2 /= divisor;
					foundDivisor = true;
					break;
				}
			}
		}
		#else //!defined(_GY_LOOKUP_H)
		for (u64 divisor = 2; divisor < result1 && divisor < result2; divisor++)
		{
			if ((result1 % divisor) == 0 && (result2 % divisor) == 0)
			{
				result1 /= divisor;
				result2 /= divisor;
				foundDivisor = true;
				break;
			}
		}
		#endif
		
		if (!foundDivisor) { break; }
	}
	
	*num1 = result1;
	*num2 = result2;
}

v2 ClosestPointOnLine(v2 lineStart, v2 lineEnd, v2 point)
{
	r32 lineLength = Vec2Length(lineEnd - lineStart);
	v2 lineNorm = (lineEnd - lineStart) / lineLength;
	r32 dot = Vec2Dot(lineNorm, point - lineStart);
	if (dot <= 0) { return lineStart; }
	if (dot >= lineLength) { return lineEnd; }
	return lineStart + (lineNorm * dot);
}
v3 ClosestPointOnLine(v3 lineStart, v3 lineEnd, v3 point)
{
	r32 lineLength = Vec3Length(lineEnd - lineStart);
	v3 lineNorm = (lineEnd - lineStart) / lineLength;
	r32 dot = Vec3Dot(lineNorm, point - lineStart);
	if (dot <= 0) { return lineStart; }
	if (dot >= lineLength) { return lineEnd; }
	return lineStart + (lineNorm * dot);
}

r32 AngleFromVec2(v2 vector)
{
	return AngleFixR32(AtanR32(vector.y, vector.x));
}
r32 AngleFromVec2(r32 x, r32 y)
{
	return AngleFixR32(AtanR32(y, x));
}

u64 PowerOfTwoGreaterThanOrEqualTo(u64 value, u8* powerOut = nullptr)
{
	if (value > 0x8000000000000000) { return 0xFFFFFFFFFFFFFFFF; }
	u8 power = 0;
	u64 result = 1;
	while (result < value)
	{
		result *= 2;
		power++;
	}
	SetOptionalOutPntr(powerOut, power);
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_MATH_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
bool IsTriangleClockwise(v2 p0, v2 p1, v2 p2)
bool IsInsideTriangle(v2 p0, v2 p1, v2 p2, v2 test)
bool MinNoInfinitiesR32(r32 value1, r32 value2, r32 value3 = 0, r32* outValue, u8* whichIsMaxOut = nullptr)
bool MinNoInfinitiesR64(r64 value1, r64 value2, r64 value3 = 0, r64* outValue, u8* whichIsMaxOut = nullptr)
bool MaxNoInfinitiesR32(r32 value1, r32 value2, r32 value3 = 0, r32* outValue, u8* whichIsMaxOut = nullptr)
bool MaxNoInfinitiesR64(r64 value1, r64 value2, r64 value3 = 0, r64* outValue, u8* whichIsMaxOut = nullptr)
u8 NumDecimalDigitsU32(u32 number)
u8 NumDecimalDigitsU64(u64 number)
void ReduceRatioU32(u32* num1, u32* num2)
void ReduceRatioU64(u64* num1, u64* num2)
v2 ClosestPointOnLine(v2 lineStart, v2 lineEnd, v2 point)
r32 AngleFromVec2(v2 vector)
r32 AngleFromVec2(r32 x, r32 y)
u64 PowerOfTwoGreaterThanOrEqualTo(u64 value, u8* powerOut = nullptr)
*/
