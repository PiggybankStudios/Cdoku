/*
File:   gy_bucket_array.h
Author: Taylor Robbins
Date:   01\08\2022
Description:
	** Holds functions that help us manage a BktArray_t. A type of stretchy
	** buffer that allocates "buckets" of memory at a time whenever it runs out of space
	** Unlike a VarArray_t, a bucket array's items will not randomly move address
	** when allocations to the array cause a memory alloc. This is useful for cases
	** where people want to hold pointers to items in the array while adding items.
	** Pointers still should not be held when removing items from the middle of the array.
	** If you want to be able to remove items while holding pointers, you should
	** have flags on your items and rather than actually remove them from the array, you
	** just clear the flag and reuse the item later.
*/

//TODO: Do we want to add a name to our BktArray_t structure? VarArray_t has it.
//TODO: Implement BktArrayInsert, BktArrayInsertBulk, and BktArrayRemoveRange
//TODO: These functions need to be tested after the move from mylib

#ifndef _GY_BUCKET_ARRAY_H
#define _GY_BUCKET_ARRAY_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                  Structure/Type Definitions                  |
// +--------------------------------------------------------------+
struct BktArrayBucket_t
{
	BktArrayBucket_t* next;
	u64 numItems;
	u64 numItemsAlloc;
	void* items;
};

struct BktArray_t
{
	MemArena_t* allocArena;
	u64 itemSize;
	u64 minBucketSize;
	
	u64 length; //total numItems in the buckets
	u64 numUselessSpaces;
	
