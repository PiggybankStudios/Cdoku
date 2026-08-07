/*
File:   gy_random.h
Author: Taylor Robbins
Date:   01\07\2022
Description:
	** Holds functions that help us keep track of and define random number sequence algorithms
Wikipedia Link: https://en.wikipedia.org/wiki/List_of_random_number_generators
Other Page: https://peteroupc.github.io/random.html#Existing_RNG_APIs_in_Programming_Languages
*/

#ifndef _GY_RANDOM_H
#define _GY_RANDOM_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"

#ifndef GYLIB_DEFAULT_RANDOM_SERIES_TYPE
#define GYLIB_DEFAULT_RANDOM_SERIES_TYPE RandomSeriesType_LinearCongruential64
#endif

#define RAND_FLOAT_PRECISION_R32 8000000UL //8 million
#define RAND_FLOAT_PRECISION_R64 400000000000000ULL //400 trillion

// +--------------------------------------------------------------+
// |                  Type/Structure Definitions                  |
// +--------------------------------------------------------------+
enum RandomSeriesType_t
{
	RandomSeriesType_None = 0,
	RandomSeriesType_Fixed,
	RandomSeriesType_Incremental,
	RandomSeriesType_LinearCongruential32,   //LCG32
	RandomSeriesType_LinearCongruential64,   //LCG64
	RandomSeriesType_PermutedCongruential64, //PCG64
	RandomSeriesType_XoroShiro128,           //XS128
	RandomSeriesType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetRandomSeriesTypeStr(RandomSeriesType_t enumValue);
const char* GetRandomSeriesTypeAcronymStr(RandomSeriesType_t enumValue);
#else
const char* GetRandomSeriesTypeStr(RandomSeriesType_t enumValue)
{
	switch (enumValue)
	{
		case RandomSeriesType_None:                   return "None";
		case RandomSeriesType_Fixed:                  return "Fixed";
		case RandomSeriesType_Incremental:            return "Incremental";
		case RandomSeriesType_LinearCongruential32:   return "LinearCongruential32";
		case RandomSeriesType_LinearCongruential64:   return "LinearCongruential64";
		case RandomSeriesType_PermutedCongruential64: return "PermutedCongruential64";
		case RandomSeriesType_XoroShiro128:           return "XoroShiro128";
		default: return "Unknown";
	}
}
const char* GetRandomSeriesTypeAcronymStr(RandomSeriesType_t enumValue)
{
	switch (enumValue)
	{
		case RandomSeriesType_None:                   return "None";
		case RandomSeriesType_Fixed:                  return "Fixed";
		case RandomSeriesType_Incremental:            return "Inc";
		case RandomSeriesType_LinearCongruential32:   return "LCG32";
		case RandomSeriesType_LinearCongruential64:   return "LCG64";
		case RandomSeriesType_PermutedCongruential64: return "PGC64";
		case RandomSeriesType_XoroShiro128:           return "XS128";
		default: return "Unknown";
	}
}
#endif

struct RandomSeries_t
{
	RandomSeriesType_t type;
	bool seeded;
	u64 state;
	u64 state128[2]; //for 128-bit RNGs
	u64 defaultIncrement;
	u64 generationCount; //how many numbers have been generated since the series was seeded
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void CreateRandomSeries(RandomSeries_t* series, RandomSeriesType_t type = GYLIB_DEFAULT_RANDOM_SERIES_TYPE, u64 defaultIncrement = 1);
	void SeedRandomSeriesU32(RandomSeries_t* series, u32 seed);
	void SeedRandomSeriesU64(RandomSeries_t* series, u64 seed);
	void SeedRandomSeriesU128(RandomSeries_t* series, u64 seed1, u64 seed2);
	void StepRandomSeries(RandomSeries_t* series, u64 numberOfSteps = 1);
	u32 GetRandU32(RandomSeries_t* series);
	u32 GetRandU32(RandomSeries_t* series, u32 min, u32 max);
	u64 GetRandU64(RandomSeries_t* series);
	u64 GetRandU64(RandomSeries_t* series, u64 min, u64 max);
	r32 GetRandR32(RandomSeries_t* series);
	r32 GetRandR32(RandomSeries_t* series, r32 min, r32 max);
	r64 GetRandR64(RandomSeries_t* series);
	r64 GetRandR64(RandomSeries_t* series, r64 min, r64 max);
	u8 GetRandU8(RandomSeries_t* series);
	u8 GetRandU8(RandomSeries_t* series, u8 min, u8 max);
	u16 GetRandU16(RandomSeries_t* series);
	u16 GetRandU16(RandomSeries_t* series, u16 min, u16 max);
	i8 GetRandI8(RandomSeries_t* series);
	i8 GetRandI8(RandomSeries_t* series, i8 min, i8 max);
	i32 GetRandI32(RandomSeries_t* series);
	i32 GetRandI32(RandomSeries_t* series, i32 min, i32 max);
#else

// +--------------------------------------------------------------+
// |                           Creation                           |
// +--------------------------------------------------------------+
void CreateRandomSeries(RandomSeries_t* series, RandomSeriesType_t type = GYLIB_DEFAULT_RANDOM_SERIES_TYPE, u64 defaultIncrement = 1)
{
	NotNull_(series);
	ClearPointer(series);
	series->seeded = false;
	series->type = type;
	series->state = 0;
	series->defaultIncrement = defaultIncrement;
	series->generationCount = 0;
}

// +--------------------------------------------------------------+
// |                             Seed                             |
// +--------------------------------------------------------------+
void SeedRandomSeriesU32(RandomSeries_t* series, u32 seed)
{
	NotNull_(series);
	series->state = (u64)seed;
	series->state128[0] = (u64)seed;
	series->state128[1] = (u64)seed;
	series->generationCount = 0;
	series->seeded = true;
}
void SeedRandomSeriesU64(RandomSeries_t* series, u64 seed)
{
	NotNull_(series);
	series->state = seed;
	series->state128[0] = seed;
	series->state128[1] = seed;
	series->generationCount = 0;
	series->seeded = true;
}
void SeedRandomSeriesU128(RandomSeries_t* series, u64 seed1, u64 seed2)
{
	NotNull_(series);
	series->state = (seed1 ^ seed2);
	series->state128[0] = seed1;
	series->state128[1] = seed2;
	series->generationCount = 0;
	series->seeded = true;
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
inline u64 XS128_rotl(const u64 x, int k)
{
	return (x << k) | (x >> (64 - k));
}

// +--------------------------------------------------------------+
// |                             Step                             |
// +--------------------------------------------------------------+
void StepRandomSeries(RandomSeries_t* series, u64 numberOfSteps = 1)
{
	NotNull_(series);
	Assert_(series->type < RandomSeriesType_NumTypes);
	Assert_(series->seeded);
	
	switch (series->type)
	{
		case RandomSeriesType_Fixed:
		{
			//Fixed doesn't do anything, it just keeps a fixed number
		} break;
		case RandomSeriesType_Incremental:
		{
			if (series->state < UINT64_MAX - numberOfSteps) { series->state += numberOfSteps; }
			else { series->state = numberOfSteps - (UINT64_MAX - numberOfSteps - series->state); }
			series->generationCount += numberOfSteps;
		} break;
		case RandomSeriesType_LinearCongruential32:
		{
			//n(x+1) = n(x) * A + C modulo M
			for (u64 sIndex = 0; sIndex < numberOfSteps; sIndex++)
			{
				u64 newState = (series->state * 1103515245ULL + 12345ULL) & 0xFFFFFFFFFFFFFFFFULL;
				series->state = newState;
			}
			series->generationCount += numberOfSteps;
		} break;
		case RandomSeriesType_LinearCongruential64:
		{
			//n(x+1) = n(x) * A + C modulo M
			//Values taken from https://nuclear.llnl.gov/CNP/rng/rngman/node4.html
			for (u64 sIndex = 0; sIndex < numberOfSteps; sIndex++)
			{
				u64 newState = (series->state * 2862933555777941757ULL + 3037000493ULL) & 0xFFFFFFFFFFFFFFFFULL;
				series->state = newState;
			}
			series->generationCount += numberOfSteps;
		} break;
		case RandomSeriesType_PermutedCongruential64:
		{
			//This code comes from BYP on Handmade Network
			for (u64 sIndex = 0; sIndex < numberOfSteps; sIndex++)
			{
				#define PCG_INC 1
				u64 newState = ((series->state * 6364136223846793005ULL) + (PCG_INC|1));
				newState = (newState ^ (newState >> 22)) >> (22 + (newState >> 61));
				series->state = newState;
			}
			series->generationCount += numberOfSteps;
		} break;
		case RandomSeriesType_XoroShiro128:
		{
			// https://xoroshiro.di.unimi.it/xoroshiro128plusplus.c
			//TODO: There are jump() and long_jump() functions we could use
			u64 s0 = series->state128[0];
			u64 s1 = series->state128[1];
			series->state = XS128_rotl(s0 + s1, 17) + s0;
			s1 ^= s0;
			series->state128[0] = XS128_rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
			series->state128[1] = XS128_rotl(s1, 28); // c
		} break;
		default: Assert_(false); break;
	}
}

// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
//NOTE: Max values are not inclusive in all these functions (except float, but that generally doesn't matter)

//TODO: Are the modulo operators here going to effect the distribution of the random series?
//      Also do the floating point ideas of "precision" actually work like I want? Can we perform floating point
//      math up in those ranges? Maybe we should modulo into a conservative estimate of r32/r64 precision
u32 GetRandU32(RandomSeries_t* series)
{
	NotNull_(series);
	StepRandomSeries(series, series->defaultIncrement);
	return (u32)(series->state % UINT32_MAX);
}
u32 GetRandU32(RandomSeries_t* series, u32 min, u32 max)
{
	NotNull_(series);
	if (max < min) { SWAP_VARIABLES(u32, min, max); }
	if (min == max) { return min; }
	u32 result = GetRandU32(series);
	result = (result % (max - min)) + min;
	return result;
}

u64 GetRandU64(RandomSeries_t* series)
{
	NotNull_(series);
	StepRandomSeries(series, series->defaultIncrement);
	return series->state;
}
u64 GetRandU64(RandomSeries_t* series, u64 min, u64 max)
{
	NotNull_(series);
	if (max < min) { SWAP_VARIABLES(u64, min, max); }
	if (min == max) { return min; }
	u64 result = GetRandU64(series);
	result = (result % (max - min)) + min;
	return result;
}

r32 GetRandR32(RandomSeries_t* series)
{
	NotNull_(series);
	StepRandomSeries(series, series->defaultIncrement);
	u64 integerRandom = (series->state % RAND_FLOAT_PRECISION_R32);
	r32 result = (r32)integerRandom / (r32)RAND_FLOAT_PRECISION_R32;
	return result;
}
r32 GetRandR32(RandomSeries_t* series, r32 min, r32 max)
{
	if (max < min) { SWAP_VARIABLES(r32, min, max); }
	return (GetRandR32(series) * (max - min)) + min;
}

r64 GetRandR64(RandomSeries_t* series)
{
	NotNull_(series);
	StepRandomSeries(series, series->defaultIncrement);
	u64 integerRandom = (series->state % RAND_FLOAT_PRECISION_R64);
	r64 result = (r64)integerRandom / (r64)RAND_FLOAT_PRECISION_R64;
	return result;
}
r64 GetRandR64(RandomSeries_t* series, r64 min, r64 max)
{
	if (max < min) { SWAP_VARIABLES(r64, min, max); }
	return (GetRandR64(series) * (max - min)) + min;
}

u8 GetRandU8(RandomSeries_t* series)
{
	u64 randU64 = GetRandU64(series);
	return (u8)(randU64 % 256);
}
u8 GetRandU8(RandomSeries_t* series, u8 min, u8 max)
{
	if (max < min) { SWAP_VARIABLES(u8, min, max); }
	if (min == max) { return min; }
	u64 randU64 = GetRandU64(series);
	return (u8)((randU64 % (max-min)) + min);
}

u16 GetRandU16(RandomSeries_t* series)
{
	u64 randU64 = GetRandU64(series);
	return (u16)(randU64 % 65536);
}
u16 GetRandU16(RandomSeries_t* series, u16 min, u16 max)
{
	if (max < min) { SWAP_VARIABLES(u16, min, max); }
	if (min == max) { return min; }
	u64 randU64 = GetRandU64(series);
	return (u16)((randU64 % (max-min)) + min);
}

i8 GetRandI8(RandomSeries_t* series)
{
	u64 randU64 = GetRandU64(series);
	return (i8)((randU64 % 256) - 128);
}
i8 GetRandI8(RandomSeries_t* series, i8 min, i8 max)
{
	if (max < min) { SWAP_VARIABLES(i8, min, max); }
	if (min == max) { return min; }
	u64 randU64 = GetRandU64(series);
	return (i8)((i32)(randU64 % (u64)(max-min)) + min);
}

i32 GetRandI32(RandomSeries_t* series)
{
	u64 randU64 = GetRandU64(series);
	return (i32)((randU64 % 65536) - 32768);
}
i32 GetRandI32(RandomSeries_t* series, i32 min, i32 max)
{
	if (max < min) { SWAP_VARIABLES(i32, min, max); }
	if (min == max) { return min; }
	u64 randU64 = GetRandU64(series);
	return (i32)((i64)(randU64 % (u64)(max-min)) + min);
}

//TODO: If we can find a better way to do the signed modulos that don't require a larger number space to accomplish then we should implement GetRandI64

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_RANDOM_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
RAND_FLOAT_PRECISION_R32
RAND_FLOAT_PRECISION_R64
GYLIB_DEFAULT_RANDOM_SERIES_TYPE
RandomSeriesType_Fixed
RandomSeriesType_Incremental
RandomSeriesType_LinearCongruential32
RandomSeriesType_LinearCongruential64
RandomSeriesType_NumTypes
@Types
RandomSeriesType_t
RandomSeries_t
@Functions
const char* GetRandomSeriesTypeStr(RandomSeriesType_t enumValue)
const char* GetRandomSeriesTypeAcronymStr(RandomSeriesType_t enumValue)
void CreateRandomSeries(RandomSeries_t* series, RandomSeriesType_t type = RandomSeriesType_LinearCongruential64, u64 defaultIncrement = 1)
void SeedRandomSeriesU32(RandomSeries_t* series, u32 seed)
void SeedRandomSeriesU64(RandomSeries_t* series, u64 seed)
void SeedRandomSeriesU128(RandomSeries_t* series, u64 seed1, u64 seed2)
static inline u64 XS128_rotl(const u64 x, int k)
void StepRandomSeries(RandomSeries_t* series, u64 numberOfSteps = 1)
u32 GetRandU32(RandomSeries_t* series)
u32 GetRandU32(RandomSeries_t* series, u32 min, u32 max)
u64 GetRandU64(RandomSeries_t* series)
u64 GetRandU64(RandomSeries_t* series, u64 min, u64 max)
r32 GetRandR32(RandomSeries_t* series)
r32 GetRandR32(RandomSeries_t* series, r32 min, r32 max)
r64 GetRandR64(RandomSeries_t* series)
r64 GetRandR64(RandomSeries_t* series, r64 min, r64 max)
u8 GetRandU8(RandomSeries_t* series)
u8 GetRandU8(RandomSeries_t* series, u8 min, u8 max)
u16 GetRandU16(RandomSeries_t* series)
u16 GetRandU16(RandomSeries_t* series, u16 min, u16 max)
i8 GetRandI8(RandomSeries_t* series)
i8 GetRandI8(RandomSeries_t* series, i8 min, i8 max)
i32 GetRandI32(RandomSeries_t* series)
i32 GetRandI32(RandomSeries_t* series, i32 min, i32 max)
*/
