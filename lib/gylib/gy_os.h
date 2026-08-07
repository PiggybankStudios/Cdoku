/*
File:   gy_os.h
Author: Taylor Robbins
Date:   08\09\2023
Description:
	** A collection of platform-dependent operations that we'd like to be able
	** to do on all shipping platforms in a consistent way.
*/

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_debug.h"
#include "gy_memory.h"
#include "gy_scratch_arenas.h"
#include "gy_string.h"

#ifndef _GY_OS_H
#define _GY_OS_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define MAX_EXECUTABLE_PATH_LENGTH 1024

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	#if GYLIB_SCRATCH_ARENA_AVAILABLE
	MyStr_t OsGetExecutablePath(MemArena_t* memArena, OsError_t* errorOut);
	#endif // GYLIB_SCRATCH_ARENA_AVAILABLE
	MyStr_t OsGetWorkingDirectory(MemArena_t* memArena, OsError_t* errorOut);
	u64 OsGetMemoryPageSize();
	void* OsReserveMemory(u64 numBytes);
	void OsCommitReservedMemory(void* memoryPntr, u64 numBytes);
	void OsFreeReservedMemory(void* memoryPntr, u64 reservedSize);
#else

// +--------------------------------------------------------------+
// |                      GetExecutablePath                       |
// +--------------------------------------------------------------+
#if GYLIB_SCRATCH_ARENA_AVAILABLE
// Always returns path with forward slashes: /
MyStr_t OsGetExecutablePath(MemArena_t* memArena, OsError_t* errorOut)
{
	NotNull(memArena);
	SetOptionalOutPntr(errorOut, OsError_None);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		MemArena_t* scratch = GetScratchArena(memArena);
		char* tempBuffer = AllocArray(scratch, char, MAX_EXECUTABLE_PATH_LENGTH);
		NotNull(tempBuffer);
		DWORD pathLength = GetModuleFileNameA(
			0,
			tempBuffer,
			MAX_EXECUTABLE_PATH_LENGTH
		);
		FreeScratchArena(scratch);
		
		if (pathLength <= 0)
		{
			SetOptionalOutPntr(errorOut, OsError_EmptyPath);
			return MyStr_Empty;
		}
		if (pathLength >= MAX_EXECUTABLE_PATH_LENGTH-1)
		{
			GyLibPrintLine_W("Our executable path is %u characters or more. Please put the executable in a shorter directory", MAX_EXECUTABLE_PATH_LENGTH-1);
			SetOptionalOutPntr(errorOut, OsError_ExePathTooLong);
			return MyStr_Empty;
		}
		
		char* resultBuffer = AllocArray(memArena, char, pathLength+1);
		if (resultBuffer == nullptr)
		{
			SetOptionalOutPntr(errorOut, OsError_FailedToAllocateMemory);
			return MyStr_Empty;
		}
		
		DWORD resultLength = GetModuleFileNameA(
			0,
			resultBuffer,
			pathLength+1
		);
		Assert(resultLength == pathLength);
		resultBuffer[resultLength] = '\0';
		
		MyStr_t result = NewStr(resultLength, resultBuffer);
		StrReplaceInPlace(result, "\\", "/");
		return result;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		Unimplemented(); //TODO: Implement me!
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |     Unsupported Platform     |
	// +==============================+
	#else
	#error GetExecutablePath does not support the current platform yet!
	#endif
}
#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

