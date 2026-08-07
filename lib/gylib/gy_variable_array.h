/*
File:   gy_variable_array.h
Author: Taylor Robbins
Date:   10\12\2021
Description:
	** The VarArray_t structure acts like a stretchy buffer, resizing as needed to hold as many fixed size elements as needed
*/

#ifndef _GY_VARIABLE_ARRAY_H
#define _GY_VARIABLE_ARRAY_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                          Structure                           |
// +--------------------------------------------------------------+
//TODO: Add support for a maximum length restriction
struct VarArray_t
{
	MemArena_t* allocArena;
	u64 itemSize;
	u64 allocChunkSize;
	bool exponentialChunkSize;
	MyStr_t name;
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	const char* creationFilePath;
	u64 creationLineNumber;
	const char* creationFuncName;
	#endif
	
	u64 length;
	u64 allocLength;
	void* items;
	bool wasExpanded; //feel free to clear this in usage code
	u64 numExpansions;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#if GYLIB_SCRATCH_ARENA_AVAILABLE
#define CreateVarArray(array, memArena, itemSize, ...) CreateVarArray_(__FILE__, __LINE__, __func__, (array), (memArena), (itemSize), ##__VA_ARGS__)
#else
#define CreateVarArray(array, memArena, itemSize, ...) CreateVarArray_((array), (memArena), (itemSize), ##__VA_ARGS__)
#endif

#define VarArrayContains(array, itemPntr) VarArrayContains_((array), (itemPntr), sizeof(*(itemPntr)))
#define VarArrayContainsTyped(array, itemPntr, type) VarArrayContains_((array), (itemPntr), sizeof(type))

#define VarArrayGetIndexOf(array, itemPntr, indexOutPntr, type) VarArrayGetIndexOf_((array), (itemPntr), (indexOutPntr), sizeof(type))

#define VarArrayGetHard(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), true))
#define VarArrayGetSoft(array, index, type) ((type*)VarArrayGet_((array), (index), sizeof(type), false))
#define VarArrayGet(array, index, type) VarArrayGetHard((array), (index), type)

#define VarArrayGetFirstHard(array, type) VarArrayGetHard((array), 0, type)
#define VarArrayGetFirstSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), 0, type) : nullptr)
#define VarArrayGetFirst(array, type) VarArrayGetFirstHard((array), type) 

#define VarArrayGetLastHard(array, type) VarArrayGetHard((array), (array)->length - 1, type)
#define VarArrayGetLastSoft(array, type) (((array)->length > 0) ? VarArrayGetSoft((array), (array)->length - 1, type) : nullptr)
#define VarArrayGetLast(array, type) VarArrayGetLastHard((array), type) 

#define VarArrayLoop(arrayPntr, indexName) for (u64 indexName = 0; indexName < (arrayPntr)->length; indexName++)
#define VarArrayLoopGet(type, varName, arrayPntr, index) type* varName = VarArrayGet((arrayPntr), (index), type)

#define VarArrayAdd(array, type) (type*)VarArrayAdd_((array), sizeof(type))
#define VarArrayPush(array, type) VarArrayAdd((array), (type))

#define VarArrayInsert(array, index, type) (type*)VarArrayInsert_((array), (index), sizeof(type))
#define VarArrayPushFront(array, type) (type*)VarArrayInsert_((array), 0, sizeof(type))

#define VarArrayRemove(array, index, type) VarArrayRemove_((array), (index), sizeof(type))
#define VarArrayPop(array, type) VarArrayRemove_((array), (array)->length - 1, sizeof(type))
#define VarArrayPopFront(array, type) VarArrayRemove_((array), 0, sizeof(type))

#define VarArrayRemoveByPntr(array, itemToRemove) VarArrayRemoveByPntr_((array), (itemToRemove), sizeof(*(itemToRemove)))
#define VarArrayRemoveByPntrTyped(array, itemToRemove, type) VarArrayRemoveByPntr_((array), (itemToRemove), sizeof(type))

#define VarArrayAddRange(array, index, newItemsCount, type) (type*)VarArrayAddRange_((array), (index), (newItemsCount), sizeof(type))

#define VarArrayMerge(destArray, sourceArray) VarArrayAddVarArray((destArray), (sourceArray), (destArray)->length, 0, (sourceArray)->length)

#define VarArrayRemoveRange(array, index, numItemsToRemove, type) VarArrayRemoveRange_((array), (index), (numItemsToRemove), sizeof(type))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeVarArray(VarArray_t* array);
	#if GYLIB_SCRATCH_ARENA_AVAILABLE
	void CreateVarArray_(const char* filePath, u64 lineNumber, const char* funcName, VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8);
	#else
	void CreateVarArray_(VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8);
	#endif
	bool VarArrayIsCreated(VarArray_t* array);
	void VarArrayName(VarArray_t* array, MyStr_t newName);
	bool VarArrayExpand(VarArray_t* array, u64 capacityRequired);
	bool VarArrayContains_(VarArray_t* array, const void* itemInQuestion, u64 itemSize);
	bool VarArrayGetIndexOf_(VarArray_t* array, const void* itemInQuestion, u64* indexOut, u64 itemSize);
	void VarArrayClear(VarArray_t* array, bool deallocate = false);
	void VarArrayClearMyStr(VarArray_t* array, MemArena_t* stringAllocArena);
	void* VarArrayGet_(VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure);
	const void* VarArrayGet_(const VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure);
	void* VarArrayAdd_(VarArray_t* array, u64 itemSize);
	void* VarArrayInsert_(VarArray_t* array, u64 index, u64 itemSize);
	void VarArrayRemove_(VarArray_t* array, u64 index, u64 itemSize);
	void VarArrayRemoveByPntr_(VarArray_t* array, const void* itemToRemove, u64 itemSize);
	void* VarArrayAddRange_(VarArray_t* array, u64 index, u64 newItemsCount, u64 itemSize);
	void VarArrayAddVarArray(VarArray_t* destArray, const VarArray_t* sourceArray, u64 destIndex, u64 sourceIndex = 0, u64 sourceCount = UINT64_MAX);
	void VarArrayRemoveRange_(VarArray_t* array, u64 index, u64 numItemsToRemove, u64 itemSize);
	void VarArrayCopy(VarArray_t* destArray, const VarArray_t* sourceArray, MemArena_t* memArena);
	void* VarArrayMove(VarArray_t* array, u64 fromIndex, u64 toIndex, bool swapWithTarget = true);
	void VarArrayReverse(VarArray_t* array, u64 startIndex, u64 endIndex);
	void VarArrayReverse(VarArray_t* array);
	#if defined(_GY_SORTING_H)
	void VarArraySort(VarArray_t* array, CompareFunc_f* compareFunc, void* contextPntr);
	#endif
#else

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeVarArray(VarArray_t* array)
{
	NotNull(array);
	if (array->items != nullptr)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->itemSize * array->allocLength);
	}
	if (!IsEmptyStr(array->name))
	{
		NotNull(array->allocArena);
		FreeString(array->allocArena, &array->name);
	}
	ClearPointer(array);
}

