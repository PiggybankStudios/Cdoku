/*
File:   gy_memory.h
Author: Taylor Robbins
Date:   09\24\2021
Description:
	** This is where we hold all of the implementations for various kinds of memory arenas
	
	@MemArenaType_Redirect An arena that simply acts as a redirect to alloc and free function pointers.
	The internal allocFunc and freeFunc should be updated after DLL reloads where appropriate.
	The used value only increases since the freeFunc does not provide size information. (unless allocSize is always provided to FreeMem)
	Alignment is not supported/
	
	@MemArenaType_Alias An arena that serves as a redirect to another arena. The size and used information
	will be directly copied from the base arena whenever an action is performed (this means the data could get stale if no actions are happening through this alias)
	The numAllocations count is local to this alias, not a copy of the base arena.
	Alignment is only supported if the base arena supports alignment
	
	@MemArenaType_StdHeap An arena that simply redirects to standard library functions malloc, free, and realloc.
	The used value only increases since the free function does not provide size information. (unless allocSize is always provided to FreeMem)
	Alignment is not supported.
	
	@MemArenaType_FixedHeap An arena that manages a space of fixed size and provides
	general purpose allocation/deallocation functionality.
	
	@MemArenaType_PagedHeap An arena that manages pages of memory and can allocate more pages from a base
	arena (or through allocFunc and freeFunc) when it runs out. On the outside this provides the same
	general purpose allocation/deallocation functionality as FixedHeap.
	
	@MemArenaType_MarkedStack An arena that acts like a "stack" where direct deallocation is not allowed
	but "marks" can be pushed and then later popped (freeing any memory allocated since the push)
	You can check for push/pop mismatches by checking the mark count when you expect it to be 0.
	The numAllocations count only increases since we don't keep track of how many allocations get freed in a pop
	
	@MemArenaType_PagedStack Similar to MarkedStack but this arena manages a linked list of pages
	and will allocate more pages whenever it runs out of memory
	
	@MemArenaType_VirtualStack Similar to MarkedStack but this arena reserves a massive amount of space
	through VirtualAlloc (or equivalent) and only commits no pages of that space as needed. This allows
	it to be completely sequential in memory but also grow to a massive size if needed
	
	@MemArenaType_Buffer An arena that provides a simple first in last out style allocation structure within
	a predefined space (usually a buffer from somewhere). When freeing memory you generally have to pass the
	size of the allocation you are freeing (except for simple single allocation scenarios).
	You can quickly create one of these buffer-backed arenas on the stack with CreateBufferArenaOnStack(arenaName, bufferName, size).
	(TODO: Add support for quickly taking a preallocated pntr and size and turning into a Buffer arena)
*/

//TODO: Add some standard way to copy an arena, perform some operations that should leave no changes, and then compare the
//      new state to the old state to find leaked allocations or inefficiencies in the arena implementation

#ifndef _GY_MEMORY_H
#define _GY_MEMORY_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_debug.h"

enum MemArenaType_t
{
	MemArenaType_None = 0,
	MemArenaType_Redirect,
	MemArenaType_Alias,
	MemArenaType_StdHeap,
	MemArenaType_FixedHeap,
	MemArenaType_PagedHeap,
	MemArenaType_MarkedStack,
	MemArenaType_PagedStack,
	MemArenaType_VirtualStack,
	MemArenaType_Buffer,
	MemArenaType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetMemArenaTypeStr(MemArenaType_t arenaType);
#else
const char* GetMemArenaTypeStr(MemArenaType_t arenaType)
{
	switch (arenaType)
	{
		case MemArenaType_None:         return "None";
		case MemArenaType_Redirect:     return "Redirect";
		case MemArenaType_Alias:        return "Alias";
		case MemArenaType_StdHeap:      return "StdHeap";
		case MemArenaType_FixedHeap:    return "FixedHeap";
		case MemArenaType_PagedHeap:    return "PagedHeap";
		case MemArenaType_MarkedStack:  return "MarkedStack";
		case MemArenaType_PagedStack:   return "PagedStack";
		case MemArenaType_VirtualStack: return "VirtualStack";
		case MemArenaType_Buffer:       return "Buffer";
		default: return "Unknown";
	}
}
#endif

enum AllocAlignment_t
{
	AllocAlignment_None    = 0,
	AllocAlignment_4Bytes  = 4,
	AllocAlignment_8Bytes  = 8,
	AllocAlignment_16Bytes = 16,
	AllocAlignment_64Bytes = 64,
	AllocAlignment_Max = AllocAlignment_64Bytes,
};

typedef void* AllocationFunction_f(u64 numBytes);
typedef void FreeFunction_f(void* memPntr);

#define HEAP_ALLOC_FILLED_FLAG 0x8000000000000000ULL
#define HEAP_ALLOC_SIZE_MASK   0x7FFFFFFFFFFFFFFFULL
struct HeapAllocPrefix_t
{
	u64 size; //top-bit is used as "filled" flag, includes prefix size
};

struct HeapPageHeader_t
{
	HeapPageHeader_t* next;
	u64 size;
	u64 used;
};

struct MarkedStackArenaHeader_t
{
	struct MarkedStackArenaHeader_t* next; //only used by PagedStack
	u64 maxNumMarks;
	u64 numMarks;
	u64 highMarkCount;
	u64 thisPageSize; //only used by PagedStack
};

enum MemArenaFlag_t
{
	MemArenaFlag_TelemetryEnabled = 0x0001,
	MemArenaFlag_SingleAlloc      = 0x0002,
	MemArenaFlag_AutoFreePages    = 0x0004,
	// MemArenaFlag_Unused           = 0x0008,
	MemArenaFlag_BreakOnAlloc     = 0x0010,
	MemArenaFlag_BreakOnFree      = 0x0020,
	MemArenaFlag_BreakOnRealloc   = 0x0040,
	MemArenaFlag_NumFlags         = 6,
};

struct MemArena_t
{
	MemArenaType_t type;
	#if GYLIB_DEBUG_NAMES_ENABLED
	const char* debugName;
	#endif
	u16 flags;
	AllocAlignment_t alignment;
	u64 pageSize;
	u64 maxSize;
	u64 maxNumPages;
	u64 debugBreakThreshold;
	
	u64 size;
	u64 used;
	u64 numPages;
	u64 numAllocations;
	u64 highUsedMark;
	u64 resettableHighUsedMark;
	u64 highAllocMark;
	
	void* headerPntr;
	void* mainPntr;
	void* otherPntr;
	AllocationFunction_f* allocFunc;
	FreeFunction_f* freeFunc;
	MemArena_t* sourceArena;
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	MemArena_t* debugArena;
	#endif
	GyMutex_t mutex;
};

struct GrowMemToken_t
{
	MemArena_t* memArena;
	void* nextSectionPntr;
	u64 nextSectionSize;
};

#if GYLIB_MEM_ARENA_DEBUG_ENABLED
struct MemArenaAllocInfo_t
{
	void* allocPntr;
	u64 allocSize;
	const char* filePath;
	u64 lineNumber;
	const char* funcName;
};
#endif

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define PackAllocPrefixSize(used, size)   ((used) ? HEAP_ALLOC_FILLED_FLAG : 0ULL) | ((size) & HEAP_ALLOC_SIZE_MASK)
#define IsAllocPrefixFilled(packedSize)   IsFlagSet((packedSize), HEAP_ALLOC_FILLED_FLAG)
#define UnpackAllocPrefixSize(packedSize) ((packedSize) & HEAP_ALLOC_SIZE_MASK)

#define PrintInArenaVa(arena, resultName, resultLengthName, formatString)                  \
char* resultName = nullptr;                                                                \
int resultLengthName = 0;                                                                  \
va_list args;                                                                              \
do                                                                                         \
{                                                                                          \
	va_start(args, formatString);                                                          \
	resultLengthName = PrintVa_Measure((formatString), args);                              \
	va_end(args);                                                                          \
	if (resultLengthName >= 0)                                                             \
	{                                                                                      \
		resultName = AllocArray((arena), char, resultLengthName+1); /*Allocate*/           \
		if (resultName == nullptr) { break; }                                              \
		va_start(args, formatString);                                                      \
		PrintVa_Print((formatString), args, resultName, resultLengthName);                 \
		va_end(args);                                                                      \
	}                                                                                      \
}                                                                                          \
while(0)

// +==============================+
// |      C++ Class Helpers       |
// +==============================+
#define InPlaceNew(type, pntrToClass, ...) new (pntrToClass) type(__VA_ARGS__)
#define ArenaNew(type, pntrToAssign, arena, ...) do  \
{                                                    \
	(pntrToAssign) = AllocStruct((arena), type);     \
	NotNull(pntrToAssign);                           \
	InPlaceNew(type, (pntrToAssign), ##__VA_ARGS__); \
} while(0)
#define ArenaDelete(type, pntr, arena) do   \
{                                           \
	(pntr)->~type();                        \
	FreeMem((arena), (pntr), sizeof(type)); \
	pntr = nullptr;                         \
} while(0)

#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	#define AllocMem(arena, numBytes, ...) AllocMem_(__FILE__, __LINE__, __func__, (arena), (numBytes), ##__VA_ARGS__)
	#define ReallocMem(arena, allocPntr, newSize, ...) ReallocMem_(__FILE__, __LINE__, __func__, (arena), (allocPntr), (newSize), ##__VA_ARGS__)
	#define HardReallocMem(arena, allocPntr, newSize) ReallocMem_(__FILE__, __LINE__, __func__, (arena), (allocPntr), (newSize), 0, AllocAlignment_None, false)
	#define SoftReallocMem(arena, allocPntr, newSize) ReallocMem_(__FILE__, __LINE__, __func__, (arena), (allocPntr), (newSize), 0, AllocAlignment_None, true)
	#define AllocStruct(arena, structName)          (structName*)AllocMem_(__FILE__, __LINE__, __func__, (arena), sizeof(structName))
	#define AllocArray(arena, structName, numItems) (structName*)AllocMem_(__FILE__, __LINE__, __func__, (arena), sizeof(structName) * (numItems))
	#define AllocBytes(arena, numBytes)             (u8*)AllocMem_(__FILE__, __LINE__, __func__, (arena), (numBytes))
	#define AllocChars(arena, numBytes)             (char*)AllocMem_(__FILE__, __LINE__, __func__, (arena), (numBytes))
#else //!GYLIB_MEM_ARENA_DEBUG_ENABLED
	#define AllocMem(arena, numBytes, ...) AllocMem_((arena), (numBytes), ##__VA_ARGS__)
	#define ReallocMem(arena, allocPntr, newSize, ...) ReallocMem_((arena), (allocPntr), (newSize), ##__VA_ARGS__)
	#define HardReallocMem(arena, allocPntr, newSize) ReallocMem_((arena), (allocPntr), (newSize), 0, AllocAlignment_None, false)
	#define SoftReallocMem(arena, allocPntr, newSize) ReallocMem_((arena), (allocPntr), (newSize), 0, AllocAlignment_None, true)
	#define AllocStruct(arena, structName)          (structName*)AllocMem_((arena), sizeof(structName))
	#define AllocArray(arena, structName, numItems) (structName*)AllocMem_((arena), sizeof(structName) * (numItems))
	#define AllocBytes(arena, numBytes)             (u8*)AllocMem_((arena), (numBytes))
	#define AllocChars(arena, numBytes)             (char*)AllocMem_((arena), (numBytes))
#endif //GYLIB_MEM_ARENA_DEBUG_ENABLED

#define HardFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, false)
#define SoftFreeMem(arena, allocPntr) FreeMem((arena), (allocPntr), 0, true)
#define FreeBufferArena(bufferArena, sourceArena) FreeMem((sourceArena), (bufferArena)->mainPntr, (bufferArena)->size)

// +--------------------------------------------------------------+
// |                    Pre-Declared Functions                    |
// +--------------------------------------------------------------+
// These are pre-declared because there are somewhat cyclical dependencies within this file
#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	void* AllocMem_(const char* filePath, u64 lineNumber, const char* funcName, MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None);
	void* ReallocMem_(const char* filePath, u64 lineNumber, const char* funcName, MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize = 0, AllocAlignment_t alignOverride = AllocAlignment_None, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
	bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize = 0, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
#else //!GYLIB_MEM_ARENA_DEBUG_ENABLED
	void* AllocMem_(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None);
	void* ReallocMem_(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize = 0, AllocAlignment_t alignOverride = AllocAlignment_None, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
	bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize = 0, bool ignoreNullptr = false, u64* oldSizeOut = nullptr);
#endif //GYLIB_MEM_ARENA_DEBUG_ENABLED

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment);
	u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment);
	bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize, bool inclusive = false);
	void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc);
	void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena);
	void InitMemArena_StdHeap(MemArena_t* arena);
	void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_PagedStackArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_PagedStackFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_VirtualStack(MemArena_t* arena, u64 maxSize, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None);
	void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc = false, AllocAlignment_t alignment = AllocAlignment_None);
	void UpdateMemArenaFuncPntrs(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc);
	bool IsInitialized(const MemArena_t* arena);
	bool DoesMemArenaSupportFreeing(MemArena_t* arena);
	bool DoesMemArenaSupportPushAndPop(MemArena_t* arena);
	u64 GetNumMarks(MemArena_t* arena);
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	void StoreAllocInfo(const MemArena_t* refArena, MemArena_t* arena, void* allocPntr, u64 allocSize, const char* filePath, u64 lineNumber, const char* funcName);
	MemArenaAllocInfo_t* FindAllocInfoFor(MemArena_t* arena, void* allocPntr);
	MemArenaAllocInfo_t* FindExtraAllocInfoInArena(MemArena_t* arena, MemArena_t* realArena);
	void* FindMissingAllocInfoInArena(MemArena_t* arena, u64* allocSizeOut = nullptr);
	void FreeAllocInfo(MemArena_t* refArena, MemArena_t* arena, void* allocPntr);
	#endif
	bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure = false);
	MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None);
	char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm = true);
	char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm = true);
	u64 GrowMemQuery(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, GrowMemToken_t* tokenOut = nullptr);
	void GrowMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize, const GrowMemToken_t* token);
	void ShrinkMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize);
	void FreeMemArena(MemArena_t* arena);
	void ClearMemArena(MemArena_t* arena);
	u64 PushMemMark(MemArena_t* arena);
	void PopMemMark(MemArena_t* arena, u64 mark = 0xFFFFFFFFFFFFFFFFULL);
	char* PrintInArena(MemArena_t* arena, const char* formatString, ...);
	int PrintVa_Measure(const char* formatString, va_list args);
	void PrintVa_Print(const char* formatString, va_list args, char* allocatedSpace, int previousResult);
#else

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+

bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return true; }
	u64 address = (u64)(memoryPntr);
	return ((address % (u64)alignment) == 0);
}

u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment)
{
	if (alignment == AllocAlignment_None) { return 0; }
	u64 address = (u64)(memoryPntr);
	if ((address % (u64)alignment) == 0) { return 0; }
	else { return (u8)alignment - (u8)(address % (u64)alignment); }
}

bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize, bool inclusive = false)
{
	if (((const u8*)testPntr) < ((const u8*)rangeBase)) { return false; }
	if (((const u8*)testPntr) > ((const u8*)rangeBase) + rangeSize) { return false; }
	if (((const u8*)testPntr) == ((const u8*)rangeBase) + rangeSize && !inclusive) { return false; }
	return true;
}

