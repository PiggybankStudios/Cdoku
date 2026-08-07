/*
File:   gy_fifo.h
Author: Taylor Robbins
Date:   01\29\2023
*/

#ifndef _GY_FIFO_H
#define _GY_FIFO_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
#define FIFO_DYNAMIC_CHUNK_SIZE 1024

enum FifoType_t
{
	FifoType_StaticDrop = 0x00,
	FifoType_StaticOverwrite,
	FifoType_Dynamic,
	FifoType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetFifoTypeStr(FifoType_t enumValue);
#else
const char* GetFifoTypeStr(FifoType_t enumValue)
{
	switch (enumValue)
	{
		case FifoType_StaticDrop:      return "StaticDrop";
		case FifoType_StaticOverwrite: return "StaticOverwrite";
		case FifoType_Dynamic:         return "Dynamic";
		default: return "Unknown";
	}
}
#endif

struct Fifo_t
{
	MemArena_t* allocArena;
	FifoType_t type;
	u64 head;
	u64 tail;
	u64 size;
	u8* buffer;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeFifo(Fifo_t* fifo);
	void CreateFifo(Fifo_t* fifo, MemArena_t* memArena, u64 size, FifoType_t type);
	u64 FifoLength(const Fifo_t* fifo);
	u8 FifoGetU8(const Fifo_t* fifo, u64 index = 0);
	u8 FifoGetLastU8(const Fifo_t* fifo);
	bool FifoGetMulti(const Fifo_t* fifo, u64 numBytes, u8* arrayPntr);
	bool FifoPushU8(Fifo_t* fifo, u8 newByte);
	bool FifoPushMulti(Fifo_t* fifo, u64 arraySize, const u8* arrayPntr);
	u8 FifoPopU8(Fifo_t* fifo);
	u8* FifoUnwrap(const Fifo_t* fifo, MemArena_t* memArena, bool addNullTerm = false);
#else

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeFifo(Fifo_t* fifo)
{
	NotNull(fifo);
	if (fifo->buffer != nullptr)
	{
		NotNull(fifo->allocArena);
		FreeMem(fifo->allocArena, fifo->buffer, fifo->size);
	}
	ClearPointer(fifo);
}

void CreateFifo(Fifo_t* fifo, MemArena_t* memArena, u64 size, FifoType_t type)
{
	ClearPointer(fifo);
	fifo->allocArena = memArena;
	fifo->type = type;
	fifo->head = 0;
	fifo->tail = 0;
	
	fifo->size = size;
	if (fifo->size > 0)
	{
		fifo->buffer = AllocArray(memArena, u8, fifo->size);
	}
	else
	{
		fifo->buffer = nullptr;
	}
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
u64 FifoLength(const Fifo_t* fifo)
{
	NotNull(fifo);
	Assert(fifo->head < fifo->size && fifo->tail < fifo->size);
	
	if (fifo->head >= fifo->tail)
	{
		return fifo->head - fifo->tail;
	}
	else
	{
		return (fifo->size - fifo->tail) + fifo->head;
	}
}

u8 FifoGetU8(const Fifo_t* fifo, u64 index = 0)
{
	NotNull(fifo);
	Assert(index < FifoLength(fifo));
	return fifo->buffer[(fifo->tail + index) % fifo->size];
}
u8 FifoGetLastU8(const Fifo_t* fifo)
{
	NotNull(fifo);
	Assert(fifo->head != fifo->tail);
	if (fifo->head == 0) { return fifo->buffer[fifo->size-1]; }
	else { return fifo->buffer[fifo->head-1]; }
}

bool FifoGetMulti(const Fifo_t* fifo, u64 numBytes, u8* arrayPntr)
{
	NotNull(fifo);
	if (numBytes == 0) { return true; }
	NotNull(arrayPntr);
	if (numBytes > FifoLength(fifo)) { return false; }
	for (u64 bIndex = 0; bIndex < numBytes; bIndex++)
	{
		arrayPntr[bIndex] = FifoGetU8(fifo, bIndex);
	}
	return true;
}

// +--------------------------------------------------------------+
// |                    Manipulation Functions                    |
// +--------------------------------------------------------------+
bool FifoPushU8(Fifo_t* fifo, u8 newByte)
{
	u64 newHead = ((fifo->head + 1) % fifo->size);
	if (fifo->size == 0 || newHead == fifo->tail)
	{
		if (fifo->type == FifoType_StaticDrop)
		{
			return false; //no space, drop the new byte
		}
		else if (fifo->type == FifoType_StaticOverwrite)
		{
			fifo->tail = ((fifo->tail + 1) % fifo->size); //move the tail up, dropping an old byte
		}
		else if (fifo->type == FifoType_Dynamic)
		{
			NotNull(fifo->allocArena);
			
			u64 newSpaceSize = fifo->size + FIFO_DYNAMIC_CHUNK_SIZE;
			u8* newSpace = AllocArray(fifo->allocArena, u8, newSpaceSize);
			NotNull(newSpace);
			
			if (fifo->buffer != nullptr && fifo->head != fifo->tail)
			{
				if (fifo->head > fifo->tail)
				{
					u64 fifoLength = fifo->head - fifo->tail;
					MyMemCopy(newSpace, &fifo->buffer[fifo->tail], fifoLength);
					fifo->tail = 0;
					fifo->head = fifoLength;
				}
				else
				{
					u64 upperLength = fifo->size - fifo->tail;
					u64 lowerLength = fifo->head;
					MyMemCopy(&newSpace[0], &fifo->buffer[fifo->tail], upperLength);
					MyMemCopy(&newSpace[upperLength], &fifo->buffer[0], lowerLength);
					fifo->tail = 0;
					fifo->head = upperLength + lowerLength;
				}
			}
			
			if (fifo->buffer != nullptr)
			{
				FreeMem(fifo->allocArena, fifo->buffer, fifo->size);
			}
			fifo->buffer = newSpace;
			fifo->size = newSpaceSize;
			Assert(fifo->tail < fifo->size);
			Assert(fifo->head < fifo->size);
		}
		else
		{
			return false; //fifo type is invalid!
		}
	}
	
	fifo->buffer[fifo->head] = newByte;
	fifo->head = ((fifo->head + 1) % fifo->size);
	return true;
}
bool FifoPushMulti(Fifo_t* fifo, u64 arraySize, const u8* arrayPntr)
{
	NotNull(fifo);
	if (arraySize == 0) { return true; }
	NotNull(arrayPntr);
	bool result = true;
	for (u64 bIndex = 0; bIndex < arraySize; bIndex++)
	{
		if (!FifoPushU8(fifo, arrayPntr[bIndex])) { result = false; }
	}
	return result;
}

u8 FifoPopU8(Fifo_t* fifo)
{
	NotNull(fifo);
	Assert(FifoLength(fifo) > 0);
	u8 result = fifo->buffer[fifo->tail];
	fifo->tail = ((fifo->tail + 1) % fifo->size);
	return result;
}
//TODO: Do we want a FifoPopMulti?

u8* FifoUnwrap(const Fifo_t* fifo, MemArena_t* memArena, bool addNullTerm = false)
{
	NotNull(fifo);
	NotNull(memArena);
	u64 fifoLength = FifoLength(fifo);
	if (fifoLength == 0 && !addNullTerm) { return nullptr; }
	u8* result = AllocArray(memArena, u8, fifoLength + (addNullTerm ? 1 : 0));
	for (u64 cIndex = 0; cIndex < fifoLength; cIndex++)
	{
		u8 nextChar = FifoGetU8(fifo, cIndex);
		result[cIndex] = nextChar;
	}
	if (addNullTerm)
	{
		result[fifoLength] = '\0';
	}
	return result;
}

//TODO: We should re-evaluate the usefulness of these functions. At the very least they need to get a once over to bring them up to Pig Engine standards
#if 0
bool FifoPeekCommand8(const Fifo_t* fifo, u8 attnChar = 0x7E, u8* payloadLengthOut = nullptr)
{
	NotNull(fifo);
	
	bool foundAttnChar = false;
	u64 cmdIndex = 0;
	u8 payloadLength = 0x00;
	
	u64 bIndex = 0;
	while (bIndex < FifoLength(fifo))
	{
		u8 newByte = FifoGet(fifo, bIndex);
		if (!foundAttnChar)
		{
			if (newByte == attnChar)
			{
				foundAttnChar = true;
				cmdIndex = 1;
			}
		}
		else
		{
			if (cmdIndex == 2)
			{
				payloadLength = newByte;
			}
			cmdIndex++;
			if (cmdIndex >= 3 && cmdIndex >= 3+(u64)payloadLength)
			{
				if (payloadLengthOut != nullptr) { *payloadLengthOut = payloadLength; }
				return true;
			}
		}
		bIndex++;
	}
	//If we broke out of the loop without returning then there's no commands ready
	return false;
}

u8* FifoPopCommand8(Fifo_t* fifo, MemArena_t* memArena, u8 attnChar = 0x7E)
{
	NotNull(fifo);
	
	u8 cmdPayloadLength = 0;
	bool foundCmd = FifoPeekCommand8(fifo, attnChar, &cmdPayloadLength);
	if (!foundCmd) { return nullptr; }
	
	//pop invalid data first
	while (FifoLength(fifo) > 0 && FifoGet(fifo, 0) != attnChar)
	{
		FifoPop(fifo);
	}
	
	u64 totalCmdSize = 3 + (u64)cmdPayloadLength;
	Assert(FifoLength(fifo) >= totalCmdSize);
	u8* result = AllocArray(memArena, u8, totalCmdSize);
	
	result[0] = FifoPop(fifo); Assert(result[0] == attnChar);
	result[1] = FifoPop(fifo);
	result[2] = FifoPop(fifo); Assert(result[2] == cmdPayloadLength);
	for (u64 bIndex = 0; bIndex < cmdPayloadLength; bIndex++)
	{
		result[3 + bIndex] = FifoPop(fifo);
	}
	
	return result;
}

bool FifoPeekCommand16(const Fifo_t* fifo, u8 attnChar = 0x7E, u16* payloadLengthOut = nullptr)
{
	NotNull(fifo);
	
	bool foundAttnChar = false;
	u64 cmdIndex = 0;
	u16 payloadLength = 0x0000;
	
	u64 bIndex = 0;
	while (bIndex < FifoLength(fifo))
	{
		u8 newByte = FifoGet(fifo, bIndex);
		if (!foundAttnChar)
		{
			if (newByte == attnChar)
			{
				foundAttnChar = true;
				cmdIndex = 1;
			}
		}
		else
		{
			if (cmdIndex == 2)
			{
				payloadLength |= ((u16)newByte << 0);
			}
			if (cmdIndex == 3)
			{
				payloadLength |= ((u16)newByte << 8);
			}
			cmdIndex++;
			if (cmdIndex >= 4 && cmdIndex >= 4+(u64)payloadLength)
			{
				if (payloadLengthOut != nullptr) { *payloadLengthOut = payloadLength; }
				return true;
			}
		}
		bIndex++;
	}
	//If we broke out of the loop without returning then there's no commands ready
	return false;
}

u8* FifoPopCommand16(Fifo_t* fifo, MemArena_t* memArena, u8 attnChar = 0x7E)
{
	NotNull(fifo);
	
	u16 cmdPayloadLength = 0;
	bool foundCmd = FifoPeekCommand16(fifo, attnChar, &cmdPayloadLength);
	if (!foundCmd) { return nullptr; }
	
	//pop invalid data first
	while (FifoLength(fifo) > 0 && FifoGet(fifo, 0) != attnChar)
	{
		FifoPop(fifo);
	}
	
	u64 totalCmdSize = 4 + (u64)cmdPayloadLength;
	Assert(FifoLength(fifo) >= totalCmdSize);
	u8* result = AllocArray(memArena, u8, totalCmdSize);
	
	result[0] = FifoPop(fifo); Assert(result[0] == attnChar);
	result[1] = FifoPop(fifo);
	result[2] = FifoPop(fifo); Assert(result[2] == (u8)(cmdPayloadLength>>0));
	result[3] = FifoPop(fifo); Assert(result[3] == (u8)(cmdPayloadLength>>8));
	for (u64 bIndex = 0; bIndex < cmdPayloadLength; bIndex++)
	{
		result[4 + bIndex] = FifoPop(fifo);
	}
	
	return result;
}

bool FifoPeekLine(const Fifo_t* fifo, u64* lineLengthOut = nullptr)
{
	NotNull(fifo);
	for (u64 bIndex = 0; bIndex < FifoLength(fifo); bIndex++)
	{
		if (FifoGet(fifo, bIndex) == '\n')
		{
			if (lineLengthOut != nullptr) { *lineLengthOut = bIndex; }
			return true;
		}
	}
	if (FifoLength(fifo) == fifo->size-1)
	{
		if (lineLengthOut != nullptr) { *lineLengthOut = fifo->size-1; }
		return true;
	}
	return false;
}

char* FifoPopLine(Fifo_t* fifo, MemArena_t* memArena)
{
	NotNull(fifo);
	
	u64 lineLength = 0;
	bool foundLine = FifoPeekLine(fifo, &lineLength);
	if (!foundLine) { return nullptr; }
	char* result = AllocArray(memArena, char, lineLength+1);
	NotNull(result);
	for (u64 bIndex = 0; bIndex < lineLength; bIndex++)
	{
		result[bIndex] = (char)FifoPop(fifo);
	}
	result[lineLength] = '\0';
	FifoPop(fifo); //pop-off \n character as well
	return result;
}
#endif

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_FIFO_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
FIFO_DYNAMIC_CHUNK_SIZE
FifoType_StaticDrop
FifoType_StaticOverwrite
FifoType_Dynamic
FifoType_NumTypes
@Types
FifoType_t
Fifo_t
@Functions
void FreeFifo(Fifo_t* fifo)
void CreateFifo(Fifo_t* fifo, MemArena_t* memArena, u64 size, FifoType_t type)
u64 FifoLength(const Fifo_t* fifo)
u8 FifoGetU8(const Fifo_t* fifo, u64 index = 0)
u8 FifoGetLastU8(const Fifo_t* fifo)
bool FifoGetMulti(const Fifo_t* fifo, u64 numBytes, u8* arrayPntr)
bool FifoPushU8(Fifo_t* fifo, u8 newByte)
bool FifoPushMulti(Fifo_t* fifo, u64 arraySize, const u8* arrayPntr)
u8 FifoPopU8(Fifo_t* fifo)
u8* FifoUnwrap(const Fifo_t* fifo, MemArena_t* memArena, bool addNullTerm = false)
*/
