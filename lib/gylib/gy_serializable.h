/*
File:   gy_serializable.h
Author: Taylor Robbins
Date:   07\22\2023
	** A serializable is simply a bundle of a pointer to some data and functions to serialize/deserialize that data to/from a byte stream
	** NOTE: Serializables are not meant to be held across frame boundaries, partially because of the function pointers in SerializableFuncs_t and also because the structPntr and contextPntr are allowed to be pointing to stack variables or other temporary structs
*/

#ifndef _GY_SERIALIZABLE_H
#define _GY_SERIALIZABLE_H

#define MAX_JOINED_SERIALIZABLES 8

// +--------------------------------------------------------------+
// |                    Function Pointer Types                    |
// +--------------------------------------------------------------+
//NOTE: Serialize is meant to handle memArena == nullptr by just returning a MyStr_t that has length declared but no data pntr
#define SERIALIZE_FUNC_DEFINITION(functionName) MyStr_t functionName(u64 structSize, const void* structPntr, MemArena_t* memArena, void* contextPntr)
typedef SERIALIZE_FUNC_DEFINITION(SerializeFunc_f);

#define DESERIALIZE_FUNC_DEFINITION(functionName) bool functionName(MyStr_t serializedData, u64 structOutSize, void* structOutPntr, MemArena_t* memArena, void* contextPntr)
typedef DESERIALIZE_FUNC_DEFINITION(DeserializeFunc_f);

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct SerializableFuncs_t
{
	SerializeFunc_f* Serialize;
	DeserializeFunc_f* Deserialize;
};
struct Serializable_t
{
	SerializableFuncs_t funcs;
	u64 structSize;
	void* structPntr;
	void* contextPntr;
};

struct JoinedSerializableContext_t
{
	u64 numSerializables;
	Serializable_t serializables[MAX_JOINED_SERIALIZABLES];
};

SERIALIZE_FUNC_DEFINITION(Serialize_JoinedSerializable);
DESERIALIZE_FUNC_DEFINITION(Deserialize_JoinedSerializable);
#ifdef GYLIB_HEADER_ONLY
extern SerializableFuncs_t SzFuncs_JoinedSerializable;
#else
SerializableFuncs_t SzFuncs_JoinedSerializable = { Serialize_JoinedSerializable, Deserialize_JoinedSerializable };
#endif

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define NewSerializable(funcs, structPntr)                         NewSerializable_((funcs), sizeof(*(structPntr)), (structPntr))
#define NewSerializableWithContext(funcs, structPntr, contextPntr) NewSerializable_((funcs), sizeof(*(structPntr)), (structPntr), (contextPntr)) 

#define Serializable_Empty { { nullptr, nullptr }, 0, nullptr, nullptr }

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Serializable_t NewSerializable_(SerializableFuncs_t funcs, u64 structSize, void* structPntr, void* contextPntr = nullptr);
	Serializable_t NewSerializable_(SerializableFuncs_t funcs, u64 structSize, const void* structPntr, void* contextPntr = nullptr);
	Serializable_t NewJoinedSerializable_(JoinedSerializableContext_t* context);
	Serializable_t NewJoinedSerializableEmpty(JoinedSerializableContext_t* contextStructPntr);
	Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2);
	Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2, Serializable_t serializable3);
	Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2, Serializable_t serializable3, Serializable_t serializable4);
	void JoinedSerializableAppend(Serializable_t* joinedSerializable, Serializable_t newSerializable);
	bool AreSerializablesEqual(const Serializable_t left, const Serializable_t right);
	bool IsEmpty(const Serializable_t serializable);
	MyStr_t Serialize(Serializable_t serializable, MemArena_t* memArena);
	bool Deserialize(Serializable_t serializable, MyStr_t serializedData, MemArena_t* memArena = nullptr);
	SERIALIZE_FUNC_DEFINITION(Serialize_JoinedSerializable);
	DESERIALIZE_FUNC_DEFINITION(Deserialize_JoinedSerializable);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
Serializable_t NewSerializable_(SerializableFuncs_t funcs, u64 structSize, void* structPntr, void* contextPntr = nullptr)
{
	Serializable_t result = {};
	result.funcs = funcs;
	result.structSize = structSize;
	result.structPntr = structPntr;
	result.contextPntr = contextPntr;
	return result;
}
Serializable_t NewSerializable_(SerializableFuncs_t funcs, u64 structSize, const void* structPntr, void* contextPntr = nullptr) //const-variant
{
	return NewSerializable_(funcs, structSize, (void*)structPntr, contextPntr);
}