#if GYLIB_SCRATCH_ARENA_AVAILABLE
void CreateVarArray_(const char* filePath, u64 lineNumber, const char* funcName, VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
#else
void CreateVarArray_(VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
#endif
{
	NotNull(array);
	NotNull(memArena);
	Assert(itemSize > 0);
	Assert(allocChunkSize > 0);
	// ClearPointer(array); //NOTE: This was somewhat slow. So instead we just make sure all variables inside the struct are set below!
	array->allocArena = memArena;
	array->itemSize = itemSize;
	array->allocChunkSize = allocChunkSize;
	array->exponentialChunkSize = exponentialChunkSize;
	array->name = MyStr_Empty;
	array->length = 0;
	array->wasExpanded = false;
	array->numExpansions = 0;
	#if GYLIB_SCRATCH_ARENA_AVAILABLE
		#if GYLIB_MEM_ARENA_DEBUG_ENABLED
		array->creationFilePath = filePath;
		array->creationLineNumber = lineNumber;
		array->creationFuncName = funcName;
		#else
		UNUSED(filePath);
		UNUSED(lineNumber);
		UNUSED(funcName);
		#endif
	#endif
	
	if (exponentialChunkSize)
	{
		array->allocLength = (initialRequiredCapacity > 0) ? allocChunkSize : 0;
		#if DEBUG_BUILD
		u64 sanityCounter = 0;
		#endif
		while (array->allocLength < initialRequiredCapacity)
		{
			DebugAssert(array->allocLength <= UINT64_MAX/2);
			array->allocLength *= 2;
			#if DEBUG_BUILD
			sanityCounter++;
			Assert(sanityCounter <= initialRequiredCapacity); //Infinite loop sanity check
			#endif
		}
	}
	else
	{
		array->allocLength = (initialRequiredCapacity > 0) ? RoundUpToU64(initialRequiredCapacity, allocChunkSize) : 0;
	}
	if (array->allocLength > 0)
	{
		#if GYLIB_MEM_ARENA_DEBUG_ENABLED
		array->items = AllocMem_(filePath, lineNumber, funcName, memArena, array->allocLength * itemSize);
		#else
		array->items = AllocMem(memArena, array->allocLength * itemSize);
		#endif
		if (array->items == nullptr)
		{
			AssertMsg(false, "Initial Allocation inside CreateVarArray failed!");
			ClearPointer(array);
			return;
		}
	}
	else { array->items = nullptr; }
}

bool VarArrayIsCreated(VarArray_t* array)
{
	NotNull(array);
	return (array->allocArena != nullptr);
}

void VarArrayName(VarArray_t* array, MyStr_t newName)
{
	NotNull(array->allocArena);
	if (array->name.pntr != nullptr)
	{
		FreeString(array->allocArena, &array->name);
	}
	array->name = AllocString(array->allocArena, &newName);
	NotNullStr(&array->name);
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
//Returns if the array actually expanded
bool VarArrayExpand(VarArray_t* array, u64 capacityRequired)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	if (array->allocLength >= capacityRequired) { return false; }
	
	u64 newLength = 0;
	if (array->exponentialChunkSize)
	{
		newLength = array->allocLength;
		if (newLength == 0) { newLength = array->allocChunkSize; }
		#if DEBUG_BUILD
		u64 sanityCounter = 0;
		#endif
		while (newLength < capacityRequired)
		{
			DebugAssert(newLength <= UINT64_MAX/2);
			newLength *= 2;
			#if DEBUG_BUILD
			sanityCounter++;
			Assert(sanityCounter <= capacityRequired); //Infinite loop sanity check
			#endif
		}
	}
	else
	{
		newLength = RoundUpToU64(capacityRequired, array->allocChunkSize);
	}
	Assert(newLength >= capacityRequired);
	Assert(newLength <= (UINT64_MAX / array->itemSize)); //u64 overflow would be weird
	
	#if GYLIB_MEM_ARENA_DEBUG_ENABLED
	void* newSpace = AllocMem_(array->creationFilePath, array->creationLineNumber, array->creationFuncName, array->allocArena, newLength * array->itemSize);
	#else
	void* newSpace = AllocMem(array->allocArena, newLength * array->itemSize);
	#endif
	if (newSpace == nullptr)
	{
		GyLibPrintLine_E("Failed to expand variable array %s to %llu items at %llu bytes each", (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", newLength, array->itemSize);
		AssertMsg(false, "Failed to expand variable array");
		return false;
	}
	
	if (array->length > 0)
	{
		MyMemCopy(newSpace, array->items, array->length * array->itemSize);
	}
	if (array->items != nullptr)
	{
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
	}
	
	array->items = newSpace;
	array->allocLength = newLength;
	array->wasExpanded = true;
	IncrementU64(array->numExpansions);
	return true;
}

bool VarArrayContains_(VarArray_t* array, const void* itemInQuestion, u64 itemSize)
{
	NotNull(array);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayContains. Make sure you're accessing the VarArray with the correct type!");
	if (itemInQuestion == nullptr) { return false; }
	if (array->items == nullptr) { return false; }
	if ((const u8*)itemInQuestion < (u8*)array->items) { return false; }
	if ((const u8*)itemInQuestion >= ((u8*)array->items) + (array->itemSize * array->length)) { return false; }
	#if DEBUG_BUILD
	//Make sure the item is aligned properly
	u64 offsetFromBase = (u64)(((const u8*)itemInQuestion) - (u8*)array->items);
	Assert((offsetFromBase % array->itemSize) == 0);
	#endif
	return true;
}

bool VarArrayGetIndexOf_(VarArray_t* array, const void* itemInQuestion, u64* indexOut, u64 itemSize)
{
	if (!VarArrayContains_(array, itemInQuestion, itemSize)) { return false; }
	u64 offsetFromBase = (u64)(((const u8*)itemInQuestion) - (u8*)array->items);
	Assert((offsetFromBase % array->itemSize) == 0);
	NotNull(indexOut);
	*indexOut = (offsetFromBase / array->itemSize);
	return true;
}

void VarArrayClear(VarArray_t* array, bool deallocate = false)
{
	NotNull(array);
	if (deallocate && array->allocLength > 0)
	{
		NotNull(array->allocArena);
		FreeMem(array->allocArena, array->items, array->allocLength * array->itemSize);
		array->items = nullptr;
		array->allocLength = 0;
	}
	array->length = 0;
}
void VarArrayClearMyStr(VarArray_t* array, MemArena_t* stringAllocArena)
{
	Assert(array->itemSize == sizeof(MyStr_t));
	for (u64 sIndex = 0; sIndex < array->length; sIndex++)
	{
		MyStr_t* str = ((MyStr_t*)array->items) + sIndex;
		FreeString(stringAllocArena, str);
	}
	VarArrayClear(array);
}

// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
void* VarArrayGet_(VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayGet. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		if (assertOnFailure)
		{
			GyLibPrintLine_E("Tried to get item %llu/%llu in VarArray %s of itemSize %llu", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
			AssertMsg(false, "VarArrayGet out of bounds!");
		}
		return nullptr;
	}
	
	NotNull(array->items);
	void* result = (((u8*)array->items) + (index * array->itemSize));
	return result;
}
const void* VarArrayGet_(const VarArray_t* array, u64 index, u64 itemSize, bool assertOnFailure) //const-variant
{
	return (const void*)VarArrayGet_((VarArray_t*)array, index, itemSize, assertOnFailure);
}

// +--------------------------------------------------------------+
// |                             Add                              |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayAdd_(VarArray_t* array, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayAdd. Make sure you're accessing the VarArray with the correct type!");
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	void* result = (((u8*)array->items) + (array->itemSize * array->length));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}

// +--------------------------------------------------------------+
// |                            Insert                            |
// +--------------------------------------------------------------+
//NOTE: This always asserts on failure to allocate since VarArrayExpand defaults to asserting
void* VarArrayInsert_(VarArray_t* array, u64 index, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayInsert. Make sure you're accessing the VarArray with the correct type!");
	Assert(index <= array->length);
	if (index == array->length) { return VarArrayAdd_(array, itemSize); }
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength >= array->length+1);
	
	//Move all items above the index up by one slot
	u8* itemPntr = (u8*)VarArrayGet_(array, array->length-1, itemSize, true);
	for (u64 iIndex = array->length; iIndex > index; iIndex--)
	{
		MyMemCopy(itemPntr + array->itemSize, itemPntr, array->itemSize);
		itemPntr -= array->itemSize;
	}
	
	void* result = (((u8*)array->items) + (array->itemSize * index));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, array->itemSize);
	#endif
	array->length++;
	
	return result;
}

// +--------------------------------------------------------------+
// |                            Remove                            |
// +--------------------------------------------------------------+
void VarArrayRemove_(VarArray_t* array, u64 index, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayRemove. Make sure you're accessing the VarArray with the correct type!");
	
	if (index >= array->length)
	{
		GyLibPrintLine_E("Tried to remove item [%llu]/%llu in VarArray %s of itemSize %llu", index, array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
		AssertMsg(false, "VarArrayRemove out of bounds!");
		return;
	}
	
	//move all items above index down by one
	for (u64 iIndex = index; iIndex+1 < array->length; iIndex++)
	{
		MyMemCopy(VarArrayGet_(array, iIndex, itemSize, true), VarArrayGet_(array, iIndex+1, itemSize, true), array->itemSize);
	}
	array->length--;
}

// +--------------------------------------------------------------+
// |                         RemoveByPntr                         |
// +--------------------------------------------------------------+
void VarArrayRemoveByPntr_(VarArray_t* array, const void* itemToRemove, u64 itemSize)
{
	u64 itemIndex = 0;
	bool itemInArray = VarArrayGetIndexOf_(array, itemToRemove, &itemIndex, itemSize);
	Assert(itemInArray);
	VarArrayRemove_(array, itemIndex, itemSize);
}

// +--------------------------------------------------------------+
// |                           AddRange                           |
// +--------------------------------------------------------------+
void* VarArrayAddRange_(VarArray_t* array, u64 index, u64 newItemsCount, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayAddRange. Make sure you're accessing the VarArray with the correct type!");
	Assert(index <= array->length);
	if (newItemsCount == 0) { return nullptr; }
	if (newItemsCount == 1) { return VarArrayInsert_(array, index, itemSize); }
	
	VarArrayExpand(array, array->length+newItemsCount);
	Assert(array->allocLength >= array->length+newItemsCount);
	
	//Move all items above the index up by however many slots are being inserted
	if (array->length > 0)
	{
		for (u64 iIndex = array->length-1; iIndex >= index; iIndex--)
		{
			MyMemCopy(VarArrayGet_(array, iIndex + newItemsCount, itemSize, true), VarArrayGet_(array, iIndex, itemSize, true), array->itemSize);
		}
	}
	
	void* result = (((u8*)array->items) + (array->itemSize * index));
	#if DEBUG_BUILD
	MyMemSet(result, 0xCC, newItemsCount * array->itemSize);
	#endif
	array->length += newItemsCount;
	
	return result;
}

// +--------------------------------------------------------------+
// |                         AddVarArray                          |
// +--------------------------------------------------------------+
void VarArrayAddVarArray(VarArray_t* destArray, const VarArray_t* sourceArray, u64 destIndex, u64 sourceIndex = 0, u64 sourceCount = UINT64_MAX)
{
	NotNull(destArray);
	NotNull(sourceArray);
	AssertMsg(destArray->itemSize == sourceArray->itemSize, "Incompatible arrays passed to VarArrayAddVarArray. Make sure the two arrays are the same type!");
	Assert(destIndex <= destArray->length);
	Assert(sourceIndex <= sourceArray->length);
	Assert(destArray != sourceArray);
	if (sourceCount == UINT64_MAX) { sourceCount = sourceArray->length; }
	Assert(sourceIndex+sourceCount <= sourceArray->length);
	if (sourceCount == 0) { return; }
	
	void* newSpace = VarArrayAddRange_(destArray, destIndex, sourceCount, destArray->itemSize);
	NotNull(newSpace);
	MyMemCopy(newSpace, VarArrayGet_(sourceArray, sourceIndex, sourceArray->itemSize, true), sourceCount * destArray->itemSize);
}

// +--------------------------------------------------------------+
// |                         RemoveRange                          |
// +--------------------------------------------------------------+
void VarArrayRemoveRange_(VarArray_t* array, u64 index, u64 numItemsToRemove, u64 itemSize)
{
	NotNull(array);
	NotNull(array->allocArena);
	Assert(array->itemSize > 0);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to VarArrayRemoveRange. Make sure you're accessing the VarArray with the correct type!");
	if (numItemsToRemove == 0) { return; }
	if (numItemsToRemove == 1) { VarArrayRemove_(array, index, itemSize); return; }
	
	if (index >= array->length || index + numItemsToRemove > array->length)
	{
		GyLibPrintLine_E("Tried to remove items [%llu-%llu]/%llu in VarArray %s of itemSize %llu", index, index + (numItemsToRemove-1), array->length, (array->name.pntr != nullptr) ? array->name.pntr : "[unnamed]", array->itemSize);
		AssertMsg(false, "VarArrayRemoveRange out of bounds!");
		return;
	}
	
	//copy all items above the removed range down by numItemsToRemove
	for (u64 iIndex = index+numItemsToRemove; iIndex < array->length; iIndex++)
	{
		MyMemCopy(VarArrayGet_(array, iIndex-numItemsToRemove, itemSize, true), VarArrayGet_(array, iIndex, itemSize, true), array->itemSize);
	}
	array->length -= numItemsToRemove;
}

// +--------------------------------------------------------------+
// |                             Copy                             |
// +--------------------------------------------------------------+
void VarArrayCopy(VarArray_t* destArray, const VarArray_t* sourceArray, MemArena_t* memArena)
{
	NotNull(memArena);
	NotNull(destArray);
	NotNull(sourceArray);
	Assert(destArray != sourceArray);
	CreateVarArray(destArray, memArena, sourceArray->itemSize, sourceArray->length, sourceArray->exponentialChunkSize, sourceArray->allocChunkSize);
	if (sourceArray->length > 0)
	{
		VarArrayMerge(destArray, sourceArray);
	}
}

// +--------------------------------------------------------------+
// |                             Move                             |
// +--------------------------------------------------------------+
void* VarArrayMove(VarArray_t* array, u64 fromIndex, u64 toIndex, bool swapWithTarget = true)
{
	NotNull(array);
	NotNull(array->items);
	Assert(fromIndex < array->length);
	Assert(toIndex < array->length);
	if (fromIndex == toIndex) { return VarArrayGet_(array, fromIndex, array->itemSize, true); } //no work to do
	
	VarArrayExpand(array, array->length+1);
	Assert(array->allocLength > array->length);
	void* tempSpace = (void*)(((u8*)array->items) + (array->length * array->itemSize));
	
	void* movingItem = VarArrayGet_(array, fromIndex, array->itemSize, true);
	void* replaceItem = VarArrayGet_(array, toIndex, array->itemSize, true);
	if (swapWithTarget)
	{
		MyMemCopy(tempSpace, movingItem, array->itemSize);
		MyMemCopy(movingItem, replaceItem, array->itemSize);
		MyMemCopy(replaceItem, tempSpace, array->itemSize);
	}
	else
	{
		MyMemCopy(tempSpace, movingItem, array->itemSize);
		if (toIndex > fromIndex)
		{
			//shift things in-between down by one
			for (u64 index = fromIndex; index < toIndex; index++)
			{
				void* thisItem = VarArrayGet_(array, index, array->itemSize, true);
				void* nextItem = VarArrayGet_(array, index+1, array->itemSize, true);
				MyMemCopy(thisItem, nextItem, array->itemSize);
			}
		}
		else
		{
			//shift things in-between up by one
			for (u64 index = fromIndex; index > toIndex; index--)
			{
				void* thisItem = VarArrayGet_(array, index, array->itemSize, true);
				void* prevItem = VarArrayGet_(array, index-1, array->itemSize, true);
				MyMemCopy(thisItem, prevItem, array->itemSize);
			}
		}
		MyMemCopy(replaceItem, tempSpace, array->itemSize);
	}
	
	return replaceItem;
}

// +--------------------------------------------------------------+
// |                           Reverse                            |
// +--------------------------------------------------------------+
//endIndex is non-inclusive
void VarArrayReverse(VarArray_t* array, u64 startIndex, u64 endIndex)
{
	Assert(startIndex <= endIndex);
	Assert(startIndex <= array->length);
	Assert(endIndex <= array->length);
	if (startIndex == endIndex) { return; }
	Assert(array->itemSize > 0);
	#if ORCA_COMPILATION || PLAYDATE_COMPILATION
	u8 tempSpace[128];
	Assert(array->itemSize < sizeof(tempSpace));
	void* workingSpace = &tempSpace[0];
	#else
	void* workingSpace = alloca(array->itemSize);
	#endif
	for (u64 iIndex = 0; iIndex < (endIndex - startIndex)/2; iIndex++)
	{
		void* item1 = ((u8*)array->items) + (array->itemSize * (startIndex + iIndex));
		void* item2 = ((u8*)array->items) + (array->itemSize * (endIndex-1 - iIndex));
		MyMemCopy(workingSpace, item1, array->itemSize);
		MyMemCopy(item1, item2, array->itemSize);
		MyMemCopy(item2, workingSpace, array->itemSize);
	}
}
void VarArrayReverse(VarArray_t* array)
{
	VarArrayReverse(array, 0, array->length);
}

// +--------------------------------------------------------------+
// |                           Sorting                            |
// +--------------------------------------------------------------+
#if defined(_GY_SORTING_H)
void VarArraySort(VarArray_t* array, CompareFunc_f* compareFunc, void* contextPntr)
{
	NotNull(array);
	NotNull(compareFunc);
	Assert(array->itemSize > 0);
	if (array->length == 0) { return; } // no sorting to do on an empty array
	
	#if ORCA_COMPILATION || PLAYDATE_COMPILATION
	u8 sortingSpace[128];
	Assert(array->itemSize*2 <= ArrayCount(sortingSpace));
	void* workingSpace = &sortingSpace[0];
	#else
	void* workingSpace = alloca(array->itemSize*2);
	#endif
	
	NotNull(workingSpace);
	QuickSort(array->items, array->length, array->itemSize, workingSpace, compareFunc, contextPntr);
}
#endif

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_VARIABLE_ARRAY_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
VarArray_t
@Functions
void FreeVarArray(VarArray_t* array)
void CreateVarArray(VarArray_t* array, MemArena_t* memArena, u64 itemSize, u64 initialRequiredCapacity = 0, bool exponentialChunkSize = true, u64 allocChunkSize = 8)
void VarArrayName(VarArray_t* array, MyStr_t newName)
bool VarArrayExpand(VarArray_t* array, u64 capacityRequired)
#define VarArrayContains(array, itemPntr)
#define VarArrayContainsTyped(array, itemPntr, type)
#define VarArrayGetIndexOf(array, itemPntr, indexOutPntr, type)
void VarArrayClear(VarArray_t* array, bool deallocate = false)
#define VarArrayGetHard(array, index, type)
#define VarArrayGetSoft(array, index, type)
#define VarArrayGet(array, index, type)
#define VarArrayGetFirstHard(array, type)
#define VarArrayGetFirstSoft(array, type)
#define VarArrayGetFirst(array, type)
#define VarArrayGetLastHard(array, type)
#define VarArrayGetLastSoft(array, type)
#define VarArrayGetLast(array, type)
#define VarArrayLoop(arrayPntr, indexName)
#define VarArrayLoopGet(type, varName, arrayPntr, index)
#define VarArrayAdd(array, type)
#define VarArrayPush(array, type)
#define VarArrayInsert(array, index, type)
#define VarArrayPushFront(array, type)
#define VarArrayRemove(array, index, type)
#define VarArrayPop(array, type)
#define VarArrayPopFront(array, type)
#define VarArrayRemoveByPntr(array, itemToRemove)
#define VarArrayRemoveByPntrTyped(array, itemToRemove, type)
#define VarArrayAddRange(array, index, newItemsCount, type)
void VarArrayAddVarArray(VarArray_t* destArray, const VarArray_t* sourceArray, u64 destIndex, u64 sourceIndex = 0, u64 sourceCount = UINT64_MAX)
#define VarArrayMerge(destArray, sourceArray)
#define VarArrayRemoveRange(array, index, numItemsToRemove, type)
void VarArrayCopy(VarArray_t* destArray, const VarArray_t* sourceArray, MemArena_t* memArena)
void* VarArrayMove(VarArray_t* array, u64 fromIndex, u64 toIndex, bool swapWithTarget = true)
void VarArrayReverse(VarArray_t* array, u64 startIndex = 0, u64 endIndex = 0)
void VarArraySort(VarArray_t* array, CompareFunc_f* compareFunc, void* contextPntr)
*/
