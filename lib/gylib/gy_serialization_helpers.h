/*
File:   gy_serialization_helpers.h
Author: Taylor Robbins
Date:   07\23\2023
Description:
	** Holds functions and macros that help us serialize and deserialize things to binary data
	** Often these are helper functions to facilitate a 2-pass approach to serialization (one pass to size, allocate, then another pass to fill)
*/

#ifndef _GY_SERIALIZATION_HELPERS_H
#define _GY_SERIALIZATION_HELPERS_H

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define BinSer_WriteStructure(result, resultSize, byteIndexPntr, type) (type*)BinSer_WriteStructure_((result), (resultSize), (byteIndexPntr), sizeof(type))

#define BinSer_WriteValue(result, resultSize, byteIndexPntr, type, value) do                           \
{                                                                                                      \
	type localVariableForValue = (value);                                                              \
	BinSer_WriteValue_((result), (resultSize), (byteIndexPntr), sizeof(type), &localVariableForValue); \
} while(0)

#define BinSer_WriteU8(result, resultSize, byteIndexPntr, value)  BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u8,  (value))
#define BinSer_WriteU16(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u16, (value))
#define BinSer_WriteU32(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u32, (value))
#define BinSer_WriteU64(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), u64, (value))
#define BinSer_WriteI8(result, resultSize, byteIndexPntr, value)  BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i8,  (value))
#define BinSer_WriteI16(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i16, (value))
#define BinSer_WriteI32(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i32, (value))
#define BinSer_WriteI64(result, resultSize, byteIndexPntr, value) BinSer_WriteValue((result), (resultSize), (byteIndexPntr), i64, (value))
#define BinSer_WriteBytes(result, resultSize, byteIndexPntr, numBytes, bytesPntr) BinSer_WriteValue_((result), (resultSize), (byteIndexPntr), (numBytes), (bytesPntr))

#define BinDeser_IsMemberPresent(structSize, type, memberName) (structSize >= (STRUCT_VAR_OFFSET(type, memberName) + sizeof(((type*)0)->memberName)))

#define BinDeser_ReadStruct(dataPntr, dataSize, byteIndexPntr, type) (type*)BinDeser_ReadStruct_((dataPntr), (dataSize), (byteIndexPntr), sizeof(type))
#define BinDeser_ReadBytes(dataPntr, dataSize, byteIndexPntr, numBytes) (u8*)BinDeser_ReadStruct_((dataPntr), (dataSize), (byteIndexPntr), (numBytes))
#define BinDeser_ReadStructDynamicSize(dataPntr, dataSize, byteIndexPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName) (type*)BinDeser_ReadStructDynamicSize_((dataPntr), (dataSize), (byteIndexPntr), (structSizeOut), STRUCT_VAR_OFFSET(type, sizeMemberName), STRUCT_VAR_OFFSET(type, firstOptionalMemberName), sizeof(type))
#define BinDeser_ReadU8(dataPntr, dataSize, byteIndexPntr, variableOut)  BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u8),  (void*)(variableOut))
#define BinDeser_ReadU16(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u16), (void*)(variableOut))
#define BinDeser_ReadU32(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u32), (void*)(variableOut))
#define BinDeser_ReadU64(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(u64), (void*)(variableOut))
#define BinDeser_ReadI8(dataPntr, dataSize, byteIndexPntr, variableOut)  BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i8),  (void*)(variableOut))
#define BinDeser_ReadI16(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i16), (void*)(variableOut))
#define BinDeser_ReadI32(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i32), (void*)(variableOut))
#define BinDeser_ReadI64(dataPntr, dataSize, byteIndexPntr, variableOut) BinDeser_ReadVariable_((dataPntr), (dataSize), (byteIndexPntr), sizeof(i64), (void*)(variableOut))

#define StreamStaticDeser_ReadStruct(streamPntr, type)          (type*)StreamDeser_ReadStruct_((streamPntr), nullptr, true, sizeof(type))
#define StreamArenaDeser_ReadStruct(streamPntr, memArena, type) (type*)StreamDeser_ReadStruct_((streamPntr), (memArena), false, sizeof(type))
#define StreamDeser_ReadStruct(streamPntr, maybeArena, type)    (type*)StreamDeser_ReadStruct_((streamPntr), (maybeArena), true, sizeof(type))

#define StreamStaticDeser_ReadBytes(streamPntr, numBytes)          (u8*)StreamDeser_ReadStruct_((streamPntr), nullptr, true, (numBytes))
#define StreamArenaDeser_ReadBytes(streamPntr, memArena, numBytes) (u8*)StreamDeser_ReadStruct_((streamPntr), (memArena), false, (numBytes))
#define StreamDeser_ReadBytes(streamPntr, maybeArena, numBytes)    (u8*)StreamDeser_ReadStruct_((streamPntr), (maybeArena), true, (numBytes))

