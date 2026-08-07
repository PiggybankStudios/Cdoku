/*
File:   gy_intrinsics.h
Author: Taylor Robbins
Date:   09\26\2021
Description:
	** Contains a bunch of definitions that remap almost directly to intrinsic functions
	** like sinf() but with a little more standardization.
	** The main purpose of this is to allow us a means by which we can change or inspect all
	** of the intrinsic functions we use throughout the entire project
*/

#ifndef _GY_INTRINSICS_H
#define _GY_INTRINSICS_H

#include "gy_defines_check.h"
#include "gy_basic_macros.h"
#include "gy_std.h"
#include "gy_types.h"

#define Square(value) ((value) * (value))
#define Cube(value)   ((value) * (value) * (value))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	u32 MinU32(u32 value1, u32 value2);
	u32 MinU32(u32 value1, u32 value2, u32 value3);
	u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4);
	u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5);
	u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6);
	u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7);
	u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7, u32 value8);

	i32 MinI32(i32 value1, i32 value2);
	i32 MinI32(i32 value1, i32 value2, i32 value3);
	i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4);
	i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5);
	i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6);
	i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7);
	i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7, i32 value8);

	r32 MinR32(r32 value1, r32 value2);
	r32 MinR32(r32 value1, r32 value2, r32 value3);
	r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4);
	r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5);
	r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6);
	r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7);
	r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7, r32 value8);

	u64 MinU64(u64 value1, u64 value2);
	u64 MinU64(u64 value1, u64 value2, u64 value3);
	u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4);
	u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5);
	u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6);
	u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7);
	u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7, u64 value8);

	i64 MinI64(i64 value1, i64 value2);
	i64 MinI64(i64 value1, i64 value2, i64 value3);
	i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4);
	i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5);
	i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6);
	i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7);
	i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7, i64 value8);

	r64 MinR64(r64 value1, r64 value2);
	r64 MinR64(r64 value1, r64 value2, r64 value3);
	r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4);
	r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5);
	r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6);
	r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7);
	r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7, r64 value8);
	
	u32 MaxU32(u32 value1, u32 value2);
	u32 MaxU32(u32 value1, u32 value2, u32 value3);
	u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4);
	u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5);
	u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6);
	u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7);
	u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7, u32 value8);
	
	i32 MaxI32(i32 value1, i32 value2);
	i32 MaxI32(i32 value1, i32 value2, i32 value3);
	i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4);
	i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5);
	i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6);
	i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7);
	i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7, i32 value8);
	
	r32 MaxR32(r32 value1, r32 value2);
	r32 MaxR32(r32 value1, r32 value2, r32 value3);
	r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4);
	r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5);
	r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6);
	r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7);
	r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7, r32 value8);
	
	u64 MaxU64(u64 value1, u64 value2);
	u64 MaxU64(u64 value1, u64 value2, u64 value3);
	u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4);
	u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5);
	u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6);
	u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7);
	u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7, u64 value8);
	
	i64 MaxI64(i64 value1, i64 value2);
	i64 MaxI64(i64 value1, i64 value2, i64 value3);
	i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4);
	i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5);
	i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6);
	i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7);
	i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7, i64 value8);
	
	r64 MaxR64(r64 value1, r64 value2);
	r64 MaxR64(r64 value1, r64 value2, r64 value3);
	r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4);
	r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5);
	r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6);
	r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7);
	r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7, r64 value8);
	
	r32 AbsR32(r32 value);
	r64 AbsR64(r64 value);
	i8 AbsI8(i8 value);
	i16 AbsI16(i16 value);
	i32 AbsI32(i32 value);
	i64 AbsI64(i64 value);
	
	r32 RoundR32(r32 value);
	i32 RoundR32i(r32 value);
	r64 RoundR64(r64 value);
	i64 RoundR64i(r64 value);
	
	r32 FloorR32(r32 value);
	i32 FloorR32i(r32 value);
	r64 FloorR64(r64 value);
	i64 FloorR64i(r64 value);
	
	r32 CeilR32(r32 value);
	i32 CeilR32i(r32 value);
	r64 CeilR64(r64 value);
	i64 CeilR64i(r64 value);
	
	i32 CeilToI32(i32 value, i32 chunkSize);
	u32 CeilToU32(u32 value, u32 chunkSize);
	
	i32 CeilDivI32(i32 dividend, i32 divisor);
	u32 CeilDivU32(u32 dividend, u32 divisor);
	u64 CeilDivU64(u64 dividend, u64 divisor);
	
	r32 SinR32(r32 angle);
	r64 SinR64(r64 angle);
	
	r32 CosR32(r32 angle);
	r64 CosR64(r64 angle);
	
	r32 TanR32(r32 angle);
	r64 TanR64(r64 angle);
	
	r32 AsinR32(r32 value);
	r64 AsinR64(r64 value);
	
	r32 AcosR32(r32 value);
	r64 AcosR64(r64 value);
	
	r32 AtanJoinedR32(r32 value);
	r64 AtanJoinedR64(r64 value);
	
	r32 AtanR32(r32 y, r32 x);
	r64 AtanR64(r64 y, r64 x);
	
	r32 SawR32(r32 angle);
	r64 SawR64(r64 angle);
	
	r32 PowR32(r32 value, r32 power);
	r64 PowR64(r64 value, r64 power);
	
	r32 LnR32(r32 value);
	r64 LnR64(r64 value);
	
	r32 Log2R32(r32 value);
	r64 Log2R64(r64 value);
	
	#if !ORCA_COMPILATION
	r32 Log10R32(r32 value);
	r64 Log10R64(r64 value);
	#endif
	
	r32 SqrtR32(r32 value);
	r64 SqrtR64(r64 value);
	
	r32 CbrtR32(r32 value);
	r64 CbrtR64(r64 value);
	
	r32 SignOfR32(r32 value);
	r64 SignOfR64(r64 value);
	i8 SignOfI8(i8 value);
	i16 SignOfI16(i16 value);
	i32 SignOfI32(i32 value);
	i64 SignOfI64(i64 value);
	
	#if !ORCA_COMPILATION
	void SeedRand(u32 seed);
	u32 RandU32(u32 min, u32 max);
	i32 RandI32(i32 min, i32 max);
	i64 RandI64(i64 min, i64 max);
	r32 RandR32(r32 min, r32 max);
	r64 RandR64(r64 min, r64 max);
	#endif
	
	r32 ClampR32(r32 value, r32 min, r32 max);
	r64 ClampR64(r64 value, r64 min, r64 max);
	i8 ClampI8(i8 value, i8 min, i8 max);
	i16 ClampI16(i16 value, i16 min, i16 max);
	i32 ClampI32(i32 value, i32 min, i32 max);
	i64 ClampI64(i64 value, i64 min, i64 max);
	u8 ClampU8(u8 value, u8 min, u8 max);
	u16 ClampU16(u16 value, u16 min, u16 max);
	u32 ClampU32(u32 value, u32 min, u32 max);
	u64 ClampU64(u64 value, u64 min, u64 max);
	
	r32 LerpR32(r32 val1, r32 val2, r32 amount);
	r64 LerpR64(r64 val1, r64 val2, r64 amount);
	
	r32 LerpClampR32(r32 val1, r32 val2, r32 amount);
	r64 LerpClampR64(r64 val1, r64 val2, r64 amount);
	
	r32 InverseLerpR32(r32 start, r32 end, r32 value, bool clamp = false);
	r64 InverseLerpR64(r64 start, r64 end, r64 value, bool clamp = false);
	
	r32 DecimalPartR32(r32 value);
	r64 DecimalPartR64(r64 value);
	
	r32 ModR32(r32 numerator, r32 denominator);
	r64 ModR64(r64 numerator, r64 denominator);
	
	u8 StoreI8inU8(i8 value);
	i8 ReadI8FromU8(u8 value);
	
	i8 ClampI32toI8(i32 value);
	u8 ClampI32toU8(i32 value);
	i16 ClampI32toI16(i32 value);
	u32 ClampI32toU32(i32 value);
	i32 ClampU32toI32(u32 value);
	i16 ClampR32toI16(r32 value);
	
	i8 ConvertSampleR64ToI8(r64 sample);
	i16 ConvertSampleR64ToI16(r64 sample);
	i32 ConvertSampleR64ToI32(r64 sample);
	r64 ConvertSampleI8ToR64(i8 sampleI8);
	r64 ConvertSampleI16ToR64(i16 sampleI16);
	r64 ConvertSampleI32ToR64(i32 sampleI32);
	
	bool BasicallyEqualR32(r32 value1, r32 value2, r32 tolerance = 0.001f);
	bool BasicallyEqualR64(r64 value1, r64 value2, r64 tolerance = 0.001);
	
	bool BasicallyGreaterThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f);
	bool BasicallyGreaterThanR64(r64 value1, r64 value2, r64 tolerance = 0.001);
	
	bool BasicallyLessThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f);
	bool BasicallyLessThanR64(r64 value1, r64 value2, r64 tolerance = 0.001);
	
	bool BasicallyBetweenR32(r32 value, r32 min, r32 max, r32 tolerance = 0.001f);
	bool BasicallyBetweenR64(r64 value, r64 min, r64 max, r64 tolerance = 0.001);
	
	u8 FlagsDiffU8(u8 flags1, u8 flags2);
	
	u8 AbsDiffU8(u8 value1, u8 value2);
	u16 AbsDiffU16(u16 value1, u16 value2);
	u32 AbsDiffU32(u32 value1, u32 value2);
	u64 AbsDiffU64(u64 value1, u64 value2);
	i8 AbsDiffI8(i8 value1, i8 value2);
	i16 AbsDiffI16(i16 value1, i16 value2);
	i32 AbsDiffI32(i32 value1, i32 value2);
	i64 AbsDiffI64(i64 value1, i64 value2);
	
	bool IsInfiniteR32(r32 value);
	bool IsInfiniteR64(r64 value);
	
	u32 RoundUpToU32(u32 value, u32 chunkSize);
	u64 RoundUpToU64(u64 value, u64 chunkSize);
	i32 RoundUpToI32(i32 value, i32 chunkSize);
	i64 RoundUpToI64(i64 value, i64 chunkSize);
	
	r32 SubAnimAmountR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd);
	r64 SubAnimAmountR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd);
	
	r32 SubAnimAmountTwoWayR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd);
	r64 SubAnimAmountTwoWayR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd);
	
	r32 AngleFixR32(r32 angle);
	r64 AngleFixR64(r64 angle);
	
	r32 AngleDiffR32(r32 left, r32 right);
	r64 AngleDiffR64(r64 left, r64 right);
	
	r32 AngleOppositeR32(r32 angle);
	r64 AngleOppositeR64(r64 angle);
	
	r32 AngleFlipVerticalR32(r32 angle, bool normalize = true);
	r64 AngleFlipVerticalR64(r64 angle, bool normalize = true);
	
	r32 AngleFlipHorizontalR32(r32 angle, bool normalize = true);
	r64 AngleFlipHorizontalR64(r64 angle, bool normalize = true);
	
	r32 AngleLerpR32(r32 angleFrom, r32 angleTo, r32 amount);
	r64 AngleLerpR64(r64 angleFrom, r64 angleTo, r64 amount);
