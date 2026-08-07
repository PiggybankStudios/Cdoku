/*
File:   gy_std.h
Author: Taylor Robbins
Date:   09\14\2021
*/

#ifndef _GY_STD_H
#define _GY_STD_H

#include "gy_defines_check.h"

#if ORCA_COMPILATION
extern "C" {
#endif // ORCA_COMPILATION

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
// #include <assert.h> //TODO: Do we want this?
#if !ORCA_COMPILATION
#include <stdio.h>
#include <new>
#endif
//TODO: I don't think we actually need to include algorithm here? fmin and similar functions come from math.h
// #include <algorithm> //Used for min and max functions
#if ORCA_COMPILATION
}
#endif // ORCA_COMPILATION

#if WINDOWS_COMPILATION
#include <intrin.h>
#elif OSX_COMPILATION
#include <signal.h>
#elif LINUX_COMPILATION
#include <signal.h>
#include <unistd.h> //needed for getpagesize
#include <sys/mman.h> //needed for mmap
#elif WASM_COMPILATION
#include <intrin.h>
#elif PLAYDATE_COMPILATION
#include "pd_api.h"
#elif ORCA_COMPILATION
#include <orca.h>
#else
#error Unsupported platform in gy_std.h
#endif

// +--------------------------------------------------------------+
// |                      Playdate Reroutes                       |
// +--------------------------------------------------------------+
#if PLAYDATE_COMPILATION

extern PlaydateAPI* pd;

void* (*pdrealloc)(void* ptr, size_t size);

#ifndef MyMalloc
#define MyMalloc(numBytes) pdrealloc(NULL, (numBytes))
#endif
#ifndef MyRealloc
#define MyRealloc(ptr, numBytes) pdrealloc((ptr), (numBytes))
#endif
#ifndef MyFree
#define MyFree(ptr) pdrealloc((ptr), 0)
#endif

#ifndef MyStrToFloat
#ifdef GYLIB_HEADER_ONLY
float ratof(char* arr);
#else
float ratof(char* arr)
{
	float val = 0;
	int afterdot=0;
	float scale=1;
	int neg = 0; 
	
	if (*arr == '-')
	{
		arr++;
		neg = 1;
	}
	while (*arr)
	{
		if (afterdot)
		{
			scale = scale/10;
			val = val + (*arr-'0')*scale;
		}
		else
		{
			if (*arr == '.') { afterdot++; }
			else { val = val * 10.0f + (*arr - '0'); }
		}
		arr++;
	}
	
	return ((neg) ? -val : val);
}
#endif
#define MyStrToFloat(nullTermStr) ratof(nullTermStr)
#endif

#if PLAYDATE_DEVICE
void* _malloc_r(struct _reent* _REENT, size_t nbytes) { return pdrealloc(NULL,nbytes); }
void* _realloc_r(struct _reent* _REENT, void* ptr, size_t nbytes) { return pdrealloc(ptr,nbytes); }
void _free_r(struct _reent* _REENT, void* ptr ) { if ( ptr != NULL ) pdrealloc(ptr,0); }
#endif //PLAYDATE_DEVICE

// On the playdate device itself, the stdlib implementation for printf style functions is unsupported, so we pull-in stb_sprintf.h and use that instead
// #if PLAYDATE_DEVICE

#ifndef GYLIB_HEADER_ONLY
#define STB_SPRINTF_IMPLEMENTATION
#endif
#include "stb/stb_sprintf.h"

#ifndef MyBufferPrintf
#define MyBufferPrintf(buffer, bufferSize, formatStr, ...) stbsp_snprintf((buffer), (bufferSize), (formatStr), ##__VA_ARGS__)
#endif
#ifndef MyVaListPrintf
#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) stbsp_vsnprintf((buffer), (bufferSize), (formatStr), vaList)
#endif

// #endif //PLAYDATE_DEVICE

#endif //PLAYDATE_COMPILATION

// +--------------------------------------------------------------+
// |                        Orca Reroutes                         |
// +--------------------------------------------------------------+
#if ORCA_COMPILATION

#ifndef MyMalloc
#define MyMalloc(numBytes) nullptr; static_assert(false, "basic malloc is not available in Web Assembly!")
#endif
#ifndef MyRealloc
#define MyRealloc(ptr, numBytes) nullptr; static_assert(false, "basic realloc is not available in Web Assembly!")
#endif
#ifndef MyFree
#define MyFree(ptr) static_assert(false, "basic free is not available in Web Assembly!")
#endif