	u64 numBuckets;
	u64 lastUsedBucket;
	BktArrayBucket_t* firstBucket;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define BktArrayGetSoft(array, type, index) ((type*)BktArrayGet_((array), sizeof(type), (index), false))
#define BktArrayGetHard(array, type, index) ((type*)BktArrayGet_((array), sizeof(type), (index), true))
#define BktArrayGet(array, type, index) BktArrayGetHard((array), type, (index));

#define BktArrayAdd(array, type) ((type*)BktArrayAdd_((array), sizeof(type)))

#define BktArrayAddBulk(array, type, numItems, mustBeConsecutive) ((type*)BktArrayAddBulk_((array), sizeof(type), (numItems), (mustBeConsecutive)))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeBktArray(BktArray_t* array);
	void CreateBktArray(BktArray_t* array, MemArena_t* memArena, u64 itemSize, u64 minBucketSize = 64, u64 initialSizeRequirement = 0);
	BktArrayBucket_t* BktArrayGetLastUsedBucket(BktArray_t* array);
	void BktArrayRefreshLastUsedBucket(BktArray_t* array);
	void BktArrayMoveEmptyBucketToEnd(BktArray_t* array, u64 bucketIndex);
	void BktArrayExpand(BktArray_t* array, u64 numNewItemsRequired);
	bool IsPntrInBktArray(const BktArray_t* array, const void* item, u64* indexOut = nullptr, bool lenient = false);
	void BktArrayClear(BktArray_t* array, bool reduceToSingleBucket = false);
	void* BktArrayGet_(BktArray_t* array, u64 itemSize, u64 index, bool assertOnFailure);
	const void* BktArrayGet_(const BktArray_t* array, u64 itemSize, u64 index, bool assertOnFailure);
	void* BktArrayAdd_(BktArray_t* array, u64 itemSize);
	void* BktArrayAddBulk_(BktArray_t* array, u64 itemSize, u64 numItems, bool mustBeConsecutive);
	void BktArrayRemoveAt(BktArray_t* array, u64 index);
	void BktArrayRemoveLast(BktArray_t* array);
	void BktArraySolidify(BktArray_t* array, bool deallocateEmptyBuckets = false, bool singleBucket = false);
	#if defined(_GY_SORTING_H) && !ORCA_COMPILATION
	void BktArraySort(BktArray_t* array, CompareFunc_f* compareFunc, void* contextPntr);
	#endif
#else

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeBktArray(BktArray_t* array)
{
	NotNull(array);
	BktArrayBucket_t* bucket = array->firstBucket;
	for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
	{
		NotNull(bucket);
		BktArrayBucket_t* nextBucket = bucket->next;
		FreeMem(array->allocArena, bucket, sizeof(BktArrayBucket_t) + (bucket->numItemsAlloc * array->itemSize));
		bucket = nextBucket;
	}
	ClearPointer(array);
}

void CreateBktArray(BktArray_t* array, MemArena_t* memArena, u64 itemSize, u64 minBucketSize = 64, u64 initialSizeRequirement = 0)
{
	NotNull(array);
	NotNull(memArena);
	Assert(itemSize > 0);
	Assert(minBucketSize > 0);
	
	ClearPointer(array);
	array->allocArena = memArena;
	array->itemSize = itemSize;
	array->minBucketSize = minBucketSize;
	array->length = 0;
	array->numUselessSpaces = 0;
	
	u64 firstBucketSize = minBucketSize;
	if (firstBucketSize < initialSizeRequirement) { firstBucketSize = initialSizeRequirement; }
	array->firstBucket = (BktArrayBucket_t*)AllocMem(memArena, sizeof(BktArrayBucket_t) + (itemSize * firstBucketSize));
	NotNull(array->firstBucket);
	ClearPointer(array->firstBucket);
	array->firstBucket->next = nullptr;
	array->firstBucket->numItems = 0;
	array->firstBucket->numItemsAlloc = firstBucketSize;
	array->firstBucket->items = (void*)(array->firstBucket + 1);
	MyMemSet(array->firstBucket->items, 0x00, itemSize * firstBucketSize);
	array->numBuckets = 1;
	array->lastUsedBucket = 0;
}

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
BktArrayBucket_t* BktArrayGetLastUsedBucket(BktArray_t* array)
{
	NotNull(array);
	if (array->numBuckets == 0) { return nullptr; }
	Assert(array->lastUsedBucket < array->numBuckets);
	BktArrayBucket_t* bucket = array->firstBucket;
	for (u64 bIndex = 0; bIndex < array->lastUsedBucket; bIndex++)
	{
		NotNull(bucket);
		bucket = bucket->next;
	}
	NotNull(bucket);
	return bucket;
}

void BktArrayRefreshLastUsedBucket(BktArray_t* array)
{
	NotNull(array);
	if (array->numBuckets == 0) { array->lastUsedBucket = 0; return; }
	NotNull(array->firstBucket);
	Assert(array->lastUsedBucket < array->numBuckets);
	
	u64 bIndex = 0;
	//Move the lastUsedBucket index down as far as possible
	while (true)
	{
		BktArrayBucket_t* lastBucket = BktArrayGetLastUsedBucket(array);
		NotNull(lastBucket);
		if (bIndex > 0 && lastBucket->numItems < lastBucket->numItemsAlloc)
		{
			u64 numReclaimedSpaces = lastBucket->numItemsAlloc - lastBucket->numItems;
			Assert(array->numUselessSpaces >= numReclaimedSpaces);
			array->numUselessSpaces -= numReclaimedSpaces;
		}
		if (lastBucket->numItems == 0 && array->lastUsedBucket > 0)
		{
			array->lastUsedBucket--;
		}
		else { return; }
		bIndex++;
		Assert(bIndex <= array->numBuckets); //make sure this doesn't become an infinite loop on accident
	}
}

void BktArrayMoveEmptyBucketToEnd(BktArray_t* array, u64 bucketIndex)
{
	NotNull(array);
	Assert(bucketIndex < array->numBuckets);
	NotNull(array->firstBucket);
	if (bucketIndex == array->numBuckets-1) { return; } //already at the end
	if (bucketIndex > array->lastUsedBucket) { return; } //doesn't matter what order the last few empty buckets come in. They are all ready to be used
	Assert(bucketIndex != array->lastUsedBucket);
	BktArrayBucket_t* prevBucket = nullptr;
	BktArrayBucket_t* moveBucket = nullptr;
	BktArrayBucket_t* bucket = array->firstBucket;
	for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
	{
		NotNull(bucket);
		if (bIndex == bucketIndex)
		{
			Assert(bucket->numItems == 0);
			moveBucket = bucket;
			if (prevBucket != nullptr) { prevBucket->next = bucket->next; }
			else { Assert(bIndex == 0); array->firstBucket = bucket->next; }
		}
		if (bIndex == array->numBuckets-1)
		{
			NotNull(moveBucket);
			bucket->next = moveBucket;
			moveBucket->next = nullptr;
			array->lastUsedBucket--;
			Assert(array->numUselessSpaces >= moveBucket->numItemsAlloc);
			array->numUselessSpaces -= moveBucket->numItemsAlloc;
		}
		if (bIndex < bucketIndex) { prevBucket = bucket; }
		bucket = bucket->next;
	}
}

void BktArrayExpand(BktArray_t* array, u64 numNewItemsRequired)
{
	NotNull(array);
	NotNull(array->allocArena);
	if (numNewItemsRequired == 0) { return; }
	BktArrayBucket_t* lastBucket = BktArrayGetLastUsedBucket(array);
	if (lastBucket != nullptr)
	{
		u64 numSpacesAvailable = lastBucket->numItemsAlloc - lastBucket->numItems;
		while (lastBucket->next != nullptr)
		{
			lastBucket = lastBucket->next;
			Assert(lastBucket->numItems == 0);
			numSpacesAvailable += lastBucket->numItemsAlloc - lastBucket->numItems;
		}
		if (numSpacesAvailable < numNewItemsRequired)
		{
			u64 newBucketSize = array->minBucketSize;
			if (newBucketSize < numNewItemsRequired - numSpacesAvailable) { newBucketSize = (numNewItemsRequired - numSpacesAvailable); }
			BktArrayBucket_t* newBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->itemSize * newBucketSize));
			NotNull(newBucket);
			ClearPointer(newBucket);
			newBucket->next = nullptr;
			newBucket->numItems = 0;
			newBucket->numItemsAlloc = newBucketSize;
			newBucket->items = (void*)(newBucket + 1);
			MyMemSet(newBucket->items, 0x00, array->itemSize * newBucketSize);
			array->numBuckets++;
			lastBucket->next = newBucket;
		}
	}
	else
	{
		Assert(array->numBuckets == 0 && array->firstBucket == nullptr);
		u64 firstBucketSize = array->minBucketSize;
		if (firstBucketSize < numNewItemsRequired) { firstBucketSize = numNewItemsRequired; }
		array->firstBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->itemSize * firstBucketSize));
		NotNull(array->firstBucket);
		ClearPointer(array->firstBucket);
		array->firstBucket->next = nullptr;
		array->firstBucket->numItems = 0;
		array->firstBucket->numItemsAlloc = firstBucketSize;
		array->firstBucket->items = (void*)(array->firstBucket + 1);
		MyMemSet(array->firstBucket->items, 0x00, array->itemSize * firstBucketSize);
		array->numBuckets = 1;
		array->lastUsedBucket = 0;
	}
}