#else

// +--------------------------------------------------------------+
// |                        Min Functions                         |
// +--------------------------------------------------------------+
	inline u32 MinU32(u32 value1, u32 value2)
	{
		return (value1 < value2) ? value1 : value2;
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3)
	{
		return MinU32(value1, MinU32(value2, value3));
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4)
	{
		return MinU32(value1, MinU32(value2, MinU32(value3, value4)));
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5)
	{
		return MinU32(value1, MinU32(value2, MinU32(value3, MinU32(value4, value5))));
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6)
	{
		return MinU32(value1, MinU32(value2, MinU32(value3, MinU32(value4, MinU32(value5, value6)))));
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7)
	{
		return MinU32(value1, MinU32(value2, MinU32(value3, MinU32(value4, MinU32(value5, MinU32(value6, value7))))));
	}
	inline u32 MinU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7, u32 value8)
	{
		return MinU32(value1, MinU32(value2, MinU32(value3, MinU32(value4, MinU32(value5, MinU32(value6, MinU32(value7, value8)))))));
	}
	
	inline i32 MinI32(i32 value1, i32 value2)
	{
		return (value1 < value2) ? value1 : value2;
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3)
	{
		return MinI32(value1, MinI32(value2, value3));
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4)
	{
		return MinI32(value1, MinI32(value2, MinI32(value3, value4)));
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5)
	{
		return MinI32(value1, MinI32(value2, MinI32(value3, MinI32(value4, value5))));
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6)
	{
		return MinI32(value1, MinI32(value2, MinI32(value3, MinI32(value4, MinI32(value5, value6)))));
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7)
	{
		return MinI32(value1, MinI32(value2, MinI32(value3, MinI32(value4, MinI32(value5, MinI32(value6, value7))))));
	}
	inline i32 MinI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7, i32 value8)
	{
		return MinI32(value1, MinI32(value2, MinI32(value3, MinI32(value4, MinI32(value5, MinI32(value6, MinI32(value7, value8)))))));
	}
	
	inline r32 MinR32(r32 value1, r32 value2)
	{
		#if ORCA_COMPILATION
		return ((value1 < value2) ? value1 : value2);
		#else
		return fminf(value1, value2);
		#endif
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3)
	{
		return MinR32(value1, MinR32(value2, value3));
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4)
	{
		return MinR32(value1, MinR32(value2, MinR32(value3, value4)));
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5)
	{
		return MinR32(value1, MinR32(value2, MinR32(value3, MinR32(value4, value5))));
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6)
	{
		return MinR32(value1, MinR32(value2, MinR32(value3, MinR32(value4, MinR32(value5, value6)))));
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7)
	{
		return MinR32(value1, MinR32(value2, MinR32(value3, MinR32(value4, MinR32(value5, MinR32(value6, value7))))));
	}
	inline r32 MinR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7, r32 value8)
	{
		return MinR32(value1, MinR32(value2, MinR32(value3, MinR32(value4, MinR32(value5, MinR32(value6, MinR32(value7, value8)))))));
	}
	
	inline u64 MinU64(u64 value1, u64 value2)
	{
		return (value1 < value2) ? value1 : value2;
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3)
	{
		return MinU64(value1, MinU64(value2, value3));
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4)
	{
		return MinU64(value1, MinU64(value2, MinU64(value3, value4)));
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5)
	{
		return MinU64(value1, MinU64(value2, MinU64(value3, MinU64(value4, value5))));
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6)
	{
		return MinU64(value1, MinU64(value2, MinU64(value3, MinU64(value4, MinU64(value5, value6)))));
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7)
	{
		return MinU64(value1, MinU64(value2, MinU64(value3, MinU64(value4, MinU64(value5, MinU64(value6, value7))))));
	}
	inline u64 MinU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7, u64 value8)
	{
		return MinU64(value1, MinU64(value2, MinU64(value3, MinU64(value4, MinU64(value5, MinU64(value6, MinU64(value7, value8)))))));
	}
	
	inline i64 MinI64(i64 value1, i64 value2)
	{
		return (value1 < value2) ? value1 : value2;
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3)
	{
		return MinI64(value1, MinI64(value2, value3));
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4)
	{
		return MinI64(value1, MinI64(value2, MinI64(value3, value4)));
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5)
	{
		return MinI64(value1, MinI64(value2, MinI64(value3, MinI64(value4, value5))));
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6)
	{
		return MinI64(value1, MinI64(value2, MinI64(value3, MinI64(value4, MinI64(value5, value6)))));
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7)
	{
		return MinI64(value1, MinI64(value2, MinI64(value3, MinI64(value4, MinI64(value5, MinI64(value6, value7))))));
	}
	inline i64 MinI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7, i64 value8)
	{
		return MinI64(value1, MinI64(value2, MinI64(value3, MinI64(value4, MinI64(value5, MinI64(value6, MinI64(value7, value8)))))));
	}
	
	inline r64 MinR64(r64 value1, r64 value2)
	{
		#if ORCA_COMPILATION
		return ((value1 < value2) ? value1 : value2);
		#else
		return fmin(value1, value2);
		#endif
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3)
	{
		return MinR64(value1, MinR64(value2, value3));
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4)
	{
		return MinR64(value1, MinR64(value2, MinR64(value3, value4)));
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5)
	{
		return MinR64(value1, MinR64(value2, MinR64(value3, MinR64(value4, value5))));
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6)
	{
		return MinR64(value1, MinR64(value2, MinR64(value3, MinR64(value4, MinR64(value5, value6)))));
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7)
	{
		return MinR64(value1, MinR64(value2, MinR64(value3, MinR64(value4, MinR64(value5, MinR64(value6, value7))))));
	}
	inline r64 MinR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7, r64 value8)
	{
		return MinR64(value1, MinR64(value2, MinR64(value3, MinR64(value4, MinR64(value5, MinR64(value6, MinR64(value7, value8)))))));
	}
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                        Max Functions                         |
// +--------------------------------------------------------------+
	inline u32 MaxU32(u32 value1, u32 value2)
	{
		return (value1 > value2) ? value1 : value2;
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3)
	{
		return MaxU32(value1, MaxU32(value2, value3));
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4)
	{
		return MaxU32(value1, MaxU32(value2, MaxU32(value3, value4)));
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5)
	{
		return MaxU32(value1, MaxU32(value2, MaxU32(value3, MaxU32(value4, value5))));
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6)
	{
		return MaxU32(value1, MaxU32(value2, MaxU32(value3, MaxU32(value4, MaxU32(value5, value6)))));
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7)
	{
		return MaxU32(value1, MaxU32(value2, MaxU32(value3, MaxU32(value4, MaxU32(value5, MaxU32(value6, value7))))));
	}
	inline u32 MaxU32(u32 value1, u32 value2, u32 value3, u32 value4, u32 value5, u32 value6, u32 value7, u32 value8)
	{
		return MaxU32(value1, MaxU32(value2, MaxU32(value3, MaxU32(value4, MaxU32(value5, MaxU32(value6, MaxU32(value7, value8)))))));
	}
	
	inline i32 MaxI32(i32 value1, i32 value2)
	{
		return (value1 > value2) ? value1 : value2;
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3)
	{
		return MaxI32(value1, MaxI32(value2, value3));
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4)
	{
		return MaxI32(value1, MaxI32(value2, MaxI32(value3, value4)));
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5)
	{
		return MaxI32(value1, MaxI32(value2, MaxI32(value3, MaxI32(value4, value5))));
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6)
	{
		return MaxI32(value1, MaxI32(value2, MaxI32(value3, MaxI32(value4, MaxI32(value5, value6)))));
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7)
	{
		return MaxI32(value1, MaxI32(value2, MaxI32(value3, MaxI32(value4, MaxI32(value5, MaxI32(value6, value7))))));
	}
	inline i32 MaxI32(i32 value1, i32 value2, i32 value3, i32 value4, i32 value5, i32 value6, i32 value7, i32 value8)
	{
		return MaxI32(value1, MaxI32(value2, MaxI32(value3, MaxI32(value4, MaxI32(value5, MaxI32(value6, MaxI32(value7, value8)))))));
	}
	
	inline r32 MaxR32(r32 value1, r32 value2)
	{
		#if ORCA_COMPILATION
		return ((value1 > value2) ? value1 : value2);
		#else
		return fmaxf(value1, value2);
		#endif
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3)
	{
		return MaxR32(value1, MaxR32(value2, value3));
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4)
	{
		return MaxR32(value1, MaxR32(value2, MaxR32(value3, value4)));
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5)
	{
		return MaxR32(value1, MaxR32(value2, MaxR32(value3, MaxR32(value4, value5))));
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6)
	{
		return MaxR32(value1, MaxR32(value2, MaxR32(value3, MaxR32(value4, MaxR32(value5, value6)))));
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7)
	{
		return MaxR32(value1, MaxR32(value2, MaxR32(value3, MaxR32(value4, MaxR32(value5, MaxR32(value6, value7))))));
	}
	inline r32 MaxR32(r32 value1, r32 value2, r32 value3, r32 value4, r32 value5, r32 value6, r32 value7, r32 value8)
	{
		return MaxR32(value1, MaxR32(value2, MaxR32(value3, MaxR32(value4, MaxR32(value5, MaxR32(value6, MaxR32(value7, value8)))))));
	}
	
	inline u64 MaxU64(u64 value1, u64 value2)
	{
		return (value1 > value2) ? value1 : value2;
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3)
	{
		return MaxU64(value1, MaxU64(value2, value3));
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4)
	{
		return MaxU64(value1, MaxU64(value2, MaxU64(value3, value4)));
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5)
	{
		return MaxU64(value1, MaxU64(value2, MaxU64(value3, MaxU64(value4, value5))));
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6)
	{
		return MaxU64(value1, MaxU64(value2, MaxU64(value3, MaxU64(value4, MaxU64(value5, value6)))));
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7)
	{
		return MaxU64(value1, MaxU64(value2, MaxU64(value3, MaxU64(value4, MaxU64(value5, MaxU64(value6, value7))))));
	}
	inline u64 MaxU64(u64 value1, u64 value2, u64 value3, u64 value4, u64 value5, u64 value6, u64 value7, u64 value8)
	{
		return MaxU64(value1, MaxU64(value2, MaxU64(value3, MaxU64(value4, MaxU64(value5, MaxU64(value6, MaxU64(value7, value8)))))));
	}
	
	inline i64 MaxI64(i64 value1, i64 value2)
	{
		return (value1 > value2) ? value1 : value2;
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3)
	{
		return MaxI64(value1, MaxI64(value2, value3));
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4)
	{
		return MaxI64(value1, MaxI64(value2, MaxI64(value3, value4)));
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5)
	{
		return MaxI64(value1, MaxI64(value2, MaxI64(value3, MaxI64(value4, value5))));
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6)
	{
		return MaxI64(value1, MaxI64(value2, MaxI64(value3, MaxI64(value4, MaxI64(value5, value6)))));
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7)
	{
		return MaxI64(value1, MaxI64(value2, MaxI64(value3, MaxI64(value4, MaxI64(value5, MaxI64(value6, value7))))));
	}
	inline i64 MaxI64(i64 value1, i64 value2, i64 value3, i64 value4, i64 value5, i64 value6, i64 value7, i64 value8)
	{
		return MaxI64(value1, MaxI64(value2, MaxI64(value3, MaxI64(value4, MaxI64(value5, MaxI64(value6, MaxI64(value7, value8)))))));
	}
	
	inline r64 MaxR64(r64 value1, r64 value2)
	{
		#if ORCA_COMPILATION
		return ((value1 > value2) ? value1 : value2);
		#else
		return fmax(value1, value2);
		#endif
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3)
	{
		return MaxR64(value1, MaxR64(value2, value3));
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4)
	{
		return MaxR64(value1, MaxR64(value2, MaxR64(value3, value4)));
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5)
	{
		return MaxR64(value1, MaxR64(value2, MaxR64(value3, MaxR64(value4, value5))));
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6)
	{
		return MaxR64(value1, MaxR64(value2, MaxR64(value3, MaxR64(value4, MaxR64(value5, value6)))));
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7)
	{
		return MaxR64(value1, MaxR64(value2, MaxR64(value3, MaxR64(value4, MaxR64(value5, MaxR64(value6, value7))))));
	}
	inline r64 MaxR64(r64 value1, r64 value2, r64 value3, r64 value4, r64 value5, r64 value6, r64 value7, r64 value8)
	{
		return MaxR64(value1, MaxR64(value2, MaxR64(value3, MaxR64(value4, MaxR64(value5, MaxR64(value6, MaxR64(value7, value8)))))));
	}