// +--------------------------------------------------------------+
// |                        Init Functions                        |
// +--------------------------------------------------------------+
void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
{
	NotNull(arena);
	NotNull(allocFunc);
	ClearPointer(arena);
	arena->type = MemArenaType_Redirect;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	arena->used = 0; //NOTE: This only tracks allocations, not deallocations, so it only goes up
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena)
{
	NotNull(arena);
	NotNull(sourceArena);
	ClearPointer(arena);
	arena->type = MemArenaType_Alias;
	arena->sourceArena = sourceArena;
	arena->used = 0; //NOTE: This MAY not track deallocations, it depends on if the sourceArena supports returning the size on FreeMem
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->resettableHighUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_StdHeap(MemArena_t* arena)
{
	#if !ORCA_COMPILATION
	NotNull(arena);
	ClearPointer(arena);
	arena->type = MemArenaType_StdHeap;
	arena->used = 0; //NOTE: This only tracks allocations, not deallocations, so it only goes up
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highAllocMark = arena->numAllocations;
	#else
	AssertMsg_(false, "StdHeap type memory arena is not supported without the standard library being present!");
	#endif //!ORCA_COMPILATION
}
void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	u64 prefixSize = sizeof(HeapAllocPrefix_t);
	Assert(size > prefixSize);
	NotNull(memoryPntr);
	Assert(IsAlignedTo(memoryPntr, alignment));
	ClearPointer(arena);
	arena->type = MemArenaType_FixedHeap;
	arena->alignment = alignment;
	arena->mainPntr = memoryPntr;
	arena->size = size;
	HeapAllocPrefix_t* firstAlloc = (HeapAllocPrefix_t*)arena->mainPntr;
	ClearPointer(firstAlloc);
	firstAlloc->size = PackAllocPrefixSize(false, arena->size);
	arena->used = prefixSize;
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->resettableHighUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}
void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(allocFunc);
	ClearPointer(arena);
	arena->type = MemArenaType_PagedHeap;
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->maxNumPages = maxNumPages;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	FlagSet(arena->flags, MemArenaFlag_AutoFreePages);
	arena->size = 0;
	arena->used = 0;
	arena->numPages = 0;
	arena->numAllocations = 0;
	arena->headerPntr = nullptr;
	arena->mainPntr = nullptr;
	arena->otherPntr = nullptr;
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	arena->highAllocMark = 0;
}
void InitMemArena_PagedHeapArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
{
	ClearPointer(arena);
	arena->type = MemArenaType_PagedHeap;
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->maxNumPages = maxNumPages;
	arena->sourceArena = sourceArena;
	arena->size = 0;
	arena->used = 0;
	arena->numPages = 0;
	arena->numAllocations = 0;
	arena->headerPntr = nullptr;
	arena->mainPntr = nullptr;
	arena->otherPntr = nullptr;
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	arena->highAllocMark = 0;
}
void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(memoryPntr);
	Assert(size > 0);
	Assert(maxNumMarks > 0);
	Assert(size > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	
	ClearPointer(arena);
	arena->type = MemArenaType_MarkedStack;
	arena->alignment = alignment;
	arena->headerPntr = (((u8*)memoryPntr) + 0);
	arena->otherPntr  = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t));
	arena->mainPntr   = (((u8*)memoryPntr) + sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->size = size - (sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	arena->used = 0;
	arena->numAllocations = 0;
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	ClearPointer(stackHeader);
	stackHeader->maxNumMarks = maxNumMarks;
	stackHeader->numMarks = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	stackHeader->highMarkCount = 0;
}
void InitMemArena_PagedStackArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(sourceArena);
	Assert(pageSize > 0);
	Assert(maxNumMarks > 0);
	Assert(pageSize > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	
	ClearPointer(arena);
	arena->type = MemArenaType_PagedStack;
	arena->sourceArena = sourceArena;
	arena->flags = 0x00;
	FlagSet(arena->flags, MemArenaFlag_AutoFreePages);
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->used = 0;
	arena->numAllocations = 0;
	
	u8* firstPageBytes = (u8*)AllocMem(sourceArena, pageSize);
	NotNull(firstPageBytes);
	MarkedStackArenaHeader_t* firstPage = (MarkedStackArenaHeader_t*)firstPageBytes;
	ClearPointer(firstPage);
	firstPage->next = nullptr;
	firstPage->thisPageSize = pageSize;
	firstPage->maxNumMarks = maxNumMarks;
	firstPage->numMarks = 0;
	
	arena->headerPntr = firstPage;
	arena->size = firstPage->thisPageSize - sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64));
	arena->numPages = 1;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	firstPage->highMarkCount = 0;
}
void InitMemArena_PagedStackFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(allocFunc);
	Assert(pageSize > 0);
	Assert(maxNumMarks > 0);
	Assert(pageSize > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	
	ClearPointer(arena);
	arena->type = MemArenaType_PagedStack;
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
	arena->flags = 0x00;
	FlagSet(arena->flags, MemArenaFlag_AutoFreePages);
	arena->alignment = alignment;
	arena->pageSize = pageSize;
	arena->used = 0;
	arena->numAllocations = 0;
	
	u8* firstPageBytes = (u8*)arena->allocFunc(pageSize);
	NotNull(firstPageBytes);
	MarkedStackArenaHeader_t* firstPage = (MarkedStackArenaHeader_t*)firstPageBytes;
	ClearPointer(firstPage);
	firstPage->next = nullptr;
	firstPage->thisPageSize = pageSize;
	firstPage->maxNumMarks = maxNumMarks;
	firstPage->numMarks = 0;
	
	arena->headerPntr = firstPage;
	arena->size = firstPage->thisPageSize - sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64));
	arena->numPages = 1;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	firstPage->highMarkCount = 0;
}
void InitMemArena_VirtualStack(MemArena_t* arena, u64 maxSize, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	Assert(maxSize > sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64)));
	Assert(maxNumMarks > 0);
	ClearPointer(arena);
	arena->type = MemArenaType_VirtualStack;
	arena->flags = 0x00;
	arena->alignment = alignment;
	arena->used = 0;
	arena->numAllocations = 0;
	
	arena->pageSize = OsGetMemoryPageSize();
	arena->maxSize = RoundUpToU64(maxSize, arena->pageSize);
	u8* reservedMemPntr = (u8*)OsReserveMemory(arena->maxSize);
	NotNull(reservedMemPntr);
	u64 headerAndMarksSize = sizeof(MarkedStackArenaHeader_t) + (maxNumMarks * sizeof(u64));
	u64 headerAndMarksSizeRoundedUp = RoundUpToU64(headerAndMarksSize, arena->pageSize);
	OsCommitReservedMemory(reservedMemPntr, headerAndMarksSizeRoundedUp);
	arena->headerPntr = &reservedMemPntr[0];
	arena->otherPntr  = &reservedMemPntr[sizeof(MarkedStackArenaHeader_t)];
	arena->mainPntr   = &reservedMemPntr[headerAndMarksSize];
	arena->size = headerAndMarksSizeRoundedUp - headerAndMarksSize;
	
	MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
	NotNull(stackHeader);
	ClearPointer(stackHeader);
	stackHeader->maxNumMarks = maxNumMarks;
	stackHeader->numMarks = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = 0;
	arena->resettableHighUsedMark = 0;
	stackHeader->highMarkCount = 0;
}
void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc = false, AllocAlignment_t alignment = AllocAlignment_None)
{
	NotNull(arena);
	NotNull(bufferPntr);
	ClearPointer(arena);
	arena->type = MemArenaType_Buffer;
	arena->alignment = alignment;
	FlagSetTo(arena->flags, MemArenaFlag_SingleAlloc, singleAlloc);
	arena->mainPntr = bufferPntr;
	arena->size = bufferSize;
	arena->used = 0;
	arena->numAllocations = 0;
	
	FlagSet(arena->flags, MemArenaFlag_TelemetryEnabled);
	arena->highUsedMark = arena->used;
	arena->resettableHighUsedMark = arena->used;
	arena->highAllocMark = arena->numAllocations;
}

#define CreateBufferArenaOnStack(arenaName, bufferName, size) MemArena_t arenaName; u8 bufferName[size]; InitMemArena_Buffer(&arenaName, (size), &bufferName[0])