//NOTE: Setting lenient to true will make this function return true if the pointer is anywhere within any of the
//      allocated space for the bucket array (disregarding alignment and empty members)
bool IsPntrInBktArray(const BktArray_t* array, const void* item, u64* indexOut = nullptr, bool lenient = false)
{
	BktArrayBucket_t* bucket = array->firstBucket;
	u64 bucketBaseIndex = 0;
	for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
	{
		if ((const u8*)item >= (const u8*)bucket->items && (const u8*)item < ((const u8*)bucket->items) + (array->itemSize * bucket->numItems))
		{
			u64 inBucketOffset = (u64)((const u8*)item - (const u8*)bucket->items);
			if (!lenient && (inBucketOffset % array->itemSize) != 0) { return false; } //the pointer is in the bucket array but isn't pointing to the beginning of an item
			u64 inBucketIndex = (inBucketOffset / array->itemSize);
			if (!lenient && inBucketIndex >= bucket->numItems) { return false; } //The pointer is in the bucket but it's pointing to an empty item
			if (indexOut != nullptr) { *indexOut = bucketBaseIndex + inBucketIndex; }
			return true;
		}
		bucketBaseIndex += bucket->numItems;
		bucket = bucket->next;
	}
	return false;
}

// +--------------------------------------------------------------+
// |                            Clear                             |
// +--------------------------------------------------------------+
void BktArrayClear(BktArray_t* array, bool reduceToSingleBucket = false)
{
	NotNull(array);
	BktArrayBucket_t* bucket = array->firstBucket;
	while (bucket != nullptr)
	{
		bucket->numItems = 0;
		bucket = bucket->next;
	}
	array->length = 0;
	array->numUselessSpaces = 0;
	array->lastUsedBucket = 0;
	if (reduceToSingleBucket)
	{
		NotNull(array->allocArena);
		BktArrayBucket_t* prevBucket = nullptr;
		bucket = array->firstBucket;
		u64 bIndex = 0;
		while (bucket != nullptr)
		{
			BktArrayBucket_t* nextBucket = bucket->next;
			if (bucket->numItems == 0 && bIndex != 0)
			{
				Assert(bIndex > array->lastUsedBucket);
				FreeMem(array->allocArena, bucket, sizeof(BktArrayBucket_t) + (array->itemSize * bucket->numItemsAlloc));
				if (prevBucket != nullptr) { prevBucket->next = nextBucket; }
				else { array->firstBucket = bucket->next; }
				Assert(array->numBuckets > 0);
				array->numBuckets--;
			}
			else { prevBucket = bucket; }
			bucket = nextBucket;
			bIndex++;
		}
		NotNull(array->firstBucket);
		Assert(array->numBuckets > 0);
		Assert(array->lastUsedBucket < array->numBuckets);
	}
}

