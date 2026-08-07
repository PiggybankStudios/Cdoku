/*
File:   gy_string_fifo.h
Author: Taylor Robbins
Date:   01\01\2022
Description:
	** Holds a bunch of functions that help us create and work with StringFifo_t structure.
	** This data structure is essentially a linked list in a fixed size buffer that we treat as a FIFO.
	** Each string can be an arbitrary length and have a meta information structure tacked to the front
	** with some options for arbitrary length strings to feed those meta structures as well.
	** This is primarily used to hold the data for a debug output console window which also stores the
	** filePath and functionName of the code that generated each line of output.
	** Later on we recognized the need for a consistent way to "build-up" a line of text between multiple calls
	** so we added StringFifoBuild which is only functional if you call StringFifoAddBuildBuffer first.
	** This facilitates patterns line StringFifoBuild(...), StringFifoBuild(...), StringFifoPushLinet(...)
	** where all 3 calls would, together, produce a single line in the string fifo.
	** The metaStruct and metaString for this line pull from the first call that started the line.
*/

#ifndef _GY_STRING_FIFO_H
#define _GY_STRING_FIFO_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"
#include "gy_scratch_arenas.h"
#include "gy_string.h"

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct StringFifoLine_t
{
	StringFifoLine_t* prev;
	StringFifoLine_t* next;
	
	//NOTE: Any information that is contained in here that isn't directly referred to by StringFifoInsertLinesFromFifo will
	//      get thrown away (regenerated) as part of that process. In this case it means that lines that were already on the
	//      buffer will get reassigned a line number. This is kind of fine though so we're not solving it for now.
	//      Just don't add any mission critical data here without solving that problem
	u64 lineNumber;
	
	//These can be found immediately following this structure
	u64 metaStructSize;
	u64 metaStringLength;
	u64 textLength;
};

struct StringFifo_t
{
	MemArena_t* allocArena; //if nullptr then DestroyStringFifo does nothing. Someone external is managing this memory
	MemArena_t* buildBuffArena;
	
	u64 numLines;
	u64 nextLineNumber;
	StringFifoLine_t* firstLine;
	StringFifoLine_t* lastLine;
	
	u64 bufferSize;
	u64 used;
	u8* buffer;
	
	u64 buildBuffSize;
	bool buildLineActive;
	StringFifoLine_t buildLine; //textLength, metaStringLength and metaStructSize are important in here
	u8* buildLineMetaStruct; //points into the buildBuff
	char* buildLineMetaStr; //points into the buildBuff
	char* buildLineText; //points into the buildBuff
	u8* buildBuff;
};

//NOTE: If you modify the size of metaStructSize then the new requested size will be allocated but not automatically filled.
//      It will be up to the after callback to fill the new space.
#define GY_STRING_FIFO_PUSH_LINES_BEFORE_CALLBACK_DEF(functionName) bool functionName(StringFifo_t* fifo, const StringFifo_t* srcFifo, const StringFifoLine_t* srcLine, u64* metaStructSize, void* userPntr)
typedef GY_STRING_FIFO_PUSH_LINES_BEFORE_CALLBACK_DEF(StringFifoPushLineBefore_f);

#define GY_STRING_FIFO_PUSH_LINES_AFTER_CALLBACK_DEF(functionName) void functionName(StringFifo_t* fifo, const StringFifo_t* srcFifo, const StringFifoLine_t* srcLine, StringFifoLine_t* newLine, void* userPntr)
typedef GY_STRING_FIFO_PUSH_LINES_AFTER_CALLBACK_DEF(StringFifoPushLineAfter_f);