void UpdateMemArenaFuncPntrs(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
{
	Assert(arena->type == MemArenaType_Redirect || arena->type == MemArenaType_PagedHeap);
	arena->allocFunc = allocFunc;
	arena->freeFunc = freeFunc;
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
bool IsInitialized(const MemArena_t* arena)
{
	NotNull(arena);
	return (arena->type != MemArenaType_None);
}

//i.e. Does FreeMem allow me to free allocations in any order
bool DoesMemArenaSupportFreeing(MemArena_t* arena)
{
	NotNull(arena);
	switch (arena->type)
	{
		case MemArenaType_Alias: return DoesMemArenaSupportFreeing(arena->sourceArena);
		case MemArenaType_MarkedStack:
		case MemArenaType_PagedStack:
		case MemArenaType_VirtualStack: return false; //stacks have a very narrow case that FreeMem actually works but they generally don't support freeing in arbitrary order
		default: return true;
	}
}

bool DoesMemArenaSupportPushAndPop(MemArena_t* arena)
{
	NotNull(arena);
	switch (arena->type)
	{
		case MemArenaType_Alias: return DoesMemArenaSupportPushAndPop(arena->sourceArena);
		case MemArenaType_MarkedStack:
		case MemArenaType_PagedStack:
		case MemArenaType_VirtualStack: return true;
		default: return false;
	}
}

u64 GetNumMarks(MemArena_t* arena)
{
	NotNull(arena);
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	u64 result = 0;
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_MarkedStack GetNumMarks |
		// +======================================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			result = stackHeader->numMarks;
		} break;
		
		// +======================================+
		// | MemArenaType_PagedStack GetNumMarks  |
		// +======================================+
		case MemArenaType_PagedStack:
		{
			NotNull(arena->headerPntr);
			MarkedStackArenaHeader_t* firstPageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(firstPageHeader->maxNumMarks > 0);
			Assert(firstPageHeader->numMarks <= firstPageHeader->maxNumMarks);
			result = firstPageHeader->numMarks;
		} break;
		
		// +========================================+
		// | MemArenaType_VirtualStack GetNumMarks  |
		// +========================================+
		case MemArenaType_VirtualStack:
		{
			NotNull(arena->headerPntr);
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			result = stackHeader->numMarks;
		} break;
		
		default: AssertMsg(false, "Tried to GetNumMarks on arena that doesn't support pushing and popping"); break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return result;
}

// +--------------------------------------------------------------+
// |                       Debug Alloc Info                       |
// +--------------------------------------------------------------+
#if GYLIB_MEM_ARENA_DEBUG_ENABLED
void StoreAllocInfo(const MemArena_t* refArena, MemArena_t* arena, void* allocPntr, u64 allocSize, const char* filePath, u64 lineNumber, const char* funcName)
{
	UNUSED(refArena);
	MemArenaAllocInfo_t* allocInfo = (MemArenaAllocInfo_t*)AllocMem(arena, sizeof(MemArenaAllocInfo_t));
	if (allocInfo == nullptr) { return; }
	// ClearPointer(allocInfo);
	// if (refArena->numAllocations != arena->numAllocations) { MyDebugBreak(); }
	allocInfo->allocPntr = allocPntr;
	allocInfo->allocSize = allocSize;
	allocInfo->filePath = filePath;
	allocInfo->lineNumber = lineNumber;
	allocInfo->funcName = funcName;
}
MemArenaAllocInfo_t* FindAllocInfoFor(MemArena_t* arena, void* allocPntr)
{
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_PagedHeap FindAllocInfo |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u64 allocOffset = 0;
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPrefix = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPrefix->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPrefix->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (isAllocFilled && allocAfterPrefixSize == sizeof(MemArenaAllocInfo_t))
					{
						MemArenaAllocInfo_t* allocInfo = (MemArenaAllocInfo_t*)allocAfterPrefixPntr;
						if (allocInfo->allocPntr == allocPntr)
						{
							if (didLock) { UnlockGyMutex(&arena->mutex); }
							return allocInfo;
						}
					}
					
					allocBytePntr += allocSize;
					allocOffset += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
		} break;
		
		default:
		{
			DebugAssertMsg(false, "Arena type not supported for store debug info! We need to implement a walk where we find allocInfo by reference pntr");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return nullptr;
}
MemArenaAllocInfo_t* FindExtraAllocInfoInArena(MemArena_t* arena, MemArena_t* realArena)
{
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_PagedHeap FreeAllocInfo |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u64 allocOffset = 0;
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPrefix = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPrefix->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPrefix->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (isAllocFilled && allocAfterPrefixSize == sizeof(MemArenaAllocInfo_t))
					{
						MemArenaAllocInfo_t* allocInfo = (MemArenaAllocInfo_t*)allocAfterPrefixPntr;
						if (!FreeMem(realArena, allocInfo->allocPntr, allocInfo->allocSize))
						{
							if (didLock) { UnlockGyMutex(&arena->mutex); }
							return allocInfo;
						}
					}
					
					allocBytePntr += allocSize;
					allocOffset += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
		} break;
		
		default:
		{
			DebugAssertMsg(false, "Arena type not supported for store debug info! We need to implement a walk where we find allocInfo by reference pntr");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return nullptr;
}
void* FindMissingAllocInfoInArena(MemArena_t* arena, u64* allocSizeOut = nullptr)
{
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	NotNull2(arena, arena->debugArena);
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_PagedHeap FreeAllocInfo |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u64 allocOffset = 0;
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPrefix = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPrefix->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPrefix->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (isAllocFilled)
					{
						MemArenaAllocInfo_t* allocInfo = FindAllocInfoFor(arena->debugArena, allocAfterPrefixPntr);
						if (allocInfo == nullptr)
						{
							MyDebugBreak();
							SetOptionalOutPntr(allocSizeOut, allocAfterPrefixSize);
							if (didLock) { UnlockGyMutex(&arena->mutex); }
							return allocAfterPrefixPntr;
						}
					}
					
					allocBytePntr += allocSize;
					allocOffset += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
		} break;
		
		default:
		{
			DebugAssertMsg(false, "Arena type not supported for store debug info! We need to implement a walk where we find allocInfo by reference pntr");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return nullptr;
}
//TODO: Add const back to refArena
void FreeAllocInfo(MemArena_t* refArena, MemArena_t* arena, void* allocPntr)
{
	UNUSED(refArena);
	MemArenaAllocInfo_t* allocInfo = FindAllocInfoFor(arena, allocPntr);
	if (allocInfo != nullptr)
	{
		FreeMem(arena, allocInfo, sizeof(MemArenaAllocInfo_t));
	}
	if (refArena->numAllocations != arena->numAllocations)
	{
		#if 1
		MyDebugBreak();
		u64 missingInfoSize = 0;
		void* missingInfoPntr = FindMissingAllocInfoInArena(refArena, &missingInfoSize);
		UNUSED(missingInfoSize);
		UNUSED(missingInfoPntr);
		refArena->debugArena = nullptr;
		MemArenaAllocInfo_t* extraAllocInfo = FindExtraAllocInfoInArena(arena, refArena);
		UNUSED(extraAllocInfo);
		refArena->debugArena = arena;
		MyDebugBreak();
		#endif
	}
}
#endif //GYLIB_MEM_ARENA_DEBUG_ENABLED

// +--------------------------------------------------------------+
// |                       Verify Function                        |
// +--------------------------------------------------------------+
bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure = false)
{
	NotNull(arena);
	if (arena->type == MemArenaType_None)
	{
		AssertIfMsg(assertOnFailure, false, "Tried to verify uninitialized arena");
		return false;
	}
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_Redirect MemArenaVerify |
		// +======================================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +====================================+
		// | MemArenaType_Alias MemArenaVerify  |
		// +====================================+
		case MemArenaType_Alias:
		{
			//TODO: Implement me!
		} break;
		
		// +======================================+
		// | MemArenaType_StdHeap MemArenaVerify  |
		// +======================================+
		// case MemArenaType_StdHeap:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +========================================+
		// | MemArenaType_FixedHeap MemArenaVerify  |
		// +========================================+
		case MemArenaType_FixedHeap:
		{
			if (arena->mainPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap mainPntr is null");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->used >= arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is larger than size");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->used > arena->highUsedMark)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is higher than high used mark");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->numAllocations > arena->highAllocMark)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap allocation count is higher than high allocation mark");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (!IsAlignedTo(arena->mainPntr, arena->alignment))
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap main memory not aligned to alignment setting");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_SingleAlloc) && arena->numAllocations > 1)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap single alloc doesn't match allocation count");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->used < sizeof(HeapAllocPrefix_t))
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is smaller than 1 prefix size");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			
			u64 numFilledSections = 0;
			bool lastSectionWasEmpty = false;
			u64 sectionIndex = 0;
			u64 totalUsed = 0;
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			HeapAllocPrefix_t* prevPrefixPntr = nullptr;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				UNUSED(allocAfterPrefixPntr);
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				if (allocSize < sizeof(HeapAllocPrefix_t))
				{
					AssertIfMsg(assertOnFailure, false, "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				UNUSED(allocAfterPrefixSize);
				if (isAllocFilled)
				{
					if (numFilledSections + 1 > arena->numAllocations)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap numAllocations doesn't match actual number of filled sections");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					numFilledSections++;
					if (totalUsed + allocSize > arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap used doesn't match actual total used area");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					totalUsed += allocSize;
					lastSectionWasEmpty = false;
				}
				else //not filled
				{
					if (lastSectionWasEmpty)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap two empty sections in a row");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (totalUsed + sizeof(HeapAllocPrefix_t) > arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "FixedHeap used doesn't match actual total used area");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					totalUsed += sizeof(HeapAllocPrefix_t);
					lastSectionWasEmpty = true;
				}
				if (allocOffset + allocSize > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "FixedHeap corrupt section size stepping us past the end of the arena memory");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				prevPrefixPntr = allocPntr;
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			UNUSED(prevPrefixPntr); //used for debug purposes
			AssertMsg(allocOffset == arena->size, "Somehow stepped past end in ArenaVerify on FixedHeap despite the in-loop check");
			
			if (totalUsed != arena->used)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap used is higher than actual used amount");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (numFilledSections != arena->numAllocations)
			{
				AssertIfMsg(assertOnFailure, false, "FixedHeap numAllocations is higher than actual used section count");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
		} break;
		
		// +========================================+
		// | MemArenaType_PagedHeap MemArenaVerify  |
		// +========================================+
		case MemArenaType_PagedHeap:
		{
			if (arena->headerPntr == nullptr && arena->numPages > 0)
			{
				AssertIfMsg(assertOnFailure, false, "headerPntr was empty but numPages > 0! Has this arena been initialized??");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if ((arena->sourceArena == nullptr) && (arena->allocFunc == nullptr || arena->freeFunc == nullptr))
			{
				AssertIfMsg(assertOnFailure, false, "PagedHeap doesn't have a allocFunc/freeFun nor a sourceArena to allocate new pages from");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->mainPntr != nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr was filled when it shouldn't be!");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->otherPntr != nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr was filled when it shouldn't be!");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->pageSize == 0)
			{
				AssertIfMsg(assertOnFailure, false, "pageSize was zero! That's invalid!");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->alignment > AllocAlignment_Max)
			{
				AssertIfMsg(assertOnFailure, false, "Invalid alignment value!");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			
			u64 numAllocations = 0;
			u64 totalNumSections = 0;
			bool missingDebugForAllocation = false;
			
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				if (pageHeader->size == 0)
				{
					AssertIfMsg(assertOnFailure, false, "Page had a size of 0!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (pageHeader->size < arena->pageSize)
				{
					AssertIfMsg(assertOnFailure, false, "Page size was less than arena->pageSize!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (pageIndex+1 < arena->numPages && pageHeader->next == nullptr)
				{
					AssertIfMsg(assertOnFailure, false, "Page next pntr was nullptr too soon! We expected more pages in the chain!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (pageHeader->used > pageHeader->size)
				{
					AssertIfMsg(assertOnFailure, false, "Page used is higher than size! That's not right!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (pageIndex >= arena->numPages)
				{
					AssertIfMsg(assertOnFailure, false, "The numPages in this paged heap is off. We have too many pages or the last pointer to a page was corrupt!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				
				u8* pageBase = (u8*)(pageHeader + 1);
				u64 allocOffset = 0;
				u8* allocBytePntr = pageBase;
				u64 sectionIndex = 0;
				HeapAllocPrefix_t* prevPrefixPntr = nullptr;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
					u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
					if (sectionSize < sizeof(HeapAllocPrefix_t))
					{
						AssertIfMsg(assertOnFailure, false, "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
					if (afterPrefixSize == 0)
					{
						AssertIfMsg(assertOnFailure, false, "Found an empty section that was only big enough to contain the allocation header");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (allocOffset + sectionSize > pageHeader->size)
					{
						AssertIfMsg(assertOnFailure, false, "Found a corrupt allocation header size. It would step us past the end of a page!");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					
					if (isSectionFilled)
					{
						#if GYLIB_MEM_ARENA_DEBUG_ENABLED
						if (arena->debugArena != nullptr)
						{
							MemArenaAllocInfo_t* allocInfo = FindAllocInfoFor(arena, afterPrefixPntr);
							if (allocInfo == nullptr)
							{
								AssertIfMsg(assertOnFailure, false, "An allocation does NOT have associated metadata backing it in the debugArena!");
								MyDebugBreak();
								missingDebugForAllocation = true;
							}
						}
						#else
						UNUSED(afterPrefixPntr);
						#endif
						
						numAllocations++;
					}
					
					prevPrefixPntr = prefixPntr;
					allocOffset += sectionSize;
					allocBytePntr += sectionSize;
					totalNumSections++;
					sectionIndex++;
				}
				
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && numAllocations != arena->numAllocations)
			{
				AssertIfMsg(assertOnFailure, false, "Actual allocation count in paged heap did not match tracked numAllocations");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			
			if (didLock) { UnlockGyMutex(&arena->mutex); }
			return !missingDebugForAllocation;
		} break;
		
		// +==========================================+
		// | MemArenaType_MarkedStack MemArenaVerify  |
		// +==========================================+
		case MemArenaType_MarkedStack:
		{
			if (arena->size == 0)
			{
				AssertIfMsg(assertOnFailure, false, "arena size is 0");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->used > arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "arena used is greater than size");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->headerPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "headerPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->otherPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->mainPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			if (stackHeader->maxNumMarks * sizeof(u64) >= arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "stackHeader for MarkedStack has invalid value for maxNumMarks (based on size of arena)");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (stackHeader->numMarks > stackHeader->maxNumMarks)
			{
				AssertIfMsg(assertOnFailure, false, "numMarks is greater than maxNumMarks in MarkedStack header");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "highUsedMark is greater than arena size");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (arena->resettableHighUsedMark > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "resettableHighUsedMark is greater than arena size");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (arena->highUsedMark < arena->used)
				{
					AssertIfMsg(assertOnFailure, false, "used is greater than current highUsedMark");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (stackHeader->highMarkCount > stackHeader->maxNumMarks)
				{
					AssertIfMsg(assertOnFailure, false, "highMarkCount is greater than maxNumMarks in MarkedStack header");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (stackHeader->highMarkCount < stackHeader->numMarks)
				{
					AssertIfMsg(assertOnFailure, false, "highMarkCount is less than numMarks in MarkedStack header");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
			}
			u8* expectedOtherPntr = ((u8*)arena->headerPntr) + sizeof(MarkedStackArenaHeader_t);
			if (arena->otherPntr != expectedOtherPntr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr is not where it's supposed to be compared to headerPntr");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			u8* expectedMainPntr = expectedOtherPntr + stackHeader->maxNumMarks * sizeof(u64);
			if (arena->mainPntr != expectedMainPntr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr is not where it's supposed to be compared to headerPntr/otherPntr");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			for (u64 mIndex = 0; mIndex < stackHeader->numMarks; mIndex++)
			{
				if (marksPntr[mIndex] > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "One of the marks has and invalid value (too big, given the arena->size)");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (marksPntr[mIndex] > arena->used)
				{
					AssertIfMsg(assertOnFailure, false, "One of the marks is above the current used amount!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
			}
		} break;
		
		// +========================================+
		// | MemArenaType_PagedStack MemArenaVerify |
		// +========================================+
		case MemArenaType_PagedStack:
		{
			if (arena->size == 0)
			{
				AssertIfMsg(assertOnFailure, false, "arena size is 0");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->used > arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "arena used is greater than size");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->headerPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "headerPntr is nullptr in PagedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			u64 pageIndex = 0;
			MarkedStackArenaHeader_t* pageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			while (pageHeader != nullptr)
			{
				if (pageIndex >= arena->numPages)
				{
					AssertIfMsg(assertOnFailure, false, "pageHeader linked list is longer than numPages in PagedStack");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (sizeof(MarkedStackArenaHeader_t) + (pageHeader->maxNumMarks * sizeof(u64)) >= pageHeader->thisPageSize)
				{
					AssertIfMsg(assertOnFailure, false, "pageHeader for PagedStack has invalid value for maxNumMarks (based on size of arena)");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				
				// u64 pageSize = pageHeader->thisPageSize - sizeof(MarkedStackArenaHeader_t) - (pageHeader->maxNumMarks * sizeof(u64));
				u64* pageMarks = (u64*)(((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t));
				// u8* pageBase = ((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t) + (pageHeader->maxNumMarks * sizeof(u64));
				
				if (pageHeader->numMarks > pageHeader->maxNumMarks)
				{
					AssertIfMsg(assertOnFailure, false, "numMarks is greater than maxNumMarks in PagedStack header");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
				{
					if (arena->highUsedMark > arena->size)
					{
						AssertIfMsg(assertOnFailure, false, "highUsedMark is greater than arena size");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (arena->resettableHighUsedMark > arena->size)
					{
						AssertIfMsg(assertOnFailure, false, "resettableHighUsedMark is greater than arena size");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (arena->highUsedMark < arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "used is greater than current highUsedMark");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (pageHeader->highMarkCount > pageHeader->maxNumMarks)
					{
						AssertIfMsg(assertOnFailure, false, "highMarkCount is greater than maxNumMarks in PagedStack header");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (pageHeader->highMarkCount < pageHeader->numMarks)
					{
						AssertIfMsg(assertOnFailure, false, "highMarkCount is less than numMarks in PagedStack header");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
				}
				
				for (u64 mIndex = 0; mIndex < pageHeader->numMarks; mIndex++)
				{
					if (pageMarks[mIndex] > arena->size)
					{
						AssertIfMsg(assertOnFailure, false, "One of the marks has and invalid value (too big, given the arena->size)");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
					if (pageMarks[mIndex] > arena->used)
					{
						AssertIfMsg(assertOnFailure, false, "One of the marks is above the current used amount!");
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return false;
					}
				}
				
				pageHeader = pageHeader->next;
			}
			
			if (pageIndex != arena->numPages)
			{
				AssertIfMsg(assertOnFailure, false, "pageHeader linked list is shorter than numPages in PagedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
		} break;
		
		// +==========================================+
		// | MemArenaType_VirtualStack MemArenaVerify |
		// +==========================================+
		case MemArenaType_VirtualStack:
		{
			if (arena->size == 0)
			{
				AssertIfMsg(assertOnFailure, false, "arena size is 0");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->used > arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "arena used is greater than size");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->headerPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "headerPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->otherPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (arena->mainPntr == nullptr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr is nullptr in MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			if (stackHeader->maxNumMarks * sizeof(u64) >= arena->size)
			{
				AssertIfMsg(assertOnFailure, false, "stackHeader for MarkedStack has invalid value for maxNumMarks (based on size of arena)");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (stackHeader->numMarks > stackHeader->maxNumMarks)
			{
				AssertIfMsg(assertOnFailure, false, "numMarks is greater than maxNumMarks in MarkedStack header");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "highUsedMark is greater than arena size");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (arena->resettableHighUsedMark > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "resettableHighUsedMark is greater than arena size");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (arena->highUsedMark < arena->used)
				{
					AssertIfMsg(assertOnFailure, false, "used is greater than current highUsedMark");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (stackHeader->highMarkCount > stackHeader->maxNumMarks)
				{
					AssertIfMsg(assertOnFailure, false, "highMarkCount is greater than maxNumMarks in MarkedStack header");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (stackHeader->highMarkCount < stackHeader->numMarks)
				{
					AssertIfMsg(assertOnFailure, false, "highMarkCount is less than numMarks in MarkedStack header");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
			}
			u8* expectedOtherPntr = ((u8*)arena->headerPntr) + sizeof(MarkedStackArenaHeader_t);
			if (arena->otherPntr != expectedOtherPntr)
			{
				AssertIfMsg(assertOnFailure, false, "otherPntr is not where it's supposed to be compared to headerPntr");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			u8* expectedMainPntr = expectedOtherPntr + stackHeader->maxNumMarks * sizeof(u64);
			if (arena->mainPntr != expectedMainPntr)
			{
				AssertIfMsg(assertOnFailure, false, "mainPntr is not where it's supposed to be compared to headerPntr/otherPntr");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return false;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			for (u64 mIndex = 0; mIndex < stackHeader->numMarks; mIndex++)
			{
				if (marksPntr[mIndex] > arena->size)
				{
					AssertIfMsg(assertOnFailure, false, "One of the marks has and invalid value (too big, given the arena->size)");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
				if (marksPntr[mIndex] > arena->used)
				{
					AssertIfMsg(assertOnFailure, false, "One of the marks is above the current used amount!");
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return false;
				}
			}
		} break;
		
		// +====================================+
		// | MemArenaType_Buffer MemArenaVerify |
		// +====================================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +====================================+
		// | Unsupported or Corrupt Arena Type  |
		// +====================================+
		default:
		{
			AssertIfMsg(assertOnFailure, false, "Unsupported or corrupt arena type found in MemArenaVerify");
			if (didLock) { UnlockGyMutex(&arena->mutex); }
			return false;
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return true;
}

// +--------------------------------------------------------------+
// |                      Allocate Function                       |
// +--------------------------------------------------------------+
#if GYLIB_MEM_ARENA_DEBUG_ENABLED
void* AllocMem_(const char* filePath, u64 lineNumber, const char* funcName, MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride) //pre-declared at top of file
#else
void* AllocMem_(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride) //pre-declared at top of file
#endif
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to allocate from uninitialized arena");
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnAlloc) && (arena->debugBreakThreshold == 0 || numBytes >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	
	if (numBytes == 0)
	{
		if (didLock) { UnlockGyMutex(&arena->mutex); }
		return nullptr;
	}
	if (IsFlagSet(arena->flags, MemArenaFlag_SingleAlloc) && arena->numAllocations > 0)
	{
		GyLibPrintLine_W("Attempted second allocation of %llu out of single alloc arena (type: %s, size: %llu, used: %llu)", numBytes, GetMemArenaTypeStr(arena->type), arena->size, arena->used);
		if (didLock) { UnlockGyMutex(&arena->mutex); }
		return nullptr;
	}
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	
	u8* result = nullptr;
	switch (arena->type)
	{
		// +================================+
		// | MemArenaType_Redirect AllocMem |
		// +================================+
		case MemArenaType_Redirect:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in Redirect arena type");
			NotNull(arena->allocFunc);
			result = (u8*)arena->allocFunc(numBytes);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +==============================+
		// | MemArenaType_Alias AllocMem  |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			#if GYLIB_MEM_ARENA_DEBUG_ENABLED
			result = (u8*)AllocMem_(filePath, lineNumber, funcName, arena->sourceArena, numBytes, alignment);
			#else
			result = (u8*)AllocMem(arena->sourceArena, numBytes, alignment);
			#endif
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		#if !ORCA_COMPILATION
		// +===============================+
		// | MemArenaType_StdHeap AllocMem |
		// +===============================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in StdHeap arena type");
			result = (u8*)MyMalloc(numBytes);
			if (result == nullptr) { break; }
			IncrementU64(arena->numAllocations);
			arena->used += numBytes;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		#endif //!ORCA_COMPILATION
		
		// +==================================+
		// | MemArenaType_FixedHeap AllocMem  |
		// +==================================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			//TODO: Assert a maximum sized based off the fact that our top bit stores filled info
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				AssertMsg(allocOffset + allocSize <= arena->size, "Found an allocation header with invalid size. Would extend past the end of the arena!");
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				if (!isAllocFilled)
				{
					u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
					if (allocAfterPrefixSize >= alignOffset + numBytes)
					{
						result = allocAfterPrefixPntr + alignOffset;
						if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
						{
							//Split the section into 2 (one filled and one empty)
							allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
							HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
							newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
							arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
							Assert(arena->used <= arena->size);
						}
						else
						{
							//This entire section is getting used (or there's not enough extra room to make another empty section)
							allocPntr->size = PackAllocPrefixSize(true, allocSize);
							arena->used += allocSize - sizeof(HeapAllocPrefix_t);
							Assert(arena->used <= arena->size);
						}
						arena->numAllocations++;
						if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
						{
							if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
							if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
							if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
						}
						break;
					}
				}
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertIfMsg(result == nullptr, allocOffset == arena->size, "A Fixed Heap is corrupt. The last allocation size does not perfectly match the size of the arena");
			
			// MemArenaVerify(arena, true); //TODO: Remove me when not debugging
		} break;
		
		// +==================================+
		// | MemArenaType_PagedHeap AllocMem  |
		// +==================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				if (pageHeader->size - pageHeader->used < numBytes)
				{
					pageHeader = pageHeader->next;
					pageIndex++;
					continue;
				}
				
				u64 allocOffset = 0;
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (!isAllocFilled)
					{
						u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
						if (allocAfterPrefixSize >= alignOffset + numBytes)
						{
							result = allocAfterPrefixPntr + alignOffset;
							if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
							{
								//Split the section into 2 (one filled and one empty)
								allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
								HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
								newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
								pageHeader->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
								arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
								Assert(pageHeader->used <= pageHeader->size);
								Assert(arena->used <= arena->size);
							}
							else
							{
								//This entire section is getting used (or there's not enough extra room to make another empty section)
								allocPntr->size = PackAllocPrefixSize(true, allocSize);
								pageHeader->used += allocSize - sizeof(HeapAllocPrefix_t);
								arena->used += allocSize - sizeof(HeapAllocPrefix_t);
								Assert(pageHeader->used <= pageHeader->size);
								Assert(arena->used <= arena->size);
							}
							arena->numAllocations++;
							if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
							{
								if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
								if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
								if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
							}
							break;
						}
					}
					
					allocBytePntr += allocSize;
					allocOffset += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				if (result != nullptr) { break; }
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			
			// +==============================+
			// |      Allocate New Page       |
			// +==============================+
			if (result == nullptr && (arena->maxNumPages == 0 || arena->numPages < arena->maxNumPages))
			{
				u64 maxNeededSize = sizeof(HeapAllocPrefix_t) + numBytes + AllocAlignment_Max;
				u64 newPageSize = arena->pageSize;
				if (newPageSize < maxNeededSize)
				{
					newPageSize = maxNeededSize;
				}
				
				HeapPageHeader_t* newPageHeader = nullptr;
				if (arena->sourceArena != nullptr)
				{
					DebugAssert(arena->sourceArena != arena);
					//NOTE: Intentionally not putting the onus on the sourceArena to align the page. We will align allocations inside the page as requested
					newPageHeader = (HeapPageHeader_t*)AllocMem(arena->sourceArena, sizeof(HeapPageHeader_t) + newPageSize);
					// NotNullMsg(newPageHeader, "Failed to allocate new page from arena for paged heap");
					if (newPageHeader == nullptr)
					{
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return nullptr;
					}
				}
				else if (arena->allocFunc != nullptr)
				{
					newPageHeader = (HeapPageHeader_t*)arena->allocFunc(sizeof(HeapPageHeader_t) + newPageSize);
					// NotNullMsg(newPageHeader, "Failed to allocate new page for paged heap");
					if (newPageHeader == nullptr)
					{
						if (didLock) { UnlockGyMutex(&arena->mutex); }
						return nullptr;
					}
				}
				NotNullMsg(newPageHeader, "sourceArena and allocFunc are both not filled!");
				
				arena->size += newPageSize;
				arena->used += sizeof(HeapAllocPrefix_t);
				
				ClearPointer(newPageHeader);
				newPageHeader->next = nullptr;
				newPageHeader->size = newPageSize;
				newPageHeader->used = sizeof(HeapAllocPrefix_t);
				
				u8* pageBase = (u8*)(newPageHeader + 1);
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)pageBase;
				u8* allocAfterPrefixPntr = (pageBase + sizeof(HeapAllocPrefix_t));
				u64 allocAfterPrefixSize = newPageSize - sizeof(HeapAllocPrefix_t);
				u8 alignOffset = OffsetToAlign(allocAfterPrefixPntr, alignment);
				AssertMsg(allocAfterPrefixSize >= alignOffset + numBytes, "Paged heap has a bug where we didn't allocate enough space in the new page to fit the allocation");
				result = allocAfterPrefixPntr + alignOffset;
				if (allocAfterPrefixSize > alignOffset + numBytes + sizeof(HeapAllocPrefix_t))
				{
					//Split the section into 2 (one filled and one empty)
					allocPntr->size = PackAllocPrefixSize(true, sizeof(HeapAllocPrefix_t) + alignOffset + numBytes);
					HeapAllocPrefix_t* newSection = (HeapAllocPrefix_t*)(allocAfterPrefixPntr + alignOffset + numBytes);
					newSection->size = PackAllocPrefixSize(false, allocAfterPrefixSize - (alignOffset + numBytes));
					newPageHeader->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
					arena->used += alignOffset + numBytes + sizeof(HeapAllocPrefix_t);
					Assert(newPageHeader->used <= newPageHeader->size);
					Assert(arena->used <= arena->size);
				}
				else
				{
					//This entire section is getting used (or there's not enough extra room to make another empty section)
					allocPntr->size = PackAllocPrefixSize(true, newPageSize);
					newPageHeader->used += newPageSize - sizeof(HeapAllocPrefix_t);
					arena->used += newPageSize - sizeof(HeapAllocPrefix_t);
					Assert(newPageHeader->used <= newPageHeader->size);
					Assert(arena->used <= arena->size);
				}
				arena->numAllocations++;
				
				if (arena->numPages == 0)
				{
					Assert(arena->headerPntr == nullptr);
					arena->headerPntr = newPageHeader;
				}
				else
				{
					NotNull(arena->headerPntr);
					HeapPageHeader_t* walkPntr = (HeapPageHeader_t*)arena->headerPntr;
					for (u64 pIndex = 0; pIndex < arena->numPages-1; pIndex++)
					{
						walkPntr = walkPntr->next;
					}
					NotNull(walkPntr);
					Assert(walkPntr->next == nullptr);
					walkPntr->next = newPageHeader;
				}
				arena->numPages++;
				
				if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
				{
					if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
					if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
					if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
				}
				break;
			}
		} break;
		
		// +====================================+
		// | MemArenaType_MarkedStack AllocMem  |
		// +====================================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			u8 alignOffset = OffsetToAlign(((u8*)arena->mainPntr) + arena->used, alignment);
			if (arena->used + alignOffset + numBytes > arena->size)
			{
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return nullptr;
			}
			result = ((u8*)arena->mainPntr) + arena->used + alignOffset;
			arena->used += alignOffset + numBytes;
			arena->numAllocations++;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
			}
		} break;
		
		// +==================================+
		// | MemArenaType_PagedStack AllocMem |
		// +==================================+
		case MemArenaType_PagedStack:
		{
			NotNull(arena->headerPntr);
			
			MarkedStackArenaHeader_t* pageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			u64 byteIndex = 0;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				Assert(byteIndex <= arena->size);
				
				u64 pageSize = pageHeader->thisPageSize - sizeof(MarkedStackArenaHeader_t) - (pageHeader->maxNumMarks * sizeof(u64));
				// u64* pageMarks = (u64*)(((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t));
				u8* pageBase = ((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t) + (pageHeader->maxNumMarks * sizeof(u64));
				Assert(byteIndex + pageSize <= arena->size);
				
				if (arena->used - byteIndex < pageSize)
				{
					//We should only find space in the final page, walking the other pages is only done in order to calculate byteIndex properly
					Assert(pageIndex == arena->numPages-1);
					Assert(pageHeader->next == nullptr);
					
					u8 alignOffset = OffsetToAlign(&pageBase[arena->used], alignment);
					if (arena->used + alignOffset + numBytes > arena->size)
					{
						// Allocate a new page!
						
						u8* newPageBytes = nullptr;
						u64 newPageSize = MaxU64(arena->pageSize, sizeof(MarkedStackArenaHeader_t) + numBytes);
						if (arena->allocFunc != nullptr)
						{
							newPageBytes = (u8*)arena->allocFunc(newPageSize);
						}
						else
						{
							NotNull(arena->sourceArena);
							newPageBytes = (u8*)AllocMem(arena->sourceArena, newPageSize);
						}
						
						if (newPageBytes != nullptr)
						{
							MarkedStackArenaHeader_t* nextPageHeader = (MarkedStackArenaHeader_t*)newPageBytes;
							ClearPointer(nextPageHeader);
							nextPageHeader->next = nullptr;
							nextPageHeader->thisPageSize = newPageSize;
							nextPageHeader->maxNumMarks = 0; //secondary pages contain no mark space
							nextPageHeader->numMarks = 0;
							nextPageHeader->highMarkCount = 0;
							
							pageHeader->next = nextPageHeader;
							arena->size += newPageSize - sizeof(MarkedStackArenaHeader_t);
							arena->numPages++;
							
							//Mark this whole page as used, since we can't have an empty end of a page with another one in the list
							arena->used = byteIndex + pageSize;
							if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
							{
								if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
								if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
							}
						}
					}
					else
					{
						// Allocate from the existing page
						
						result = &pageBase[arena->used + alignOffset];
						arena->used += alignOffset + numBytes;
						arena->numAllocations++;
						if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
						{
							if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
							if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
						}
						
						break;
					}
				}
				
				byteIndex += pageSize;
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			
		} break;
		
		// +==============================+
		// | MemArenaType_Buffer AllocMem |
		// +==============================+
		case MemArenaType_Buffer:
		{
			u8* basePntr = (u8*)arena->mainPntr;
			u64 neededSize = numBytes;
			result = basePntr + arena->used;
			u8 alignOffset = OffsetToAlign(result, alignment);
			neededSize += alignOffset;
			if (arena->size - arena->used < neededSize) { result = nullptr; break; }
			result += alignOffset;
			IncrementU64(arena->numAllocations);
			arena->used += neededSize;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		// +====================================+
		// | MemArenaType_VirtualStack AllocMem |
		// +====================================+
		case MemArenaType_VirtualStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			u8 alignOffset = OffsetToAlign(((u8*)arena->mainPntr) + arena->used, alignment);
			if (arena->used + alignOffset + numBytes > arena->size)
			{
				if (arena->used + alignOffset + numBytes <= arena->maxSize)
				{
					u64 numNewBytesNeeded = (arena->used + alignOffset + numBytes) - arena->size;
					u64 newPagesNeededSize = RoundUpToU64(numNewBytesNeeded, arena->pageSize);
					OsCommitReservedMemory(((u8*)arena->mainPntr) + arena->size, newPagesNeededSize);
					arena->size += newPagesNeededSize;
					DebugAssert(arena->size <= arena->maxSize);
				}
				else
				{
					if (didLock) { UnlockGyMutex(&arena->mutex); }
					return nullptr;
				}
			}
			result = ((u8*)arena->mainPntr) + arena->used + alignOffset;
			arena->used += alignOffset + numBytes;
			arena->numAllocations++;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
			}
		} break;
		
		// +==================================+
		// | Unsupported Arena Type AllocMem  |
		// +==================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in AllocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in AllocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	if (result != nullptr && arena->debugArena != nullptr) { StoreAllocInfo(arena, arena->debugArena, result, numBytes, filePath, lineNumber, funcName); }
	#endif
	
	AssertMsg(IsAlignedTo(result, alignment), "An arena has a bug where it tried to return mis-aligned memory");
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return (void*)result;
}

MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None)
{
	MemArena_t result;
	void* allocatedMemory = AllocMem(sourceArena, numBytes, alignOverride);
	NotNull(allocatedMemory);
	InitMemArena_Buffer(&result, numBytes, allocatedMemory, true);
	return result;
}

char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm = true)
{
	NotNull(arena);
	Assert(dataForFill != nullptr || numChars == 0);
	if (numChars == 0 && !addNullTerm) { return nullptr; }
	char* result = (char*)AllocMem(arena, numChars + (addNullTerm ? 1 : 0));
	if (result == nullptr) { return nullptr; }
	if (numChars > 0)
	{
		MyMemCopy(result, dataForFill, numChars);
	}
	if (addNullTerm)
	{
		result[numChars] = '\0';
	}
	return result;
}
char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm = true)
{
	NotNull(arena);
	NotNull(nullTermStr);
	u64 numChars = MyStrLength64(nullTermStr);
	return AllocCharsAndFill(arena, numChars, nullTermStr, addNullTerm);
}

// +--------------------------------------------------------------+
// |                        Free Function                         |
// +--------------------------------------------------------------+
//NOTE: oldSizeOut may not be written. In some cases it is written to allocSize if provided and no sanity checks are done
bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize, bool ignoreNullptr, u64* oldSizeOut) //pre-declared at top of file
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to free from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	if (allocPntr == nullptr) { return false; }
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled) && arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnFree) && (arena->debugBreakThreshold == 0 || allocSize >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	
	bool result = false;
	switch (arena->type)
	{
		// +===============================+
		// | MemArenaType_Redirect FreeMem |
		// +===============================+
		case MemArenaType_Redirect:
		{
			NotNull(arena->freeFunc);
			arena->freeFunc(allocPntr);
			result = true;
			Decrement(arena->numAllocations);
			DecrementBy(arena->used, allocSize);
			if (oldSizeOut != nullptr && allocSize != 0) { *oldSizeOut = allocSize; }
		} break;
		
		// +==============================+
		// |  MemArenaType_Alias FreeMem  |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = FreeMem(arena->sourceArena, allocPntr, allocSize, ignoreNullptr, oldSizeOut);
			Decrement(arena->numAllocations);
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
		} break;
		
		#if !ORCA_COMPILATION
		// +==============================+
		// | MemArenaType_StdHeap FreeMem |
		// +==============================+
		case MemArenaType_StdHeap:
		{
			MyFree(allocPntr);
			Decrement(arena->numAllocations);
			DecrementBy(arena->used, allocSize);
			result = true;
		} break;
		#endif //!ORCA_COMPILATION
		
		// +================================+
		// | MemArenaType_FixedHeap FreeMem |
		// +================================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			HeapAllocPrefix_t* prevPrefixPntr = nullptr;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
				u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
				AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
				
				if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
				{
					AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to free a pointer that pointed into a Fixed Heap header. This is a corrupt pointer!");
					//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
					AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to free a pointer that pointed to the middle of a Fixed Heap section. This is a corrupt pointer!");
					AssertMsg(isSectionFilled, "Tried to double free section in Fixed Heap. This is a memory management bug");
					if (allocSize != 0)
					{
						//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
						//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
						//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
						u64 allowedSlop = OffsetToAlign(afterPrefixPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
						AssertMsg(AbsDiffU64(allocSize, afterPrefixSize) <= allowedSlop, "Given size did not match actual allocation size in Fixed Heap during FreeMem. This is a memory management bug");
					}
					
					result = true;
					prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
					AssertMsg(arena->used >= afterPrefixSize, "Fixed Heap used tracker was corrupted. Reached 0 too soon!");
					arena->used -= afterPrefixSize;
					AssertMsg(arena->numAllocations > 0, "Fixed Heap numAllocations was corrupted. Reached 0 too soon!");
					arena->numAllocations--;
					
					if (allocOffset + sectionSize < arena->size)
					{
						Assert(allocOffset + sectionSize + sizeof(HeapAllocPrefix_t) <= arena->size);
						HeapAllocPrefix_t* nextPrefixPntr = (HeapAllocPrefix_t*)(allocBytePntr + sectionSize);
						if (!IsAllocPrefixFilled(nextPrefixPntr->size))
						{
							// Merge the next section with this one by making this section bigger
							sectionSize += UnpackAllocPrefixSize(nextPrefixPntr->size);
							prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
							AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Fixed Heap used tracker was corrupted. Reached 0 too soon.");
							arena->used -= sizeof(HeapAllocPrefix_t);
						}
					}
					if (prevPrefixPntr != nullptr && !IsAllocPrefixFilled(prevPrefixPntr->size))
					{
						// Merge the previous section with this one by making it's sectionSize bigger
						prevPrefixPntr->size = PackAllocPrefixSize(false, UnpackAllocPrefixSize(prevPrefixPntr->size) + sectionSize);
						AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Fixed Heap used tracker was corrupted. Reached 0 too soon.");
						arena->used -= sizeof(HeapAllocPrefix_t);
					}
					
					break;
				}
				
				prevPrefixPntr = prefixPntr;
				allocOffset += sectionSize;
				allocBytePntr += sectionSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertMsg(result == true, "Tried to free an unknown pointer from Fixed Heap. The pointer must be corrupt or was freed from the wrong heap. This is a memory management bug");
			
			// MemArenaVerify(arena, true); //TODO: Remove me when not debugging
		} break;
		
		// +================================+
		// | MemArenaType_PagedHeap FreeMem |
		// +================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* prevPageHeader = nullptr;
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u8* pageBase = (u8*)(pageHeader + 1);
				if (allocPntr >= pageBase && allocPntr < pageBase + pageHeader->size)
				{
					bool foundAlloc = false;
					u64 allocOffset = 0;
					u8* allocBytePntr = pageBase;
					u64 sectionIndex = 0;
					HeapAllocPrefix_t* prevPrefixPntr = nullptr;
					while (allocOffset < pageHeader->size)
					{
						HeapAllocPrefix_t* prefixPntr = (HeapAllocPrefix_t*)allocBytePntr;
						u8* afterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
						bool isSectionFilled = IsAllocPrefixFilled(prefixPntr->size);
						u64 sectionSize = UnpackAllocPrefixSize(prefixPntr->size);
						AssertMsg(sectionSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Paged Heap");
						u64 afterPrefixSize = sectionSize - sizeof(HeapAllocPrefix_t);
						
						if ((u8*)allocPntr >= allocBytePntr && (u8*)allocPntr < allocBytePntr + sectionSize)
						{
							AssertMsg((u8*)allocPntr >= afterPrefixPntr, "Tried to free a pointer that pointed into a Paged Heap header. This is a corrupt pointer!");
							//TODO: Check if the allocation was actually aligned. Be more strict if it wasn't aligned
							AssertMsg((u8*)allocPntr <= afterPrefixPntr + OffsetToAlign(afterPrefixPntr, AllocAlignment_Max), "Tried to free a pointer that pointed to the middle of a Paged Heap section. This is a corrupt pointer!");
							AssertMsg(isSectionFilled, "Tried to double free section in Paged Heap. This is a memory management bug");
							if (allocSize != 0)
							{
								//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
								//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
								//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
								u64 allowedSlop = OffsetToAlign(afterPrefixPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
								AssertMsg(AbsDiffU64(allocSize, afterPrefixSize) <= allowedSlop, "Given size did not match actual allocation size in Paged Heap during FreeMem. This is a memory management bug");
							}
							
							result = true;
							foundAlloc = true;
							if (oldSizeOut != nullptr) { *oldSizeOut = afterPrefixSize; }
							
							// +==============================+
							// |   Free Paged Heap Section    |
							// +==============================+
							prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
							AssertMsg(pageHeader->used >= afterPrefixSize, "Paged Heap used tracker was corrupted. Reached 0 too soon!");
							AssertMsg(arena->used >= afterPrefixSize, "Paged Heap used tracker was corrupted. Reached 0 too soon!");
							pageHeader->used -= afterPrefixSize;
							arena->used -= afterPrefixSize;
							AssertMsg(arena->numAllocations > 0, "Paged Heap numAllocations was corrupted. Reached 0 too soon!");
							arena->numAllocations--;
							
							// +==============================+
							// | Merge Sections After Freeing |
							// +==============================+
							if (allocOffset + sectionSize < pageHeader->size)
							{
								Assert(allocOffset + sectionSize + sizeof(HeapAllocPrefix_t) <= pageHeader->size);
								HeapAllocPrefix_t* nextPrefixPntr = (HeapAllocPrefix_t*)(allocBytePntr + sectionSize);
								if (!IsAllocPrefixFilled(nextPrefixPntr->size))
								{
									// Merge the next section with this one by making this section bigger
									sectionSize += UnpackAllocPrefixSize(nextPrefixPntr->size);
									prefixPntr->size = PackAllocPrefixSize(false, sectionSize);
									AssertMsg(pageHeader->used >= sizeof(HeapAllocPrefix_t), "Paged Heap page->used tracker was corrupted. Reached 0 too soon.");
									AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Paged Heap used tracker was corrupted. Reached 0 too soon.");
									pageHeader->used -= sizeof(HeapAllocPrefix_t);
									arena->used -= sizeof(HeapAllocPrefix_t);
								}
							}
							if (prevPrefixPntr != nullptr && !IsAllocPrefixFilled(prevPrefixPntr->size))
							{
								// Merge the previous section with this one by making it's sectionSize bigger
								prevPrefixPntr->size = PackAllocPrefixSize(false, UnpackAllocPrefixSize(prevPrefixPntr->size) + sectionSize);
								AssertMsg(pageHeader->used >= sizeof(HeapAllocPrefix_t), "Paged Heap page->used tracker was corrupted. Reached 0 too soon.");
								AssertMsg(arena->used >= sizeof(HeapAllocPrefix_t), "Paged Heap used tracker was corrupted. Reached 0 too soon.");
								pageHeader->used -= sizeof(HeapAllocPrefix_t);
								arena->used -= sizeof(HeapAllocPrefix_t);
							}
							
							// +==============================+
							// |       Free Empty Page        |
							// +==============================+
							if (pageHeader->used <= sizeof(HeapAllocPrefix_t) && IsFlagSet(arena->flags, MemArenaFlag_AutoFreePages) && pageIndex > 0)
							{
								prevPageHeader->next = pageHeader->next;
								arena->size -= pageHeader->size;
								arena->used -= sizeof(HeapAllocPrefix_t);
								if (arena->freeFunc != nullptr)
								{
									arena->freeFunc(pageHeader);
								}
								else if (arena->sourceArena != nullptr)
								{
									FreeMem(arena->sourceArena, pageHeader, sizeof(HeapPageHeader_t) + pageHeader->size);
								}
								arena->numPages--;
							}
							
							break;
						}
						
						prevPrefixPntr = prefixPntr;
						allocOffset += sectionSize;
						allocBytePntr += sectionSize;
						sectionIndex++;
					}
					AssertMsg(foundAlloc, "We have a bug in our freeing walk. Couldn't find section that contained the pntr in this page!");
					break;
				}
				
				prevPageHeader = pageHeader;
				pageHeader = pageHeader->next;
				pageIndex++;
			}
			AssertMsg(result == true, "Tried to free pntr that isn't in any of the pages of this arena!");
		} break;
		
		// +==================================+
		// | MemArenaType_MarkedStack FreeMem |
		// +==================================+
		case MemArenaType_MarkedStack:
		{
			Assert(IsPntrInsideRange(allocPntr, arena->mainPntr, arena->size));
			Assert(IsPntrInsideRange(((u8*)allocPntr) + allocSize, arena->mainPntr, arena->size));
			if (allocSize > 0)
			{
				// If the allocation is the last one in the stack then we can actually free it by just moving our used amount down.
				// This is the only scenario where we support freeing
				u64 allocOffset = (u64)(((u8*)allocPntr) - ((u8*)arena->mainPntr));
				if (allocOffset + allocSize == arena->used)
				{
					arena->used -= allocSize;
				}
			}
			
			// NOTE: We used to assert here because we were worried we had memory bugs if someone was freeing something that didn't need freeing
			// Turns out, in almost all cases, we are just freeing to be compliant with any arena type that was passed to us and we had
			// to start checking if the arena that was passed was a MarkedStack just so we didn't hit this assert. This was not helpful.
			// AssertMsg(false, "FreeMem is not supported for a MarkedStack. Are you trying to free memory allocated on the TempArena");
		} break;
		
		// +==================================+
		// | MemArenaType_PagedStack FreeMem  |
		// +==================================+
		case MemArenaType_PagedStack:
		{
			if (allocSize > 0)
			{
				// If the allocation is the last one in the stack then we can actually free it by just moving our used amount down.
				// This is the only scenario where we support freeing
				
				MarkedStackArenaHeader_t* prevPageHeader = nullptr;
				MarkedStackArenaHeader_t* pageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
				u64 pageIndex = 0;
				u64 byteIndex = 0;
				while (pageHeader != nullptr)
				{
					Assert(byteIndex <= arena->size);
					
					u64 pageSize = pageHeader->thisPageSize - sizeof(MarkedStackArenaHeader_t) - (pageHeader->maxNumMarks * sizeof(u64));
					// u64* pageMarks = (u64*)(((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t));
					u8* pageBase = ((u8*)pageHeader) + sizeof(MarkedStackArenaHeader_t) + (pageHeader->maxNumMarks * sizeof(u64));
					Assert(byteIndex + pageSize <= arena->size);
					
					if (IsPntrInsideRange(allocPntr, pageBase, pageSize))
					{
						if (pageIndex < arena->numPages - 1) { break; } //if it's not in the last page, we can't free it
						
						u64 allocOffset = (u64)(((u8*)allocPntr) - pageBase);
						if (allocOffset + allocSize == arena->used - byteIndex)
						{
							arena->used -= allocSize;
							
							Assert(arena->used >= byteIndex);
							if (arena->used == byteIndex && prevPageHeader != nullptr)
							{
								// Deallocate the empty page
								if (arena->freeFunc != nullptr)
								{
									arena->freeFunc(pageHeader);
								}
								else
								{
									NotNull(arena->sourceArena);
									FreeMem(arena->sourceArena, pageHeader, pageHeader->thisPageSize);
								}
								prevPageHeader->next = nullptr;
								arena->size -= pageSize;
							}
							
							break;
						}
					}
					
					prevPageHeader = pageHeader;
					pageHeader = pageHeader->next;
					byteIndex += pageSize;
					pageIndex++;
				}
			}
		} break;
		
		// +====================================+
		// | MemArenaType_VirtualStack FreeMem  |
		// +====================================+
		case MemArenaType_VirtualStack:
		{
			Assert(IsPntrInsideRange(allocPntr, arena->mainPntr, arena->size));
			Assert(IsPntrInsideRange(((u8*)allocPntr) + allocSize, arena->mainPntr, arena->size));
			if (allocSize > 0)
			{
				// If the allocation is the last one in the stack then we can actually free it by just moving our used amount down.
				// This is the only scenario where we support freeing
				u64 allocOffset = (u64)(((u8*)allocPntr) - ((u8*)arena->mainPntr));
				if (allocOffset + allocSize == arena->used)
				{
					arena->used -= allocSize;
				}
			}
		} break;
		
		// +==============================+
		// | MemArenaType_Buffer FreeMem  |
		// +==============================+
		case MemArenaType_Buffer:
		{
			NotNull(arena->mainPntr);
			u8* basePntr = (u8*)arena->mainPntr;
			Assert(IsPntrInsideRange(allocPntr, arena->mainPntr, arena->size));
			if (allocSize == 0 && allocPntr == arena->mainPntr && arena->numAllocations == 1) //the one scenario where allocSize can be assumed
			{
				allocSize = arena->used;
			}
			AssertMsg(allocSize != 0, "Tried to deallocate from Buffer type arena without specifying allocSize and old size could not be assumed");
			AssertMsg(((u8*)allocPntr + allocSize) == (basePntr + arena->used), "Tried to deallocate out of order in Buffer type arena. Ordered frees only please!");
			arena->used -= allocSize;
			Decrement(arena->numAllocations);
			result = true;
		} break;
		
		// +================================+
		// | Unsupported Arena Type FreeMem |
		// +================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in FreeMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in FreeMem. Maybe the arena is corrupted?");
		} break;
	}
	
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	if (result && arena->debugArena != nullptr) { FreeAllocInfo(arena, arena->debugArena, allocPntr); }
	#endif
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return result;
}

// +--------------------------------------------------------------+
// |                     Reallocate Function                      |
// +--------------------------------------------------------------+
#if GYLIB_MEM_ARENA_DEBUG_ENABLED
void* ReallocMem_(const char* filePath, u64 lineNumber, const char* funcName, MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignOverride, bool ignoreNullptr, u64* oldSizeOut) //pre-declared at top of file
#else
void* ReallocMem_(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize, AllocAlignment_t alignOverride, bool ignoreNullptr, u64* oldSizeOut) //pre-declared at top of file
#endif
{
	NotNull(arena);
	AssertMsg(arena->type != MemArenaType_None, "Tried to realloc from uninitialized arena");
	Assert(ignoreNullptr || allocPntr != nullptr);
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	if (IsFlagSet(arena->flags, MemArenaFlag_BreakOnRealloc) && (arena->debugBreakThreshold == 0 || newSize >= arena->debugBreakThreshold || oldSize >= arena->debugBreakThreshold))
	{
		MyDebugBreak();
	}
	
	AllocAlignment_t alignment = (alignOverride != AllocAlignment_None) ? alignOverride : arena->alignment;
	if (newSize == oldSize && (allocPntr != nullptr || oldSize != 0) && IsAlignedTo(allocPntr, alignment)) //not resizing, just keep the memory where it's at
	{
		if (didLock) { UnlockGyMutex(&arena->mutex); }
		return allocPntr;
	}
	if (newSize == 0) //Resizing to 0 is basically freeing
	{
		bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, oldSizeOut);
		AssertMsg(freeSuccess, "Failed attempt to free memory in arena when Realloc'd to 0 bytes");
		if (didLock) { UnlockGyMutex(&arena->mutex); }
		return nullptr;
	}
	
	bool knownOldSize = (oldSize != 0 || allocPntr == nullptr);
	bool isRealigning = !IsAlignedTo(allocPntr, alignment);
	bool increasingSize = (knownOldSize && newSize > oldSize);
	bool decreasingSize = (knownOldSize && newSize < oldSize);
	u64 sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
	
	UNUSED(isRealigning); //TODO: Remove me!
	u8* result = nullptr;
	switch (arena->type)
	{
		// +======================================+
		// | Temporary AllocMem+FreeMem Solution  |
		// +======================================+
		case MemArenaType_Redirect:
		case MemArenaType_FixedHeap:
		case MemArenaType_PagedHeap:
		case MemArenaType_Buffer:
		{
			#if GYLIB_MEM_ARENA_DEBUG_ENABLED
			result = (u8*)AllocMem_(filePath, lineNumber, funcName, arena, newSize, alignOverride);
			#else
			result = (u8*)AllocMem(arena, newSize, alignOverride);
			#endif
			if (result == nullptr)
			{
				if (allocPntr != nullptr)
				{
					u64 reportedOldSize = oldSize;
					bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
					AssertMsg(freeSuccess, "Failed to FreeMem after a failed AllocMem in ReallocMem! Something is probably wrong with this arena");
					Assert(oldSize == 0 || oldSize == reportedOldSize);
					if (oldSize != 0)
					{
						//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
						//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
						//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
						u64 allowedSlop = OffsetToAlign(allocPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
						AssertMsg(AbsDiffU64(oldSize, reportedOldSize) <= allowedSlop, "Given size did not match actual allocation size in Fixed Heap during ReallocMem. This is a memory management bug");
					}
					oldSize = reportedOldSize;
					increasingSize = (newSize > oldSize);
					decreasingSize = (newSize < oldSize);
					sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				}
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
				break;
			}
			if (allocPntr != nullptr)
			{
				// if (oldSize == 0) { oldSize = GetAllocSize(arena, allocPntr); } //TODO: Uncomment me!
				Assert(oldSize != 0);
				MyMemCopy(result, allocPntr, MinU64(oldSize, newSize));
			}
			{
				u64 reportedOldSize = oldSize;
				bool freeSuccess = FreeMem(arena, allocPntr, oldSize, ignoreNullptr, &reportedOldSize);
				AssertMsg(freeSuccess, "Failed to FreeMem in ReallocMem! Does this arena type support freeing memory?");
				if (oldSize != 0)
				{
					//TODO: Handle scenarios where the alignment offset or bad-fit scenarios caused the section to be slightly larger than the requested allocation size
					//NOTE: Right now we allow for slop in both the alignment offset and scenarios where the section that was used was only slighly larger than needed
					//      and a second section couldn't be created because there wasn't even enough room for a HeapAllocPrefix_t
					u64 allowedSlop = OffsetToAlign(allocPntr, AllocAlignment_Max) + sizeof(HeapAllocPrefix_t);
					AssertMsg(AbsDiffU64(oldSize, reportedOldSize) <= allowedSlop, "Given size did not match actual allocation size in Fixed Heap during ReallocMem. This is a memory management bug");
				}
				oldSize = reportedOldSize;
				increasingSize = (newSize > oldSize);
				decreasingSize = (newSize < oldSize);
				sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
				if (oldSizeOut != nullptr) { *oldSizeOut = oldSize; }
			}
		} break;
		
		// +==================================+
		// | MemArenaType_Redirect ReallocMem |
		// +==================================+
		// case MemArenaType_Redirect:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +===============================+
		// | MemArenaType_Alias ReallocMem |
		// +===============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			u64 reportedOldSize = oldSize;
			result = (u8*)ReallocMem(arena->sourceArena, allocPntr, newSize, oldSize, alignment, ignoreNullptr, &reportedOldSize);
			Assert(oldSize == 0 || oldSize == reportedOldSize);
			oldSize = reportedOldSize;
			increasingSize = (newSize > oldSize);
			decreasingSize = (newSize < oldSize);
			sizeChangeAmount = (newSize >= oldSize) ? (newSize - oldSize) : (oldSize - newSize);
			if (oldSizeOut != nullptr) { *oldSizeOut = reportedOldSize; }
			if (result == nullptr)
			{
				Decrement(arena->numAllocations);
				arena->size = arena->sourceArena->size;
				arena->used = arena->sourceArena->used;
				break;
			}
			arena->size = arena->sourceArena->size;
			arena->used = arena->sourceArena->used;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
				if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
			}
		} break;
		
		#if !ORCA_COMPILATION
		// +==================================+
		// | MemArenaType_StdHeap ReallocMem  |
		// +==================================+
		case MemArenaType_StdHeap:
		{
			AssertMsg(alignment == AllocAlignment_None, "Tried to align memory in StdHeap type arena");
			result = (u8*)realloc(allocPntr, newSize);
			if (result == nullptr)
			{
				DecrementBy(arena->used, oldSize);
				Decrement(arena->numAllocations);
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return nullptr;
			}
			if (increasingSize) { arena->used += sizeChangeAmount; }
			else if (decreasingSize) { DecrementBy(arena->used, sizeChangeAmount); }
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (increasingSize && arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (increasingSize && arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
			}
		} break;
		#endif //!ORCA_COMPILATION
		
		// +====================================+
		// | MemArenaType_FixedHeap ReallocMem  |
		// +====================================+
		// case MemArenaType_FixedHeap:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +====================================+
		// | MemArenaType_PagedHeap ReallocMem  |
		// +====================================+
		// case MemArenaType_PagedHeap:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +======================================+
		// | MemArenaType_MarkedStack ReallocMem  |
		// +======================================+
		// case MemArenaType_MarkedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +====================================+
		// | MemArenaType_PagedStack ReallocMem |
		// +====================================+
		// case MemArenaType_PagedStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +======================================+
		// | MemArenaType_VirtualStack ReallocMem |
		// +======================================+
		// case MemArenaType_VirtualStack:
		// {
		// 	//TODO: Implement me!
		// } break;
		
		// +================================+
		// | MemArenaType_Buffer ReallocMem |
		// +================================+
		// case MemArenaType_Buffer:
		// {
		// 	//TODO: Implement me, and remove from above!
		// } break;
		
		// +====================================+
		// | Unsupported Arena Type ReallocMem  |
		// +====================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in ReallocMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in ReallocMem. Maybe the arena is corrupted?");
		} break;
	}
	
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	if (result != allocPntr && arena->debugArena != nullptr)
	{
		FreeAllocInfo(arena, arena->debugArena, allocPntr);
		if (result != nullptr) { StoreAllocInfo(arena, arena->debugArena, result, newSize, filePath, lineNumber, funcName); }
	}
	#endif
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return result;
}

// +--------------------------------------------------------------+
// |                        Grow Function                         |
// +--------------------------------------------------------------+
//NOTE: We need to store some info somewhere in order for some arenas
// (like FixedHeap and PagedHeap) to work properly when the calling code starts
// writing into the space immediately following an allocation. So if you plan to
// start using this space and then telling the arena about the usage AFTER you've
// changed the bytes, please take the tokenOut NOW so you can feed it to
// GrowMem later.
//TODO: Enforce that allocLength matches the original allocation if it's not 0
u64 GrowMemQuery(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, GrowMemToken_t* tokenOut = nullptr)
{
	NotNull(arena);
	NotNull(prevAllocPntr);
	Assert(prevAllocSize > 0);
	u64 result = 0;
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		case MemArenaType_StdHeap: break; //no support
		case MemArenaType_Redirect: break; //no support
		
		// +==================================+
		// | MemArenaType_Alias GrowMemQuery  |
		// +==================================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			result = GrowMemQuery(arena->sourceArena, prevAllocPntr, prevAllocSize, tokenOut);
		} break;
		
		// +======================================+
		// | MemArenaType_FixedHeap GrowMemQuery  |
		// +======================================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				AssertMsg(allocOffset + allocSize <= arena->size, "Found an allocation header with invalid size. Would extend past the end of the arena!");
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				if (allocAfterPrefixPntr == prevAllocPntr)
				{
					Assert(isAllocFilled);
					AssertMsg(allocAfterPrefixSize >= prevAllocSize, "prevAllocSize passed to GrowMemQuery was too large");
					AssertMsg(allocAfterPrefixSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to GrowMemQuery was too small (even given some slop for alignment)");
					u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
					result += extraBytesInThisAlloc;
					if (allocOffset + allocSize < arena->size)
					{
						HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
						u64 nextAllocSize = UnpackAllocPrefixSize(nextAllocPntr->size);
						if (!IsAllocPrefixFilled(nextAllocPntr->size))
						{
							if (tokenOut != nullptr)
							{
								tokenOut->memArena = arena;
								tokenOut->nextSectionPntr = nextAllocPntr;
								tokenOut->nextSectionSize = nextAllocSize;
							}
							result += nextAllocSize;
						}
					}
					break;
				}
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
		} break;
		
		// +======================================+
		// | MemArenaType_PagedHeap GrowMemQuery  |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				if (!IsPntrInsideRange(prevAllocPntr, allocBytePntr, pageHeader->size))
				{
					pageHeader = pageHeader->next;
					pageIndex++;
					continue;
				}
				
				u64 allocOffset = 0;
				u64 sectionIndex = 0;
				bool foundPrevAlloc = false;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (allocAfterPrefixPntr == prevAllocPntr)
					{
						Assert(isAllocFilled);
						AssertMsg(allocAfterPrefixSize >= prevAllocSize, "prevAllocSize passed to GrowMemQuery was too large");
						AssertMsg(allocAfterPrefixSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to GrowMemQuery was too small (even given some slop for alignment)");
						u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
						result += extraBytesInThisAlloc;
						if (allocOffset + allocSize < pageHeader->size)
						{
							HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
							u64 nextAllocSize = UnpackAllocPrefixSize(nextAllocPntr->size);
							if (!IsAllocPrefixFilled(nextAllocPntr->size))
							{
								if (tokenOut != nullptr)
								{
									tokenOut->memArena = arena;
									tokenOut->nextSectionPntr = nextAllocPntr;
									tokenOut->nextSectionSize = nextAllocSize;
								}
								result += nextAllocSize;
							}
						}
						foundPrevAlloc = true;
						break;
					}
					allocOffset += allocSize;
					allocBytePntr += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				if (foundPrevAlloc) { break; }
			}
			UNUSED(pageIndex); //used for debug purposes
		} break;
		
		// +========================================+
		// | MemArenaType_MarkedStack GrowMemQuery  |
		// +========================================+
		case MemArenaType_MarkedStack:
		{
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size), "prevAllocPntr passed to GrowMemQuery is not in this MarkedStack arena!");
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->used), "prevAllocPntr passed to GrowMemQuery is not in this MarkedStack arena's used space");
			AssertMsg(IsPntrInsideRange(((u8*)prevAllocPntr) + prevAllocSize, arena->mainPntr, arena->size, true), "prevAllocPntr+prevAllocSize passed to GrowMemQuery is not in this MarkedStack arena!");
			AssertMsg(IsPntrInsideRange(((u8*)prevAllocPntr) + prevAllocSize, arena->mainPntr, arena->used, true), "prevAllocPntr+prevAllocSize passed to GrowMemQuery is not in this MarkedStack arena's used space");
			u8* usedEndPntr = ((u8*)arena->mainPntr) + arena->used;
			u8* prevAllocEndPntr = ((u8*)prevAllocPntr) + prevAllocSize;
			if (prevAllocEndPntr == usedEndPntr)
			{
				Assert(arena->size >= arena->used);
				if (tokenOut != nullptr)
				{
					tokenOut->memArena = arena;
					tokenOut->nextSectionPntr = usedEndPntr;
					tokenOut->nextSectionSize = arena->size - arena->used;
				}
				result = arena->size - arena->used;
			}
		} break;
		
		// +======================================+
		// | MemArenaType_PagedStack GrowMemQuery |
		// +======================================+
		case MemArenaType_PagedStack:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +========================================+
		// | MemArenaType_VirtualStack GrowMemQuery |
		// +========================================+
		case MemArenaType_VirtualStack:
		{
			//TODO: We could allow for growing memory into the next page maybe? Maybe we just go ahead and allocate a new page if we are near the current committed size?
			
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size), "prevAllocPntr passed to GrowMemQuery is not in this VirtualStack arena!");
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->used), "prevAllocPntr passed to GrowMemQuery is not in this VirtualStack arena's used space");
			AssertMsg(IsPntrInsideRange(((u8*)prevAllocPntr) + prevAllocSize, arena->mainPntr, arena->size, true), "prevAllocPntr+prevAllocSize passed to GrowMemQuery is not in this VirtualStack arena!");
			AssertMsg(IsPntrInsideRange(((u8*)prevAllocPntr) + prevAllocSize, arena->mainPntr, arena->used, true), "prevAllocPntr+prevAllocSize passed to GrowMemQuery is not in this VirtualStack arena's used space");
			u8* usedEndPntr = ((u8*)arena->mainPntr) + arena->used;
			u8* prevAllocEndPntr = ((u8*)prevAllocPntr) + prevAllocSize;
			if (prevAllocEndPntr == usedEndPntr)
			{
				Assert(arena->size >= arena->used);
				if (tokenOut != nullptr)
				{
					tokenOut->memArena = arena;
					tokenOut->nextSectionPntr = usedEndPntr;
					tokenOut->nextSectionSize = arena->size - arena->used;
				}
				result = arena->size - arena->used;
			}
		} break;
		
		// +==================================+
		// | MemArenaType_Buffer GrowMemQuery |
		// +==================================+
		case MemArenaType_Buffer:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +======================================+
		// | Unsupported Arena Type GrowMemQuery  |
		// +======================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in GrowMemQuery: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in GrowMemQuery. Maybe the arena is corrupted?");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return result;
}

void GrowMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize, const GrowMemToken_t* token)
{
	NotNull(arena);
	NotNull(token);
	NotNull(prevAllocPntr);
	Assert(prevAllocSize > 0);
	Assert(newAllocSize >= prevAllocSize);
	if (newAllocSize == prevAllocSize) { return; } //no work to do
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		case MemArenaType_StdHeap: Unimplemented(); break; //no support
		case MemArenaType_Redirect: Unimplemented(); break; //no support
		
		// +==============================+
		// |  MemArenaType_Alias GrowMem  |
		// +==============================+
		case MemArenaType_Alias:
		{
			NotNull(arena->sourceArena);
			GrowMem(arena->sourceArena, prevAllocPntr, prevAllocSize, newAllocSize, token);
		} break;
		
		// +================================+
		// | MemArenaType_FixedHeap GrowMem |
		// +================================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			Assert(token->memArena == arena);
			NotNull(token->nextSectionPntr);
			Assert(token->nextSectionSize > 0);
			
			bool foundAllocation = false;
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				AssertMsg(allocOffset + allocSize <= arena->size, "Found an allocation header with invalid size. Would extend past the end of the arena!");
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				if (allocAfterPrefixPntr == prevAllocPntr)
				{
					Assert(isAllocFilled);
					AssertMsg(allocAfterPrefixSize >= prevAllocSize, "prevAllocSize passed to GrowMemQuery was too large");
					AssertMsg(allocAfterPrefixSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to GrowMemQuery was too small (even given some slop for alignment)");
					foundAllocation = true;
					
					if (allocAfterPrefixSize >= newAllocSize)
					{
						//the calling code grew into it's already available extra space. No fixup work is needed
						break;
					}
					u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
					u64 numNewBytesUsed = (newAllocSize - prevAllocSize) - extraBytesInThisAlloc;
					Assert(numNewBytesUsed <= token->nextSectionSize);
					AssertMsg(allocBytePntr + prevAllocSize >= ((u8*)token->nextSectionPntr) - AllocAlignment_Max, "GrowMemQuery token had invalid nextSectionPntr based on info passed to GrowMem. Are you re-using a token to grow? Or is the token corrupt?");
					AssertMsg(allocBytePntr + prevAllocSize <= ((u8*)token->nextSectionPntr) + AllocAlignment_Max, "GrowMemQuery token had invalid nextSectionPntr based on info passed to GrowMem. Are you re-using a token to grow? Or is the token corrupt?");
					
					allocSize = sizeof(HeapAllocPrefix_t) + newAllocSize;
					allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
					arena->used += numNewBytesUsed;
					
					if (token->nextSectionSize - numNewBytesUsed > sizeof(HeapAllocPrefix_t))
					{
						//shrink the next section
						HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
						u64 nextAllocSize = token->nextSectionSize - numNewBytesUsed;
						nextAllocPntr->size = PackAllocPrefixSize(false, nextAllocSize);
						#if ASSERTIONS_ENABLED
						//Let's try and check the integrity of our linked list of sections to make sure we didn't make any mistakes (and make sure the calling code didn't write out of bounds)
						if (allocOffset + allocSize + nextAllocSize < arena->size)
						{
							HeapAllocPrefix_t* nextNextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize + nextAllocSize);
							Assert(IsAllocPrefixFilled(nextNextAllocPntr->size)); //we shouldn't have any consecutive unfilled sections
							Assert(allocOffset + allocSize + nextAllocSize + UnpackAllocPrefixSize(nextNextAllocPntr->size) <= arena->size);
						}
						#endif
					}
					else
					{
						//next section got too small or was entirely used up
						u64 numBytesLeftover = token->nextSectionSize - numNewBytesUsed;
						if (numBytesLeftover > 0)
						{
							allocSize += numBytesLeftover;
							allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
						}
						
						arena->used += numBytesLeftover;
						arena->used -= sizeof(HeapAllocPrefix_t); //a section went away, so it's usage needs to be subtracted
						
						#if ASSERTIONS_ENABLED
						//Let's try and check the integrity of our linked list of sections to make sure we didn't make any mistakes (and make sure the calling code didn't write out of bounds)
						if (allocOffset + allocSize < arena->size)
						{
							HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
							Assert(IsAllocPrefixFilled(nextAllocPntr->size)); //we shouldn't have any consecutive unfilled sections
							Assert(allocOffset + allocSize + UnpackAllocPrefixSize(nextAllocPntr->size) <= arena->size);
						}
						#endif
					}
					
					if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
					{
						if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
						if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
						if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
					}
					
					break;
				}
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertMsg(foundAllocation, "Tried to grow an allocation from the incorrect arena. Or the prevAllocPntr was misaligned. Maybe the arena is corrupt or the memory pntr was mishandled?");
		} break;
		
		// +================================+
		// | MemArenaType_PagedHeap GrowMem |
		// +================================+
		case MemArenaType_PagedHeap:
		{
			bool foundAllocation = false;
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				if (!IsPntrInsideRange(prevAllocPntr, allocBytePntr, pageHeader->size))
				{
					pageHeader = pageHeader->next;
					pageIndex++;
					continue;
				}
				
				u64 allocOffset = 0;
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (allocAfterPrefixPntr == prevAllocPntr)
					{
						Assert(isAllocFilled);
						AssertMsg(allocSize >= prevAllocSize, "prevAllocSize passed to GrowMemQuery was too large");
						AssertMsg(allocSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to GrowMemQuery was too small (even given some slop for alignment)");
						foundAllocation = true;
						
						if (allocAfterPrefixSize >= newAllocSize)
						{
							//the calling code grew into it's already available extra space. No fixup work is needed
							break;
						}
						u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
						u64 numNewBytesUsed = (newAllocSize - prevAllocSize) - extraBytesInThisAlloc;
						Assert(numNewBytesUsed <= token->nextSectionSize);
						AssertMsg(allocBytePntr + prevAllocSize >= ((u8*)token->nextSectionPntr) - AllocAlignment_Max, "GrowMemQuery token had invalid nextSectionPntr based on info passed to GrowMem. Are you re-using a token to grow? Or is the token corrupt?");
						AssertMsg(allocBytePntr + prevAllocSize <= ((u8*)token->nextSectionPntr) + AllocAlignment_Max, "GrowMemQuery token had invalid nextSectionPntr based on info passed to GrowMem. Are you re-using a token to grow? Or is the token corrupt?");
						
						allocSize = newAllocSize + sizeof(HeapAllocPrefix_t);
						allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
						arena->used += numNewBytesUsed;
						
						if (token->nextSectionSize - numNewBytesUsed > sizeof(HeapAllocPrefix_t))
						{
							//shrink the next section
							HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
							u64 nextAllocSize = token->nextSectionSize - numNewBytesUsed;
							nextAllocPntr->size = PackAllocPrefixSize(false, nextAllocSize);
							#if ASSERTIONS_ENABLED
							//Let's try and check the integrity of our linked list of sections to make sure we didn't make any mistakes (and make sure the calling code didn't write out of bounds)
							if (allocOffset + allocSize + nextAllocSize < pageHeader->size)
							{
								HeapAllocPrefix_t* nextNextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize + nextAllocSize);
								Assert(IsAllocPrefixFilled(nextNextAllocPntr->size)); //we shouldn't have any consecutive unfilled sections
								Assert(allocOffset + allocSize + nextAllocSize + UnpackAllocPrefixSize(nextNextAllocPntr->size) <= pageHeader->size);
							}
							#endif
						}
						else
						{
							//next section got too small or was entirely used up
							u64 numBytesLeftover = token->nextSectionSize - numNewBytesUsed;
							if (numBytesLeftover > 0)
							{
								allocSize += numBytesLeftover;
								allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
								arena->used += numBytesLeftover;
								arena->used -= sizeof(HeapAllocPrefix_t); //a section went away, so it's usage needs to be subtracted
							}
							#if ASSERTIONS_ENABLED
							//Let's try and check the integrity of our linked list of sections to make sure we didn't make any mistakes (and make sure the calling code didn't write out of bounds)
							if (allocOffset + allocSize < pageHeader->size)
							{
								HeapAllocPrefix_t* nextAllocPntr = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
								Assert(IsAllocPrefixFilled(nextAllocPntr->size)); //we shouldn't have any consecutive unfilled sections
								Assert(allocOffset + allocSize + UnpackAllocPrefixSize(nextAllocPntr->size) <= pageHeader->size);
							}
							#endif
						}
						
						if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
						{
							if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
							if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
							if (arena->highAllocMark < arena->numAllocations) { arena->highAllocMark = arena->numAllocations; }
						}
						
						break;
					}
					allocOffset += allocSize;
					allocBytePntr += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				if (foundAllocation) { break; }
			}
			UNUSED(pageIndex); //used for debug purposes
			AssertMsg(foundAllocation, "Tried to grow an allocation from the incorrect arena. Or the prevAllocPntr was misaligned. Maybe the arena is corrupt or the memory pntr was mishandled?");
		} break;
		
		// +==================================+
		// | MemArenaType_MarkedStack GrowMem |
		// +==================================+
		case MemArenaType_MarkedStack:
		{
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size), "prevAllocPntr passed to GrowMemQuery is not in this MarkedStack arena!");
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->used), "prevAllocPntr passed to GrowMemQuery is not in this MarkedStack arena's used space");
			AssertMsg((((u8*)prevAllocPntr) + prevAllocSize) == (((u8*)arena->mainPntr) + arena->used), "Something went wrong between GrowMemQuery and GrowMem in MarkedStack. The grown section isn't at the end of the stack!");
			arena->used += newAllocSize - prevAllocSize;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
			}
			Assert(arena->used <= arena->size);
		} break;
		
		// +==================================+
		// | MemArenaType_PagedStack GrowMem  |
		// +==================================+
		case MemArenaType_PagedStack:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +====================================+
		// | MemArenaType_VirtualStack GrowMem  |
		// +====================================+
		case MemArenaType_VirtualStack:
		{
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size), "prevAllocPntr passed to GrowMemQuery is not in this VirtualStack arena!");
			AssertMsg(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->used), "prevAllocPntr passed to GrowMemQuery is not in this VirtualStack arena's used space");
			AssertMsg((((u8*)prevAllocPntr) + prevAllocSize) == (((u8*)arena->mainPntr) + arena->used), "Something went wrong between GrowMemQuery and GrowMem in MarkedStack. The grown section isn't at the end of the stack!");
			arena->used += newAllocSize - prevAllocSize;
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (arena->highUsedMark < arena->used) { arena->highUsedMark = arena->used; }
				if (arena->resettableHighUsedMark < arena->used) { arena->resettableHighUsedMark = arena->used; }
			}
			Assert(arena->used <= arena->size);
		} break;
		
		// +==============================+
		// | MemArenaType_Buffer GrowMem  |
		// +==============================+
		case MemArenaType_Buffer:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +======================================+
		// | Unsupported Arena Type GrowMemQuery  |
		// +======================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in GrowMemQuery: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in GrowMemQuery. Maybe the arena is corrupted?");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
}

// +--------------------------------------------------------------+
// |                       Shrink Function                        |
// +--------------------------------------------------------------+
void ShrinkMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize)
{
	NotNull(arena);
	NotNull(prevAllocPntr);
	Assert(newAllocSize <= prevAllocSize);
	Assert(newAllocSize > 0);
	if (prevAllocSize == newAllocSize) { return; }
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		case MemArenaType_Redirect: Unimplemented(); break; //No support
		
		// +================================+
		// | MemArenaType_StdHeap ShrinkMem |
		// +================================+
		case MemArenaType_StdHeap:
		{
			//We don't really need to do anything for these arenas because they are fine if you pass the incorrect size to FreeMem
		} break;
		
		// +====================================+
		// | MemArenaType_MarkedStack ShrinkMem |
		// +====================================+
		case MemArenaType_MarkedStack:
		{
			Assert(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size));
			u64 prevAllocOffset = (u64)(((u8*)prevAllocPntr) - ((u8*)arena->mainPntr));
			Assert(prevAllocOffset + prevAllocSize <= arena->used);
			// If the allocation is the last one in the stack then we can actually shrink it by just moving our used amount down.
			// This is the only scenario where we support shrinking
			if (prevAllocOffset + prevAllocSize == arena->used)
			{
				arena->used -= (prevAllocSize - newAllocSize);
			}
		} break;
		
		// +====================================+
		// | MemArenaType_PagedStack ShrinkMem  |
		// +====================================+
		case MemArenaType_PagedStack:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +======================================+
		// | MemArenaType_VirtualStack ShrinkMem  |
		// +======================================+
		case MemArenaType_VirtualStack:
		{
			Assert(IsPntrInsideRange(prevAllocPntr, arena->mainPntr, arena->size));
			u64 prevAllocOffset = (u64)(((u8*)prevAllocPntr) - ((u8*)arena->mainPntr));
			Assert(prevAllocOffset + prevAllocSize <= arena->used);
			// If the allocation is the last one in the stack then we can actually shrink it by just moving our used amount down.
			// This is the only scenario where we support shrinking
			if (prevAllocOffset + prevAllocSize == arena->used)
			{
				arena->used -= (prevAllocSize - newAllocSize);
			}
		} break;
		
		// +==============================+
		// | MemArenaType_Alias ShrinkMem |
		// +==============================+
		case MemArenaType_Alias:
		{
			ShrinkMem(arena->sourceArena, prevAllocPntr, prevAllocSize, newAllocSize);
		} break;
		
		// +==================================+
		// | MemArenaType_FixedHeap ShrinkMem |
		// +==================================+
		case MemArenaType_FixedHeap:
		{
			NotNull(arena->mainPntr);
			
			bool foundAllocation = false;
			u64 allocOffset = 0;
			u8* allocBytePntr = (u8*)arena->mainPntr;
			u64 sectionIndex = 0;
			while (allocOffset < arena->size)
			{
				HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
				u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
				bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
				u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
				AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
				AssertMsg(allocOffset + allocSize <= arena->size, "Found an allocation header with invalid size. Would extend past the end of the arena!");
				u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
				if (allocAfterPrefixPntr == prevAllocPntr)
				{
					Assert(isAllocFilled);
					AssertMsg(allocAfterPrefixSize >= prevAllocSize, "prevAllocSize passed to ShrinkMem was too large");
					AssertMsg(allocAfterPrefixSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to ShrinkMem was too small (even given some slop for alignment)");
					foundAllocation = true;
					
					u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
					u64 shrinkAmount = (prevAllocSize - newAllocSize) + extraBytesInThisAlloc;
					bool nextAllocExists = (allocOffset + allocSize < arena->size);
					HeapAllocPrefix_t* nextAllocPntr = (nextAllocExists ? (HeapAllocPrefix_t*)(allocBytePntr + allocSize) : nullptr);
					bool isNextAllocFilled = (nextAllocExists ? IsAllocPrefixFilled(nextAllocPntr->size) : true);
					u64 nextAllocSize = (nextAllocExists ? UnpackAllocPrefixSize(nextAllocPntr->size) : 0);
					if (!isNextAllocFilled || shrinkAmount > sizeof(HeapAllocPrefix_t))
					{
						allocSize -= shrinkAmount;
						allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
						HeapAllocPrefix_t* newEmptyAlloc = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
						newEmptyAlloc->size = PackAllocPrefixSize(isNextAllocFilled, (isNextAllocFilled ? 0 : nextAllocSize) + shrinkAmount);
						arena->used -= shrinkAmount - (isNextAllocFilled ? 0 : sizeof(HeapAllocPrefix_t));
					}
					else
					{
						//If we aren't shrinking enough to make a new section and the next section is filled, we should be safe to silently keep the allocation the same size
					}
					
					break;
				}
				allocOffset += allocSize;
				allocBytePntr += allocSize;
				sectionIndex++;
			}
			UNUSED(sectionIndex); //used for debug purposes
			AssertMsg(foundAllocation, "Tried to shrink an allocation from the incorrect arena. Or the prevAllocPntr was misaligned. Maybe the arena is corrupt or the memory pntr was mishandled?");
		} break;
		
		// +==================================+
		// | MemArenaType_PagedHeap ShrinkMem |
		// +==================================+
		case MemArenaType_PagedHeap:
		{
			bool foundAllocation = false;
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				u8* allocBytePntr = (u8*)(pageHeader + 1);
				if (!IsPntrInsideRange(prevAllocPntr, allocBytePntr, pageHeader->size))
				{
					pageHeader = pageHeader->next;
					pageIndex++;
					continue;
				}
				
				u64 allocOffset = 0;
				u64 sectionIndex = 0;
				while (allocOffset < pageHeader->size)
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)allocBytePntr;
					u8* allocAfterPrefixPntr = (allocBytePntr + sizeof(HeapAllocPrefix_t));
					bool isAllocFilled = IsAllocPrefixFilled(allocPntr->size);
					u64 allocSize = UnpackAllocPrefixSize(allocPntr->size);
					AssertMsg(allocSize >= sizeof(HeapAllocPrefix_t), "Found an allocation header that claimed to be smaller than the header itself in Fixed Heap");
					AssertMsg(allocOffset + allocSize <= pageHeader->size, "Found an allocation header with invalid size. Would extend past the end of a page!");
					u64 allocAfterPrefixSize = allocSize - sizeof(HeapAllocPrefix_t);
					if (allocAfterPrefixPntr == prevAllocPntr)
					{
						Assert(isAllocFilled);
						AssertMsg(allocSize >= prevAllocSize, "prevAllocSize passed to ShrinkMem was too large");
						AssertMsg(allocSize <= prevAllocSize + AllocAlignment_Max, "prevAllocSize passed to ShrinkMem was too small (even given some slop for alignment)");
						foundAllocation = true;
						
						u64 extraBytesInThisAlloc = allocAfterPrefixSize - prevAllocSize;
						u64 shrinkAmount = (prevAllocSize - newAllocSize) + extraBytesInThisAlloc;
						bool nextAllocExists = (allocOffset + allocSize < pageHeader->size);
						HeapAllocPrefix_t* nextAllocPntr = (nextAllocExists ? (HeapAllocPrefix_t*)(allocBytePntr + allocSize) : nullptr);
						bool isNextAllocFilled = (nextAllocExists ? IsAllocPrefixFilled(nextAllocPntr->size) : true);
						u64 nextAllocSize = (nextAllocExists ? UnpackAllocPrefixSize(nextAllocPntr->size) : 0);
						if (!isNextAllocFilled || shrinkAmount > sizeof(HeapAllocPrefix_t))
						{
							allocSize -= shrinkAmount;
							allocPntr->size = PackAllocPrefixSize(isAllocFilled, allocSize);
							HeapAllocPrefix_t* newEmptyAlloc = (HeapAllocPrefix_t*)(allocBytePntr + allocSize);
							newEmptyAlloc->size = PackAllocPrefixSize(isNextAllocFilled, (isNextAllocFilled ? 0 : nextAllocSize) + shrinkAmount);
							arena->used -= shrinkAmount - (isNextAllocFilled ? 0 : sizeof(HeapAllocPrefix_t));
						}
						else
						{
							//If we aren't shrinking enough to make a new section and the next section is filled, we should be safe to silently keep the allocation the same size
						}
						
						break;
					}
					allocOffset += allocSize;
					allocBytePntr += allocSize;
					sectionIndex++;
				}
				UNUSED(sectionIndex); //used for debug purposes
				
				if (foundAllocation) { break; }
			}
			UNUSED(pageIndex); //used for debug purposes
			AssertMsg(foundAllocation, "Tried to shrink an allocation from the incorrect arena. Or the prevAllocPntr was misaligned. Maybe the arena is corrupt or the memory pntr was mishandled?");
		} break;
		
		// +===============================+
		// | MemArenaType_Buffer ShrinkMem |
		// +===============================+
		case MemArenaType_Buffer:
		{
			Unimplemented(); //TODO: Implement me!
		} break;
		
		// +==================================+
		// | Unsupported Arena Type ShrinkMem |
		// +==================================+
		default:
		{
			GyLibPrintLine_E("Unsupported arena type in ShrinkMem: %u (size: %llu, used: %llu)", arena->type, arena->size, arena->used);
			AssertMsg(false, "Unsupported arena type in ShrinkMem. Maybe the arena is corrupted?");
		} break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
}

// +--------------------------------------------------------------+
// |                     Free Arena Functions                     |
// +--------------------------------------------------------------+
void FreeMemArena(MemArena_t* arena)
{
	NotNull(arena);
	
	if (IsValidGyMutex(&arena->mutex)) { FreeGyMutex(&arena->mutex); }
	
	switch (arena->type)
	{
		// +==================================+
		// | MemArenaType_Alias FreeMemArena  |
		// +==================================+
		case MemArenaType_Alias:
		{
			FreeMemArena(arena->sourceArena);
		} break;
		
		// +======================================+
		// | MemArenaType_PagedHeap FreeMemArena  |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				HeapPageHeader_t* nextPageHeader = pageHeader->next;
				if (arena->sourceArena != nullptr)
				{
					FreeMem(arena->sourceArena, pageHeader, sizeof(HeapPageHeader_t) + pageHeader->size);
				}
				else if (arena->freeFunc != nullptr)
				{
					arena->freeFunc(pageHeader);
				}
				else
				{
					AssertMsg(false, "This PageHeap cannot be freed because it doesn't have a sourceArena or freeFunc pointer!");
				}
				pageHeader = nextPageHeader;
				pageIndex++;
			}
		} break;
		
		// +======================================+
		// | MemArenaType_PagedStack FreeMemArena |
		// +======================================+
		case MemArenaType_PagedStack:
		{
			MarkedStackArenaHeader_t* pageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				MarkedStackArenaHeader_t* nextPageHeader = pageHeader->next;
				if (arena->freeFunc != nullptr)
				{
					arena->freeFunc(pageHeader);
				}
				else if (arena->sourceArena != nullptr)
				{
					FreeMem(arena->sourceArena, pageHeader, pageHeader->thisPageSize);
				}
				else
				{
					AssertMsg(false, "This PageStack cannot be freed because it doesn't have a sourceArena or freeFunc pointer!");
				}
				pageHeader = nextPageHeader;
				pageIndex++;
			}
		} break;
		
		// +========================================+
		// | MemArenaType_VirtualStack FreeMemArena |
		// +========================================+
		case MemArenaType_VirtualStack:
		{
			Assert((arena->mainPntr == nullptr) == (arena->maxSize > 0));
			if (arena->mainPntr != nullptr)
			{
				OsFreeReservedMemory(arena->mainPntr, arena->maxSize);
			}
		} break;
		
		default: AssertMsg(false, "Tried to FreeMemArena on arena that doesn't know where it got it's memory from"); break;
	}
	
	ClearPointer(arena);
}