// +--------------------------------------------------------------+
// |                             Get                              |
// +--------------------------------------------------------------+
void* BktArrayGet_(BktArray_t* array, u64 itemSize, u64 index, bool assertOnFailure)
{
	Assert(array != nullptr);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to BktArrayGet. Make sure you're accessing the BktArray with the correct type!");
	if (index >= array->length)
	{
		if (assertOnFailure)
		{
			GyLibPrintLine_E("Tried to get item %llu/%llu in BktArray of itemSize %llu", index, array->length, array->itemSize);
			AssertMsg(false, "BktArrayGet out of bounds!");
		}
		return nullptr;
	}
	u64 foundIndex = 0;
	BktArrayBucket_t* bucket = array->firstBucket;
	for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
	{
		if (index < foundIndex + bucket->numItems)
		{
			void* result = (void*)(((u8*)bucket->items) + (array->itemSize * (index - foundIndex)));
			return result;
		}
		foundIndex += bucket->numItems;
		bucket = bucket->next;
	}
	AssertMsg(false, "BktArray had mismatch between it's recorded length and actual aggregate length from each bucket");
	return nullptr;
}
const void* BktArrayGet_(const BktArray_t* array, u64 itemSize, u64 index, bool assertOnFailure) //const version
{
	return (const void*)BktArrayGet_((BktArray_t*)array, itemSize, index, assertOnFailure);
}

// +--------------------------------------------------------------+
// |                             Add                              |
// +--------------------------------------------------------------+
void* BktArrayAdd_(BktArray_t* array, u64 itemSize)
{
	NotNull(array);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to BktArrayAdd. Make sure you're accessing the BktArray with the correct type!");
	NotNull(array->allocArena);
	
	BktArrayExpand(array, 1);
	
	BktArrayBucket_t* bucket = BktArrayGetLastUsedBucket(array);
	NotNull(bucket);
	if (bucket->numItems >= bucket->numItemsAlloc)
	{
		bucket = bucket->next;
		array->lastUsedBucket++;
	}
	NotNull(bucket);
	Assert(bucket->numItems < bucket->numItemsAlloc);
	NotNull(bucket->items);
	
	void* result = ((u8*)bucket->items) + (array->itemSize * bucket->numItems);
	bucket->numItems += 1;
	array->length += 1;
	return result;
}

//TODO: Implement BktArrayInsert
#if 0
void* BktArrayInsert_(BktArray_t* array, u64 itemSize, u64 index)
{
	NotNull(array);
	Unimplemented(); //TODO: Implement me!
	return nullptr;
}
#define BktArrayInsert(array, type, index) ((type*)BktArrayInsert_((array), sizeof(type), (index)))
#endif