// +--------------------------------------------------------------+

// +--------------------------------------------------------------+
// |                       Other Functions                        |
// +--------------------------------------------------------------+
// +==============================+
// |             Abs              |
// +==============================+
inline r32 AbsR32(r32 value)
{
	return fabsf(value);
}
inline r64 AbsR64(r64 value)
{
	return fabs(value);
}
inline i8 AbsI8(i8 value)
{
	return (value < 0) ? -value : value;
}
inline i16 AbsI16(i16 value)
{
	return (value < 0) ? -value : value;
}
inline i32 AbsI32(i32 value)
{
	return (value < 0 ? -value : value);
}
inline i64 AbsI64(i64 value)
{
	return (value < 0 ? -value : value);
}

// +==============================+
// |            Round             |
// +==============================+
inline r32 RoundR32(r32 value)
{
	#if ORCA_COMPILATION
	return (r32)((i32)(value + 0.5f));
	#else
	return roundf(value);
	#endif
}
inline i32 RoundR32i(r32 value)
{
	#if ORCA_COMPILATION
	return (i32)(value + 0.5f);
	#else
	return (i32)roundf(value);
	#endif
}
inline r64 RoundR64(r64 value)
{
	#if ORCA_COMPILATION
	return (r64)((i64)(value + 0.5));
	#else
	return round(value);
	#endif
}
inline i64 RoundR64i(r64 value)
{
	#if ORCA_COMPILATION
	return (i64)(value + 0.5);
	#else
	return (i64)round(value);
	#endif
}

