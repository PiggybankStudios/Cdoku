/*
File:   gy_scratch_arenas.h
Author: Taylor Robbins
Date:   08\09\2023
Description:
	** This file is always included by gy.h but it will only have an affect if GYLIB_SCRATCH_ARENA_AVAILABLE is defined.
	** By defining this macro, you allow functions inside gylib that rely on scratch arenas to appear. You must then
	** compile gy_scratch_arenas.cpp so that the linker doesn't complain about references to the GetScratchArena function
*/

#ifndef _GY_SCRATCH_ARENAS_H
#define _GY_SCRATCH_ARENAS_H

#ifdef GYLIB_SCRATCH_ARENA_AVAILABLE
#undef GYLIB_SCRATCH_ARENA_AVAILABLE
#define GYLIB_SCRATCH_ARENA_AVAILABLE 1
#else
#define GYLIB_SCRATCH_ARENA_AVAILABLE 0
#endif

#if GYLIB_SCRATCH_ARENA_AVAILABLE

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define NUM_SCRATCH_ARENAS_PER_THREAD   3

// +--------------------------------------------------------------+
// |                        Main Interface                        |
// +--------------------------------------------------------------+
#ifdef GYLIB_SCRATCH_ARENAS_STATIC
static MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1 = nullptr, MemArena_t* avoidConflictWith2 = nullptr);
static void FreeScratchArena(MemArena_t* scratchArena);
#else
inline MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1 = nullptr, MemArena_t* avoidConflictWith2 = nullptr);
inline void FreeScratchArena(MemArena_t* scratchArena);
#endif

#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

#endif //  _GY_SCRATCH_ARENAS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
GYLIB_SCRATCH_ARENA_AVAILABLE
NUM_SCRATCH_ARENAS_PER_THREAD
@Functions
MemArena_t* GetScratchArena(MemArena_t* avoidConflictWith1 = nullptr, MemArena_t* avoidConflictWith2 = nullptr)
void FreeScratchArena(MemArena_t* scratchArena)
*/
