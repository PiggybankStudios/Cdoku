/*
File:   gy_string_builder.h
Author: Taylor Robbins
Date:   05\12\2023
Description:
	** A string builder is something that efficiently manages building a string
	** without knowing how long the string is going to be beforehand. Often this
	** is serialization code that wants to use printf style formatting a bunch
	** of times in a row and end up with a large string holding the serialized data.
	** Unlike StringBuffer_t, the backing memory is allocated from a MemArena_t
	** and expands dynamically as more and more characters are added to the builder.
*/

#ifndef _GY_STRING_BUILDER_H
#define _GY_STRING_BUILDER_H

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct StringBuilder_t
{
	MemArena_t* allocArena;
	const char* newLineStyle;
	u64 length; //doesn't include null-term char so should always be strictly less than allocLength
	u64 allocLength;
	char* chars;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeStringBuilder(StringBuilder_t* builder);
	void NewStringBuilder(StringBuilder_t* builder, MemArena_t* memArena, u64 initialRequiredCapacity = 0);
	MyStr_t ToMyStr(const StringBuilder_t* builder);
	char* ToStr(StringBuilder_t* builder);
	u64 StringBuilderGetNumUnusedBytes(StringBuilder_t* builder, bool includeMemArenaSpace = false, GrowMemToken_t* growTokenOut = nullptr);
	void StringBuilderShrink(StringBuilder_t* builder, bool deallocateIfNoChars = true);
	MyStr_t TakeString(StringBuilder_t* builder, MemArena_t* memArena = nullptr);
	void StringBuilderAllocMoreMem(StringBuilder_t* builder, u64 spaceRequired);
	void StringBuilderClear(StringBuilder_t* builder, bool deallocate = false);
	void StringBuilderAppend(StringBuilder_t* builder, MyStr_t str);
	void StringBuilderAppend(StringBuilder_t* builder, const char* nullTermStr);
	void StringBuilderAppendLine(StringBuilder_t* builder);
	void StringBuilderAppendLine(StringBuilder_t* builder, MyStr_t str);
	void StringBuilderAppendLine(StringBuilder_t* builder, const char* nullTermStr);
	void StringBuilderSet(StringBuilder_t* builder, MyStr_t str);
	void StringBuilderSet(StringBuilder_t* builder, const char* nullTermStr);
	void StringBuilderAppendPrintVa(StringBuilder_t* builder, const char* formatString, va_list args1, va_list args2);
	void StringBuilderAppendPrint(StringBuilder_t* builder, const char* formatString, ...);
	void StringBuilderAppendPrintLine(StringBuilder_t* builder, const char* formatString, ...);
	void StringBuilderPrint(StringBuilder_t* builder, const char* formatString, ...);
#else

// +--------------------------------------------------------------+
// |                    New and Free Functions                    |
// +--------------------------------------------------------------+
void FreeStringBuilder(StringBuilder_t* builder)
{
	NotNull(builder);
	if (builder->chars != nullptr)
	{
		NotNull(builder->allocArena);
		if (DoesMemArenaSupportFreeing(builder->allocArena)) { FreeMem(builder->allocArena, builder->chars, builder->allocLength); }
	}
	ClearPointer(builder);
}
void NewStringBuilder(StringBuilder_t* builder, MemArena_t* memArena, u64 initialRequiredCapacity = 0)
{
	NotNull(builder);
	NotNull(memArena);
	ClearPointer(builder);
	builder->allocArena = memArena;
	builder->allocLength = 0;
	if (initialRequiredCapacity > 0) { builder->allocLength = PowerOfTwoGreaterThanOrEqualTo(initialRequiredCapacity); }
	if (builder->allocLength > 0)
	{
		builder->chars = AllocArray(builder->allocArena, char, builder->allocLength);
		NotNull(builder->chars);
		builder->chars[0] = '\0';
	}
}

// +--------------------------------------------------------------+
// |                  Get Information Functions                   |
// +--------------------------------------------------------------+
MyStr_t ToMyStr(const StringBuilder_t* builder)
{
	NotNull(builder);
	if (builder->length == 0) { return MyStr_Empty; }
	NotNull(builder->chars);
	return NewStr(builder->length, builder->chars);
}
char* ToStr(StringBuilder_t* builder)
{
	NotNull(builder);
	if (builder->length == 0) { return (char*)""; }
	NotNull(builder->chars);
	DebugAssert(builder->chars[builder->length] == '\0');
	return builder->chars;
}

//returns the number of bytes at the end of the allocation that are currently unused (with the option to query the memory arena for space that is following our allocation that is also unallocated)
u64 StringBuilderGetNumUnusedBytes(StringBuilder_t* builder, bool includeMemArenaSpace = false, GrowMemToken_t* growTokenOut = nullptr)
{
	NotNull2(builder, builder->allocArena);
	u64 growableSpace = 0;
	if (includeMemArenaSpace && builder->chars != nullptr)
	{
		NotNull(growTokenOut);
		growableSpace = GrowMemQuery(builder->allocArena, builder->chars, builder->allocLength, growTokenOut);
	}
	u64 result = growableSpace;
	if (builder->allocLength > 0) { result += (builder->allocLength - builder->length); }
	if (result > 0) { result--; } //subtract 1 for null-term char
	return result;
}

// +--------------------------------------------------------------+
// |                            Shrink                            |
// +--------------------------------------------------------------+
void StringBuilderShrink(StringBuilder_t* builder, bool deallocateIfNoChars = true)
{
	NotNull(builder);
	NotNull(builder->allocArena);
	
	if (builder->length == 0 && deallocateIfNoChars)
	{
		if (builder->allocLength > 0)
		{
			NotNull(builder->chars);
			if (DoesMemArenaSupportFreeing(builder->allocArena)) { FreeMem(builder->allocArena, builder->chars, builder->allocLength); }
			builder->chars = nullptr;
		}
		return;
	}
	
	if (builder->allocLength > builder->length+1)
	{
		ShrinkMem(builder->allocArena, builder->chars, builder->allocLength, builder->length+1);
		builder->allocLength = builder->length+1;
	}
}

// Calling this function implies that the allocation is going to be freed by the calling code, rather than FreeStringBuilder. So we do StringBuilderShrink, reallocate in requested arena if needed, clear our own reference to the allocation, and return the MyStr_t
MyStr_t TakeString(StringBuilder_t* builder, MemArena_t* memArena = nullptr)
{
	NotNull(builder);
	NotNull(builder->allocArena);
	
	MyStr_t result = ((builder->length > 0) ? NewStr(builder->length, builder->chars) : MyStr_Empty);
	StringBuilderShrink(builder);
	
	if (memArena != nullptr && memArena != builder->allocArena)
	{
		MyStr_t newResult = AllocString(memArena, &result);
		if (DoesMemArenaSupportFreeing(builder->allocArena)) { FreeString(builder->allocArena, &result); }
		result = newResult;
	}
	
	return result;
}

// +--------------------------------------------------------------+
// |                            Append                            |
// +--------------------------------------------------------------+
//spaceRequired should include +1 for null-term char
void StringBuilderAllocMoreMem(StringBuilder_t* builder, u64 spaceRequired)
{
	NotNull(builder);
	NotNull(builder->allocArena);
	u64 newAllocLength = PowerOfTwoGreaterThanOrEqualTo(spaceRequired);
	//TODO: This should probably be a ReallocMem, but I don't remember if that functionality is working yet in gylib
	char* newChars = AllocArray(builder->allocArena, char, newAllocLength);
	NotNull(newChars);
	if (builder->allocLength > 0)
	{
		MyMemCopy(newChars, builder->chars, builder->allocLength);
		if (DoesMemArenaSupportFreeing(builder->allocArena)) { FreeMem(builder->allocArena, builder->chars); }
	}
	builder->chars = newChars;
	builder->allocLength = newAllocLength;
}

void StringBuilderClear(StringBuilder_t* builder, bool deallocate = false)
{
	NotNull(builder);
	builder->length = 0;
	if (deallocate)
	{
		if (builder->allocLength > 0)
		{
			NotNull2(builder->allocArena, builder->chars);
			if (DoesMemArenaSupportFreeing(builder->allocArena)) { FreeMem(builder->allocArena, builder->chars, builder->allocLength); }
			builder->allocLength = 0;
			builder->chars = nullptr;
		}
	}
	else if (builder->allocLength > 0)
	{
		NotNull(builder->chars);
		builder->chars[0] = '\0';
	}
}

void StringBuilderAppend(StringBuilder_t* builder, MyStr_t str)
{
	NotNull(builder);
	if (str.length == 0) { return; } //no work to do
	
	u64 normalSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, false);
	if (normalSpaceAvailable >= str.length)
	{
		MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
		builder->length += str.length;
		builder->chars[builder->length] = '\0';
	}
	else
	{
		GrowMemToken_t growToken = {};
		u64 growableSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, true, &growToken);
		if (growableSpaceAvailable >= str.length+1)
		{
			u64 numNewBytesUsed = (str.length+1 - normalSpaceAvailable);
			MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
			builder->length += str.length;
			builder->chars[builder->length] = '\0';
			GrowMem(builder->allocArena, builder->chars, builder->allocLength, builder->allocLength + numNewBytesUsed, &growToken);
			builder->allocLength += numNewBytesUsed;
		}
		else
		{
			StringBuilderAllocMoreMem(builder, builder->length + str.length + 1);
			MyMemCopy(&builder->chars[builder->length], str.chars, str.length);
			builder->length += str.length;
			builder->chars[builder->length] = '\0';
		}
	}
}
void StringBuilderAppend(StringBuilder_t* builder, const char* nullTermStr)
{
	StringBuilderAppend(builder, NewStr(nullTermStr));
}