#define StreamStaticDeser_ReadStructDynamicSize(streamPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName)          (type*)StreamDeser_ReadStructDynamicSize_((streamPntr), nullptr, true, (structSizeOut), STRUCT_VAR_OFFSET(type, sizeMemberName), STRUCT_VAR_OFFSET(type, firstOptionalMemberName), sizeof(type))
#define StreamArenaDeser_ReadStructDynamicSize(streamPntr, memArena, structSizeOut, type, sizeMemberName, firstOptionalMemberName) (type*)StreamDeser_ReadStructDynamicSize_((streamPntr), (memArena), false, (structSizeOut), STRUCT_VAR_OFFSET(type, sizeMemberName), STRUCT_VAR_OFFSET(type, firstOptionalMemberName), sizeof(type))
#define StreamDeser_ReadStructDynamicSize(streamPntr, maybeArena, structSizeOut, type, sizeMemberName, firstOptionalMemberName)    (type*)StreamDeser_ReadStructDynamicSize_((streamPntr), (maybeArena), true, (structSizeOut), STRUCT_VAR_OFFSET(type, sizeMemberName), STRUCT_VAR_OFFSET(type, firstOptionalMemberName), sizeof(type))

#define StreamStaticDeser_ReadU8(streamPntr, variableOut)   StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(u8),  (void*)(variableOut))
#define StreamStaticDeser_ReadU16(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(u16), (void*)(variableOut))
#define StreamStaticDeser_ReadU32(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(u32), (void*)(variableOut))
#define StreamStaticDeser_ReadU64(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(u64), (void*)(variableOut))
#define StreamStaticDeser_ReadI8(streamPntr, variableOut)   StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(i8),  (void*)(variableOut))
#define StreamStaticDeser_ReadI16(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(i16), (void*)(variableOut))
#define StreamStaticDeser_ReadI32(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(i32), (void*)(variableOut))
#define StreamStaticDeser_ReadI64(streamPntr, variableOut)  StreamDeser_ReadVariable_((streamPntr), nullptr, true, sizeof(i64), (void*)(variableOut))

#define StreamArenaDeser_ReadU8(streamPntr, memArena, variableOut)   StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(u8),  (void*)(variableOut))
#define StreamArenaDeser_ReadU16(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(u16), (void*)(variableOut))
#define StreamArenaDeser_ReadU32(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(u32), (void*)(variableOut))
#define StreamArenaDeser_ReadU64(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(u64), (void*)(variableOut))
#define StreamArenaDeser_ReadI8(streamPntr, memArena, variableOut)   StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(i8),  (void*)(variableOut))
#define StreamArenaDeser_ReadI16(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(i16), (void*)(variableOut))
#define StreamArenaDeser_ReadI32(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(i32), (void*)(variableOut))
#define StreamArenaDeser_ReadI64(streamPntr, memArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (memArena), false, sizeof(i64), (void*)(variableOut))

#define StreamDeser_ReadU8(streamPntr, maybeArena, variableOut)   StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(u8),  (void*)(variableOut))
#define StreamDeser_ReadU16(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(u16), (void*)(variableOut))
#define StreamDeser_ReadU32(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(u32), (void*)(variableOut))
#define StreamDeser_ReadU64(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(u64), (void*)(variableOut))
#define StreamDeser_ReadI8(streamPntr, maybeArena, variableOut)   StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(i8),  (void*)(variableOut))
#define StreamDeser_ReadI16(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(i16), (void*)(variableOut))
#define StreamDeser_ReadI32(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(i32), (void*)(variableOut))
#define StreamDeser_ReadI64(streamPntr, maybeArena, variableOut)  StreamDeser_ReadVariable_((streamPntr), (maybeArena), true, sizeof(i64), (void*)(variableOut))

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize);
	void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr);
	const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize);
	const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize);
	bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr);
	const void* StreamDeser_ReadStruct_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64 structureSize);
	const void* StreamDeser_ReadStructDynamicSize_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize);
	bool StreamDeser_ReadVariable_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64 valueSize, void* valueOutPntr);
#else

// +--------------------------------------------------------------+
// |                 Binary Serialization Headers                 |
// +--------------------------------------------------------------+
void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize)
{
	NotNull(byteIndexPntr);
	AssertIf(result != nullptr, (*byteIndexPntr) + structureSize <= resultSize);
	void* structPntr = (result != nullptr) ? (((u8*)result) + (*byteIndexPntr)) : nullptr;
	*byteIndexPntr = (*byteIndexPntr) + structureSize;
	return structPntr;
}