void* BktArrayAddBulk_(BktArray_t* array, u64 itemSize, u64 numItems, bool mustBeConsecutive)
{
	NotNull(array);
	AssertMsg(array->itemSize == itemSize, "Invalid itemSize passed to BktArrayAddBulk. Make sure you're accessing the BktArray with the correct type!");
	NotNull(array->allocArena);
	if (numItems == 0) { return nullptr; }
	
	if (mustBeConsecutive)
	{
		BktArrayBucket_t* lastBucket = BktArrayGetLastUsedBucket(array);
		if (lastBucket != nullptr)
		{
			//TODO: This is a bit inefficient if numItems > minBucketSize of the array and there are empty buckets at the end of the array
			//      This will skip all those buckets and add a new, larger, one at the end to hold the items thereby adding all the empty
			//      buckets to our "unused space" count. We really should just insert a bucket after the lastUsedBucket.
			BktArrayBucket_t* prevBucket = nullptr;
			while (lastBucket != nullptr)
			{
				u64 numSpacesAvailable = lastBucket->numItemsAlloc - lastBucket->numItems;
				if (numSpacesAvailable >= numItems)
				{
					void* result = ((u8*)lastBucket->items) + (array->itemSize * lastBucket->numItems);
					lastBucket->numItems += numItems;
					array->length += numItems;
					return result;
				}
				else
				{
					array->lastUsedBucket++;
					array->numUselessSpaces += numSpacesAvailable;
					prevBucket = lastBucket;
					lastBucket = lastBucket->next;
				}
			}
			
			Assert(lastBucket == nullptr);
			NotNull(prevBucket);
			Assert(array->lastUsedBucket == array->numBuckets);
			u64 newBucketSize = array->minBucketSize;
			if (newBucketSize < numItems) { newBucketSize = numItems; }
			BktArrayBucket_t* newBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->itemSize * newBucketSize));
			NotNull(newBucket);
			ClearPointer(newBucket);
			prevBucket->next = newBucket;
			newBucket->next = nullptr;
			newBucket->numItems = numItems;
			newBucket->numItemsAlloc = newBucketSize;
			newBucket->items = (void*)(newBucket + 1);
			MyMemSet(newBucket->items, 0x00, array->itemSize * newBucketSize);
			array->numBuckets++;
			array->length += numItems;
			return newBucket->items;
		}
		else
		{
			Assert(array->numBuckets == 0 && array->firstBucket == nullptr);
			u64 firstBucketSize = array->minBucketSize;
			if (firstBucketSize < numItems) { firstBucketSize = numItems; }
			array->firstBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->itemSize * firstBucketSize));
			NotNull(array->firstBucket);
			ClearPointer(array->firstBucket);
			array->firstBucket->next = nullptr;
			array->firstBucket->numItems = numItems;
			array->firstBucket->numItemsAlloc = firstBucketSize;
			array->firstBucket->items = (void*)(array->firstBucket + 1);
			MyMemSet(array->firstBucket->items, 0x00, array->itemSize * firstBucketSize);
			array->numBuckets = 1;
			array->lastUsedBucket = 0;
			array->length = numItems;
			return array->firstBucket->items;
		}
	}
	else
	{
		BktArrayExpand(array, numItems);
		void* result = nullptr;
		for (u64 iIndex = 0; iIndex < numItems; iIndex++)
		{
			void* newItem = BktArrayAdd_(array, itemSize);
			Assert(newItem != nullptr);
			if (result == nullptr) { result = newItem; }
		}
		return result;
	}
}

//TODO: Implement BktArrayInsertBulk
#if 0
void* BktArrayInsertBulk_(BktArray_t* array, u64 itemSize, u64 index, u64 numItems, bool mustBeConsecutive)
{
	if (numItems == 0) { return nullptr; }
	if (index == array->length) { return BktArrayAddBulk_(array, itemSize, numItems, mustBeConsecutive); }
	Unimplemented(); //TODO: Implement me!
	return nullptr;
}
#define BktArrayInsertBulk(array, type, index, numItems, mustBeConsecutive) ((type*)BktArrayInsertBulk_((array), sizeof(type), (index), (numItems), (mustBeConsecutive)))
#endif

// +--------------------------------------------------------------+
// |                            Remove                            |
// +--------------------------------------------------------------+
void BktArrayRemoveAt(BktArray_t* array, u64 index)
{
	NotNull(array);
	Assert(index < array->length);
	Assert(array->numBuckets > 0);
	NotNull(array->firstBucket);
	u64 bucketBaseIndex = 0;
	BktArrayBucket_t* bucket = array->firstBucket;
	BktArrayBucket_t* prevBucket = nullptr;
	for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
	{
		if (index < bucketBaseIndex + bucket->numItems)
		{
			u64 removeIndex = (index - bucketBaseIndex);
			//copy following items down
			for (u64 iIndex = removeIndex; iIndex+1 < bucket->numItems; iIndex++)
			{
				MyMemCopy(((u8*)bucket->items) + (array->itemSize * iIndex), ((u8*)bucket->items) + (array->itemSize * (iIndex+1)), array->itemSize);
			}
			bucket->numItems--;
			array->length--;
			
			if (bIndex < array->lastUsedBucket)
			{
				//this space at the end of the bucket will become useless until the array is solidified or we become the lastUsedBucket
				array->numUselessSpaces++;
				if (bucket->numItems == 0) { BktArrayMoveEmptyBucketToEnd(array, bIndex); }
			}
			else
			{
				Assert(bIndex == array->lastUsedBucket);
				if (bucket->numItems == 0) { BktArrayRefreshLastUsedBucket(array); }
			}
			return;
		}
		bucketBaseIndex += bucket->numItems;
		prevBucket = bucket;
		bucket = bucket->next;
	}
	AssertMsg(false, "BktArray had mismatch between it's recorded length and actual aggregate length from each bucket");
}
void BktArrayRemoveLast(BktArray_t* array)
{
	NotNull(array);
	Assert(array->length > 0);
	return BktArrayRemoveAt(array, array->length-1);
}

