/*
File:   gy_os_defs.h
Author: Taylor Robbins
Date:   08\11\2023
*/

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

#ifndef _GY_OS_DEFS_H
#define _GY_OS_DEFS_H

// +--------------------------------------------------------------+
// |                      Error Enumeration                       |
// +--------------------------------------------------------------+
enum OsError_t
{
	OsError_None = 0,
	OsError_UnsupportedPlatform,
	OsError_FailedToAllocateMemory,
	OsError_EmptyPath,
	OsError_ExePathTooLong,
	OsError_NumErrors,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetOsErrorStr(OsError_t enumValue);
#else
const char* GetOsErrorStr(OsError_t enumValue)
{
	switch (enumValue)
	{
		case OsError_None:                   return "None";
		case OsError_UnsupportedPlatform:    return "UnsupportedPlatform";
		case OsError_FailedToAllocateMemory: return "FailedToAllocateMemory";
		case OsError_EmptyPath:              return "EmptyPath";
		case OsError_ExePathTooLong:         return "ExePathTooLong";
		default: return "Unknown";
	}
}
#endif

// +--------------------------------------------------------------+
// |                    Function Declarations                     |
// +--------------------------------------------------------------+
struct MyStr_t OsGetExecutablePath(struct MemArena_t* memArena, OsError_t* errorOut);
struct MyStr_t OsGetWorkingDirectory(struct MemArena_t* memArena, OsError_t* errorOut = nullptr);
u64 OsGetMemoryPageSize();
void* OsReserveMemory(u64 numBytes);
void OsCommitReservedMemory(void* memoryPntr, u64 numBytes);
void OsFreeReservedMemory(void* memoryPntr, u64 reservedSize);

#endif //  _GY_OS_DEFS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
OsError_None
OsError_UnsupportedPlatform
OsError_FailedToAllocateMemory
OsError_EmptyPath
OsError_ExePathTooLong
OsError_NumErrors
@Types
OsError_t
@Functions
const char* GetOsErrorStr(OsError_t enumValue)
struct MyStr_t OsGetExecutablePath(struct MemArena_t* memArena, OsError_t* errorOut)
struct MyStr_t OsGetWorkingDirectory(struct MemArena_t* memArena, OsError_t* errorOut = nullptr)
u64 OsGetMemoryPageSize()
void* OsReserveMemory(u64 numBytes)
void OsCommitReservedMemory(void* memoryPntr, u64 numBytes)
void OsFreeReservedMemory(void* memoryPntr, u64 reservedSize)
*/