void StringBuilderAppendLine(StringBuilder_t* builder)
{
	NotNull(builder);
	StringBuilderAppend(builder, (builder->newLineStyle != nullptr) ? builder->newLineStyle : "\n");
}
void StringBuilderAppendLine(StringBuilder_t* builder, MyStr_t str)
{
	StringBuilderAppend(builder, str);
	StringBuilderAppendLine(builder);
}
void StringBuilderAppendLine(StringBuilder_t* builder, const char* nullTermStr)
{
	StringBuilderAppend(builder, nullTermStr);
	StringBuilderAppendLine(builder);
}

void StringBuilderSet(StringBuilder_t* builder, MyStr_t str)
{
	StringBuilderClear(builder);
	StringBuilderAppend(builder, str);
}
void StringBuilderSet(StringBuilder_t* builder, const char* nullTermStr)
{
	StringBuilderSet(builder, NewStr(nullTermStr));
}

void StringBuilderAppendPrintVa(StringBuilder_t* builder, const char* formatString, va_list args1, va_list args2)
{
	NotNull(builder);
	
	u64 normalSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, false);
	GrowMemToken_t growToken = {};
	u64 growableSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, true, &growToken);
	
	if (growableSpaceAvailable > 0)
	{
		int printResult = MyVaListPrintf(&builder->chars[builder->length], (int)(growableSpaceAvailable+1), formatString, args1);
		Assert(printResult >= 0); //TODO: Can we handle this condition rather than assert? What has happened to the buffer we gave it? Should we call GrowMem??
		
		bool increasedLength = false;
		if ((u64)printResult > normalSpaceAvailable && growableSpaceAvailable > normalSpaceAvailable)
		{
			u64 newAllocLength = builder->length + MinU64((u64)printResult, growableSpaceAvailable) + 1;
			GrowMem(builder->allocArena, builder->chars, builder->allocLength, newAllocLength, &growToken);
			builder->allocLength = newAllocLength;
			increasedLength = true;
		}
		
		if ((u64)printResult > growableSpaceAvailable)
		{
			//We need more memory, allocate some more and then try the print again
			StringBuilderAllocMoreMem(builder, builder->length + (u64)printResult + 1);
			
			normalSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, false);
			Assert(normalSpaceAvailable >= (u64)printResult);
			int secondPrintResult = MyVaListPrintf(&builder->chars[builder->length], (int)(normalSpaceAvailable+1), formatString, args2);
			Assert(secondPrintResult == printResult);
			
			builder->length += (u64)secondPrintResult;
			increasedLength = true;
		}
		else
		{
			builder->length += (u64)printResult;
		}
		
		Assert(builder->length < builder->allocLength);
		builder->chars[builder->length] = '\0';
	}
	else
	{
		int printResult = MyVaListPrintf(nullptr, 0, formatString, args1);
		Assert(printResult >= 0); //TODO: Can we handle this condition rather than assert? What has happened to the buffer we gave it? Should we call GrowMem??
		
		//We need more memory, allocate some more and then try the print again
		StringBuilderAllocMoreMem(builder, builder->length + (u64)printResult + 1);
		
		normalSpaceAvailable = StringBuilderGetNumUnusedBytes(builder, false);
		Assert(normalSpaceAvailable >= (u64)printResult);
		int secondPrintResult = MyVaListPrintf(&builder->chars[builder->length], (int)(normalSpaceAvailable+1), formatString, args2);
		Assert(secondPrintResult == printResult);
		
		builder->length += (u64)secondPrintResult;
		
		Assert(builder->length < builder->allocLength);
		builder->chars[builder->length] = '\0';
	}
}