// +--------------------------------------------------------------+
// |                    New JoinedSerializable                    |
// +--------------------------------------------------------------+
Serializable_t NewJoinedSerializable_(JoinedSerializableContext_t* context) { return NewSerializable(SzFuncs_JoinedSerializable, context); }

Serializable_t NewJoinedSerializableEmpty(JoinedSerializableContext_t* contextStructPntr)
{
	NotNull(contextStructPntr);
	ClearPointer(contextStructPntr);
	contextStructPntr->numSerializables = 0;
	return NewJoinedSerializable_(contextStructPntr);
}
Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2)
{
	NotNull(contextStructPntr);
	ClearPointer(contextStructPntr);
	contextStructPntr->numSerializables = 2;
	contextStructPntr->serializables[0] = serializable1;
	contextStructPntr->serializables[1] = serializable2;
	return NewJoinedSerializable_(contextStructPntr);
}
Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2, Serializable_t serializable3)
{
	NotNull(contextStructPntr);
	ClearPointer(contextStructPntr);
	contextStructPntr->numSerializables = 3;
	contextStructPntr->serializables[0] = serializable1;
	contextStructPntr->serializables[1] = serializable2;
	contextStructPntr->serializables[2] = serializable3;
	return NewJoinedSerializable_(contextStructPntr);
}
Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2, Serializable_t serializable3, Serializable_t serializable4)
{
	NotNull(contextStructPntr);
	ClearPointer(contextStructPntr);
	contextStructPntr->numSerializables = 4;
	contextStructPntr->serializables[0] = serializable1;
	contextStructPntr->serializables[1] = serializable2;
	contextStructPntr->serializables[2] = serializable3;
	contextStructPntr->serializables[3] = serializable4;
	return NewJoinedSerializable_(contextStructPntr);
}

