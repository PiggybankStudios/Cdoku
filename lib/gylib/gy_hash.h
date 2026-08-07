/*
File:   gy_hash.h
Author: Taylor Robbins
Date:   07\29\2022
Description:
	** Holds all of our hashing functions
*/

#ifndef _GY_HASH_H
#define _GY_HASH_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

#define FNV_HASH_BASE_U64   0xcbf29ce484222325ULL
#define FNV_HASH_PRIME_U64  0x100000001b3ULL

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	u64 FnvHashU64(const void* bufferPntr, u64 numBytes, u64 startingState = FNV_HASH_BASE_U64);
	u32 FnvHashU32(const void* bufferPntr, u64 numBytes);
	u16 FnvHashU16(const void* bufferPntr, u64 numBytes);
	u8 FnvHashU8(const void* bufferPntr, u64 numBytes);
#else

u64 FnvHashU64(const void* bufferPntr, u64 numBytes, u64 startingState = FNV_HASH_BASE_U64)
{
	const u8* bytePntr = (const u8*)bufferPntr;
	u64 result = startingState;
	for (u64 bIndex = 0; bIndex < numBytes; bIndex++)
	{
		result = result ^ bytePntr[bIndex];
		result = result * FNV_HASH_PRIME_U64;
	}
	return result;
}
u32 FnvHashU32(const void* bufferPntr, u64 numBytes)
{
	return (u32)FnvHashU64(bufferPntr, numBytes);
}
u16 FnvHashU16(const void* bufferPntr, u64 numBytes)
{
	return (u16)FnvHashU64(bufferPntr, numBytes);
}
u8 FnvHashU8(const void* bufferPntr, u64 numBytes)
{
	return (u8)FnvHashU64(bufferPntr, numBytes);
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_HASH_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
FNV_HASH_BASE_U64
FNV_HASH_PRIME_U64
@Types
@Functions
u64 FnvHashU64(const void* bufferPntr, u64 numBytes, u64 startingState = FNV_HASH_BASE_U64)
u32 FnvHashU32(const void* bufferPntr, u64 numBytes)
u16 FnvHashU16(const void* bufferPntr, u64 numBytes)
u8 FnvHashU8(const void* bufferPntr, u64 numBytes)
*/