#ifndef MyStrToFloat
#ifdef GYLIB_HEADER_ONLY
float ratof(char* arr);
#else
float ratof(char* arr)
{
	float val = 0;
	int afterdot=0;
	float scale=1;
	int neg = 0; 
	
	if (*arr == '-')
	{
		arr++;
		neg = 1;
	}
	while (*arr)
	{
		if (afterdot)
		{
			scale = scale/10;
			val = val + (*arr-'0')*scale;
		}
		else
		{
			if (*arr == '.') { afterdot++; }
			else { val = val * 10.0f + (*arr - '0'); }
		}
		arr++;
	}
	
	return ((neg) ? -val : val);
}
#endif
#define MyStrToFloat(nullTermStr) ratof(nullTermStr)
#endif

#endif //ORCA_COMPILATION

// +--------------------------------------------------------------+
// |                    Our Reroute Functions                     |
// +--------------------------------------------------------------+

#ifndef MyMalloc
#define MyMalloc(numBytes) malloc(numBytes)
#endif
#ifndef MyRealloc
#define MyRealloc(ptr, numBytes) realloc((ptr), (numBytes))
#endif
#ifndef MyFree
#define MyFree(ptr) free(ptr)
#endif
#ifndef MyMemSet
#define MyMemSet(dest, value, length)     memset((dest), (value), (length))
#endif
#ifndef MyMemCompare
#define MyMemCompare(ptr1, ptr2, length)  memcmp((ptr1), (ptr2), (length))
#endif
#ifndef MyMemCopy
#define MyMemCopy(dest, source, length)   memcpy((dest), (source), (length))
#endif
#ifndef MyMemMove
#define MyMemMove(dest, source, length)   memmove((dest), (source), (length))
#endif
#ifndef MyStrCopyNt
#define MyStrCopyNt(dest, source)         strcpy((dest), (source))
#endif
#ifndef MyStrCompareNt
#define MyStrCompareNt(str1, str2)        strcmp((str1), (str2))
#endif
#ifndef MyStrCompare
#define MyStrCompare(str1, str2, length)  strncmp((str1), (str2), (length))
#endif
#ifndef MyStrLength
#define MyStrLength(str)                  strlen(str)
#endif
#ifndef MyStrLength32
#define MyStrLength32(str)                ((u32)strlen(str))
#endif
#ifndef MyStrLength64
#define MyStrLength64(str)                ((u64)strlen(str))
#endif
#if WINDOWS_COMPILATION
	#ifndef MyWideStrLength
	#define MyWideStrLength(str)              wcslen(str)
	#endif
	#ifndef MyWideStrLength32
	#define MyWideStrLength32(str)            ((u32)wcslen(str))
	#endif
#endif
#ifndef MyStrStrNt
#define MyStrStrNt(str1, str2)            strstr((str1), (str2))
#endif
#ifndef MyBufferPrintf
#define MyBufferPrintf(buffer, bufferSize, formatStr, ...) snprintf((buffer), (bufferSize), (formatStr), ##__VA_ARGS__)
#endif
#ifndef MyVaListPrintf
	#if 0//TODO: Do we want to default back to vsnprintf in some cases?
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf_s(buffer, bufferSize, _TRUNCATE, formatStr, vaList)
	#else
	#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList) vsnprintf((buffer), (bufferSize), (formatStr), vaList)
	#endif
#endif
#ifndef MyHostToNetworkByteOrderU16
#define MyHostToNetworkByteOrderU16(integer) htons(integer)
#endif
#ifndef MyHostToNetworkByteOrderU32
#define MyHostToNetworkByteOrderU32(integer) htonl(integer)
#endif
#ifndef MyNetworkToHostByteOrderU16
#define MyNetworkToHostByteOrderU16(integer) ntohs(integer)
#endif
#ifndef MyNetworkToHostByteOrderU32
#define MyNetworkToHostByteOrderU32(integer) ntohl(integer)
#endif
#ifndef MyStrToFloat
#define MyStrToFloat(nullTermStr) atof(nullTermStr)
#endif

#endif // _GY_STD_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
@Functions
#define MyMalloc(numBytes)
#define MyRealloc(ptr, numBytes)
#define MyFree(ptr)
#define MyMemSet(dest, value, length)
#define MyMemCompare(ptr1, ptr2, length)
#define MyMemCopy(dest, source, length)
#define MyMemMove(dest, source, length)
#define MyStrCopyNt(dest, source)
#define MyStrCompareNt(str1, str2)
#define MyStrCompare(str1, str2, length)
#define MyStrLength(str)
#define MyStrLength32(str)
#define MyStrLength64(str)
#define MyWideStrLength(str)
#define MyWideStrLength32(str)
#define MyStrStrNt(str1, str2)
#define MyBufferPrintf(buffer, bufferSize, formatStr, ...)
#define MyVaListPrintf(buffer, bufferSize, formatStr, vaList)
#define MyHostToNetworkByteOrderU16(integer)
#define MyHostToNetworkByteOrderU32(integer)
#define MyNetworkToHostByteOrderU16(integer)
#define MyNetworkToHostByteOrderU32(integer)
*/