void JoinedSerializableAppend(Serializable_t* joinedSerializable, Serializable_t newSerializable)
{
	NotNull(joinedSerializable);
	Assert(joinedSerializable->structSize == sizeof(JoinedSerializableContext_t));
	NotNull(joinedSerializable->structPntr);
	JoinedSerializableContext_t* joinedContext = (JoinedSerializableContext_t*)joinedSerializable->structPntr;
	Assert(joinedContext->numSerializables < MAX_JOINED_SERIALIZABLES);
	joinedContext->serializables[joinedContext->numSerializables] = newSerializable;
	joinedContext->numSerializables++;
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
bool AreSerializablesEqual(const Serializable_t left, const Serializable_t right)
{
	return (
		left.funcs.Serialize == right.funcs.Serialize &&
		left.funcs.Deserialize == right.funcs.Deserialize &&
		left.structSize == right.structSize &&
		left.structPntr == right.structPntr &&
		left.contextPntr == right.contextPntr
	);
}
bool IsEmpty(const Serializable_t serializable)
{
	return AreSerializablesEqual(serializable, Serializable_Empty);
}

// +--------------------------------------------------------------+
// |          Serialize/Deserialize Quickhand Functions           |
// +--------------------------------------------------------------+
MyStr_t Serialize(Serializable_t serializable, MemArena_t* memArena)
{
	NotNull(serializable.funcs.Serialize);
	NotNull(serializable.structPntr);
	return serializable.funcs.Serialize(serializable.structSize, serializable.structPntr, memArena, serializable.contextPntr);
}
bool Deserialize(Serializable_t serializable, MyStr_t serializedData, MemArena_t* memArena = nullptr)
{
	NotNull(serializable.funcs.Deserialize);
	NotNull(serializable.structPntr);
	return serializable.funcs.Deserialize(serializedData, serializable.structSize, serializable.structPntr, memArena, serializable.contextPntr);
}

// +--------------------------------------------------------------+
// |                   JoinedSerializable Funcs                   |
// +--------------------------------------------------------------+
// +==============================+
// | Serialize_JoinedSerializable |
// +==============================+
SERIALIZE_FUNC_DEFINITION(Serialize_JoinedSerializable)
{
	UNUSED(contextPntr);
	Assert(structSize == sizeof(JoinedSerializableContext_t));
	NotNull(structPntr);
	JoinedSerializableContext_t* joinedContext = (JoinedSerializableContext_t*)structPntr;
	u64 numBytesNeeded = 0;
	for (u64 sIndex = 0; sIndex < joinedContext->numSerializables; sIndex++)
	{
		numBytesNeeded += Serialize(joinedContext->serializables[sIndex], nullptr).length;
	}
	if (memArena == nullptr) { return NewStrLengthOnly(numBytesNeeded); }
	MyStr_t result;
	result.length = numBytesNeeded;
	result.bytes = AllocArray(memArena, u8, result.length);
	NotNull(result.bytes);
	u64 writeIndex = 0;
	for (u64 sIndex = 0; sIndex < joinedContext->numSerializables; sIndex++)
	{
		MemArena_t bufferArena;
		InitMemArena_Buffer(&bufferArena, result.length - writeIndex, &result.bytes[writeIndex], true);
		MyStr_t serializedStr = Serialize(joinedContext->serializables[sIndex], &bufferArena);
		writeIndex += serializedStr.length;
	}
	Assert(writeIndex == result.length);
	return MyStr_Empty;
}
// +================================+
// | Deserialize_JoinedSerializable |
// +================================+
//NOTE: For now, you must free all the serializable structures if deserialization fails and memArena is used for allocations!
DESERIALIZE_FUNC_DEFINITION(Deserialize_JoinedSerializable)
{
	UNUSED(contextPntr);
	//TODO: We have a problem in all the cases where we return false after staring the for loop, things may have been allocated with memArena in serializables that succeeded up to that point! Somehow they need to be freed!
	//TODO: Maybe we could add another optional function to SerializableFuncs_t that does Cleanup/Freeing when deserialization fails?
	Assert(structOutSize == sizeof(JoinedSerializableContext_t));
	NotNull(structOutPntr);
	JoinedSerializableContext_t* joinedContext = (JoinedSerializableContext_t*)structOutPntr;
	u64 readIndex = 0;
	for (u64 sIndex = 0; sIndex < joinedContext->numSerializables; sIndex++)
	{
		if (readIndex >= serializedData.length) { return false; }
		
		//NOTE: For now, we just keep asking the deserializer to deserialize smaller and smaller chunks of the leftover data until it succeeds since we don't know how many bytes it wants beforehand.
		//TODO: We should add another function to SerializableFuncs_t that allows us to ask the number of bytes that want to be consumed by the deserializer?
		u64 numBytesLeft = serializedData.length - readIndex;
		while (numBytesLeft > 0)
		{
			if (Deserialize(joinedContext->serializables[sIndex], StrSubstring(&serializedData, readIndex, numBytesLeft), memArena))
			{
				readIndex += numBytesLeft;
				break;
			}
			numBytesLeft--;
		}
		
		if (numBytesLeft == 0) { return false; }
	}
	if (readIndex < serializedData.length) { return false; }
	return false;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_SERIALIZABLE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MAX_JOINED_SERIALIZABLES
Serializable_Empty
@Types
SerializeFunc_f
DeserializeFunc_f
SerializableFuncs_t
Serializable_t
JoinedSerializableContext_t
@Globals
SzFuncs_JoinedSerializable
@Functions
MyStr_t SERIALIZE_FUNC_DEFINITION(u64 structSize, const void* structPntr, MemArena_t* memArena, void* contextPntr)
bool DESERIALIZE_FUNC_DEFINITION(MyStr_t serializedData, u64 structOutSize, void* structOutPntr, MemArena_t* memArena, void* contextPntr)
#define NewSerializable(funcs, structPntr)
#define NewSerializableWithContext(funcs, structPntr, contextPntr)
Serializable_t NewSerializable_(SerializableFuncs_t funcs, u64 structSize, void* structPntr, void* contextPntr = nullptr)
Serializable_t NewJoinedSerializableEmpty(JoinedSerializableContext_t* contextStructPntr)
Serializable_t NewJoinedSerializable(JoinedSerializableContext_t* contextStructPntr, Serializable_t serializable1, Serializable_t serializable2, Serializable_t serializable3 = {}, Serializable_t serializable4 = {})
void JoinedSerializableAppend(Serializable_t* joinedSerializable, Serializable_t newSerializable)
bool AreSerializablesEqual(const Serializable_t left, const Serializable_t right)
bool IsEmpty(const Serializable_t serializable)
MyStr_t Serialize(Serializable_t serializable, MemArena_t* memArena)
bool Deserialize(Serializable_t serializable, MyStr_t serializedData, MemArena_t* memArena = nullptr)
MyStr_t Serialize_JoinedSerializable(u64 structSize, const void* structPntr, MemArena_t* memArena, void* contextPntr)
bool Deserialize_JoinedSerializable(MyStr_t serializedData, u64 structOutSize, void* structOutPntr, MemArena_t* memArena, void* contextPntr)
*/
