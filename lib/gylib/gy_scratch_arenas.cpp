/*
File:   gy_scratch_arenas.cpp
Author: Taylor Robbins
Date:   08\09\2023
Description:
	** Scratch arenas are a concept built around thread-local storage
	** The idea is to keep a few stack style arenas handy for things that need to
	** allocate and forget about memory in a thread-safe way. Having a few stacks
	** and the ability to pass in "conflict" arenas to avoid aliasing, we are able
	** to serve many users of scratch arenas all piled on top of each other
*/

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
thread_local MemArena_t ThreadLocalScratchArenas[NUM_SCRATCH_ARENAS_PER_THREAD];

// +--------------------------------------------------------------+
// |                        Init and Free                         |
// +--------------------------------------------------------------+
void FreeThreadLocalScratchArenas()
{
	for (u64 aIndex = 0; aIndex < ArrayCount(ThreadLocalScratchArenas); aIndex++)
	{
		if (ThreadLocalScratchArenas[aIndex].size > 0)
		{
			FreeMemArena(&ThreadLocalScratchArenas[aIndex]);
		}
		else
		{
			ClearStruct(ThreadLocalScratchArenas[aIndex]);
		}
	}
}
void InitThreadLocalScratchArenasVirtual(u64 maxVirtualSize, u64 scratchMaxMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	for (u64 aIndex = 0; aIndex < ArrayCount(ThreadLocalScratchArenas); aIndex++)
	{
		if (maxVirtualSize > 0)
		{
			InitMemArena_VirtualStack(&ThreadLocalScratchArenas[aIndex], maxVirtualSize, scratchMaxMarks, alignment);
		}
		else
		{
			ClearStruct(ThreadLocalScratchArenas[aIndex]);
		}
	}
}
void InitThreadLocalScratchArenasPaged(MemArena_t* sourceArena, u64 scratchPageSize, u64 scratchMaxMarks, AllocAlignment_t alignment = AllocAlignment_None)
{
	AssertIf(scratchPageSize > 0, sourceArena != nullptr);
	for (u64 aIndex = 0; aIndex < ArrayCount(ThreadLocalScratchArenas); aIndex++)
	{
		if (scratchPageSize > 0)
		{
			InitMemArena_PagedStackArena(&ThreadLocalScratchArenas[aIndex], scratchPageSize, sourceArena, scratchMaxMarks, alignment);
		}
		else
		{
			ClearStruct(ThreadLocalScratchArenas[aIndex]);
		}
	}
}

// +--------------------------------------------------------------+
// |                        Main Interface                        |
// +--------------------------------------------------------------+
// Pass in 1 or 2 arenas that you DON'T want to get back from this function. i.e. you are going to use those arenas for memory that outlives your scratch memory
inline MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1, MemArena_t* avoidConflictWith2) //pre-declared in gy_scratch_arenas.h
{
	u64 scratchIndex = 0;
	while ((avoidConflictWith1 != nullptr && avoidConflictWith1 == &ThreadLocalScratchArenas[scratchIndex]) ||
	       (avoidConflictWith2 != nullptr && avoidConflictWith2 == &ThreadLocalScratchArenas[scratchIndex]))
	{
		scratchIndex++;
	}
	Assert(scratchIndex < NUM_SCRATCH_ARENAS_PER_THREAD);
	MemArena_t* result = &ThreadLocalScratchArenas[scratchIndex];
	if (!IsInitialized(result)) { return nullptr; }
	PushMemMark(result);
	return result;
}

inline void FreeScratchArena(MemArena_t* scratchArena) //pre-declared in gy_scratch_arenas.h
{
	NotNull(scratchArena);
	Assert(IsPntrInsideRange(scratchArena, &ThreadLocalScratchArenas[0], sizeof(MemArena_t) * NUM_SCRATCH_ARENAS_PER_THREAD));
	if (scratchArena->size == 0) { return; }
	PopMemMark(scratchArena);
}

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
NUM_SCRATCH_ARENAS_PER_THREAD
@Globals
ThreadLocalScratchArenas
@Functions
void FreeThreadLocalScratchArenas()
void InitThreadLocalScratchArenasVirtual(u64 maxVirtualSize, u64 scratchMaxMarks, AllocAlignment_t alignment = AllocAlignment_None)
void InitThreadLocalScratchArenasPaged(MemArena_t* sourceArena, u64 scratchPageSize, u64 scratchMaxMarks, AllocAlignment_t alignment = AllocAlignment_None)
inline MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1 = nullptr, MemArena_t* avoidConflictWith2 = nullptr)
inline void FreeScratchArena(MemArena_t* scratchArena)
*/
