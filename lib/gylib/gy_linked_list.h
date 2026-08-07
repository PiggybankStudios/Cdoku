/*
File:   gy_linked_list.h
Author: Taylor Robbins
Date:   09\25\2021
Description:
	** Contains the definnition and implementation of LinkedList_t, IntLinkedList_t, IntDblLinkedList_t
	
	LinkedList_t works by allocating LinkedListNode_t immediately before each allocation that holds
	the link pointers (next and previous).
	
	InLinkedList_t and InDblLinkedList_t (In = Intrusive) work by assuming the beginning of the
	allocated items contain 1 or 2 pointers (single or double "Dbl" linked list)
*/

#ifndef _GY_LINKED_LIST_H
#define _GY_LINKED_LIST_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct LinkedListNode_t
{
	LinkedListNode_t* next;
	LinkedListNode_t* prev;
};

struct LinkedList_t
{
	MemArena_t* allocArena;
	u64 count;
	u64 maxCount;
	u64 itemSize;
	LinkedListNode_t* firstItem;
	LinkedListNode_t* lastItem;
};

struct InLinkedList_t
{
	MemArena_t* allocArena;
	u64 count;
	u64 maxCount;
	u64 itemSize;
	void* firstItem;
	void* lastItem;
};
struct InDblLinkedList_t
{
	MemArena_t* allocArena;
	u64 count;
	u64 maxCount;
	u64 itemSize;
	void* firstItem;
	void* lastItem;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define CreateLinkedList(list, memArena, type) CreateLinkedList_((list), (memArena), sizeof(type))
#define LinkedListFirst(list, type)            (type*)LinkedListFirst_((list), sizeof(type))
#define LinkedListLast(list, type)             (type*)LinkedListLast_((list), sizeof(type))
#define LinkedListNext(list, type, itemPntr)   (type*)LinkedListNext_((list), sizeof(type), (itemPntr))
#define LinkedListPrev(list, type, itemPntr)   (type*)LinkedListPrev_((list), sizeof(type), (itemPntr))
#define LinkedListAdd(list, type)              (type*)LinkedListAdd_((list), sizeof(type))
#define LinkedListRemove(list, type, itemPntr) LinkedListRemove_((list), sizeof(type), (itemPntr), false)
#define LinkedListUnlist(list, type, itemPntr) LinkedListRemove_((list), sizeof(type), (itemPntr), true)
#define LinkedListGet(list, type, index)       (type*)LinkedListGet_((list), sizeof(type), (index))
#define LinkedListInsert(list, type, index)    (type*)LinkedListInsert_((list), sizeof(type), (index))
#define LinkedListClear(list, type)            LinkedListClear_((list), sizeof(type))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void CreateLinkedList_(LinkedList_t* list, MemArena_t* memArena, u64 itemSize);
	void CreateLinkedList_(InLinkedList_t* list, MemArena_t* memArena, u64 itemSize);
	void CreateLinkedList_(InDblLinkedList_t* list, MemArena_t* memArena, u64 itemSize);
	void FreeLinkedList(LinkedList_t* list);
	void FreeLinkedList(InLinkedList_t* list);
	void FreeLinkedList(InDblLinkedList_t* list);
	bool IsItemInLinkedList(const LinkedList_t* list, const void* itemPntr, u64* indexOut = nullptr);
	u64 GetItemIndexInLinkedList(const LinkedList_t* list, const void* itemPntr);
	void* LinkedListFirst_(LinkedList_t* list, u64 itemSize);
	const void* LinkedListFirst_(const LinkedList_t* list, u64 itemSize);
	void* LinkedListLast_(LinkedList_t* list, u64 itemSize);
	const void* LinkedListLast_(const LinkedList_t* list, u64 itemSize);
	void* LinkedListNext_(LinkedList_t* list, u64 itemSize, void* itemPntr);
	const void* LinkedListNext_(const LinkedList_t* list, u64 itemSize, const void* itemPntr);
	void* LinkedListPrev_(LinkedList_t* list, u64 itemSize, void* itemPntr);
	const void* LinkedListPrev_(const LinkedList_t* list, u64 itemSize, const void* itemPntr);
	void* LinkedListAdd_(LinkedList_t* list, u64 itemSize);
	void LinkedListRemove_(LinkedList_t* list, u64 itemSize, void* itemPntr, bool skipDealloc = false);
	void* LinkedListGet_(LinkedList_t* list, u64 itemSize, u64 index);
	const void* LinkedListGet_(const LinkedList_t* list, u64 itemSize, u64 index);
	void* LinkedListInsert_(LinkedList_t* list, u64 itemSize, u64 index);
	void LinkedListClear_(LinkedList_t* list, u64 itemSize);
#else

// +--------------------------------------------------------------+
// |                      Create and Destroy                      |
// +--------------------------------------------------------------+
void CreateLinkedList_(LinkedList_t* list, MemArena_t* memArena, u64 itemSize)
{
	NotNull(list);
	NotNull(memArena);
	Assert(itemSize > 0);
	
	ClearPointer(list);
	list->allocArena = memArena;
	list->count = 0;
	list->maxCount = 0;
	list->itemSize = itemSize;
	list->firstItem = nullptr;
	list->lastItem = nullptr;
}
void CreateLinkedList_(InLinkedList_t* list, MemArena_t* memArena, u64 itemSize)
{
	NotNull(list);
	NotNull(memArena);
	Assert(itemSize > 0);
	
	ClearPointer(list);
	list->allocArena = memArena;
	list->count = 0;
	list->maxCount = 0;
	list->itemSize = itemSize;
	list->firstItem = nullptr;
	list->lastItem = nullptr;
}
void CreateLinkedList_(InDblLinkedList_t* list, MemArena_t* memArena, u64 itemSize)
{
	NotNull(list);
	NotNull(memArena);
	Assert(itemSize > 0);
	
	ClearPointer(list);
	list->allocArena = memArena;
	list->count = 0;
	list->maxCount = 0;
	list->itemSize = itemSize;
	list->firstItem = nullptr;
	list->lastItem = nullptr;
}

void FreeLinkedList(LinkedList_t* list)
{
	NotNull(list);
	Assert(list->firstItem != nullptr || list->count == 0);
	Assert(list->lastItem != nullptr || list->count == 0);
	if (list->count > 0)
	{
		NotNull(list->allocArena);
		LinkedListNode_t* node = list->firstItem;
		while (node != nullptr)
		{
			LinkedListNode_t* nextNode = node->next;
			FreeMem(list->allocArena, node, sizeof(LinkedListNode_t) + list->itemSize);
			node = nextNode;
		}
	}
	ClearPointer(list);
}
void FreeLinkedList(InLinkedList_t* list)
{
	NotNull(list);
	Assert(list->firstItem != nullptr || list->count == 0);
	Assert(list->lastItem != nullptr || list->count == 0);
	if (list->count > 0)
	{
		NotNull(list->allocArena);
		void* itemPntr = list->firstItem;
		while (itemPntr != nullptr)
		{
			void* nextItem = *((void**)itemPntr);
			FreeMem(list->allocArena, itemPntr, list->itemSize);
			itemPntr = nextItem;
		}
	}
	ClearPointer(list);
}
void FreeLinkedList(InDblLinkedList_t* list)
{
	NotNull(list);
	Assert(list->firstItem != nullptr || list->count == 0);
	Assert(list->lastItem != nullptr || list->count == 0);
	if (list->count > 0)
	{
		NotNull(list->allocArena);
		void* itemPntr = list->firstItem;
		while (itemPntr != nullptr)
		{
			void* nextItem = *((void**)itemPntr);
			FreeMem(list->allocArena, itemPntr, list->itemSize);
			itemPntr = nextItem;
		}
	}
	ClearPointer(list);
}

bool IsItemInLinkedList(const LinkedList_t* list, const void* itemPntr, u64* indexOut = nullptr)
{
	NotNull(list);
	NotNull(itemPntr);
	if (list->count == 0) { return false; }
	NotNull(list->firstItem);
	LinkedListNode_t* node = list->firstItem;
	u64 itemIndex = 0;
	while (node != nullptr)
	{
		if ((void*)(((u8*)node) + sizeof(LinkedListNode_t)) == itemPntr)
		{
			if (indexOut != nullptr) { *indexOut = itemIndex; }
			return true;
		}
		node = node->next;
		itemIndex++;
	}
	return false;
}
//TODO: Implement intrusive variants
u64 GetItemIndexInLinkedList(const LinkedList_t* list, const void* itemPntr)
{
	u64 itemIndex = 0;
	bool foundItem = IsItemInLinkedList(list, itemPntr, &itemIndex);
	Assert(foundItem);
	return itemIndex;
}
//TODO: Implement intrusive variants

void* LinkedListFirst_(LinkedList_t* list, u64 itemSize)
{
	NotNull(list);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	if (list->firstItem == nullptr) { Assert(list->count == 0); return nullptr; }
	return (void*)(((u8*)list->firstItem) + sizeof(LinkedListNode_t));
}
const void* LinkedListFirst_(const LinkedList_t* list, u64 itemSize)
{
	return (const void*)LinkedListFirst_((LinkedList_t*)list, itemSize);
}
//TODO: Implement intrusive variants

void* LinkedListLast_(LinkedList_t* list, u64 itemSize)
{
	NotNull(list);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	if (list->lastItem == nullptr) { Assert(list->count == 0); return nullptr; }
	return (void*)(((u8*)list->lastItem) + sizeof(LinkedListNode_t));
}
const void* LinkedListLast_(const LinkedList_t* list, u64 itemSize)
{
	return (const void*)LinkedListFirst_((LinkedList_t*)list, itemSize);
}
//TODO: Implement intrusive variants

void* LinkedListNext_(LinkedList_t* list, u64 itemSize, void* itemPntr)
{
	NotNull(list);
	NotNull(itemPntr);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	DebugAssert(IsItemInLinkedList(list, itemPntr));
	LinkedListNode_t* nodePntr = (LinkedListNode_t*)(((u8*)itemPntr) - sizeof(LinkedListNode_t));
	LinkedListNode_t* nextNode = nodePntr->next;
	if (nextNode == nullptr) { return nullptr; }
	return (void*)(((u8*)nextNode) + sizeof(LinkedListNode_t));
}
const void* LinkedListNext_(const LinkedList_t* list, u64 itemSize, const void* itemPntr)
{
	return (const void*)LinkedListNext_((LinkedList_t*)list, itemSize, (void*)itemPntr);
}
//TODO: Implement intrusive variants

void* LinkedListPrev_(LinkedList_t* list, u64 itemSize, void* itemPntr)
{
	NotNull(list);
	NotNull(itemPntr);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	DebugAssert(IsItemInLinkedList(list, itemPntr));
	LinkedListNode_t* nodePntr = (LinkedListNode_t*)(((u8*)itemPntr) - sizeof(LinkedListNode_t));
	LinkedListNode_t* prevNode = nodePntr->prev;
	if (prevNode == nullptr) { return nullptr; }
	return (void*)(((u8*)prevNode) + sizeof(LinkedListNode_t));
}
const void* LinkedListPrev_(const LinkedList_t* list, u64 itemSize, const void* itemPntr)
{
	return (const void*)LinkedListPrev_((LinkedList_t*)list, itemSize, (void*)itemPntr);
}
//TODO: Implement intrusive variants

void* LinkedListAdd_(LinkedList_t* list, u64 itemSize)
{
	NotNull(list);
	NotNull(list->allocArena);
	Assert(list->itemSize > 0);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	
	if (list->maxCount > 0 && list->count >= list->maxCount)
	{
		return nullptr;
	}
	
	u8* newItemSpace = AllocBytes(list->allocArena, sizeof(LinkedListNode_t) + list->itemSize);
	if (newItemSpace == nullptr) { return nullptr; }
	LinkedListNode_t* newNode = (LinkedListNode_t*)newItemSpace;
	void* result = (void*)(newItemSpace + sizeof(LinkedListNode_t));
	newNode->next = nullptr;
	
	list->count++;
	if (list->firstItem == nullptr)
	{
		Assert(list->lastItem == nullptr);
		list->firstItem = newNode;
		list->lastItem = newNode;
		newNode->prev = nullptr;
	}
	else
	{
		Assert(list->lastItem != nullptr && list->lastItem->next == nullptr);
		list->lastItem->next = newNode;
		newNode->prev = list->lastItem;
		list->lastItem = newNode;
	}
	
	return result;
}
//TODO: Implement intrusive variants

void LinkedListRemove_(LinkedList_t* list, u64 itemSize, void* itemPntr, bool skipDealloc = false)
{
	NotNull(list);
	NotNull(list->allocArena);
	NotNull(itemPntr);
	Assert(list->itemSize > 0);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	Assert(list->count > 0);
	DebugAssert(IsItemInLinkedList(list, itemPntr));
	
	LinkedListNode_t* nodePntr = (LinkedListNode_t*)(((u8*)itemPntr) - sizeof(LinkedListNode_t));
	if (list->lastItem == nodePntr)
	{
		list->lastItem = nodePntr->prev;
	}
	if (list->firstItem == nodePntr)
	{
		list->firstItem = nodePntr->next;
	}
	if (nodePntr->prev != nullptr)
	{
		nodePntr->prev->next = nodePntr->next;
	}
	if (nodePntr->next != nullptr)
	{
		nodePntr->next->prev = nodePntr->prev;
	}
	list->count--;
	
	if (!skipDealloc)
	{
		FreeMem(list->allocArena, nodePntr, sizeof(LinkedListNode_t) + list->itemSize);
	}
}
//TODO: Implement intrusive variants

void* LinkedListGet_(LinkedList_t* list, u64 itemSize, u64 index)
{
	NotNull(list);
	Assert(list->itemSize > 0);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	Assert(index < list->count);
	
	LinkedListNode_t* nodePntr = nullptr;
	if (index > list->count/2)
	{
		nodePntr = list->lastItem;
		for (u64 itemIndex = list->count-1; itemIndex > index; itemIndex--)
		{
			NotNull(nodePntr);
			nodePntr = nodePntr->prev;
		}
	}
	else
	{
		nodePntr = list->firstItem;
		for (u64 itemIndex = 0; itemIndex < index; itemIndex++)
		{
			NotNull(nodePntr);
			nodePntr = nodePntr->next;
		}
	}
	NotNull(nodePntr);
	
	return (void*)(((u8*)nodePntr) + sizeof(LinkedListNode_t));
}
const void* LinkedListGet_(const LinkedList_t* list, u64 itemSize, u64 index)
{
	return (const void*)LinkedListGet_((LinkedList_t*)list, itemSize, index);
}
//TODO: Implement intrusive variants

void* LinkedListInsert_(LinkedList_t* list, u64 itemSize, u64 index)
{
	NotNull(list);
	NotNull(list->allocArena);
	Assert(list->itemSize > 0);
	AssertMsg(itemSize == list->itemSize, "Tried to access LinkedList with wrong type");
	Assert(index <= list->count);
	
	if (list->maxCount > 0 && list->count >= list->maxCount)
	{
		return nullptr;
	}
	
	LinkedListNode_t* prevPntr = nullptr;
	LinkedListNode_t* nextPntr = nullptr;
	LinkedListNode_t** prevNextPntr = nullptr;
	LinkedListNode_t** nextPrevPntr = nullptr;
	if (index < list->count)
	{
		void* existingItem = LinkedListGet_(list, itemSize, index);
		NotNull(existingItem);
		LinkedListNode_t* existingNode = (LinkedListNode_t*)(((u8*)existingItem) - sizeof(LinkedListNode_t));
		nextPntr = existingNode;
		if (existingNode->prev != nullptr)
		{
			prevPntr = existingNode->prev;
			prevNextPntr = &existingNode->prev->next;
		}
		else
		{
			Assert(index == 0);
			prevPntr = nullptr;
			prevNextPntr = &list->firstItem;
		}
		nextPrevPntr = &existingNode->prev;
	}
	else
	{
		Assert(index == list->count-1);
		prevPntr = list->lastItem;
		nextPntr = nullptr;
		if (list->lastItem != nullptr)
		{
			prevNextPntr = &list->lastItem->next;
		}
		else
		{
			Assert(index == 0);
			prevNextPntr = &list->firstItem;
		}
		nextPrevPntr = &list->lastItem;
	}
	NotNull2(prevNextPntr, nextPrevPntr);
	
	u8* newItemSpace = AllocBytes(list->allocArena, sizeof(LinkedListNode_t) + list->itemSize);
	if (newItemSpace == nullptr) { return nullptr; }
	LinkedListNode_t* newNode = (LinkedListNode_t*)newItemSpace;
	void* result = (void*)(newItemSpace + sizeof(LinkedListNode_t));
	
	newNode->next = nextPntr;
	newNode->prev = prevPntr;
	*prevNextPntr = newNode;
	*nextPrevPntr = newNode;
	list->count++;
	
	return result;
}
//TODO: Implement intrusive variants

void LinkedListClear_(LinkedList_t* list, u64 itemSize)
{
	NotNull(list);
	NotNull(list->allocArena);
	UNUSED(itemSize); //potentially can be unused by FreeMem
	LinkedListNode_t* nodePntr = list->firstItem;
	while (nodePntr != nullptr)
	{
		FreeMem(list->allocArena, nodePntr, sizeof(LinkedListNode_t) + list->itemSize);
		nodePntr = nodePntr->next;
	}
	list->count = 0;
	list->firstItem = nullptr;
	list->lastItem = nullptr;
}
//TODO: Implement intrusive variants

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_LINKED_LIST_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
LinkedListNode_t
LinkedList_t
InLinkedList_t
InDblLinkedList_t
@Functions
#define CreateLinkedList(list, memArena, type)
void FreeLinkedList(LinkedList_t* list)
bool IsItemInLinkedList(const LinkedList_t* list, const void* itemPntr, u64* indexOut = nullptr)
u64 GetItemIndexInLinkedList(const LinkedList_t* list, const void* itemPntr)
#define LinkedListFirst(list, type)
#define LinkedListLast(list, type)
#define LinkedListNext(list, type, itemPntr)
#define LinkedListPrev(list, type, itemPntr)
#define LinkedListAdd(list, type)
#define LinkedListRemove(list, type, itemPntr)
#define LinkedListUnlist(list, type, itemPntr)
#define LinkedListGet(list, type, index)
#define LinkedListInsert(list, type, index)
#define LinkedListClear(list, type)
*/