void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr)
{
	NotNull2(byteIndexPntr, valuePntr);
	if (result != nullptr)
	{
		Assert((*byteIndexPntr) + valueSize <= resultSize);
		void* valueWritePntr = ((u8*)result) + (*byteIndexPntr);
		MyMemCopy(valueWritePntr, valuePntr, valueSize);
	}
	*byteIndexPntr = (*byteIndexPntr) + valueSize;
}

// +--------------------------------------------------------------+
// |                Binary Deserialization Helpers                |
// +--------------------------------------------------------------+
const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize)
{
	NotNull2(dataPntr, byteIndexPntr);
	if ((*byteIndexPntr) + structureSize > dataSize) { return nullptr; }
	const void* result = ((u8*)dataPntr) + (*byteIndexPntr);
	*byteIndexPntr = (*byteIndexPntr) + structureSize;
	return result;
}

const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize)
{
	NotNull2(dataPntr, byteIndexPntr);
	Assert (sizeMemberOffset + sizeof(u64) <= minStructSize);
	if ((*byteIndexPntr) + minStructSize > dataSize) { return nullptr; }
	const u64* sizePntr = (const u64*)(((u8*)dataPntr) + (*byteIndexPntr) + sizeMemberOffset);
	if (*sizePntr < minStructSize) { return nullptr; }
	if (*sizePntr > maxStructSize) { return nullptr; }
	if (structSizeOut != nullptr) { *structSizeOut = *sizePntr; }
	const void* result = ((u8*)dataPntr) + (*byteIndexPntr);
	*byteIndexPntr = (*byteIndexPntr) + (*sizePntr);
	return result;
}

bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr)
{
	NotNull3(dataPntr, byteIndexPntr, valueOutPntr);
	if ((*byteIndexPntr) + valueSize > dataSize) { return false; }
	const void* valueReadPntr = ((u8*)dataPntr) + (*byteIndexPntr);
	MyMemCopy(valueOutPntr, valueReadPntr, valueSize);
	*byteIndexPntr = (*byteIndexPntr) + valueSize;
	return true;
}

const void* StreamDeser_ReadStruct_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64 structureSize)
{
	NotNull(stream);
	if (tryStaticRead && IsFlagSet(stream->capabilities, StreamCapability_StaticRead))
	{
		u64 numBytesRead = 0;
		void* result = StreamRead(stream, structureSize, &numBytesRead);
		if (numBytesRead < structureSize) { return nullptr; }
		return result;
	}
	else
	{
		NotNull(memArena);
		u64 numBytesRead = 0;
		void* result = StreamReadInArena(stream, structureSize, memArena, &numBytesRead);
		if (numBytesRead < structureSize) { return nullptr; }
		return result;
	}
}

//TODO: We should add error outputs to this function, since it can fail in a variety of ways, and the calling code might want to report why it failed exactly
const void* StreamDeser_ReadStructDynamicSize_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize)
{
	NotNull(stream);
	Assert (sizeMemberOffset + sizeof(u64) <= minStructSize);
	if (tryStaticRead && IsFlagSet(stream->capabilities, StreamCapability_StaticRead) && IsFlagSet(stream->capabilities, StreamCapability_Backtracking))
	{
		u64 initialReadSize = sizeMemberOffset + sizeof(u64);
		u64 numBytesRead = 0;
		u8* initialRead = (u8*)StreamRead(stream, initialReadSize, &numBytesRead);
		if (numBytesRead < initialReadSize) { return nullptr; }
		u64 structSize = *(u64*)(initialRead + sizeMemberOffset);
		if (structSize < minStructSize) { return nullptr; }
		if (structSize > maxStructSize) { return nullptr; }
		StreamMoveBack(stream, initialReadSize);
		void* result = (u8*)StreamRead(stream, structSize, &numBytesRead);
		if (numBytesRead < structSize) { return nullptr; }
		SetOptionalOutPntr(structSizeOut, structSize);
		return result;
	}
	else
	{
		NotNull(memArena);
		u64 initialReadSize = sizeMemberOffset + sizeof(u64);
		u64 numBytesRead = 0;
		u8* initialRead = (u8*)StreamReadInArena(stream, initialReadSize, memArena, &numBytesRead);
		if (numBytesRead < initialReadSize) { return nullptr; }
		u64 structSize = *(u64*)(initialRead + sizeMemberOffset);
		if (structSize < minStructSize) { return nullptr; }
		if (structSize > maxStructSize) { return nullptr; }
		SetOptionalOutPntr(structSizeOut, structSize);
		if (structSize > initialReadSize)
		{
			u64 newPortionSize = (structSize - initialReadSize);
			u8* result = (u8*)ReallocMem(memArena, initialRead, structSize, initialReadSize);
			NotNull(result);
			numBytesRead = StreamReadInto(stream, newPortionSize, &result[initialReadSize]);
			if (numBytesRead < newPortionSize) { return nullptr; }
			return result;
		}
		else { return initialRead; }
	}
}