// +==============================+
// |            Floor             |
// +==============================+
inline r32 FloorR32(r32 value)
{
	#if ORCA_COMPILATION
	return (r32)floor((r64)value);
	#else
	return floorf(value);
	#endif
}
inline i32 FloorR32i(r32 value)
{
	#if ORCA_COMPILATION
	return (i32)floor((r64)value);
	#else
	return (i32)floorf(value);
	#endif
}
inline r64 FloorR64(r64 value)
{
	return floor(value);
}
inline i64 FloorR64i(r64 value)
{
	return (i64)floor(value);
}

// +==============================+
// |             Ceil             |
// +==============================+
inline r32 CeilR32(r32 value)
{
	#if ORCA_COMPILATION
	return (r32)ceil((r64)value);
	#else
	return ceilf(value);
	#endif
}
inline i32 CeilR32i(r32 value)
{
	#if ORCA_COMPILATION
	return (i32)ceil((r64)value);
	#else
	return (i32)ceilf(value);
	#endif
}
inline r64 CeilR64(r64 value)
{
	return ceil(value);
}
inline i64 CeilR64i(r64 value)
{
	return (i64)ceil(value);
}

// +==============================+
// |            CeilTo            |
// +==============================+
inline i32 CeilToI32(i32 value, i32 chunkSize)
{
	if ((value%chunkSize) == 0) { return value; }
	return value + (chunkSize - (value%chunkSize));
}
inline u32 CeilToU32(u32 value, u32 chunkSize)
{
	if ((value%chunkSize) == 0) { return value; }
	return value + (chunkSize - (value%chunkSize));
}

// +==============================+
// |           CeilDiv            |
// +==============================+
inline i32 CeilDivI32(i32 dividend, i32 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}
inline u32 CeilDivU32(u32 dividend, u32 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}
inline u64 CeilDivU64(u64 dividend, u64 divisor)
{
	if ((dividend % divisor) == 0) { return dividend / divisor; }
	else { return (dividend / divisor) + 1; }
}

// +==============================+
// |             Sin              |
// +==============================+
inline r32 SinR32(r32 angle)
{
	return sinf(angle);
}
inline r64 SinR64(r64 angle)
{
	return sin(angle);
}

// +==============================+
// |             Cos              |
// +==============================+
inline r32 CosR32(r32 angle)
{
	return cosf(angle);
}
inline r64 CosR64(r64 angle)
{
	return cos(angle);
}

// +==============================+
// |             Tan              |
// +==============================+
inline r32 TanR32(r32 angle)
{
	return tanf(angle);
}
inline r64 TanR64(r64 angle)
{
	return tan(angle);
}