void ClearMemArena(MemArena_t* arena)
{
	NotNull(arena);
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +==================================+
		// | MemArenaType_Alias ClearMemArena |
		// +==================================+
		case MemArenaType_Alias:
		{
			ClearMemArena(arena->sourceArena);
		} break;
		
		// +======================================+
		// | MemArenaType_PagedHeap ClearMemArena |
		// +======================================+
		case MemArenaType_PagedHeap:
		{
			HeapPageHeader_t* pageHeader = (HeapPageHeader_t*)arena->headerPntr;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				HeapPageHeader_t* nextPageHeader = pageHeader->next;
				if (IsFlagSet(arena->flags, MemArenaFlag_AutoFreePages))
				{
					if (arena->sourceArena != nullptr)
					{
						FreeMem(arena->sourceArena, pageHeader, sizeof(HeapPageHeader_t) + pageHeader->size);
					}
					else if (arena->freeFunc != nullptr)
					{
						arena->freeFunc(pageHeader);
					}
					else
					{
						AssertMsg(false, "This PageHeap cannot be freed because it doesn't have a sourceArena of freeFunc pointer!");
					}
				}
				else
				{
					HeapAllocPrefix_t* allocPntr = (HeapAllocPrefix_t*)(pageHeader + 1);
					allocPntr->size = PackAllocPrefixSize(false, pageHeader->size);
				}
				pageHeader = nextPageHeader;
				pageIndex++;
			}
			
			arena->used = arena->numPages * sizeof(HeapAllocPrefix_t); //one empty header for each page
			arena->numAllocations = 0;
			if (IsFlagSet(arena->flags, MemArenaFlag_AutoFreePages))
			{
				arena->numPages = 0;
				arena->size = 0;
				arena->headerPntr = nullptr;
			}
		} break;
		
		// +========================================+
		// | MemArenaType_PagedStack ClearMemArena  |
		// +========================================+
		case MemArenaType_PagedStack:
		{
			MarkedStackArenaHeader_t* firstPageHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			MarkedStackArenaHeader_t* pageHeader = firstPageHeader;
			u64 pageIndex = 0;
			while (pageHeader != nullptr)
			{
				MarkedStackArenaHeader_t* nextPageHeader = pageHeader->next;
				if (pageIndex > 0)
				{
					if (arena->freeFunc != nullptr)
					{
						arena->freeFunc(pageHeader);
					}
					else if (arena->sourceArena != nullptr)
					{
						FreeMem(arena->sourceArena, pageHeader, pageHeader->thisPageSize);
					}
					else
					{
						AssertMsg(false, "This PageStack cannot be cleared because it doesn't have a sourceArena or freeFunc pointer!");
					}
				}
				pageHeader = nextPageHeader;
				pageIndex++;
			}
			
			firstPageHeader->numMarks = 0;
			arena->size = firstPageHeader->thisPageSize - sizeof(MarkedStackArenaHeader_t) - (firstPageHeader->maxNumMarks * sizeof(u64));
			arena->used = 0;
		} break;
		
		default: AssertMsg(false, "Tried to ClearMemArena on arena that doesn't know how to clear itself"); break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
}