// +--------------------------------------------------------------+
// |                     GetWorkingDirectory                      |
// +--------------------------------------------------------------+
// Always returns path with forward slashes: /
// Guarantees that the path ends with a forward slash
MyStr_t OsGetWorkingDirectory(MemArena_t* memArena, OsError_t* errorOut)
{
	NotNull(memArena);
	SetOptionalOutPntr(errorOut, OsError_None);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		DWORD bufferSizeNeeded = GetCurrentDirectory(0, nullptr);
		if (bufferSizeNeeded <= 0)
		{
			SetOptionalOutPntr(errorOut, OsError_EmptyPath);
			return MyStr_Empty;
		}
		
		char* resultBuffer = AllocArray(memArena, char, bufferSizeNeeded+1);
		if (resultBuffer == nullptr)
		{
			SetOptionalOutPntr(errorOut, OsError_FailedToAllocateMemory);
			return MyStr_Empty;
		}
		
		DWORD resultLength = GetCurrentDirectoryA(
			bufferSizeNeeded,
			resultBuffer
		);
		Assert(resultLength == bufferSizeNeeded-1);
		
		if (resultBuffer[resultLength] != '\\' && resultBuffer[resultLength] != '/')
		{
			resultBuffer[resultLength] = '\\';
			resultLength++;
		}
		else
		{
			ShrinkMem(memArena, resultBuffer, bufferSizeNeeded+1, bufferSizeNeeded);
		}
		
		resultBuffer[resultLength] = '\0';
		MyStr_t result = NewStr(resultLength, resultBuffer);
		StrReplaceInPlace(result, "\\", "/");
		return result;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		Unimplemented(); //TODO: Implement me!
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
		return MyStr_Empty;
	}
	#else
	#error GetWorkingDirectory does not support the current platform yet!
	#endif
}

// +--------------------------------------------------------------+
// |                     OsGetMemoryPageSize                      |
// +--------------------------------------------------------------+
u64 OsGetMemoryPageSize()
{
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		Assert(systemInfo.dwPageSize > 0);
		return (u64)systemInfo.dwPageSize;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		//TODO: Maybe we should be doing sysconf(_SC_PAGESIZE) instead?
		int result = getpagesize();
		Assert(result > 0);
		return (u64)result;
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		//This is the size of the L1 Cache (Since Playdate doesn't have virtual memory we'll just use this as a standin)
		return Kilobytes(8);
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		//This is the size of the allocation pages in the WASM memory model, and WASM doesn't support virtual memory afaik
		return Kilobytes(64);
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		//This is the size of the allocation pages in the WASM memory model, and WASM doesn't support virtual memory afaik
		return Kilobytes(64);
	}
	#else
	#error OsGetMemoryPageSize does not support the current platform yet!
	#endif
}

// +--------------------------------------------------------------+
// |                       OsReserveMemory                        |
// +--------------------------------------------------------------+
//NOTE: numBytes must be a multiple of memory page size
void* OsReserveMemory(u64 numBytes)
{
	if (numBytes == 0) { return nullptr; }
	u64 pageSize = OsGetMemoryPageSize();
	Assert(numBytes % pageSize == 0);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		void* result = VirtualAlloc(
			nullptr, //lpAddress
			numBytes, //dwSize
			MEM_RESERVE, //flAllocationType
			PAGE_READWRITE //flProtect
		);
		AssertIf(result != nullptr, (u64)result % pageSize == 0);
		return result;
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		// https://unix.stackexchange.com/questions/405883/can-an-application-explicitly-commit-and-decommit-memory
		// It seems like we can't force the application to commit any of this memory. Writing to pages automatically commits them.
		// That's not the end of the world but it is a difference in behavior
		// TODO: Maybe we can use mprotect? Do we need to calling code to manage the protection since it knows the size of the reserved memory?
		void* result = mmap(
			nullptr, //addr
			numBytes, //length
			PROT_READ|PROT_WRITE, //prot
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, //fd,
			0 //offset
		);
		AssertIf(result != nullptr, (u64)result % pageSize == 0);
		Assert(result != MAP_FAILED);
		return result;
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		return nullptr;
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		return nullptr;
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		return nullptr;
	}
	#else
	#error OsReserveMemory does not support the current platform yet!
	#endif
}