//NOTE: fifoLine may come from EITHER fifo or srcFifo
#define GY_STRING_FIFO_PUSH_LINES_SORT_CALLBACK_DEF(functionName) u64 functionName(StringFifo_t* fifo, const StringFifo_t* srcFifo, const StringFifoLine_t* fifoLine, void* userPntr)
typedef GY_STRING_FIFO_PUSH_LINES_SORT_CALLBACK_DEF(StringFifoPushLineSort_f);

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
//TODO: This define technically throws away const correctness because of the non-const type* cast
#define GetFifoLineMetaStruct(linePntr, type) (type*)GetFifoLineMetaStruct_((linePntr), sizeof(type))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsInitialized(const StringFifo_t* stringFifo);
	u64 GetFifoLineMetaSize(const StringFifoLine_t* line);
	u64 GetFifoLineTotalSize(const StringFifoLine_t* line);
	void* GetFifoLineMetaStruct_(StringFifoLine_t* line, u64 expectedStructSize);
	const void* GetFifoLineMetaStruct_(const StringFifoLine_t* line, u64 expectedStructSize);
	MyStr_t GetFifoLineMetaString(const StringFifoLine_t* line);
	MyStr_t GetFifoLineText(const StringFifoLine_t* line);
	const void* GetFifoLineEndPntr(const StringFifoLine_t* line);
	u64 GetStringFifoPntrIndex(const StringFifo_t* fifo, const void* pntr);
	u64 GetStringFifoTailIndex(const StringFifo_t* fifo);
	u64 GetStringFifoHeadIndex(const StringFifo_t* fifo);
	void DestroyStringFifo(StringFifo_t* fifo);
	void CreateStringFifo(StringFifo_t* fifo, u64 bufferSize, u8* bufferPntr);
	void CreateStringFifoInArena(StringFifo_t* fifo, MemArena_t* memArena, u64 bufferSize);
	void StringFifoAddBuildBufferInArena(StringFifo_t* fifo, u64 buildBufferSize, MemArena_t* memArena);
	void StringFifoAddBuildBuffer(StringFifo_t* fifo, u64 buildBufferSize, u8* buildSpace);
	void ClearStringFifo(StringFifo_t* fifo);
	void StringFifoPopLine(StringFifo_t* fifo);
	StringFifoLine_t* StringFifoPushLineExt(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString);
	StringFifoLine_t* StringFifoPushLine(StringFifo_t* fifo, MyStr_t text);
	void StringFifoBuildEx(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString);
	void StringFifoBuild(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString);
	void CopyStringFifo(StringFifo_t* destFifo, const StringFifo_t* srcFifo, MemArena_t* memArena, bool shrinkBufferToMatchContents);
	void StringFifoPushLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo, bool includeMetaStructs = true, bool includeMetaStrings = true, StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr);
	#if GYLIB_SCRATCH_ARENA_AVAILABLE
	void StringFifoInsertLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo, StringFifoPushLineSort_f* sortCallback, bool includeMetaStructs = true, bool includeMetaStrings = true, StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr);
	#endif // GYLIB_SCRATCH_ARENA_AVAILABLE
#else

// +--------------------------------------------------------------+
// |                       Helper Functions                       |
// +--------------------------------------------------------------+
bool IsInitialized(const StringFifo_t* stringFifo)
{
	NotNull(stringFifo);
	return (stringFifo->buffer != nullptr);
}

u64 GetFifoLineMetaSize(const StringFifoLine_t* line)
{
	NotNull_(line);
	return line->metaStructSize + line->metaStringLength+1;
}
u64 GetFifoLineTotalSize(const StringFifoLine_t* line)
{
	NotNull_(line);
	return sizeof(StringFifoLine_t) + line->metaStructSize + line->metaStringLength+1 + line->textLength+1;
}
void* GetFifoLineMetaStruct_(StringFifoLine_t* line, u64 expectedStructSize)
{
	NotNull_(line);
	Assert_(line->metaStructSize == expectedStructSize);
	return (void*)(line + 1);
}
const void* GetFifoLineMetaStruct_(const StringFifoLine_t* line, u64 expectedStructSize) //const-variant
{
	return (const void*)GetFifoLineMetaStruct_((StringFifoLine_t*)line, expectedStructSize);
}
MyStr_t GetFifoLineMetaString(const StringFifoLine_t* line)
{
	NotNull_(line);
	MyStr_t result;
	result.pntr = (char*)(line + 1) + line->metaStructSize;
	result.length = line->metaStringLength;
	return result;
}
MyStr_t GetFifoLineText(const StringFifoLine_t* line)
{
	NotNull_(line);
	MyStr_t result;
	result.pntr = (char*)(line + 1) + line->metaStructSize + line->metaStringLength+1;
	result.length = line->textLength;
	return result;
}
const void* GetFifoLineEndPntr(const StringFifoLine_t* line)
{
	NotNull_(line);
	return (const void*)((const char*)(line + 1) + line->metaStructSize + line->metaStringLength+1 + line->textLength+1);
}

u64 GetStringFifoPntrIndex(const StringFifo_t* fifo, const void* pntr)
{
	NotNull_(fifo);
	NotNull_(pntr);
	Assert_(((const u8*)pntr) >= fifo->buffer && ((const u8*)pntr) < fifo->buffer + fifo->bufferSize);
	return (u64)(((const u8*)pntr) - ((const u8*)fifo->buffer));
}
u64 GetStringFifoTailIndex(const StringFifo_t* fifo)
{
	if (fifo->firstLine == nullptr)
	{
		Assert_(fifo->lastLine == nullptr);
		return 0;
	}
	return GetStringFifoPntrIndex(fifo, fifo->firstLine);
}
u64 GetStringFifoHeadIndex(const StringFifo_t* fifo)
{
	if (fifo->lastLine == nullptr)
	{
		Assert_(fifo->firstLine == nullptr);
		return 0;
	}
	return GetStringFifoPntrIndex(fifo, GetFifoLineEndPntr((const StringFifoLine_t*)fifo->lastLine));
}