//TODO: Implement BktArrayRemoveRange
#if 0
bool BktArrayRemoveRange(BktArray_t* array, u64 index, u64 numItems)
{
	Unimplemented(); //TODO: Implement me!
	return false;
}
#endif

// +--------------------------------------------------------------+
// |                            Other                             |
// +--------------------------------------------------------------+
void BktArraySolidify(BktArray_t* array, bool deallocateEmptyBuckets = false, bool singleBucket = false)
{
	NotNull(array);
	NotNull(array->allocArena);
	if (array->numBuckets == 0) { return; }
	NotNull(array->firstBucket);
	if (singleBucket)
	{
		if (array->length != 0)
		{
			BktArrayBucket_t* newBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->length * array->itemSize));
			NotNull(newBucket);
			ClearPointer(newBucket);
			newBucket->items = (void*)(newBucket + 1);
			newBucket->numItemsAlloc = array->length;
			newBucket->numItems = 0;
			BktArrayBucket_t* bucket = array->firstBucket;
			for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
			{
				for (u64 iIndex = 0; iIndex < bucket->numItems; iIndex++)
				{
					Assert(newBucket->numItems < newBucket->numItemsAlloc);
					MyMemCopy(((u8*)newBucket->items) + (array->itemSize * newBucket->numItems), ((u8*)bucket->items) + (array->itemSize * iIndex), array->itemSize);
					newBucket->numItems++;
				}
				BktArrayBucket_t* nextBucket = bucket->next;
				FreeMem(array->allocArena, bucket, sizeof(BktArrayBucket_t) + (array->itemSize * bucket->numItemsAlloc));
				bucket = nextBucket;
			}
			Assert(newBucket->numItems == newBucket->numItemsAlloc);
			array->numBuckets = 1;
			array->firstBucket = newBucket;
			array->lastUsedBucket = 0;
			array->numUselessSpaces = 0;
		}
		else
		{
			//We kind of need to do something special for 0 item array being solidified into a single bucket.
			//This doesn't really need any memory but we are just going to allocate a normally sized bucket
			//so that the BktArray_t doesn't need to be initialized again in order to keep using it after this
			BktArrayBucket_t* newBucket = (BktArrayBucket_t*)AllocMem(array->allocArena, sizeof(BktArrayBucket_t) + (array->minBucketSize * array->itemSize));
			NotNull(newBucket);
			ClearPointer(newBucket);
			newBucket->items = (void*)(newBucket + 1);
			newBucket->numItemsAlloc = array->minBucketSize;
			newBucket->numItems = 0;
			BktArrayBucket_t* bucket = array->firstBucket;
			for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
			{
				BktArrayBucket_t* nextBucket = bucket->next;
				FreeMem(array->allocArena, bucket, sizeof(BktArrayBucket_t) + (array->itemSize * bucket->numItemsAlloc));
				bucket = nextBucket;
			}
			array->numBuckets = 1;
			array->firstBucket = newBucket;
			array->lastUsedBucket = 0;
			array->numUselessSpaces = 0;
		}
	}
	else
	{
		u64 newLastUsedBucket = 0;
		BktArrayBucket_t* destBucket = array->firstBucket;
		u64 destBucketIndex = 0;
		BktArrayBucket_t* bucket = array->firstBucket;
		for (u64 bIndex = 0; bIndex < array->numBuckets; bIndex++)
		{
			for (u64 iIndex = 0; iIndex < bucket->numItems; iIndex++)
			{
				if (bucket != destBucket || iIndex != destBucketIndex)
				{
					NotNull(destBucket);
					Assert(destBucketIndex < destBucket->numItemsAlloc);
					MyMemCopy(((u8*)destBucket->items) + (array->itemSize * destBucketIndex), ((u8*)bucket->items) + (array->itemSize * iIndex), array->itemSize);
				}
				destBucketIndex++;
				if (destBucketIndex >= destBucket->numItemsAlloc)
				{
					destBucket->numItems = destBucket->numItemsAlloc;
					destBucket = destBucket->next;
					destBucketIndex = 0;
					newLastUsedBucket++;
				}
			}
			bucket = bucket->next;
		}
		if (destBucketIndex == 0 && newLastUsedBucket > 0) { newLastUsedBucket--; } //we didn't actually use that bucket so go back one
		Assert(newLastUsedBucket < array->numBuckets);
		if (destBucket != nullptr)
		{
			Assert(destBucketIndex < destBucket->numItemsAlloc);
			destBucket->numItems = destBucketIndex;
			while (destBucket->next != nullptr)
			{
				destBucket = destBucket->next;
				destBucket->numItems = 0;
			}
		}
		array->numUselessSpaces = 0;
		array->lastUsedBucket = newLastUsedBucket;
		
		if (deallocateEmptyBuckets)
		{
			BktArrayBucket_t* prevBucket = nullptr;
			bucket = array->firstBucket;
			u64 bIndex = 0;
			while (bucket != nullptr)
			{
				BktArrayBucket_t* nextBucket = bucket->next;
				if (bucket->numItems == 0 && bIndex != 0)
				{
					Assert(bIndex > array->lastUsedBucket);
					FreeMem(array->allocArena, bucket, sizeof(BktArrayBucket_t) + (array->itemSize * bucket->numItemsAlloc));
					if (prevBucket != nullptr) { prevBucket->next = nextBucket; }
					else { array->firstBucket = bucket->next; }
					Assert(array->numBuckets > 0);
					array->numBuckets--;
				}
				else { prevBucket = bucket; }
				bucket = nextBucket;
				bIndex++;
			}
			NotNull(array->firstBucket);
			Assert(array->numBuckets > 0);
			Assert(array->lastUsedBucket < array->numBuckets);
		}
	}
}