// +==============================+
// |             Asin             |
// +==============================+
inline r32 AsinR32(r32 value)
{
	return asinf(value);
}
inline r64 AsinR64(r64 value)
{
	return asin(value);
}

// +==============================+
// |             Acos             |
// +==============================+
inline r32 AcosR32(r32 value)
{
	#if ORCA_COMPILATION
	return (r32)acos((r64)value);
	#else
	return acosf(value);
	#endif
}
inline r64 AcosR64(r64 value)
{
	return acos(value);
}

// +==============================+
// |             Atan             |
// +==============================+
inline r32 AtanJoinedR32(r32 value)
{
	return atanf(value);
}
inline r64 AtanJoinedR64(r64 value)
{
	return atan(value);
}
inline r32 AtanR32(r32 y, r32 x)
{
	return atan2f(y, x);
}
inline r64 AtanR64(r64 y, r64 x)
{
	return atan2(y, x);
}

// +==============================+
// |             Saw              |
// +==============================+
inline r32 SawR32(r32 angle)
{
	r32 x = (angle-Pi32/2) / (Pi32);
	i32 section = FloorR32i(x);
	r32 offset = x - (r32)section;
	if ((section%2) == 0)
	{
		return -1 + (offset * 2);
	}
	else
	{
		return 1 - (offset * 2);
	}
}
inline r64 SawR64(r64 angle)
{
	r64 x = (angle-Pi64/2) / (Pi64);
	i64 section = FloorR64i(x);
	r64 offset = x - (r64)section;
	if ((section%2) == 0)
	{
		return -1 + (offset * 2);
	}
	else
	{
		return 1 - (offset * 2);
	}
}

// +==============================+
// |             Pow              |
// +==============================+
inline r32 PowR32(r32 value, r32 power)
{
	#if ORCA_COMPILATION
	return (r32)pow((r64)value, (r64)power);
	#else
	return powf(value, power);
	#endif
}
inline r64 PowR64(r64 value, r64 power)
{
	return pow(value, power);
}

// +==============================+
// |             Log              |
// +==============================+
inline r32 LnR32(r32 value)
{
	return logf(value);
}
inline r64 LnR64(r64 value)
{
	return log(value);
}
inline r32 Log2R32(r32 value)
{
	return log2f(value);
}
inline r64 Log2R64(r64 value)
{
	return log2(value);
}
#if !ORCA_COMPILATION
inline r32 Log10R32(r32 value)
{
	return log10f(value);
}
inline r64 Log10R64(r64 value)
{
	return log10(value);
}
#endif //!ORCA_COMPILATION

// +==============================+
// |             Sqrt             |
// +==============================+
inline r32 SqrtR32(r32 value)
{
	return sqrtf(value);
}
inline r64 SqrtR64(r64 value)
{
	return sqrt(value);
}

// +==============================+
// |             Cbrt             |
// +==============================+
inline r32 CbrtR32(r32 value)
{
	return cbrtf(value);
}
inline r64 CbrtR64(r64 value)
{
	return cbrt(value);
}

// +==============================+
// |             Sign             |
// +==============================+
//TODO: Should this use intrinsic calls?
inline r32 SignOfR32(r32 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}
inline r64 SignOfR64(r64 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}
inline i8 SignOfI8(i8 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}
inline i16 SignOfI16(i16 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}
inline i32 SignOfI32(i32 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}
inline i64 SignOfI64(i64 value)
{
	     if (value < 0) { return -1; }
	else if (value > 0) { return  1; }
	else                { return  0; }
}

// +==============================+
// |             Rand             |
// +==============================+
#if !ORCA_COMPILATION
inline void SeedRand(u32 seed)
{
	srand((unsigned int)seed);
}
inline u32 RandU32(u32 min, u32 max)
{
	r64 scaled = (r64)rand() / RAND_MAX;
	if (scaled == 1.0) { return max-1; }
	return (u32)((max - min)*scaled) + min;
}
inline i32 RandI32(i32 min, i32 max)
{
	r64 scaled = (r64)rand() / RAND_MAX;
	return (i32)((max - min)*scaled) + min;
}
inline i64 RandI64(i64 min, i64 max)
{
	r64 scaled = (r64)rand() / RAND_MAX;
	return RoundR64i((max - min)*scaled) + min;
}
inline r32 RandR32(r32 min, r32 max)
{
	r64 scaled = (r64)rand() / RAND_MAX;
	return (r32)((max - min)*(r32)scaled) + min;
}
inline r64 RandR64(r64 min, r64 max)
{
	r64 scaled = (r64)rand() / RAND_MAX;
	return ((max - min)*scaled) + min;
}
#endif // !ORCA_COMPILATION