// +--------------------------------------------------------------+
// |                Creation/Destruction Functions                |
// +--------------------------------------------------------------+
void DestroyStringFifo(StringFifo_t* fifo)
{
	NotNull_(fifo);
	if (fifo->allocArena != nullptr)
	{
		NotNull(fifo->buffer);
		FreeMem(fifo->allocArena, fifo->buffer, fifo->bufferSize);
	}
	if (fifo->buildBuffArena != nullptr)
	{
		FreeMem(fifo->buildBuffArena, fifo->buildBuff, fifo->buildBuffSize);
	}
	ClearPointer(fifo);
}

void CreateStringFifo(StringFifo_t* fifo, u64 bufferSize, u8* bufferPntr)
{
	NotNull_(fifo);
	NotNull_(bufferPntr);
	Assert_(bufferSize > 0);
	ClearPointer(fifo);
	fifo->allocArena = nullptr;
	fifo->bufferSize = bufferSize;
	fifo->buffer = bufferPntr;
	fifo->used = 0;
	fifo->numLines = 0;
	fifo->firstLine = nullptr;
	fifo->lastLine = nullptr;
	fifo->nextLineNumber = 1;
}
void CreateStringFifoInArena(StringFifo_t* fifo, MemArena_t* memArena, u64 bufferSize)
{
	NotNull_(fifo);
	Assert_(bufferSize > 0);
	ClearPointer(fifo);
	fifo->allocArena = memArena;
	fifo->bufferSize = bufferSize;
	fifo->used = 0;
	fifo->numLines = 0;
	fifo->firstLine = nullptr;
	fifo->lastLine = nullptr;
	fifo->nextLineNumber = 1;
	
	fifo->buffer = AllocArray(memArena, u8, bufferSize);
	NotNull_(fifo->buffer);
	MyMemSet(&fifo->buffer[0], 0x00, bufferSize);
}

void StringFifoAddBuildBufferInArena(StringFifo_t* fifo, u64 buildBufferSize, MemArena_t* memArena)
{
	NotNull(fifo);
	Assert(fifo->buildBuff == nullptr);
	Assert(buildBufferSize > 0);
	NotNull(memArena);
	
	fifo->buildBuffArena = memArena;
	fifo->buildBuff = AllocArray(memArena, u8, buildBufferSize);
	NotNull(fifo->buildBuff);
	fifo->buildBuffSize = buildBufferSize;
	fifo->buildLineActive = false;
	ClearStruct(fifo->buildLine);
	fifo->buildLineMetaStruct = nullptr;
	fifo->buildLineMetaStr = nullptr;
	fifo->buildLineText = nullptr;
}
void StringFifoAddBuildBuffer(StringFifo_t* fifo, u64 buildBufferSize, u8* buildSpace)
{
	NotNull(fifo);
	Assert(fifo->buildBuff == nullptr);
	Assert(buildBufferSize > 0);
	NotNull(buildSpace);
	
	fifo->buildBuffArena = nullptr;
	fifo->buildBuff = buildSpace;
	NotNull(fifo->buildBuff);
	fifo->buildBuffSize = buildBufferSize;
	fifo->buildLineActive = false;
	ClearStruct(fifo->buildLine);
	fifo->buildLineMetaStruct = nullptr;
	fifo->buildLineMetaStr = nullptr;
	fifo->buildLineText = nullptr;
}

void ClearStringFifo(StringFifo_t* fifo)
{
	NotNull(fifo);
	fifo->used = 0;
	fifo->firstLine = nullptr;
	fifo->lastLine = nullptr;
	fifo->numLines = 0;
}

// +--------------------------------------------------------------+
// |                             Pop                              |
// +--------------------------------------------------------------+
void StringFifoPopLine(StringFifo_t* fifo)
{
	NotNull_(fifo);
	Assert_(fifo->numLines > 0);
	NotNull_(fifo->buffer);
	NotNull_(fifo->firstLine);
	NotNull_(fifo->lastLine);
	
	//TODO: Add a callback to let the code that manages this fifo know that a line is getting popped off
	
	u64 poppedLineSize = GetFifoLineTotalSize(fifo->firstLine);
	
	StringFifoLine_t* secondLine = fifo->firstLine->next;
	fifo->firstLine = secondLine;
	if (secondLine != nullptr)
	{
		secondLine->prev = nullptr;
	}
	fifo->numLines--;
	if (fifo->numLines == 0)
	{
		fifo->lastLine = nullptr;
	}
	Assert_(fifo->used >= poppedLineSize);
	fifo->used -= poppedLineSize;
}