// +--------------------------------------------------------------+
// |                    OsCommitReservedMemory                    |
// +--------------------------------------------------------------+
//NOTE: numBytes must be a multiple of memory page size, and memoryPntr must be aligned to the beginning of a page
void OsCommitReservedMemory(void* memoryPntr, u64 numBytes)
{
	if (numBytes == 0) { return; }
	u64 pageSize = OsGetMemoryPageSize();
	NotNull(memoryPntr);
	Assert((u64)memoryPntr % pageSize == 0);
	Assert(numBytes % pageSize == 0);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		void* commitResult = VirtualAlloc(
			memoryPntr, //lpAddress
			numBytes, //dwSize
			MEM_COMMIT, //flAllocationType
			PAGE_READWRITE //flProtect
		);
		Assert(commitResult == memoryPntr); //TODO: Handle errors, call GetLastError and return an OsError_t
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		int protectResult = mprotect(
			memoryPntr,
			numBytes,
			PROT_READ|PROT_WRITE
		);
		Assert(protectResult == 0); //TODO: Handle errors, check errno and return an OsError_t
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		AssertMsg(false, "OsCommitReservedMemory is not supported on PLAYDATE");
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		AssertMsg(false, "OsCommitReservedMemory is not supported on ORCA");
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		AssertMsg(false, "OsCommitReservedMemory is not supported on WASM");
	}
	#else
	#error OsReserveMemory does not support the current platform yet!
	#endif
}

// +--------------------------------------------------------------+
// |                     OsFreeReservedMemory                     |
// +--------------------------------------------------------------+
void OsFreeReservedMemory(void* memoryPntr, u64 reservedSize)
{
	Assert((memoryPntr == nullptr) == (reservedSize == 0));
	if (memoryPntr == nullptr) { return; }
	
	u64 pageSize = OsGetMemoryPageSize();
	Assert((u64)memoryPntr % pageSize == 0);
	Assert(reservedSize % pageSize == 0);
	
	// +==============================+
	// |           Windows            |
	// +==============================+
	#if WINDOWS_COMPILATION
	{
		BOOL freeResult = VirtualFree(
			memoryPntr, //lpAddress
			reservedSize, //dwSize
			MEM_RELEASE //dwFreeType
		);
		Assert(freeResult != 0); //TODO: Handle errors, call GetLastError and return an OsError_t
	}
	// +==============================+
	// |            Linux             |
	// +==============================+
	#elif LINUX_COMPILATION
	{
		int unmapResult = munmap(memoryPntr, reservedSize);
		Assert(unmapResult == 0); //TODO: Handle errors, check errno and return an OsError_t
	}
	// +==============================+
	// |             OSX              |
	// +==============================+
	// #elif OSX_COMPILATION
	// {
	// 	SetOptionalOutPntr(errorOut, OsError_UnsupportedPlatform);
	// }
	// +==============================+
	// |           Playdate           |
	// +==============================+
	#elif PLAYDATE_COMPILATION
	{
		AssertMsg(false, "OsFreeReservedMemory is not supported on PLAYDATE");
	}
	// +==============================+
	// |             Orca             |
	// +==============================+
	#elif ORCA_COMPILATION
	{
		AssertMsg(false, "OsFreeReservedMemory is not supported on ORCA");
	}
	// +==============================+
	// |             WASM             |
	// +==============================+
	#elif WASM_COMPILATION
	{
		AssertMsg(false, "OsFreeReservedMemory is not supported on WASM");
	}
	#else
	#error OsReserveMemory does not support the current platform yet!
	#endif
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_OS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MAX_EXECUTABLE_PATH_LENGTH
@Functions
MyStr_t OsGetExecutablePath(MemArena_t* memArena, OsError_t* errorOut)
MyStr_t OsGetWorkingDirectory(MemArena_t* memArena, OsError_t* errorOut)
u64 OsGetMemoryPageSize()
void* OsReserveMemory(u64 numBytes)
void OsCommitReservedMemory(void* memoryPntr, u64 numBytes)
void OsFreeReservedMemory(void* memoryPntr, u64 reservedSize)
*/