void StringBuilderAppendPrint(StringBuilder_t* builder, const char* formatString, ...)
{
	va_list args1;
	va_list args2;
	va_start(args1, formatString);
	va_start(args2, formatString);
	StringBuilderAppendPrintVa(builder, formatString, args1, args2);
	va_end(args1);
	va_end(args2);
}
void StringBuilderAppendPrintLine(StringBuilder_t* builder, const char* formatString, ...)
{
	va_list args1;
	va_list args2;
	va_start(args1, formatString);
	va_start(args2, formatString);
	StringBuilderAppendPrintVa(builder, formatString, args1, args2);
	va_end(args1);
	va_end(args2);
	StringBuilderAppendLine(builder);
}

void StringBuilderPrint(StringBuilder_t* builder, const char* formatString, ...)
{
	StringBuilderClear(builder);
	va_list args1;
	va_list args2;
	va_start(args1, formatString);
	va_start(args2, formatString);
	StringBuilderAppendPrintVa(builder, formatString, args1, args2);
	va_end(args1);
	va_end(args2);
}

//TODO: void StringBuilderReplace(StringBuilder_t* builder, MyStr_t target, MyStr_t replacement, bool ignoreCase = true)

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_STRING_BUILDER_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
@Types
StringBuilder_t
@Functions
void FreeStringBuilder(StringBuilder_t* builder)
void NewStringBuilder(StringBuilder_t* builder, MemArena_t* memArena, u64 initialRequiredCapacity = 0)
MyStr_t ToMyStr(const StringBuilder_t* builder)
char* ToStr(StringBuilder_t* builder)
u64 StringBuilderGetNumUnusedBytes(StringBuilder_t* builder, bool includeMemArenaSpace = false, GrowMemToken_t* growTokenOut = nullptr)
void StringBuilderShrink(StringBuilder_t* builder, bool deallocateIfNoChars = true)
MyStr_t TakeString(StringBuilder_t* builder, MemArena_t* memArena = nullptr)
void StringBuilderAllocMoreMem(StringBuilder_t* builder, u64 spaceRequired)
void StringBuilderClear(StringBuilder_t* builder, bool deallocate = false)
void StringBuilderAppend(StringBuilder_t* builder, MyStr_t str)
void StringBuilderAppendLine(StringBuilder_t* builder, const char* nullTermStr = "")
void StringBuilderSet(StringBuilder_t* builder, MyStr_t str)
void StringBuilderAppendPrintVa(StringBuilder_t* builder, const char* formatString, va_list args1, va_list args2)
void StringBuilderAppendPrint(StringBuilder_t* builder, const char* formatString, ...)
void StringBuilderAppendPrintLine(StringBuilder_t* builder, const char* formatString, ...)
void StringBuilderPrint(StringBuilder_t* builder, const char* formatString, ...)
*/