// +--------------------------------------------------------------+
// |                             Push                             |
// +--------------------------------------------------------------+
StringFifoLine_t* StringFifoPushLineExt(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
{
	NotNull_(fifo);
	NotNull_(fifo->buffer);
	NotNullStr(&text);
	NotNullStr(&metaString);
	
	StringFifoLine_t dummyLine = {};
	if (fifo->buildLineActive)
	{
		dummyLine.metaStructSize = fifo->buildLine.metaStructSize;
		dummyLine.metaStringLength = fifo->buildLine.metaStringLength;
		dummyLine.textLength = fifo->buildLine.textLength + text.length;
	}
	else
	{
		dummyLine.metaStructSize = metaStructSize;
		dummyLine.metaStringLength = metaString.length;
		dummyLine.textLength = text.length;
	}
	u64 allocationSize = GetFifoLineTotalSize(&dummyLine);
	
	if (allocationSize > fifo->bufferSize)
	{
		return nullptr;
	}
	
	StringFifoLine_t* result = nullptr;
	
	bool triedEmptyArena = false;
	while (fifo->numLines > 0 || !triedEmptyArena)
	{
		if (fifo->numLines == 0) { triedEmptyArena = true; }
		DebugAssert_(fifo->used <= fifo->bufferSize);
		
		u64 fifoHeadIndex = GetStringFifoHeadIndex(fifo);
		u64 fifoTailIndex = GetStringFifoTailIndex(fifo);
		
		u64 nextAvailableSize = 0;
		u64 otherAvailableSize = 0;
		if (fifoHeadIndex > fifoTailIndex || (fifoHeadIndex == fifoTailIndex && fifo->numLines == 0))
		{
			//other   T      H   next
			//[-------<######>------]
			nextAvailableSize = fifo->bufferSize - fifoHeadIndex;
			otherAvailableSize = fifoTailIndex;
		}
		else
		{
			//     H  next   T       (no other)
			//[####>---------<######] 
			// (also includes scenario where we are perfectly caught up with our tail)
			nextAvailableSize = fifoTailIndex - fifoHeadIndex;
			otherAvailableSize = 0;
		}
		
		if (nextAvailableSize >= allocationSize)
		{
			result = (StringFifoLine_t*)(&fifo->buffer[fifoHeadIndex]);
			MyMemSet(result, 0x00, allocationSize);
			result->prev = fifo->lastLine;
			if (fifo->lastLine != nullptr) { fifo->lastLine->next = result; }
			else { fifo->firstLine = result; }
			fifo->numLines++;
			fifo->lastLine = result;
			fifo->used += allocationSize;
			Assert_(fifo->used <= fifo->bufferSize);
			break;
		}
		else if (otherAvailableSize >= allocationSize)
		{
			result = (StringFifoLine_t*)(&fifo->buffer[0]);
			MyMemSet(result, 0x00, allocationSize);
			result->prev = fifo->lastLine;
			if (fifo->lastLine != nullptr) { fifo->lastLine->next = result; }
			else { fifo->firstLine = result; }
			fifo->numLines++;
			fifo->lastLine = result;
			fifo->used += allocationSize;
			Assert_(fifo->used <= fifo->bufferSize);
			break;
		}
		else
		{
			if (fifo->numLines > 0) { StringFifoPopLine(fifo); }
		}
	}
	
	if (result != nullptr)
	{
		result->lineNumber = fifo->nextLineNumber;
		fifo->nextLineNumber++;
		
		if (fifo->buildLineActive)
		{
			result->metaStructSize = fifo->buildLine.metaStructSize;
			result->metaStringLength = fifo->buildLine.metaStringLength;
			result->textLength = fifo->buildLine.textLength + text.length;
			
			void* metaStructSpace = GetFifoLineMetaStruct_(result, metaStructSize);
			MyStr_t metaStringSpace = GetFifoLineMetaString(result);
			MyStr_t textSpace = GetFifoLineText(result);
			
			if (fifo->buildLine.metaStructSize > 0)
			{
				NotNull(fifo->buildLineMetaStruct);
				MyMemCopy(metaStructSpace, fifo->buildLineMetaStruct, fifo->buildLine.metaStructSize);
			}
			if (fifo->buildLine.metaStringLength > 0)
			{
				NotNull(fifo->buildLineMetaStr);
				MyMemCopy(metaStringSpace.pntr, fifo->buildLineMetaStr, fifo->buildLine.metaStringLength);
				metaStringSpace.pntr[fifo->buildLine.metaStringLength] = '\0';
			}
			if (fifo->buildLine.textLength + text.length > 0)
			{
				if (fifo->buildLine.textLength > 0)
				{
					NotNull(fifo->buildLineText);
					MyMemCopy(&textSpace.pntr[0], fifo->buildLineText, fifo->buildLine.textLength);
				}
				if (text.length > 0)
				{
					MyMemCopy(&textSpace.pntr[fifo->buildLine.textLength], text.pntr, text.length);
				}
				textSpace.pntr[fifo->buildLine.textLength + text.length] = '\0';
			}
			
			fifo->buildLineActive = false;
		}
		else
		{
			result->metaStructSize = metaStructSize;
			result->metaStringLength = metaString.length;
			result->textLength = text.length;
			
			//Copy the data into the correct places
			void* metaStructSpace = GetFifoLineMetaStruct_(result, metaStructSize);
			MyStr_t metaStringSpace = GetFifoLineMetaString(result);
			MyStr_t textSpace = GetFifoLineText(result);
			
			if (metaStructSize > 0 && metaStructPntr != nullptr)
			{
				MyMemCopy(metaStructSpace, metaStructPntr, metaStructSize);
			}
			if (metaString.length > 0)
			{
				MyMemCopy(metaStringSpace.pntr, metaString.pntr, metaString.length);
				metaStringSpace.pntr[metaString.length] = '\0';
			}
			if (text.length > 0)
			{
				MyMemCopy(textSpace.pntr, text.pntr, text.length);
				textSpace.pntr[text.length] = '\0';
			}
		}
	}
	
	DebugAssert_(fifo->firstLine == nullptr || fifo->firstLine->prev == nullptr);
	DebugAssert_(fifo->lastLine == nullptr || fifo->lastLine->next == nullptr);
	return result;
}
StringFifoLine_t* StringFifoPushLine(StringFifo_t* fifo, MyStr_t text)
{
	return StringFifoPushLineExt(fifo, text, 0, nullptr, MyStr_Empty);
}

void StringFifoBuildEx(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
{
	NotNull(fifo);
	Assert(fifo->buildBuff != nullptr);
	NotNullStr(&text);
	AssertIf(metaStructSize > 0, metaStructPntr != nullptr);
	NotNullStr(&metaString);
	
	if (!fifo->buildLineActive)
	{
		u64 metaSpaceSize = metaStructSize + metaString.length+1;
		if (metaSpaceSize >= fifo->buildBuffSize-1) { return; } //We can't fit the meta info with any space left for the line
		ClearStruct(fifo->buildLine);
		fifo->buildLine.textLength = text.length;
		fifo->buildLine.metaStructSize = metaStructSize;
		fifo->buildLine.metaStringLength = metaString.length;
		if (fifo->buildLine.textLength > fifo->buildBuffSize-1 - metaSpaceSize)
		{
			fifo->buildLine.textLength = fifo->buildBuffSize-1 - metaSpaceSize;
		}
		fifo->buildLineMetaStruct = &fifo->buildBuff[0];
		fifo->buildLineMetaStr = (char*)(&fifo->buildBuff[metaStructSize]);
		fifo->buildLineText = (char*)(&fifo->buildBuff[metaSpaceSize]);
		if (metaStructSize > 0)
		{
			MyMemCopy(fifo->buildLineMetaStruct, metaStructPntr, metaStructSize);
		}
		if (metaString.length > 0)
		{
			MyMemCopy(fifo->buildLineMetaStr, metaString.pntr, metaString.length);
			fifo->buildLineMetaStr[metaString.length] = '\0';
		}
		if (fifo->buildLine.textLength > 0)
		{
			MyMemCopy(fifo->buildLineText, text.pntr, fifo->buildLine.textLength);
			fifo->buildLineText[fifo->buildLine.textLength] = '\0';
		}
		fifo->buildLineActive = true;
	}
	else
	{
		u64 metaSpaceSize = fifo->buildLine.metaStructSize + fifo->buildLine.metaStringLength+1;
		Assert(metaSpaceSize + fifo->buildLine.textLength <= fifo->buildBuffSize-1);
		u64 textSpaceLeft = fifo->buildBuffSize - (metaSpaceSize + fifo->buildLine.textLength+1);
		if (text.length > textSpaceLeft) { text.length = textSpaceLeft; }
		if (textSpaceLeft > 0)
		{
			MyMemCopy(&fifo->buildLineText[fifo->buildLine.textLength], text.pntr, text.length);
			fifo->buildLine.textLength += text.length;
		}
	}
}
void StringFifoBuild(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
{
	UNUSED(metaString);
	UNUSED(metaStructPntr);
	UNUSED(metaStructSize);
	StringFifoBuildEx(fifo, text, 0, nullptr, MyStr_Empty);
}

// +--------------------------------------------------------------+
// |                             Copy                             |
// +--------------------------------------------------------------+
void CopyStringFifo(StringFifo_t* destFifo, const StringFifo_t* srcFifo, MemArena_t* memArena, bool shrinkBufferToMatchContents)
{
	NotNull(destFifo);
	NotNull(srcFifo);
	if (srcFifo->used == 0 && shrinkBufferToMatchContents)
	{
		//Nothing to do in this scenario
		ClearPointer(destFifo);
		return;
	}
	
	CreateStringFifoInArena(destFifo, memArena, shrinkBufferToMatchContents ? srcFifo->used : srcFifo->bufferSize);
	if (shrinkBufferToMatchContents)
	{
		const StringFifoLine_t* srcLine = srcFifo->firstLine;
		while (srcLine != nullptr)
		{
			MyStr_t srcText = GetFifoLineText(srcLine);
			const void* srcMetaStruct = ((srcLine->metaStructSize > 0) ? GetFifoLineMetaStruct_(srcLine, srcLine->metaStructSize) : nullptr);
			MyStr_t srcMetaString = GetFifoLineMetaString(srcLine);
			StringFifoLine_t* newLine = StringFifoPushLineExt(destFifo, srcText, srcLine->metaStructSize, srcMetaStruct, srcMetaString);
			NotNull(newLine);
			srcLine = srcLine->next;
		}
		DebugAssert(destFifo->used == destFifo->bufferSize);
	}
	else
	{
		//In this case, we can just copy and fixup the pointers and then we are done
		destFifo->used = srcFifo->used;
		destFifo->numLines = srcFifo->numLines;
		MyMemCopy(destFifo->buffer, srcFifo->buffer, srcFifo->bufferSize);
		NotNull(srcFifo->firstLine);
		destFifo->firstLine = (StringFifoLine_t*)(destFifo->buffer + (u64)((u8*)srcFifo->firstLine - srcFifo->buffer));
		destFifo->lastLine = (StringFifoLine_t*)(destFifo->buffer + (u64)((u8*)srcFifo->lastLine - srcFifo->buffer));
		StringFifoLine_t* line = destFifo->firstLine;
		StringFifoLine_t* prevLine = nullptr;
		while (line != nullptr)
		{
			Assert((u8*)line >= destFifo->buffer && (u8*)line < destFifo->buffer + destFifo->bufferSize);
			line->next = (StringFifoLine_t*)(destFifo->buffer + (u64)((u8*)line->next - srcFifo->buffer));
			line->prev = prevLine;
			prevLine = line;
			line = line->next;
		}
	}
}

// +--------------------------------------------------------------+
// |                      Push another Fifo                       |
// +--------------------------------------------------------------+
void StringFifoPushLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo,
	bool includeMetaStructs = true, bool includeMetaStrings = true,
	StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr)
{
	NotNull(fifo);
	NotNull(srcFifo);
	if (srcFifo->numLines == 0) { return; }
	NotNull(srcFifo->firstLine);
	
	const StringFifoLine_t* srcLine = srcFifo->firstLine;
	while (srcLine != nullptr)
	{
		u64 newMetaStructSize = 0;
		if (beforeCallback == nullptr || beforeCallback(fifo, srcFifo, srcLine, &newMetaStructSize, userPntr))
		{
			MyStr_t text = GetFifoLineText(srcLine);
			const void* metaStruct = ((includeMetaStructs && newMetaStructSize == 0) ? GetFifoLineMetaStruct_(srcLine, srcLine->metaStructSize) : nullptr);
			MyStr_t metaString = GetFifoLineMetaString(srcLine);
			StringFifoLine_t* newLine = StringFifoPushLineExt(
				fifo,
				text,
				((newMetaStructSize == 0) ? (includeMetaStructs ? srcLine->metaStructSize : 0) : newMetaStructSize),
				metaStruct,
				(includeMetaStrings ? metaString : MyStr_Empty)
			);
			if (afterCallback != nullptr)
			{
				afterCallback(fifo, srcFifo, srcLine, newLine, userPntr);
			}
		}
		srcLine = srcLine->next;
	}
}

#if GYLIB_SCRATCH_ARENA_AVAILABLE
//TODO: We probably need to take into account the lineNumber and update nextLineNumber in the fifo
void StringFifoInsertLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo, StringFifoPushLineSort_f* sortCallback,
	bool includeMetaStructs = true, bool includeMetaStrings = true,
	StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr)
{
	NotNull(fifo);
	NotNull(srcFifo);
	NotNull(sortCallback);
	if (srcFifo->numLines == 0) { return; }
	
	// +================================================+
	// | Find the minimum sort value fomr srcFifo lines |
	// +================================================+
	u64 minSrcSortValue = UINT64_MAX;
	const StringFifoLine_t* srcLine = srcFifo->firstLine;
	while (srcLine != nullptr)
	{
		u64 sortValue = sortCallback(fifo, srcFifo, srcLine, userPntr);
		if (sortValue < minSrcSortValue)
		{
			minSrcSortValue = sortValue;
		}
		srcLine = srcLine->next;
	}
	
	// +==================================================+
	// | Figure out how many lines we need to interleave  |
	// +==================================================+
	u64 totalTempSpaceNeeded = 0;
	u64 numLinesToInterleave = 0;
	StringFifoLine_t* fifoLine = fifo->lastLine;
	while (fifoLine != nullptr)
	{
		u64 sortValue = sortCallback(fifo, srcFifo, fifoLine, userPntr);
		if (sortValue <= minSrcSortValue) { break; }
		u64 lineSize = GetFifoLineTotalSize(fifoLine);
		totalTempSpaceNeeded += lineSize;
		numLinesToInterleave++;
		fifoLine = fifoLine->prev;
	}
	if (numLinesToInterleave == 0)
	{
		//no interleaving required. That's the same as StringFifoPushLinesFromFifo
		StringFifoPushLinesFromFifo(fifo, srcFifo, includeMetaStructs, includeMetaStrings, beforeCallback, afterCallback, userPntr);
		return;
	}
	
	// +====================================================+
	// | Move lines already in Fifo into Temporary Storage  |
	// +====================================================+
	MemArena_t* scratchArena = GetScratchArena();
	u8* tempSpace = AllocArray(scratchArena, u8, totalTempSpaceNeeded);
	NotNull(tempSpace);
	
	//walk back to the first line we want to store in temporary buffer
	fifoLine = fifo->lastLine;
	for (u64 lIndex = 0; lIndex < numLinesToInterleave-1; lIndex++) { NotNull(fifoLine); fifoLine = fifoLine->prev; }
	
	StringFifoLine_t* firstTempLine = (StringFifoLine_t*)&tempSpace[0];
	u64 tempSpaceWriteIndex = 0;
	StringFifoLine_t* prevLine = nullptr;
	while (fifoLine != nullptr)
	{
		u64 lineSize = GetFifoLineTotalSize(fifoLine);
		Assert(tempSpaceWriteIndex + lineSize <= totalTempSpaceNeeded);
		StringFifoLine_t* tempLine = (StringFifoLine_t*)&tempSpace[tempSpaceWriteIndex];
		MyMemCopy(tempLine, fifoLine, lineSize);
		tempLine->prev = prevLine;
		tempLine->next = nullptr;
		if (prevLine != nullptr) { prevLine->next = tempLine; }
		prevLine = tempLine;
		tempSpaceWriteIndex += lineSize;
		fifoLine = fifoLine->next;
	}
	
	// +==============================+
	// | Remove lines from main fifo  |
	// +==============================+
	StringFifoLine_t* lastLineBeforeInterleave = fifo->lastLine;
	for (u64 lIndex = 0; lIndex < numLinesToInterleave; lIndex++) { NotNull(lastLineBeforeInterleave); lastLineBeforeInterleave = lastLineBeforeInterleave->prev; }
	if (lastLineBeforeInterleave != nullptr)
	{
		lastLineBeforeInterleave->next = nullptr;
		fifo->lastLine = lastLineBeforeInterleave;
		Assert(totalTempSpaceNeeded < fifo->used);
		Assert(numLinesToInterleave < fifo->numLines);
		fifo->used -= totalTempSpaceNeeded;
		fifo->numLines -= numLinesToInterleave;
	}
	else
	{
		//we must be removing all of the lines
		Assert(numLinesToInterleave == fifo->numLines);
		fifo->firstLine = nullptr;
		fifo->lastLine = nullptr;
		fifo->numLines = 0;
		fifo->used = 0;
	}
	
	const StringFifoLine_t* srcLineIter = srcFifo->firstLine;
	const StringFifoLine_t* tempLineIter = firstTempLine;
	while (srcLineIter != nullptr || tempLineIter != nullptr)
	{
		bool addSrcLine = false;
		if (srcLineIter != nullptr && tempLineIter != nullptr)
		{
			u64 srcSortValue = sortCallback(fifo, srcFifo, srcLineIter, userPntr);
			u64 tempSortValue = sortCallback(fifo, srcFifo, tempLineIter, userPntr);
			addSrcLine = (tempSortValue > srcSortValue);
		}
		else if (srcLineIter != nullptr)
		{
			addSrcLine = true;
		}
		else if (tempLineIter != nullptr)
		{
			addSrcLine = false;
		}
		else { Assert(false); } //should be impossible because of while condition
		
		
		if (addSrcLine)
		{
			u64 newMetaStructSize = 0;
			if (beforeCallback != nullptr)
			{
				bool beforeResult = beforeCallback(fifo, srcFifo, srcLineIter, &newMetaStructSize, userPntr);
				AssertMsg(beforeResult, "We don't support adding only some liens when interleaving right now");
			}
			
			MyStr_t srcText = GetFifoLineText(srcLineIter);
			const void* srcMetaStruct = ((includeMetaStructs && newMetaStructSize == 0) ? GetFifoLineMetaStruct_(srcLineIter, srcLineIter->metaStructSize) : nullptr);
			MyStr_t srcMetaString = GetFifoLineMetaString(srcLineIter);
			StringFifoLine_t* newLine = StringFifoPushLineExt(
				fifo,
				srcText,
				((newMetaStructSize == 0) ? (includeMetaStructs ? srcLineIter->metaStructSize : 0) : newMetaStructSize),
				srcMetaStruct,
				(includeMetaStrings ? srcMetaString : MyStr_Empty)
			);
			if (newLine != nullptr && afterCallback != nullptr)
			{
				afterCallback(fifo, srcFifo, srcLineIter, newLine, userPntr);
			}
			srcLineIter = srcLineIter->next;
		}
		else
		{
			MyStr_t tempText = GetFifoLineText(tempLineIter);
			const void* tempMetaStruct = GetFifoLineMetaStruct_(tempLineIter, tempLineIter->metaStructSize);
			MyStr_t tempMetaString = GetFifoLineMetaString(tempLineIter);
			StringFifoLine_t* newLine = StringFifoPushLineExt(fifo, tempText, tempLineIter->metaStructSize, tempMetaStruct, tempMetaString);
			UNUSED(newLine);
			tempLineIter = tempLineIter->next;
		}
	}
	
	FreeScratchArena(scratchArena);
}
#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_STRING_FIFO_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
StringFifoLine_t
StringFifo_t
StringFifoPushLineBefore_f
StringFifoPushLineAfter_f
StringFifoPushLineSort_f
@Functions
bool IsInitialized(const StringFifo_t* stringFifo)
u64 GetFifoLineMetaSize(const StringFifoLine_t* line)
u64 GetFifoLineTotalSize(const StringFifoLine_t* line)
void* GetFifoLineMetaStruct_(StringFifoLine_t* line, u64 expectedStructSize)
#define GetFifoLineMetaStruct(linePntr, type)
MyStr_t GetFifoLineMetaString(const StringFifoLine_t* line)
MyStr_t GetFifoLineText(const StringFifoLine_t* line)
const void* GetFifoLineEndPntr(const StringFifoLine_t* line)
u64 GetStringFifoPntrIndex(const StringFifo_t* fifo, const void* pntr)
u64 GetStringFifoTailIndex(const StringFifo_t* fifo)
u64 GetStringFifoHeadIndex(const StringFifo_t* fifo)
void DestroyStringFifo(StringFifo_t* fifo)
void CreateStringFifo(StringFifo_t* fifo, u64 bufferSize, u8* bufferPntr)
void CreateStringFifoInArena(StringFifo_t* fifo, MemArena_t* memArena, u64 bufferSize)
void StringFifoAddBuildBufferInArena(StringFifo_t* fifo, u64 buildBufferSize, MemArena_t* memArena)
void StringFifoAddBuildBuffer(StringFifo_t* fifo, u64 buildBufferSize, u8* buildSpace)
void StringFifoPopLine(StringFifo_t* fifo)
StringFifoLine_t* StringFifoPushLineExt(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
StringFifoLine_t* StringFifoPushLine(StringFifo_t* fifo, MyStr_t text)
void StringFifoBuildEx(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
void StringFifoBuild(StringFifo_t* fifo, MyStr_t text, u64 metaStructSize, const void* metaStructPntr, MyStr_t metaString)
void CopyStringFifo(StringFifo_t* destFifo, const StringFifo_t* srcFifo, MemArena_t* memArena, bool shrinkBufferToMatchContents)
#define GY_STRING_FIFO_PUSH_LINES_BEFORE_CALLBACK_DEF(functionName)
#define GY_STRING_FIFO_PUSH_LINES_AFTER_CALLBACK_DEF(functionName)
void StringFifoPushLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo, bool includeMetaStructs = true, bool includeMetaStrings = true, StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr)
#define GY_STRING_FIFO_PUSH_LINES_SORT_CALLBACK_DEF(functionName)
void StringFifoInsertLinesFromFifo(StringFifo_t* fifo, const StringFifo_t* srcFifo, StringFifoPushLineSort_f* sortCallback, bool includeMetaStructs = true, bool includeMetaStrings = true, StringFifoPushLineBefore_f* beforeCallback = nullptr, StringFifoPushLineAfter_f* afterCallback = nullptr, void* userPntr = nullptr)
*/