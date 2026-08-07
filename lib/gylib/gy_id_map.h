/*
File:   gy_id_map.h
Author: Taylor Robbins
Date:   03\18\2024
Description:
	** An IdMap_t is a particular kind of Hash map that relates a u64 id to a
	** particular pointer. This is a common thing we do in games where items in
	** a list are each given an ID to use as a unique identifier, and later
	** (often many times a frame) we want to find the particular item using the ID
	** and we expect that ID -> item lookup to be relatively fast.
	** The IdMap_t also comes with a "nextId" field which can track the next
	** available u64 that is guaranteed not to conflict with any existing items.
*/

#ifndef _GY_ID_MAP_H
#define _GY_ID_MAP_H

struct IdMap_t
{
	MemArena_t* allocArena;
	u64 nextId;
	u64 count;
	u64 numSlots;
	void** slots;
};

//TODO: Implement this!

#endif //  _GY_ID_MAP_H