bool StreamDeser_ReadVariable_(Stream_t* stream, MemArena_t* memArena, bool tryStaticRead, u64 valueSize, void* valueOutPntr)
{
	NotNull(stream);
	if (tryStaticRead && IsFlagSet(stream->capabilities, StreamCapability_StaticRead))
	{
		u64 numBytesRead = 0;
		void* value = StreamRead(stream, valueSize, &numBytesRead);
		if (numBytesRead < valueSize) { return false; }
		if (valueOutPntr != nullptr) { MyMemCopy(valueOutPntr, value, valueSize); }
		return true;
	}
	else
	{
		NotNull(memArena);
		u64 numBytesRead = 0;
		void* value = StreamReadInArena(stream, valueSize, memArena, &numBytesRead);
		if (numBytesRead < valueSize) { return false; }
		if (valueOutPntr != nullptr) { MyMemCopy(valueOutPntr, value, valueSize); }
		return true;
	}
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_SERIALIZATION_HELPERS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Functions
void* BinSer_WriteStructure_(void* result, u64 resultSize, u64* byteIndexPntr, u64 structureSize)
#define BinSer_WriteStructure(result, resultSize, byteIndexPntr, type)
void BinSer_WriteValue_(void* result, u64 resultSize, u64* byteIndexPntr, u64 valueSize, const void* valuePntr)
#define BinSer_WriteValue(result, resultSize, byteIndexPntr, type, value)
#define BinSer_WriteU8(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU16(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU32(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteU64(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI8(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI16(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI32(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteI64(result, resultSize, byteIndexPntr, value)
#define BinSer_WriteBytes(result, resultSize, byteIndexPntr, numBytes, bytesPntr)
const void* BinDeser_ReadStruct_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 structureSize)
#define BinDeser_ReadStruct(dataPntr, dataSize, byteIndexPntr, type)
#define BinDeser_ReadBytes(dataPntr, dataSize, byteIndexPntr, numBytes)
const void* BinDeser_ReadStructDynamicSize_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64* structSizeOut, u64 sizeMemberOffset, u64 minStructSize, u64 maxStructSize)
#define BinDeser_ReadStructDynamicSize(dataPntr, dataSize, byteIndexPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName)
#define BinDeser_IsMemberPresent(structSize, type, memberName)
bool BinDeser_ReadVariable_(const void* dataPntr, u64 dataSize, u64* byteIndexPntr, u64 valueSize, void* valueOutPntr)
#define BinDeser_ReadU8(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU16(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU32(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadU64(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI8(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI16(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI32(dataPntr, dataSize, byteIndexPntr, variableOut)
#define BinDeser_ReadI64(dataPntr, dataSize, byteIndexPntr, variableOut)
#define StreamStaticDeser_ReadStruct(streamPntr, type)
#define StreamArenaDeser_ReadStruct(streamPntr, memArena, type)
#define StreamDeser_ReadStruct(streamPntr, maybeArena, type)
#define StreamStaticDeser_ReadBytes(streamPntr, numBytes)
#define StreamArenaDeser_ReadBytes(streamPntr, memArena, numBytes)
#define StreamDeser_ReadBytes(streamPntr, maybeArena, numBytes)
#define StreamStaticDeser_ReadStructDynamicSize(streamPntr, structSizeOut, type, sizeMemberName, firstOptionalMemberName)
#define StreamArenaDeser_ReadStructDynamicSize(streamPntr, memArena, structSizeOut, type, sizeMemberName, firstOptionalMemberName)
#define StreamDeser_ReadStructDynamicSize(streamPntr, maybeArena, structSizeOut, type, sizeMemberName, firstOptionalMemberName)
#define StreamStaticDeser_ReadU8(streamPntr, variableOut)
#define StreamStaticDeser_ReadU16(streamPntr, variableOut)
#define StreamStaticDeser_ReadU32(streamPntr, variableOut)
#define StreamStaticDeser_ReadU64(streamPntr, variableOut)
#define StreamStaticDeser_ReadI8(streamPntr, variableOut)
#define StreamStaticDeser_ReadI16(streamPntr, variableOut)
#define StreamStaticDeser_ReadI32(streamPntr, variableOut)
#define StreamStaticDeser_ReadI64(streamPntr, variableOut)
#define StreamArenaDeser_ReadU8(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadU16(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadU32(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadU64(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadI8(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadI16(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadI32(streamPntr, memArena, variableOut)
#define StreamArenaDeser_ReadI64(streamPntr, memArena, variableOut)
#define StreamDeser_ReadU8(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadU16(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadU32(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadU64(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadI8(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadI16(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadI32(streamPntr, maybeArena, variableOut)
#define StreamDeser_ReadI64(streamPntr, maybeArena, variableOut)
*/