// +--------------------------------------------------------------+
// |                 Push And Pop Mark Functions                  |
// +--------------------------------------------------------------+
u64 PushMemMark(MemArena_t* arena)
{
	NotNull(arena);
	u64 result = 0;
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_MarkedStack PushMemMark |
		// +======================================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			if (stackHeader->numMarks >= stackHeader->maxNumMarks)
			{
				GyLibPrintLine_E("Tried to push mark %llu onto marked stack which only has support for %llu marks", stackHeader->numMarks+1, stackHeader->maxNumMarks);
				AssertMsg(false, "Too many marks pushed onto a MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return 0;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			result = arena->used;
			marksPntr[stackHeader->numMarks] = result;
			stackHeader->numMarks++;
			
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (stackHeader->highMarkCount < stackHeader->numMarks) { stackHeader->highMarkCount = stackHeader->numMarks; }
			}
		} break;
		
		// +======================================+
		// | MemArenaType_PagedStack PushMemMark  |
		// +======================================+
		case MemArenaType_PagedStack:
		{
			NotNull(arena->headerPntr);
			
			MarkedStackArenaHeader_t* firstPage = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(firstPage->maxNumMarks > 0);
			Assert(firstPage->numMarks <= firstPage->maxNumMarks);
			if (firstPage->numMarks >= firstPage->maxNumMarks)
			{
				GyLibPrintLine_E("Tried to push mark %llu onto paged stack which only has support for %llu marks", firstPage->numMarks+1, firstPage->maxNumMarks);
				AssertMsg(false, "Too many marks pushed onto a PagedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return 0;
			}
			
			u64* firstPageMarks = (u64*)(((u8*)firstPage) + sizeof(MarkedStackArenaHeader_t));
			result = arena->used;
			firstPageMarks[firstPage->numMarks] = result;
			firstPage->numMarks++;
			
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (firstPage->highMarkCount < firstPage->numMarks) { firstPage->highMarkCount = firstPage->numMarks; }
			}
		} break;
		
		// +========================================+
		// | MemArenaType_VirtualStack PushMemMark  |
		// +========================================+
		case MemArenaType_VirtualStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			if (stackHeader->numMarks >= stackHeader->maxNumMarks)
			{
				GyLibPrintLine_E("Tried to push mark %llu onto virtual stack which only has support for %llu marks", stackHeader->numMarks+1, stackHeader->maxNumMarks);
				AssertMsg(false, "Too many marks pushed onto a VirtualStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return 0;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			result = arena->used;
			marksPntr[stackHeader->numMarks] = result;
			stackHeader->numMarks++;
			
			if (IsFlagSet(arena->flags, MemArenaFlag_TelemetryEnabled))
			{
				if (stackHeader->highMarkCount < stackHeader->numMarks) { stackHeader->highMarkCount = stackHeader->numMarks; }
			}
		} break;
		
		default: AssertMsg(false, "Tried to PushMemMark on arena that doesn't support pushing and popping"); break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
	return result;
}

void PopMemMark(MemArena_t* arena, u64 mark = 0xFFFFFFFFFFFFFFFFULL)
{
	NotNull(arena);
	
	bool didLock = false;
	if (IsValidGyMutex(&arena->mutex)) { LockGyMutex(&arena->mutex); didLock = true; }
	
	switch (arena->type)
	{
		// +======================================+
		// | MemArenaType_MarkedStack PopMemMark  |
		// +======================================+
		case MemArenaType_MarkedStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			if (stackHeader->numMarks == 0)
			{
				GyLibWriteLine_E("Tried to pop stack mark when no marks were left");
				AssertMsg(false, "Tried to pop too many times on a MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			Assert(marksPntr[stackHeader->numMarks-1] <= arena->used);
			Assert(marksPntr[stackHeader->numMarks-1] <= arena->size);
			AssertIf(mark != 0xFFFFFFFFFFFFFFFFULL, mark == marksPntr[stackHeader->numMarks-1]);
			arena->used = marksPntr[stackHeader->numMarks-1];
			stackHeader->numMarks--;
		} break;
		
		// +====================================+
		// | MemArenaType_PagedStack PopMemMark |
		// +====================================+
		case MemArenaType_PagedStack:
		{
			NotNull(arena->headerPntr);
			
			MarkedStackArenaHeader_t* firstPage = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(firstPage->maxNumMarks > 0);
			Assert(firstPage->numMarks <= firstPage->maxNumMarks);
			if (firstPage->numMarks == 0)
			{
				GyLibWriteLine_E("Tried to pop stack mark when no marks were left");
				AssertMsg(false, "Tried to pop too many times on a MarkedStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return;
			}
			
			u64* firstPageMarks = (u64*)(((u8*)firstPage) + sizeof(MarkedStackArenaHeader_t));
			Assert(firstPageMarks[firstPage->numMarks-1] <= arena->used);
			Assert(firstPageMarks[firstPage->numMarks-1] <= arena->size);
			AssertIf(mark != 0xFFFFFFFFFFFFFFFFULL, mark == firstPageMarks[firstPage->numMarks-1]);
			arena->used = firstPageMarks[firstPage->numMarks-1];
			firstPage->numMarks--;
			
			if (IsFlagSet(arena->flags, MemArenaFlag_AutoFreePages))
			{
				MarkedStackArenaHeader_t* prevPageHeader = nullptr;
				MarkedStackArenaHeader_t* pageHeader = firstPage;
				u64 pageIndex = 0;
				u64 byteIndex = 0;
				while (pageHeader != nullptr)
				{
					Assert(byteIndex <= arena->size);
					
					MarkedStackArenaHeader_t* nextPageHeader = pageHeader->next;
					u64 pageSize = pageHeader->thisPageSize - sizeof(MarkedStackArenaHeader_t) - (pageHeader->maxNumMarks * sizeof(u64));
					Assert(byteIndex + pageSize <= arena->size);
					
					if (byteIndex >= arena->used && pageIndex > 0)
					{
						if (prevPageHeader != nullptr) { prevPageHeader->next = nullptr; }
						if (arena->freeFunc != nullptr)
						{
							arena->freeFunc(pageHeader);
						}
						else
						{
							NotNull(arena->sourceArena);
							FreeMem(arena->sourceArena, pageHeader, pageHeader->thisPageSize);
						}
						arena->size -= pageSize;
						arena->numPages--;
						prevPageHeader = nullptr;
					}
					else { prevPageHeader = pageHeader; }
					
					pageHeader = nextPageHeader;
					byteIndex += pageSize;
					pageIndex++;
				}
			}
		} break;
		
		// +======================================+
		// | MemArenaType_VirtualStack PopMemMark |
		// +======================================+
		case MemArenaType_VirtualStack:
		{
			NotNull(arena->headerPntr);
			NotNull(arena->otherPntr);
			
			MarkedStackArenaHeader_t* stackHeader = (MarkedStackArenaHeader_t*)arena->headerPntr;
			Assert(stackHeader->maxNumMarks > 0);
			Assert(stackHeader->numMarks <= stackHeader->maxNumMarks);
			if (stackHeader->numMarks == 0)
			{
				GyLibWriteLine_E("Tried to pop stack mark when no marks were left");
				AssertMsg(false, "Tried to pop too many times on a VirtualStack");
				if (didLock) { UnlockGyMutex(&arena->mutex); }
				return;
			}
			
			u64* marksPntr = (u64*)arena->otherPntr;
			Assert(marksPntr[stackHeader->numMarks-1] <= arena->used);
			Assert(marksPntr[stackHeader->numMarks-1] <= arena->size);
			AssertIf(mark != 0xFFFFFFFFFFFFFFFFULL, mark == marksPntr[stackHeader->numMarks-1]);
			arena->used = marksPntr[stackHeader->numMarks-1];
			stackHeader->numMarks--;
		} break;
		
		default: AssertMsg(false, "Tried to PopMemMark on arena that doesn't support pushing and popping"); break;
	}
	
	if (didLock) { UnlockGyMutex(&arena->mutex); }
}

// +--------------------------------------------------------------+
// |                    Arena Print Functions                     |
// +--------------------------------------------------------------+
char* PrintInArena(MemArena_t* arena, const char* formatString, ...)
{
	NotNull(arena);
	NotNull(formatString);
	
	char* result = nullptr;
	va_list args;
	
	va_start(args, formatString);
	int length = MyVaListPrintf(result, 0, formatString, args); //Measure first
	Assert(length >= 0);
	va_end(args);
	
	result = AllocArray(arena, char, length+1); //Allocate
	if (result == nullptr) { return result; }
	
	va_start(args, formatString);
	MyVaListPrintf(result, (size_t)(length+1), formatString, args); //Real printf
	va_end(args);
	
	result[length] = '\0';
	
	return result;
}

int PrintVa_Measure(const char* formatString, va_list args)
{
	int result = MyVaListPrintf(nullptr, 0, formatString, args);
	return result;
}
void PrintVa_Print(const char* formatString, va_list args, char* allocatedSpace, int previousResult)
{
	Assert(previousResult >= 0);
	NotNull(allocatedSpace);
	int printResult = MyVaListPrintf(allocatedSpace, previousResult+1, formatString, args);
	Assert(printResult == previousResult);
	allocatedSpace[previousResult] = '\0';
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_MEMORY_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MemArenaType_None
MemArenaType_Redirect
MemArenaType_Alias
MemArenaType_StdHeap
MemArenaType_FixedHeap
MemArenaType_PagedHeap
MemArenaType_MarkedStack
MemArenaType_PagedStack
MemArenaType_VirtualStack
MemArenaType_Buffer
AllocAlignment_None
AllocAlignment_4Bytes
AllocAlignment_8Bytes
AllocAlignment_16Bytes
AllocAlignment_64Bytes
AllocAlignment_Max
@Types
MemArenaType_t
AllocAlignment_t
HeapAllocPrefix_t
HeapPageHeader_t
MarkedStackArenaHeader_t
MemArena_t
AllocationFunction_f
FreeFunction_f
@Functions
const char* GetMemArenaTypeStr(MemArenaType_t arenaType)
#define PackAllocPrefixSize(used, size)
#define IsAllocPrefixFilled(packedSize)
#define UnpackAllocPrefixSize(packedSize)
bool IsAlignedTo(const void* memoryPntr, AllocAlignment_t alignment)
u8 OffsetToAlign(const void* memoryPntr, AllocAlignment_t alignment)
bool IsPntrInsideRange(const void* testPntr, const void* rangeBase, u64 rangeSize, bool inclusive = false)
void FreeMemArena(MemArena_t* arena)
void ClearMemArena(MemArena_t* arena)
void InitMemArena_Redirect(MemArena_t* arena, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc)
void InitMemArena_Alias(MemArena_t* arena, MemArena_t* sourceArena)
void InitMemArena_StdHeap(MemArena_t* arena)
void InitMemArena_FixedHeap(MemArena_t* arena, u64 size, void* memoryPntr, AllocAlignment_t alignment = AllocAlignment_None)
void InitMemArena_PagedHeapFuncs(MemArena_t* arena, u64 pageSize, AllocationFunction_f* allocFunc, FreeFunction_f* freeFunc, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
void InitMemArena_PagedHeapArena(MemArena_t* arena, u64 pageSize, MemArena_t* sourceArena, u64 maxNumPages = 0, AllocAlignment_t alignment = AllocAlignment_None)
void InitMemArena_MarkedStack(MemArena_t* arena, u64 size, void* memoryPntr, u64 maxNumMarks, AllocAlignment_t alignment = AllocAlignment_None)
void InitMemArena_Buffer(MemArena_t* arena, u64 bufferSize, void* bufferPntr, bool singleAlloc = false, AllocAlignment_t alignment = AllocAlignment_None)
#define CreateStackBufferArena(arenaName, bufferName, size)
bool IsInitialized(const MemArena_t* arena)
bool DoesMemArenaSupportFreeing(MemArena_t* arena)
bool DoesMemArenaSupportPushAndPop(MemArena_t* arena)
bool MemArenaVerify(MemArena_t* arena, bool assertOnFailure = false)
u64 GetNumMarks(MemArena_t* arena)
void* AllocMem(MemArena_t* arena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None)
#define AllocStruct(arena, structName)
#define AllocArray(arena, structName, numItems)
#define AllocBytes(arena, numBytes)
#define AllocChars(arena, numBytes)
MemArena_t AllocBufferArena(MemArena_t* sourceArena, u64 numBytes, AllocAlignment_t alignOverride = AllocAlignment_None)
char* AllocCharsAndFill(MemArena_t* arena, u64 numChars, const char* dataForFill, bool addNullTerm = true)
char* AllocCharsAndFillNt(MemArena_t* arena, const char* nullTermStr, bool addNullTerm = true)
u64 GrowMemQuery(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, GrowMemToken_t* tokenOut = nullptr)
void GrowMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize, const GrowMemToken_t* token)
void ShrinkMem(MemArena_t* arena, const void* prevAllocPntr, u64 prevAllocSize, u64 newAllocSize)
bool FreeMem(MemArena_t* arena, void* allocPntr, u64 allocSize = 0, bool ignoreNullptr = false, u64* oldSizeOut = nullptr)
#define HardFreeMem(arena, allocPntr)
#define SoftFreeMem(arena, allocPntr)
#define FreeBufferArena(bufferArena, sourceArena)
void* ReallocMem(MemArena_t* arena, void* allocPntr, u64 newSize, u64 oldSize = 0, AllocAlignment_t alignOverride = AllocAlignment_None, bool ignoreNullptr = false, u64* oldSizeOut = nullptr)
#define HardReallocMem(arena, allocPntr, newSize)
#define SoftReallocMem(arena, allocPntr, newSize)
u64 PushMemMark(MemArena_t* arena)
void PopMemMark(MemArena_t* arena, u64 mark = 0xFFFFFFFFFFFFFFFFULL)
char* PrintInArena(MemArena_t* arena, const char* formatString, ...)
int PrintVa_Measure(const char* formatString, va_list args)
void PrintVa_Print(const char* formatString, va_list args, char* allocatedSpace, int previousResult)
#define PrintInArenaVa(arena, resultName, resultLengthName, formatString)
#define InPlaceNew(type, pntrToClass, ...)
#define ArenaNew(type, pntrToAssign, arena, ...)
#define ArenaDelete(type, pntr, arena)
*/