// +--------------------------------------------------------------+
// |                           Sorting                            |
// +--------------------------------------------------------------+
#if defined(_GY_SORTING_H) && !ORCA_COMPILATION
//TODO: Can we get this to work without solidifying the array?
void BktArraySort(BktArray_t* array, CompareFunc_f* compareFunc, void* contextPntr)
{
	NotNull(array);
	NotNull(compareFunc);
	Assert(array->itemSize > 0);
	void* workingSpace = alloca(array->itemSize*2);
	NotNull(workingSpace);
	BktArraySolidify(array, true, true);
	Assert(array->numBuckets == 1);
	Assert(array->length == 0 || array->length == array->firstBucket->numItems);
	QuickSort(array->firstBucket->items, array->firstBucket->numItems, array->itemSize, workingSpace, compareFunc, contextPntr);
}
#endif

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_BUCKET_ARRAY_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
BktArrayBucket_t
BktArray_t
@Functions
void FreeBktArray(BktArray_t* array)
void CreateBktArray(BktArray_t* array, MemArena_t* memArena, u64 itemSize, u64 minBucketSize = 64, u64 initialSizeRequirement = 0)
BktArrayBucket_t* BktArrayGetLastUsedBucket(BktArray_t* array)
void BktArrayRefreshLastUsedBucket(BktArray_t* array)
void BktArrayMoveEmptyBucketToEnd(BktArray_t* array, u64 bucketIndex)
void BktArrayExpand(BktArray_t* array, u64 numNewItemsRequired)
bool IsPntrInBktArray(const BktArray_t* array, const void* item, u64* indexOut = nullptr, bool lenient = false)
void BktArrayClear(BktArray_t* array, bool reduceToSingleBucket = false)
#define BktArrayGetSoft(array, type, index)
#define BktArrayGetHard(array, type, index)
#define BktArrayGet(array, type, index)
#define BktArrayAdd(array, type)
#define BktArrayAddBulk(array, type, numItems, mustBeConsecutive)
void BktArrayRemoveAt(BktArray_t* array, u64 index)
void BktArrayRemoveLast(BktArray_t* array)
void BktArraySolidify(BktArray_t* array, bool deallocateEmptyBuckets = false, bool singleBucket = false)
void BktArraySort(BktArray_t* array, CompareFunc_f* compareFunc, void* contextPntr)
*/