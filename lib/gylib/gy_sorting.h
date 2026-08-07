/*
File:   gy_sorting.h
Author: Taylor Robbins
Date:   01\08\2022
Description:
	** This file holds all of the sorting algorithms that we have implemented.
	** Some algorithms are implemented in different ways to provide different ways of using them
*/

//TODO: These functions have not been tested since they were imported in from mylib

#ifndef _GY_SORTING_H
#define _GY_SORTING_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                  Structure/Type Definitions                  |
// +--------------------------------------------------------------+
//-1: <    0: ==   1: >
#define COMPARE_FUNC_DEFINITION(functionName) i32 functionName(const void* left, const void* right, void* contextPntr)
typedef COMPARE_FUNC_DEFINITION(CompareFunc_f);

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	i32 CompareFuncU8(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncU16(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncU32(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncU64(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncI8(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncI16(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncI32(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncI64(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncR32(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncR64(const void* left, const void* right, void* contextPntr);
	i32 CompareFuncMyStr(const void* left, const void* right, void* contextPntr);
	i64 QuickSortPartition(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr = nullptr);
	void QuickSort(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr);
	void QuickSortAlloc(void* arrayPntr, u64 numElements, u64 elementSize, MemArena_t* workingAllocArena, CompareFunc_f* compareFunc, void* contextPntr = nullptr);
#else

// +--------------------------------------------------------------+
// |                      Compare Functions                       |
// +--------------------------------------------------------------+
i32 CompareFuncU8(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	u8* leftValue = (u8*)left;
	u8* rightValue = (u8*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncU16(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	u16* leftValue = (u16*)left;
	u16* rightValue = (u16*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncU32(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	u32* leftValue = (u32*)left;
	u32* rightValue = (u32*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncU64(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	u64* leftValue = (u64*)left;
	u64* rightValue = (u64*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncI8(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	i8* leftValue = (i8*)left;
	i8* rightValue = (i8*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncI16(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	i16* leftValue = (i16*)left;
	i16* rightValue = (i16*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncI32(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	i32* leftValue = (i32*)left;
	i32* rightValue = (i32*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncI64(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	i64* leftValue = (i64*)left;
	i64* rightValue = (i64*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncR32(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	r32* leftValue = (r32*)left;
	r32* rightValue = (r32*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncR64(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	r64* leftValue = (r64*)left;
	r64* rightValue = (r64*)right;
	if (*leftValue < *rightValue) { return -1; }
	else if (*leftValue > *rightValue) { return 1; }
	else { return 0; }
}
i32 CompareFuncMyStr(const void* left, const void* right, void* contextPntr)
{
	UNUSED(contextPntr);
	NotNull2(left, right);
	MyStr_t* leftStr = (MyStr_t*)left;
	MyStr_t* rightStr = (MyStr_t*)right;
	u64 leftIndex = 0;
	u64 rightIndex = 0;
	while (leftIndex <= leftStr->length && rightIndex <= rightStr->length)
	{
		if (leftIndex == leftStr->length && rightIndex == rightStr->length) { break; }
		if (leftIndex == leftStr->length) { return -1; }
		if (rightIndex == rightStr->length) { return 1; }
		u32 leftCodepoint = 0;
		u8 leftCharSize = GetCodepointForUtf8(leftStr->length - leftIndex, &leftStr->pntr[leftIndex], &leftCodepoint);
		u32 rightCodepoint = 0;
		u8 rightCharSize = GetCodepointForUtf8(rightStr->length - rightIndex, &rightStr->pntr[rightIndex], &rightCodepoint);
		i32 charCompareResult = CompareCodepoints(leftCodepoint, rightCodepoint);
		if (charCompareResult != 0) { return charCompareResult; }
		leftIndex += leftCharSize;
		rightIndex += rightCharSize;
	}
	return 0;
}

// +--------------------------------------------------------------+
// |                          Quick Sort                          |
// +--------------------------------------------------------------+
i64 QuickSortPartition(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
{
	NotNull(arrayPntr);
	Assert(elementSize > 0);
	NotNull(workingSpace);
	NotNull(compareFunc);
	u8* bytePntr = (u8*)arrayPntr;
	
	u8* tempSpace = ((u8*)workingSpace) + 0;
	u8* pivotCopy = ((u8*)workingSpace) + elementSize;
	u8* pivotPntr = bytePntr + elementSize*(numElements-1);
	MyMemCopy(pivotCopy, pivotPntr, elementSize);
	
	i64 smallIndex = -1;
	u8* smallPntr = bytePntr - elementSize;
	for (u64 eIndex = 0; eIndex < numElements-1; eIndex++)
	{
		u8* elementPntr = bytePntr + elementSize*(eIndex);
		i32 compareResult = compareFunc((const void*)elementPntr, (const void*)pivotCopy, contextPntr);
		if (compareResult < 0)
		{
			smallIndex++;
			smallPntr += elementSize;
			
			//swap smallPntr and elementPntr
			MyMemCopy(tempSpace, smallPntr, elementSize);
			MyMemCopy(smallPntr, elementPntr, elementSize);
			MyMemCopy(elementPntr, tempSpace, elementSize);
		}
	}
	
	smallIndex++;
	smallPntr += elementSize;
	Assert((u64)smallIndex < numElements);
	MyMemCopy(pivotPntr, smallPntr, elementSize);
	MyMemCopy(smallPntr, pivotCopy, elementSize);
	
	return smallIndex;
}

//NOTE: workingSpace must be a space large enough to hold two elements. This space is used to perform swaps and to hold the pivot element
void QuickSort(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr)
{
	NotNull(arrayPntr);
	Assert(elementSize > 0);
	NotNull(workingSpace);
	NotNull(compareFunc);
	if (numElements <= 1) { return; } //nothing to sort
	u8* bytePntr = (u8*)arrayPntr;
	
	#if DEBUG_BUILD
	//make sure the workingSpace is large enough by writing it to zero to make sure we don't have any memory access exceptions happen
	MyMemSet(workingSpace, 0x00, elementSize*2);
	#endif
	
	//parition the array
	i64 pi = QuickSortPartition(arrayPntr, numElements, elementSize, workingSpace, compareFunc, contextPntr);
	
	//sort lower partition
	QuickSort(arrayPntr, (u64)pi, elementSize, workingSpace, compareFunc, contextPntr);
	
	//sort upper partition
	QuickSort((void*)(bytePntr + elementSize*(pi+1)), (u64)(numElements - (pi+1)), elementSize, workingSpace, compareFunc, contextPntr);
}

//Allocates 2x elementSize space in workingAllocArena. This space is used to perform swaps and to hold the pivot element
void QuickSortAlloc(void* arrayPntr, u64 numElements, u64 elementSize, MemArena_t* workingAllocArena, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
{
	Assert(elementSize > 0);
	Assert(workingAllocArena != nullptr);
	u8* workingSpace = AllocArray(workingAllocArena, u8, elementSize*2);
	Assert(workingSpace != nullptr);
	QuickSort(arrayPntr, numElements, elementSize, workingSpace, compareFunc, contextPntr);
	FreeMem(workingAllocArena, workingSpace, elementSize*2);
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_SORTING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
CompareFunc_f
@Functions
i32 COMPARE_FUNC_DEFINITION(const void* left, const void* right, void* contextPntr)
i32 CompareFuncU8(const void* left, const void* right, void* contextPntr)
i32 CompareFuncU16(const void* left, const void* right, void* contextPntr)
i32 CompareFuncU32(const void* left, const void* right, void* contextPntr)
i32 CompareFuncU64(const void* left, const void* right, void* contextPntr)
i32 CompareFuncI8(const void* left, const void* right, void* contextPntr)
i32 CompareFuncI16(const void* left, const void* right, void* contextPntr)
i32 CompareFuncI32(const void* left, const void* right, void* contextPntr)
i32 CompareFuncI64(const void* left, const void* right, void* contextPntr)
i32 CompareFuncR32(const void* left, const void* right, void* contextPntr)
i32 CompareFuncR64(const void* left, const void* right, void* contextPntr)
i32 CompareFuncMyStr(const void* left, const void* right, void* contextPntr)
i64 QuickSortPartition(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
void QuickSort(void* arrayPntr, u64 numElements, u64 elementSize, void* workingSpace, CompareFunc_f* compareFunc, void* contextPntr)
void QuickSortAlloc(void* arrayPntr, u64 numElements, u64 elementSize, MemArena_t* workingAllocArena, CompareFunc_f* compareFunc, void* contextPntr = nullptr)
*/