// +==============================+
// |            Clamp             |
// +==============================+
//TODO: Should these use intrinsic calls of some sort?
inline r32 ClampR32(r32 value, r32 min, r32 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline r64 ClampR64(r64 value, r64 min, r64 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline i8 ClampI8(i8 value, i8 min, i8 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline i16 ClampI16(i16 value, i16 min, i16 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline i32 ClampI32(i32 value, i32 min, i32 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline i64 ClampI64(i64 value, i64 min, i64 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline u8 ClampU8(u8 value, u8 min, u8 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline u16 ClampU16(u16 value, u16 min, u16 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline u32 ClampU32(u32 value, u32 min, u32 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}
inline u64 ClampU64(u64 value, u64 min, u64 max)
{
	if (value < min) { return min; }
	else if (value > max) { return max; }
	else { return value; }
}

// +==============================+
// |             Lerp             |
// +==============================+
inline r32 LerpR32(r32 val1, r32 val2, r32 amount)
{
	return (val1 + (val2 - val1) * amount);
}
inline r64 LerpR64(r64 val1, r64 val2, r64 amount)
{
	return (val1 + (val2 - val1) * amount);
}
inline r32 LerpClampR32(r32 val1, r32 val2, r32 amount)
{
	return ClampR32(val1 + (val2 - val1) * amount, MinR32(val1, val2), MaxR32(val1, val2));
}
inline r64 LerpClampR64(r64 val1, r64 val2, r64 amount)
{
	return ClampR64(val1 + (val2 - val1) * amount, MinR64(val1, val2), MaxR64(val1, val2));
}

// +==============================+
// |         InverseLerp          |
// +==============================+
inline r32 InverseLerpR32(r32 start, r32 end, r32 value, bool clamp = false)
{
	r32 result = (value - start) / (end - start);
	if (clamp) { result = ClampR32(result, 0.0f, 1.0f); }
	return result;
}
inline r64 InverseLerpR64(r64 start, r64 end, r64 value, bool clamp = false)
{
	r64 result = (value - start) / (end - start);
	if (clamp) { result = ClampR64(result, 0.0f, 1.0f); }
	return result;
}

//TODO: Rename to FractionalPart
// +==============================+
// |         DecimalPart          |
// +==============================+
inline r32 DecimalPartR32(r32 value)
{
	r32 floor = FloorR32(value);
	return AbsR32(value - floor);
}
inline r64 DecimalPartR64(r64 value)
{
	r64 floor = FloorR64(value);
	return AbsR64(value - floor);
}

// +==============================+
// |             Mod              |
// +==============================+
inline r32 ModR32(r32 numerator, r32 denominator)
{
	#if ORCA_COMPILATION
	return (r32)fmod((r64)numerator, (r64)denominator);
	#else
	return fmodf(numerator, denominator);
	#endif
}
inline r64 ModR64(r64 numerator, r64 denominator)
{
	return fmod(numerator, denominator);
}

//TODO: Rename these "ClampCast"
// +================================+
// | Primitive Conversion Functions |
// +================================+
//TODO: Make these functions work without pointer arithmetic
inline u8 StoreI8inU8(i8 value)
{
	u8 result = *((u8*)&value);
	return result;
}
inline i8 ReadI8FromU8(u8 value)
{
	i8 result = *((i8*)&value);
	return result;
}
inline i8 ClampI32toI8(i32 value)
{
	if (value > 127) { return 127; }
	if (value < -128) { return -128; }
	return (i8)value;
}
inline u8 ClampI32toU8(i32 value)
{
	if (value > 255) { return 255; }
	if (value < 0) { return 0; }
	return (u8)value;
}
inline i16 ClampI32toI16(i32 value)
{
	if (value > 32767) { return 32767; }
	if (value < -32768) { return -32768; }
	return (i16)value;
}
inline u32 ClampI32toU32(i32 value)
{
	if (value < 0) { return 0; }
	return (u32)value;
}
inline i32 ClampU32toI32(u32 value)
{
	if (value > 0x7FFFFFFF) { return (i32)0x7FFFFFFF; }
	return (i32)value;
}
inline i16 ClampR32toI16(r32 value)
{
	i32 roundedValue = RoundR32i(value);
	if (roundedValue > 0x7FFF) { return 0x7FFF; }
	else if (roundedValue < -0x8000) { return -0x8000; }
	else { return (i16)roundedValue; }
}

// TODO: Use ClampCast in these conversions!
// +==============================+
// |      ConvertSampleXToY       |
// +==============================+
inline i8 ConvertSampleR64ToI8(r64 sample)
{
	return (i8)RoundR64i(sample * INT8_MAX);
}
inline i16 ConvertSampleR64ToI16(r64 sample)
{
	return (i16)RoundR64i(sample * INT16_MAX);
}
inline i32 ConvertSampleR64ToI32(r64 sample)
{
	return (i32)RoundR64i(sample * INT32_MAX);
}
inline r64 ConvertSampleI8ToR64(i8 sampleI8)
{
	return ((r64)sampleI8 / (r64)INT8_MAX);
}
inline r64 ConvertSampleI16ToR64(i16 sampleI16)
{
	return ((r64)sampleI16 / (r64)INT16_MAX);
}
inline r64 ConvertSampleI32ToR64(i32 sampleI32)
{
	return ((r64)sampleI32 / (r64)INT32_MAX);
}

// +==============================+
// |        BasicallyEqual        |
// +==============================+
inline bool BasicallyEqualR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
{
	return (AbsR32(value1 - value2) <= tolerance);
}
inline bool BasicallyEqualR64(r64 value1, r64 value2, r64 tolerance = 0.001)
{
	return (AbsR64(value1 - value2) <= tolerance);
}

inline bool BasicallyGreaterThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
{
	return ((value1 > value2) || BasicallyEqualR32(value1, value2, tolerance));
}
inline bool BasicallyGreaterThanR64(r64 value1, r64 value2, r64 tolerance = 0.001)
{
	return ((value1 > value2) || BasicallyEqualR64(value1, value2, tolerance));
}

inline bool BasicallyLessThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
{
	return ((value1 < value2) || BasicallyEqualR32(value1, value2, tolerance));
}
inline bool BasicallyLessThanR64(r64 value1, r64 value2, r64 tolerance = 0.001)
{
	return ((value1 < value2) || BasicallyEqualR64(value1, value2, tolerance));
}

inline bool BasicallyBetweenR32(r32 value, r32 min, r32 max, r32 tolerance = 0.001f)
{
	return ((value > min && value < max) || BasicallyEqualR32(value, min, tolerance) || BasicallyEqualR32(value, max, tolerance));
}
inline bool BasicallyBetweenR64(r64 value, r64 min, r64 max, r64 tolerance = 0.001)
{
	return ((value > min && value < max) || BasicallyEqualR64(value, min, tolerance) || BasicallyEqualR64(value, max, tolerance));
}

// +==============================+
// |          FlagsDiff           |
// +==============================+
//Counts the number of bits that are different between 2 u8 values
inline u8 FlagsDiffU8(u8 flags1, u8 flags2)
{
	u8 diff = (u8)(flags1 ^ flags2); //TODO: Can we get a garunteed u8 XOR on VS2013 and not have to do a u8 cast?
	u8 result = 0;
	while (diff != 0)
	{
		if (diff & 0x01) { result++; }
		diff >>= 1;
	}
	return result;
}

// +==============================+
// |           AbsDiff            |
// +==============================+
inline u8 AbsDiffU8(u8 value1, u8 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u16 AbsDiffU16(u16 value1, u16 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u32 AbsDiffU32(u32 value1, u32 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline u64 AbsDiffU64(u64 value1, u64 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}

inline i8 AbsDiffI8(i8 value1, i8 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline i16 AbsDiffI16(i16 value1, i16 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline i32 AbsDiffI32(i32 value1, i32 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}
inline i64 AbsDiffI64(i64 value1, i64 value2)
{
	if (value1 > value2) { return value1 - value2; }
	else { return value2 - value1; }
}

// +==============================+
// |          IsInfinite          |
// +==============================+
inline bool IsInfiniteR32(r32 value)
{
	return (isinf(value) || isnan(value)); //TODO: Should these by like fisinf or fisnan?
}
inline bool IsInfiniteR64(r64 value)
{
	return (isinf(value) || isnan(value));
}

// +==============================+
// |          RoundUpTo           |
// +==============================+
//TODO: These are basically CeilTo functions! Do we need both for some reason?
u32 RoundUpToU32(u32 value, u32 chunkSize)
{
	if (chunkSize <= 1) { return value; } //eat degenerate and invalid
	if ((value % chunkSize) == 0) { return value; } //totally valid scenario
	return value + (chunkSize - (value % chunkSize)); //the real calculation
}
u64 RoundUpToU64(u64 value, u64 chunkSize)
{
	if (chunkSize <= 1) { return value; } //eat degenerate and invalid
	if ((value % chunkSize) == 0) { return value; } //totally valid scenario
	return value + (chunkSize - (value % chunkSize)); //the real calculation
}

//On negative numbers "up" is really "more negative"
i32 RoundUpToI32(i32 value, i32 chunkSize)
{
	if (chunkSize <= 1) { return value; } //eat degenerate and invalid
	bool isNegative = (value < 0);
	value = AbsI32(value);
	if ((value % chunkSize) == 0) { return value; } //totally valid scenario
	i32 result = value + (chunkSize - (value % chunkSize)); //the real calculation
	return (isNegative ? -1 : 1) * result;
}
i64 RoundUpToI64(i64 value, i64 chunkSize)
{
	if (chunkSize <= 1) { return value; } //eat degenerate and invalid
	bool isNegative = (value < 0);
	value = AbsI64(value);
	if ((value % chunkSize) == 0) { return value; } //totally valid scenario
	i64 result = value + (chunkSize - (value % chunkSize)); //the real calculation
	return (isNegative ? -1 : 1) * result;
}

//TODO: Rename these LinearStep and LinearSpike
// +==============================+
// | Normalized Float Operations  |
// +==============================+
r32 SubAnimAmountR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
{
	return ClampR32((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0f, 1.0f);
}
r64 SubAnimAmountR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
{
	return ClampR64((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0, 1.0);
}

r32 SubAnimAmountTwoWayR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
{
	r32 subAnimAmount = ClampR32((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0f, 1.0f);
	if (subAnimAmount < 0.5f) { return subAnimAmount*2; }
	else { return (1.0f-subAnimAmount)*2; }
}
r64 SubAnimAmountTwoWayR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
{
	r64 subAnimAmount = ClampR64((animAmount - subPieceStart) / (subPieceEnd - subPieceStart), 0.0, 1.0);
	if (subAnimAmount < 0.5) { return subAnimAmount*2; }
	else { return (1.0-subAnimAmount)*2; }
}

// +==============================+
// |   Angle Related Functions    |
// +==============================+
r32 AngleFixR32(r32 angle)
{
	if (IsInfiniteR32(angle)) { return angle; }
	r32 result = angle;
	if (result >= TwoPi32) { result = ModR32(result, TwoPi32); }
	if (result < 0) { result = TwoPi32 - ModR32(-result, TwoPi32); }
	return result;
}
r64 AngleFixR64(r64 angle)
{
	if (IsInfiniteR64(angle)) { return angle; }
	r64 result = angle;
	if (result >= TwoPi64) { result = ModR64(result, TwoPi64); }
	if (result < 0) { result = TwoPi64 - ModR64(-result, TwoPi64); }
	return result;
}

r32 AngleDiffR32(r32 left, r32 right)
{
	r32 fixedLeft = AngleFixR32(left);
	r32 fixedRight = AngleFixR32(right);
	if (fixedLeft - fixedRight > Pi32) { fixedLeft -= TwoPi32; }
	if (fixedLeft - fixedRight < -Pi32) { fixedLeft += TwoPi32; }
	return fixedLeft - fixedRight;
}
r64 AngleDiffR64(r64 left, r64 right)
{
	r64 fixedLeft = AngleFixR64(left);
	r64 fixedRight = AngleFixR64(right);
	if (fixedLeft - fixedRight > Pi64) { fixedLeft -= TwoPi64; }
	if (fixedLeft - fixedRight < -Pi64) { fixedLeft += TwoPi64; }
	return fixedLeft - fixedRight;
}

//TODO: Is this actually right??? Should be Pi32 not TwoPi32 right?
r32 AngleOppositeR32(r32 angle)
{
	return AngleFixR32(angle + TwoPi32);
}
r64 AngleOppositeR64(r64 angle)
{
	return AngleFixR64(angle + TwoPi64);
}

r32 AngleFlipVerticalR32(r32 angle, bool normalize = true)
{
	r32 result = TwoPi32 - angle;
	if (normalize) { result = AngleFixR32(result); }
	return result;
}
r32 AngleFlipHorizontalR32(r32 angle, bool normalize = true)
{
	r32 result = Pi32 - angle;
	if (normalize) { result = AngleFixR32(result); }
	return result;
}
r64 AngleFlipVerticalR64(r64 angle, bool normalize = true)
{
	r64 result = TwoPi64 - angle;
	if (normalize) { result = AngleFixR64(result); }
	return result;
}
r64 AngleFlipHorizontalR64(r64 angle, bool normalize = true)
{
	r64 result = Pi64 - angle;
	if (normalize) { result = AngleFixR64(result); }
	return result;
}

//TODO: Should we do a final AngleFix on the result of this?
r32 AngleLerpR32(r32 angleFrom, r32 angleTo, r32 amount)
{
	r32 from = AngleFixR32(angleFrom);
	r32 to = AngleFixR32(angleTo);
	if (to - from > Pi32) { to -= TwoPi32; }
	if (to - from < -Pi32) { to += TwoPi32; }
	return from + (to - from) * amount;
}
r64 AngleLerpR64(r64 angleFrom, r64 angleTo, r64 amount)
{
	r64 from = AngleFixR64(angleFrom);
	r64 to = AngleFixR64(angleTo);
	if (to - from > Pi64) { to -= TwoPi64; }
	if (to - from < -Pi64) { to += TwoPi64; }
	return from + (to - from) * amount;
}

#endif //GYLIB_HEADER_ONLY

// +--------------------------------------------------------------+
// |                     Threading Intrinsics                     |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION

//TODO: Figure out if we actually need the _mm_sfence() instruction
#define ThreadingWriteBarrier()             _WriteBarrier(); _mm_sfence()
#define ThreadingReadBarrier()              _ReadBarrier()
#define ThreadSafeIncrement(variablePntr)   InterlockedIncrement(variablePntr)
#define ThreadSafeClaimByBool(variablePntr) !_interlockedbittestandset((LONG volatile*)(variablePntr), 0)

#elif OSX_COMPILATION

//TODO: Implement these intrinsics

#elif LINUX_COMPILATION

//TODO: Implement these intrinsics

#endif

#endif //  _GY_INTRINSICS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
#define Square(value)
#define Cube(value)
inline u32 MinU32(u32 value1, u32 value2)
inline i32 MinI32(i32 value1, i32 value2)
inline r32 MinR32(r32 value1, r32 value2)
inline u64 MinU64(u64 value1, u64 value2)
inline i64 MinI64(i64 value1, i64 value2)
inline r64 MinR64(r64 value1, r64 value2)
inline u32 MaxU32(u32 value1, u32 value2)
inline i32 MaxI32(i32 value1, i32 value2)
inline r32 MaxR32(r32 value1, r32 value2)
inline u64 MaxU64(u64 value1, u64 value2)
inline i64 MaxI64(i64 value1, i64 value2)
inline r64 MaxR64(r64 value1, r64 value2)
inline r32 AbsR32(r32 value)
inline r64 AbsR64(r64 value)
inline i8 AbsI8(i8 value)
inline i16 AbsI16(i16 value)
inline i32 AbsI32(i32 value)
inline i64 AbsI64(i64 value)
inline r32 RoundR32(r32 value)
inline i32 RoundR32i(r32 value)
inline r64 RoundR64(r64 value)
inline i64 RoundR64i(r64 value)
inline r32 FloorR32(r32 value)
inline i32 FloorR32i(r32 value)
inline r64 FloorR64(r64 value)
inline i64 FloorR64i(r64 value)
inline r32 CeilR32(r32 value)
inline i32 CeilR32i(r32 value)
inline r64 CeilR64(r64 value)
inline i64 CeilR64i(r64 value)
inline i32 CeilToI32(i32 value, i32 chunkSize)
inline u32 CeilToU32(u32 value, u32 chunkSize)
inline i32 CeilDivI32(i32 dividend, i32 divisor)
inline u32 CeilDivU32(u32 dividend, u32 divisor)
inline u64 CeilDivU64(u64 dividend, u64 divisor)
inline r32 SinR32(r32 angle)
inline r64 SinR64(r64 angle)
inline r32 CosR32(r32 angle)
inline r64 CosR64(r64 angle)
inline r32 TanR32(r32 angle)
inline r64 TanR64(r64 angle)
inline r32 AsinR32(r32 value)
inline r64 AsinR64(r64 value)
inline r32 AcosR32(r32 value)
inline r64 AcosR64(r64 value)
inline r32 AtanJoinedR32(r32 value)
inline r64 AtanJoinedR64(r64 value)
inline r32 AtanR32(r32 y, r32 x)
inline r64 AtanR64(r64 y, r64 x)
inline r32 SawR32(r32 angle)
inline r64 SawR64(r64 angle)
inline r32 PowR32(r32 value, r32 power)
inline r64 PowR64(r64 value, r64 power)
inline r32 SqrtR32(r32 value)
inline r64 SqrtR64(r64 value)
inline r32 CbrtR32(r32 value)
inline r64 CbrtR64(r64 value)
inline r32 SignOfR32(r32 value)
inline r64 SignOfR64(r64 value)
inline i8 SignOfI8(i8 value)
inline i16 SignOfI16(i16 value)
inline i32 SignOfI32(i32 value)
inline i64 SignOfI64(i64 value)
inline u32 RandU32(u32 min, u32 max)
inline i32 RandI32(i32 min, i32 max)
inline i64 RandI64(i64 min, i64 max)
inline r32 RandR32(r32 min, r32 max)
inline r64 RandR64(r64 min, r64 max)
inline r32 ClampR32(r32 value, r32 min, r32 max)
inline r64 ClampR64(r64 value, r64 min, r64 max)
inline i8 ClampI8(i8 value, i8 min, i8 max)
inline i16 ClampI16(i16 value, i16 min, i16 max)
inline i32 ClampI32(i32 value, i32 min, i32 max)
inline i64 ClampI64(i64 value, i64 min, i64 max)
inline u8 ClampU8(u8 value, u8 min, u8 max)
inline u16 ClampU16(u16 value, u16 min, u16 max)
inline u32 ClampU32(u32 value, u32 min, u32 max)
inline u64 ClampU64(u64 value, u64 min, u64 max)
inline r32 LerpR32(r32 val1, r32 val2, r32 amount)
inline r64 LerpR64(r64 val1, r64 val2, r64 amount)
inline r32 LerpClampR32(r32 val1, r32 val2, r32 amount)
inline r64 LerpClampR64(r64 val1, r64 val2, r64 amount)
inline r32 InverseLerpR32(r32 start, r32 end, r32 value, bool clamp = false)
inline r64 InverseLerpR64(r64 start, r64 end, r64 value, bool clamp = false)
inline r32 DecimalPartR32(r32 value)
inline r64 DecimalPartR64(r64 value)
inline r32 ModR32(r32 numerator, r32 denominator)
inline r64 ModR64(r64 numerator, r64 denominator)
inline u8 StoreI8inU8(i8 value)
inline i8 ReadI8FromU8(u8 value)
inline i8 ClampI32toI8(i32 value)
inline u8 ClampI32toU8(i32 value)
inline i16 ClampI32toI16(i32 value)
inline u32 ClampI32toU32(i32 value)
inline i32 ClampU32toI32(u32 value)
inline i16 ClampR32toI16(r32 value)
inline i8 ConvertSampleR64ToI8(r64 sample)
inline i16 ConvertSampleR64ToI16(r64 sample)
inline i32 ConvertSampleR64ToI32(r64 sample)
inline r64 ConvertSampleI8ToR64(i8 sampleI8)
inline r64 ConvertSampleI16ToR64(i16 sampleI16)
inline r64 ConvertSampleI32ToR64(i32 sampleI32)
inline bool BasicallyEqualR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
inline bool BasicallyEqualR64(r64 value1, r64 value2, r64 tolerance = 0.001)
inline bool BasicallyGreaterThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
inline bool BasicallyGreaterThanR64(r64 value1, r64 value2, r64 tolerance = 0.001)
inline bool BasicallyLessThanR32(r32 value1, r32 value2, r32 tolerance = 0.001f)
inline bool BasicallyLessThanR64(r64 value1, r64 value2, r64 tolerance = 0.001)
inline bool BasicallyBetweenR32(r32 value, r32 min, r32 max, r32 tolerance = 0.001f)
inline bool BasicallyBetweenR64(r64 value, r64 min, r64 max, r64 tolerance = 0.001)
inline u8 FlagsDiffU8(u8 flags1, u8 flags2)
inline u8 AbsDiffU8(u8 value1, u8 value2)
inline u16 AbsDiffU16(u16 value1, u16 value2)
inline u32 AbsDiffU32(u32 value1, u32 value2)
inline u64 AbsDiffU64(u64 value1, u64 value2)
inline i8 AbsDiffI8(i8 value1, i8 value2)
inline i16 AbsDiffI16(i16 value1, i16 value2)
inline i32 AbsDiffI32(i32 value1, i32 value2)
inline i64 AbsDiffI64(i64 value1, i64 value2)
inline bool IsInfiniteR32(r32 value)
inline bool IsInfiniteR64(r64 value)
u32 RoundUpToU32(u32 value, u32 chunkSize)
u64 RoundUpToU64(u64 value, u64 chunkSize)
i32 RoundUpToI32(i32 value, i32 chunkSize)
i64 RoundUpToI64(i64 value, i64 chunkSize)
r32 SubAnimAmountR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
r64 SubAnimAmountR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
r32 SubAnimAmountTwoWayR32(r32 animAmount, r32 subPieceStart, r32 subPieceEnd)
r64 SubAnimAmountTwoWayR64(r64 animAmount, r64 subPieceStart, r64 subPieceEnd)
r32 AngleFixR32(r32 angle)
r64 AngleFixR64(r64 angle)
r32 AngleDiffR32(r32 left, r32 right)
r64 AngleDiffR64(r64 left, r64 right)
r32 AngleOppositeR32(r32 angle)
r64 AngleOppositeR64(r64 angle)
r32 AngleFlipVerticalR32(r32 angle, bool normalize = true)
r32 AngleFlipHorizontalR32(r32 angle, bool normalize = true)
r64 AngleFlipVerticalR64(r64 angle, bool normalize = true)
r64 AngleFlipHorizontalR64(r64 angle, bool normalize = true)
r32 AngleLerpR32(r32 angleFrom, r32 angleTo, r32 amount)
r64 AngleLerpR64(r64 angleFrom, r64 angleTo, r64 amount)
#define ThreadingWriteBarrier()
#define ThreadingReadBarrier()
#define ThreadSafeIncrement(variablePntr)
#define ThreadSafeClaimByBool(variablePntr)
*/