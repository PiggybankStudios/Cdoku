/*
File:   gy_cereal.h
Author: Taylor Robbins
Date:   02\17\2024
Description: 
	** The "Cereal Engine" is a system by which we serialize and deserialize various
	** types to binary file formats in a backwards compatible way (using simple versioning),
	** and with compositional structure (types can exist in multiple file formats)
NOTE: Crl stands for Cereal
*/

//NOTE: "Optional" data is data that was added in later versions of the binary format.
// The "structSize" member tells us the size of the structure when it was serialized
// Anything that at an offset above the structSize is considered to be omitted
// The "optional split" is between the "original" version of the structure and the first optional member

//TODO: Remove array count embedding, let the application handle storing array counts somewhere. Or maybe just give them an option? We would need a callback or something to do a bit of work when the array count is read for the first time
//TODO: Add support for context values that get deallocated automatically
//TODO: How do we fill in information from later serialization processes, earlier in the file? Like having file offsets written to a header?
//TODO: Test all the error scenarios to make sure they are working properly (Write some unit tests maybe?)
//TODO: Test adding new versions of registered types (can we remove the need for a Serialize implementation for every version but the newest?)
//TODO: Add support for skipping tasks that are pushed that have not been registered?
//TODO: Add a debug mode that tracks who pushes what context and what tasks, and outputs a graphviz file so we can visualize the dependency graph

#ifndef _GY_CEREAL_H
#define _GY_CEREAL_H

#if GYLIB_SCRATCH_ARENA_AVAILABLE

#define CRL_TYPE_DESIGNATION_SIZE 4 //chars
#define CRL_CONTEXT_MAX_VALUE_SIZE 8 //bytes

struct CrlVersion_t
{
	u8 major;
	u8 minor;
};

enum CrlError_t
{
	CrlError_None = 0,
	CrlError_IncompleteData,
	CrlError_InvalidDesignation,
	CrlError_EmptySerialization,
	CrlError_StructSizeTooSmall,
	CrlError_StructSizeTooLarge,
	CrlError_NumErrors,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetCrlErrorStr(CrlError_t enumValue);
#else
const char* GetCrlErrorStr(CrlError_t enumValue)
{
	switch (enumValue)
	{
		case CrlError_None:               return "None";
		case CrlError_IncompleteData:     return "IncompleteData";
		case CrlError_InvalidDesignation: return "InvalidDesignation";
		case CrlError_EmptySerialization: return "EmptySerialization";
		case CrlError_StructSizeTooSmall: return "StructSizeTooSmall";
		case CrlError_StructSizeTooLarge: return "StructSizeTooLarge";
		default: return "Unknown";
	}
}
#endif

struct CrlEngine_t;
#define CRL_SERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, u64 arrayIndex, u64 runtimeItemSize, const void* runtimeItemPntr, u64 structSize, u8* structPntr)
typedef CRL_SERIALIZE_DEF(CrlSerialize_f);
#define CRL_DESERIALIZE_DEF(functionName) bool functionName(CrlEngine_t* crl, MemArena_t* memArena, MemArena_t* scratch, u64 arrayIndex, u64 structSize, const u8* structPntr)
typedef CRL_DESERIALIZE_DEF(CrlDeserialize_f);

struct CrlRegisteredTypeVersion_t
{
	CrlVersion_t version;
	char designation[CRL_TYPE_DESIGNATION_SIZE];
	bool customReadingLogic;
	u64 serializedSize; //Max is UINT16_MAX (unless customReadingLogic is used)
	u64 minimumSize;
	CrlSerialize_f* Serialize;
	CrlDeserialize_f* Deserialize;
	#if DEBUG_BUILD
	MyStr_t debugName;
	#endif
};

struct CrlRegisteredType_t
{
	u64 index;
	VarArray_t versions; //CrlRegisteredTypeVersion_t
};

struct CrlContextEntry_t
{
	u64 index;
	bool keepForSecondPass;
	bool isFilled;
	bool isAllocated;
	u64 size;
	u64 count;
	union
	{
		u8 valueBytes[CRL_CONTEXT_MAX_VALUE_SIZE];
		void* pntr;
		i8 valueI8;
		i16 valueI16;
		i32 valueI32;
		i64 valueI64;
		u8 valueU8;
		u16 valueU16;
		u32 valueU32;
		u64 valueU64;
		r32 valueR32;
		r64 valueR64;
		bool valueBool;
	};
};

struct CrlTask_t
{
	CrlRegisteredType_t* type;
	CrlRegisteredTypeVersion_t* typeVersion;
	bool isArray;
	u64 arraySize; //(Optional in Deser)
	
	//Serialization Only
	bool isRuntimeVarArray;
	bool isAllocated;
	u64 runtimeItemSize;
	const void* runtimeItemPntr;
	
	//Deserialization Only
	u64 predeclaredSize; //(Optional)
	
	bool started;
	u64 taskStartFileOffset;
	u64 itemStartFileOffset;
	u16 deserStructSize;
	u64 progressIndex;
};

struct CrlEngine_t
{
	MemArena_t* allocArena;
	MemArena_t* serializedOutputArena;
	MemArena_t* deserOutputArena;
	bool isDeserializing;
	CrlVersion_t version;
	bool onSecondSerializationPass;
	
	//serializing
	u64 outputSize;
	u8* outputPntr;
	u64 writeIndex;
	
	//deserializing
	Stream_t* inputStream;
	ProcessLog_t* log;
	
	u64 numRegisteredTypes;
	u64 numRegisteredTypesAlloc;
	CrlRegisteredType_t* registeredTypes;
	
	u64 numContextEntries;
	CrlContextEntry_t* contextEntries;
	
	VarArray_t taskStack; //CrlTask_t
	CrlTask_t* currentTask;
};

#define CrlVersion_Zero       NewCrlVersion(0, 0)
#define CrlVersion_Zero_Const { 0, 0 }
#define CrlVersion_Max        NewCrlVersion(255, 255)
#define CrlVersion_Max_Const  { 255, 255 }

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define CrlPushContext(crl, index, typedPntr, ...) CrlPushContext_((crl), (index), sizeof(*(typedPntr)), (void*)(typedPntr), ##__VA_ARGS__)
#define CrlPushContextArray(crl, index, type, count, ...) (type*)CrlPushContextArray_((crl), (index), sizeof(type), (count), ##__VA_ARGS__)
#define CrlPushContextI8(crl, index, value, ...)   do { i8   _valueI8   = (value); CrlPushContextValue_((crl), (index), sizeof(i8),   &_valueI8,   ##__VA_ARGS__); } while(0)
#define CrlPushContextI16(crl, index, value, ...)  do { i16  _valueI16  = (value); CrlPushContextValue_((crl), (index), sizeof(i16),  &_valueI16,  ##__VA_ARGS__); } while(0)
#define CrlPushContextI32(crl, index, value, ...)  do { i32  _valueI32  = (value); CrlPushContextValue_((crl), (index), sizeof(i32),  &_valueI32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextI64(crl, index, value, ...)  do { i64  _valueI64  = (value); CrlPushContextValue_((crl), (index), sizeof(i64),  &_valueI64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU8(crl, index, value, ...)   do { u8   _valueU8   = (value); CrlPushContextValue_((crl), (index), sizeof(u8),   &_valueU8,   ##__VA_ARGS__); } while(0)
#define CrlPushContextU16(crl, index, value, ...)  do { u16  _valueU16  = (value); CrlPushContextValue_((crl), (index), sizeof(u16),  &_valueU16,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU32(crl, index, value, ...)  do { u32  _valueU32  = (value); CrlPushContextValue_((crl), (index), sizeof(u32),  &_valueU32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextU64(crl, index, value, ...)  do { u64  _valueU64  = (value); CrlPushContextValue_((crl), (index), sizeof(u64),  &_valueU64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextR32(crl, index, value, ...)  do { r32  _valueR32  = (value); CrlPushContextValue_((crl), (index), sizeof(r32),  &_valueR32,  ##__VA_ARGS__); } while(0)
#define CrlPushContextR64(crl, index, value, ...)  do { r64  _valueR64  = (value); CrlPushContextValue_((crl), (index), sizeof(r64),  &_valueR64,  ##__VA_ARGS__); } while(0)
#define CrlPushContextBool(crl, index, value, ...) do { bool _valueBool = (value); CrlPushContextValue_((crl), (index), sizeof(bool), &_valueBool, ##__VA_ARGS__); } while(0)

#define CrlGetContextHard(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), true)
#define CrlGetContextSoft(crl, index, type) (type*)CrlGetContext_((crl), (index), sizeof(type), false)
#define CrlGetContext(crl, index, type) CrlGetContextHard((crl), (index), type)

#define CrlGetContextArrayHard(crl, index, type, countOut) (type*)CrlGetContextArray_((crl), (index), sizeof(type), (countOut), true)
#define CrlGetContextArraySoft(crl, index, type, countOut) (type*)CrlGetContextArray_((crl), (index), sizeof(type), (countOut), false)
#define CrlGetContextArray(crl, index, type, countOut) CrlGetContextArrayHard((crl), (index), type, (countOut))

#define CrlGetContextI8(crl, index)   CrlGetContextRaw_((crl), (index), sizeof(i8), true)->valueI8
#define CrlGetContextI16(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i16), true)->valueI16
#define CrlGetContextI32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i32), true)->valueI32
#define CrlGetContextI64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(i64), true)->valueI64
#define CrlGetContextU8(crl, index)   CrlGetContextRaw_((crl), (index), sizeof(u8), true)->valueU8
#define CrlGetContextU16(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u16), true)->valueU16
#define CrlGetContextU32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u32), true)->valueU32
#define CrlGetContextU64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(u64), true)->valueU64
#define CrlGetContextR32(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(r32), true)->valueR32
#define CrlGetContextR64(crl, index)  CrlGetContextRaw_((crl), (index), sizeof(r64), true)->valueR64
#define CrlGetContextBool(crl, index) CrlGetContextRaw_((crl), (index), sizeof(bool), true)->valueBool

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	CrlVersion_t NewCrlVersion(u8 major, u8 minor);
	bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false);
	bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false);
	bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right);
	MyStr_t CrlGetDebugStackString(CrlEngine_t* crl, MemArena_t* memArena);
	void FreeCrlTask(CrlEngine_t* crl, CrlTask_t* task);
	void FreeCrlEngine(CrlEngine_t* crl);
	void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream);
	void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* deserOutputArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream);
	void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* serializedOutputArena, u64 numTypes, u64 numContextEntries);
	CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index);
	CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true);
	CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, const char* debugName, u64 index, MyStr_t designation, bool customReadingLogic, CrlVersion_t version, u64 serializedSize, u64 minimumSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc);
	void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false, bool allocate = false, bool keepForSecondPass = false);
	void* CrlPushContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64 itemCount, bool allowOverwrite = false, bool keepForSecondPass = false);
	void CrlPushContextValue_(CrlEngine_t* crl, u64 index, u64 valueSize, const void* valuePntr, bool allowOverwrite = false, bool keepForSecondPass = false);
	void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure);
	void* CrlGetContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64* countOut, bool assertOnFailure);
	CrlContextEntry_t* CrlGetContextRaw_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure);
	CrlTask_t* CrlPushTask(CrlEngine_t* crl, u64 typeIndex, bool isArray, bool isVarArray, u64 arraySize, const void* runtimeItemPntr, u64 runtimeItemSize, u64 predeclaredSize);
	CrlTask_t* CrlPushSingleTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize);
	CrlTask_t* CrlPushArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize);
	CrlTask_t* CrlPushVarArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray);
	CrlTask_t* CrlPushExplicitArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, u64 runtimeItemSize, u64 arraySize);
	void CrlPushExplicitArrayItemSer(CrlEngine_t* crl, CrlTask_t* explicitArrayTask, void* runtimeItemPntr);
	CrlTask_t* CrlPushSingleTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredSize = 0);
	CrlTask_t* CrlPushArrayTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredArraySize = 0, u64 predeclaredSize = 0);
	bool CrlEngineRun(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr, u64 firstTaskPredeclaredSize);
	bool CrlEngineSerialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr);
	bool CrlEngineDeserialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskPredeclaredSize = 0);
	MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl);
#else

// +--------------------------------------------------------------+
// |                      Version Functions                       |
// +--------------------------------------------------------------+
CrlVersion_t NewCrlVersion(u8 major, u8 minor)
{
	CrlVersion_t result;
	result.major = major;
	result.minor = minor;
	return result;
}
bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
{
	if (left.major > right.major) { return true; }
	else if (left.major == right.major)
	{
		if (left.minor > right.minor) { return true; }
		else if (allowEqual && left.minor == right.minor) { return true; }
		else { return false; }
	}
	else { return false; }
}
bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
{
	if (left.major < right.major) { return true; }
	else if (left.major == right.major)
	{
		if (left.minor < right.minor) { return true; }
		else if (allowEqual && left.minor == right.minor) { return true; }
		else { return false; }
	}
	else { return false; }
}
bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right)
{
	return (left.major == right.major && left.minor == right.minor);
}

// +--------------------------------------------------------------+
// |                    Information Functions                     |
// +--------------------------------------------------------------+
MyStr_t CrlGetDebugStackString(CrlEngine_t* crl, MemArena_t* memArena)
{
	NotNull(crl);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	VarArrayLoop(&crl->taskStack, tIndex)
	{
		VarArrayLoopGet(CrlTask_t, task, &crl->taskStack, tIndex);
		if (tIndex > 0) { StringBuilderAppend(&builder, " > "); }
		#if DEBUG_BUILD
		StringBuilderAppend(&builder, task->typeVersion->debugName);
		#else
		StringBuilderAppendPrint(&builder, "%llu", task->type->index);
		#endif
		if (task->isArray)
		{
			StringBuilderAppendPrint(&builder, "[%llu/%llu]", task->progressIndex, task->arraySize);
		}
	}
	return ToMyStr(&builder);
}

// +--------------------------------------------------------------+
// |                       Free and Create                        |
// +--------------------------------------------------------------+
void FreeCrlTask(CrlEngine_t* crl, CrlTask_t* task)
{
	NotNull2(crl, task);
	if (task->isAllocated)
	{
		NotNull(crl->allocArena);
		Assert(task->isArray);
		Assert(task->arraySize > 0);
		FreeMem(crl->allocArena, (void*)task->runtimeItemPntr, sizeof(void*) * task->arraySize);
	}
	ClearPointer(task);
}
void FreeCrlEngine(CrlEngine_t* crl)
{
	NotNull(crl);
	if (crl->registeredTypes != nullptr)
	{
		NotNull(crl->allocArena);
		for (u64 tIndex = 0; tIndex < crl->numRegisteredTypes; tIndex++)
		{
			CrlRegisteredType_t* registeredType = &crl->registeredTypes[tIndex];
			FreeVarArray(&registeredType->versions);
		}
		FreeMem(crl->allocArena, crl->registeredTypes, sizeof(CrlRegisteredType_t) * crl->numRegisteredTypesAlloc);
	}
	if (crl->contextEntries != nullptr)
	{
		NotNull(crl->allocArena);
		for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
		{
			CrlContextEntry_t* contextEntry = &crl->contextEntries[cIndex];
			if (contextEntry->isFilled && contextEntry->isAllocated)
			{
				FreeMem(crl->allocArena, contextEntry->pntr, contextEntry->size * contextEntry->count);
			}
		}
		FreeMem(crl->allocArena, crl->contextEntries, sizeof(CrlContextEntry_t) * crl->numContextEntries);
	}
	if (crl->outputPntr != nullptr)
	{
		MemArena_t* outputArena = ((crl->serializedOutputArena != nullptr) ? crl->serializedOutputArena : crl->allocArena);
		NotNull(outputArena);
		FreeMem(outputArena, crl->outputPntr, crl->outputSize);
	}
	FreeVarArray(&crl->taskStack);
	ClearPointer(crl);
}

void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
{
	NotNull(crl);
	AssertIf(deserializing, processLog != nullptr);
	AssertIf(deserializing, stream != nullptr);
	ClearPointer(crl);
	crl->allocArena = memArena;
	crl->version = version;
	crl->log = processLog;
	crl->inputStream = stream;
	crl->isDeserializing = deserializing;
	crl->numRegisteredTypesAlloc = numTypes;
	if (numTypes > 0)
	{
		crl->registeredTypes = AllocArray(crl->allocArena, CrlRegisteredType_t, crl->numRegisteredTypesAlloc);
		NotNull(crl->registeredTypes);
	}
	crl->numContextEntries = numContextEntries;
	if (crl->numContextEntries > 0)
	{
		crl->contextEntries = AllocArray(crl->allocArena, CrlContextEntry_t, crl->numContextEntries);
		NotNull(crl->contextEntries);
		for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
		{
			CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
			ClearPointer(entry);
			entry->index = cIndex;
		}
	}
	CreateVarArray(&crl->taskStack, crl->allocArena, sizeof(CrlTask_t));
}
//We need to know which arena is being used, so we don't share scratch arenas, but CrlEngine generally won't do anything with deserOutputArena directly. Each task can use it however it wants
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* deserOutputArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
{
	NotNull2(processLog, stream);
	CreateCrlEngine(crl, true, version, memArena, numTypes, numContextEntries, processLog, stream);
	crl->deserOutputArena = deserOutputArena;
}
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* serializedOutputArena, u64 numTypes, u64 numContextEntries)
{
	CreateCrlEngine(crl, false, version, memArena, numTypes, numContextEntries, nullptr, nullptr);
	crl->serializedOutputArena = serializedOutputArena;
}

// +--------------------------------------------------------------+
// |                         Registration                         |
// +--------------------------------------------------------------+
CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index)
{
	Assert(index < crl->numRegisteredTypes);
	return &crl->registeredTypes[index];
}
CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true)
{
	CrlRegisteredType_t* type = CrlGetType(crl, index);
	CrlRegisteredTypeVersion_t* result = nullptr;
	VarArrayLoop(&type->versions, vIndex)
	{
		VarArrayLoopGet(CrlRegisteredTypeVersion_t, versionPntr, &type->versions, vIndex);
		if (IsCrlVersionEqual(versionPntr->version, version)) { return versionPntr; }
		else if (allowLowerVersions && IsCrlVersionLessThan(versionPntr->version, version)) { result = versionPntr; }
	}
	return result;
}

//The index must increase with each registration, OR stay the same and the version must then increase by some amount
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, const char* debugName,
	u64 index, MyStr_t designation, bool customReadingLogic, CrlVersion_t version,
	u64 serializedSize, u64 minimumSize,
	CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
{
	UNUSED(debugName);
	Assert(crl->numRegisteredTypes < crl->numRegisteredTypesAlloc);
	Assert(index >= crl->numRegisteredTypes || index == crl->numRegisteredTypes-1);
	NotNull2(serializeFunc, deserializeFunc);
	Assert(designation.length == CRL_TYPE_DESIGNATION_SIZE || designation.length == 0);
	Assert(serializedSize > 0);
	//We actual encode sizes as u16 so it can't be bigger than that, unless customReadingLogic is used
	AssertIf(!customReadingLogic, serializedSize <= UINT16_MAX);
	
	CrlRegisteredType_t* type = nullptr;
	if (index == crl->numRegisteredTypes-1)
	{
		CrlRegisteredType_t* existingType = &crl->registeredTypes[index];
		#if ASSERTIONS_ENABLED
		if (existingType->versions.length > 0)
		{
			CrlRegisteredTypeVersion_t* prevVersion = VarArrayGetHard(&existingType->versions, existingType->versions.length-1, CrlRegisteredTypeVersion_t);
			Assert(IsCrlVersionGreaterThan(version, prevVersion->version));
		}
		#endif
		type = existingType;
	}
	else
	{
		while (crl->numRegisteredTypes < index)
		{
			ClearPointer(&crl->registeredTypes[crl->numRegisteredTypes]);
			CreateVarArray(&crl->registeredTypes[crl->numRegisteredTypes].versions, crl->allocArena, sizeof(CrlRegisteredTypeVersion_t));
			crl->numRegisteredTypes++;
		}
		
		CrlRegisteredType_t* newType = &crl->registeredTypes[crl->numRegisteredTypes];
		crl->numRegisteredTypes++;
		ClearPointer(newType);
		CreateVarArray(&newType->versions, crl->allocArena, sizeof(CrlRegisteredTypeVersion_t), 1);
		type = newType;
	}
	
	CrlRegisteredTypeVersion_t* newVersion = VarArrayAdd(&type->versions, CrlRegisteredTypeVersion_t);
	NotNull(newVersion);
	ClearPointer(newVersion);
	newVersion->version = version;
	if (designation.length == CRL_TYPE_DESIGNATION_SIZE) { MyMemCopy(&newVersion->designation, designation.chars, CRL_TYPE_DESIGNATION_SIZE); }
	else { newVersion->designation[0] = '\0'; }
	newVersion->customReadingLogic = customReadingLogic;
	newVersion->serializedSize = serializedSize;
	newVersion->minimumSize = minimumSize;
	newVersion->Serialize = serializeFunc;
	newVersion->Deserialize = deserializeFunc;
	
	#if DEBUG_BUILD
	newVersion->debugName = NewStr(debugName);
	#endif
	
	return type;
}

// +--------------------------------------------------------------+
// |                           Context                            |
// +--------------------------------------------------------------+
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false, bool allocate = false, bool keepForSecondPass = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	AssertIf(pntr != nullptr, size > 0);
	AssertIf(pntr == nullptr, keepForSecondPass == false);
	AssertIf(pntr == nullptr, allocate == false);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled && entry->isAllocated) { FreeMem(crl->allocArena, entry->pntr, entry->size * entry->count); }
	if (pntr != nullptr)
	{
		AssertIf(!allowOverwrite, !entry->isFilled);
		entry->size = size;
		entry->count = 1;
		entry->pntr = pntr;
		if (allocate)
		{
			entry->pntr = AllocMem(crl->allocArena, size);
			NotNull(entry->pntr);
			MyMemCopy(entry->pntr, pntr, size);
		}
		entry->keepForSecondPass = keepForSecondPass;
		entry->isAllocated = allocate;
		entry->isFilled = true;
	}
	else
	{
		AssertIf(!allowOverwrite, entry->isFilled);
		entry->size = 0;
		entry->count = 0;
		entry->pntr = nullptr;
		entry->keepForSecondPass = false;
		entry->isAllocated = false;
		entry->isFilled = false;
	}
}
void* CrlPushContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64 itemCount, bool allowOverwrite = false, bool keepForSecondPass = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	Assert(itemSize > 0);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled && entry->isAllocated) { FreeMem(crl->allocArena, entry->pntr, entry->size * entry->count); }
	if (itemCount > 0)
	{
		AssertIf(!allowOverwrite, !entry->isFilled);
		entry->size = itemSize;
		entry->count = itemCount;
		entry->pntr = AllocMem(crl->allocArena, itemSize * itemCount);
		NotNull(entry->pntr);
		entry->keepForSecondPass = keepForSecondPass;
		entry->isAllocated = true;
		entry->isFilled = true;
		return entry->pntr;
	}
	else
	{
		// AssertIf(!allowOverwrite, entry->isFilled); //You're allowed to set a 0 size array, even if it's not filled previously, we'll just treat it as an unset value
		entry->size = 0;
		entry->count = 0;
		entry->pntr = nullptr;
		entry->keepForSecondPass = false;
		entry->isAllocated = false;
		entry->isFilled = false;
		return nullptr;
	}
}
void CrlPushContextValue_(CrlEngine_t* crl, u64 index, u64 valueSize, const void* valuePntr, bool allowOverwrite = false, bool keepForSecondPass = false)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	AssertIf(valuePntr != nullptr, valueSize > 0);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	Assert(valueSize <= CRL_CONTEXT_MAX_VALUE_SIZE);
	AssertIf(valuePntr == nullptr, keepForSecondPass == false);
	if (entry->isFilled && entry->isAllocated) { FreeMem(crl->allocArena, entry->pntr, entry->size * entry->count); }
	if (valuePntr != nullptr)
	{
		AssertIf(!allowOverwrite, !entry->isFilled);
		entry->size = valueSize;
		entry->count = 1;
		MyMemCopy(&entry->valueBytes[0], valuePntr, valueSize);
		entry->keepForSecondPass = keepForSecondPass;
		entry->isAllocated = false;
		entry->isFilled = true;
	}
	else
	{
		AssertIf(!allowOverwrite, entry->isFilled);
		entry->size = 0;
		entry->count = 0;
		MyMemSet(&entry->valueBytes[0], 0x00, CRL_CONTEXT_MAX_VALUE_SIZE);
		entry->keepForSecondPass = false;
		entry->isAllocated = false;
		entry->isFilled = false;
	}
}

void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled)
	{
		NotNull(entry->pntr);
		Assert(entry->size == size);
		return entry->pntr;
	}
	else if (assertOnFailure)
	{
		AssertMsg(false, "Failed to get CrlEngine context entry!");
	}
	return nullptr;
}
void* CrlGetContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64* countOut, bool assertOnFailure)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled)
	{
		NotNull(entry->pntr);
		Assert(entry->size == itemSize);
		SetOptionalOutPntr(countOut, entry->count);
		return entry->pntr;
	}
	else if (assertOnFailure)
	{
		AssertMsg(false, "Failed to get CrlEngine context entry!");
	}
	return nullptr;
}
CrlContextEntry_t* CrlGetContextRaw_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
{
	NotNull(crl);
	Assert(index < crl->numContextEntries);
	CrlContextEntry_t* entry = &crl->contextEntries[index];
	if (entry->isFilled)
	{
		Assert(entry->size == size);
		return entry;
	}
	else if (assertOnFailure)
	{
		AssertMsg(false, "Failed to get CrlEngine context entry!");
	}
	return nullptr;
}

// +--------------------------------------------------------------+
// |                            Tasks                             |
// +--------------------------------------------------------------+
//TODO: runtimeItemPntr and runtimeItemSize are not important for Deserialization.
//      Maybe we should just completely separate Deserializing vs. Serializing PushTask functions?
CrlTask_t* CrlPushTask(CrlEngine_t* crl, u64 typeIndex,
	bool isArray, bool isVarArray, bool isAllocated, u64 arraySize,
	const void* runtimeItemPntr, u64 runtimeItemSize, //Serialize
	u64 predeclaredSize) //Deserialize
{
	NotNull(crl);
	if (isArray && arraySize == 0) { return nullptr; } //Pushing a task with no elements is pointless
	CrlRegisteredType_t* type = CrlGetType(crl, typeIndex);
	CrlRegisteredTypeVersion_t* typeVersion = CrlGetTypeVersion(crl, typeIndex, crl->version, true);
	CrlTask_t* newTask = VarArrayAdd(&crl->taskStack, CrlTask_t);
	NotNull(newTask);
	ClearPointer(newTask);
	newTask->isArray = isArray;
	newTask->type = type;
	newTask->typeVersion = typeVersion;
	newTask->runtimeItemPntr = runtimeItemPntr;
	newTask->runtimeItemSize = runtimeItemSize;
	newTask->arraySize = arraySize;
	newTask->isRuntimeVarArray = isVarArray;
	newTask->predeclaredSize = predeclaredSize;
	newTask->isAllocated = isAllocated;
	newTask->started = false;
	
	return newTask;
}

CrlTask_t* CrlPushSingleTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
{
	Assert(crl != nullptr && !crl->isDeserializing);
	return CrlPushTask(crl, typeIndex, false, false, false, 0, runtimeItemPntr, runtimeItemSize, 0);
}
CrlTask_t* CrlPushArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize)
{
	Assert(crl != nullptr && !crl->isDeserializing);
	return CrlPushTask(crl, typeIndex, true, false, false, arraySize, runtimeItemPntr, runtimeItemSize, 0);
}
CrlTask_t* CrlPushVarArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray)
{
	Assert(crl != nullptr && !crl->isDeserializing);
	return CrlPushTask(crl, typeIndex, true, true, false, runtimeVarArray->length, runtimeVarArray, runtimeVarArray->itemSize, 0);
}
CrlTask_t* CrlPushExplicitArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, u64 runtimeItemSize, u64 arraySize)
{
	Assert(crl != nullptr && !crl->isDeserializing);
	void* itemsSpace = nullptr;
	if (arraySize > 0)
	{
		itemsSpace = AllocMem(crl->allocArena, sizeof(void*) * arraySize);
		NotNull(itemsSpace);
		MyMemSet(itemsSpace, 0x00, sizeof(void*) * arraySize);
	}
	return CrlPushTask(crl, typeIndex, true, false, true, arraySize, itemsSpace, runtimeItemSize, 0);
}
void CrlPushExplicitArrayItemSer(CrlEngine_t* crl, CrlTask_t* explicitArrayTask, const void* runtimeItemPntr, u64 arrayIndex)
{
	NotNull2(crl, explicitArrayTask);
	Assert(explicitArrayTask->isArray);
	Assert(explicitArrayTask->isAllocated);
	NotNull(explicitArrayTask->runtimeItemPntr);
	Assert(arrayIndex < explicitArrayTask->arraySize);
	void** itemPntr = &((void**)explicitArrayTask->runtimeItemPntr)[arrayIndex];
	*itemPntr = (void*)runtimeItemPntr;
}

CrlTask_t* CrlPushSingleTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredSize = 0)
{
	Assert(crl != nullptr && crl->isDeserializing);
	return CrlPushTask(crl, typeIndex, false, false, false, 0, nullptr, 0, predeclaredSize);
}
CrlTask_t* CrlPushArrayTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredArraySize = 0, u64 predeclaredSize = 0)
{
	Assert(crl != nullptr && crl->isDeserializing);
	return CrlPushTask(crl, typeIndex, true, false, false, predeclaredArraySize, nullptr, 0, predeclaredSize);
}

// +--------------------------------------------------------------+
// |                             Run                              |
// +--------------------------------------------------------------+
bool CrlEngineRun(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr, u64 firstTaskPredeclaredSize)
{
	#define CrlEngine_ReadTyped(type, varPntr, debugName) do                           \
	{                                                                                  \
		u64 _numBytesRead = StreamReadInto(crl->inputStream, sizeof(type), (varPntr)); \
		if (_numBytesRead < sizeof(type))                                              \
		{                                                                              \
			LogPrintLine_E(crl->log,                                                   \
				"Expected %u more bytes for %s before end of file. Found %llu bytes",  \
				sizeof(type),                                                          \
				(debugName),                                                           \
				_numBytesRead                                                          \
			);                                                                         \
			LogExitFailure(crl->log, CrlError_IncompleteData);                         \
			FreeScratchArena(scratch);                                                 \
			return false;                                                              \
		}                                                                              \
	} while(0)
	#define CrlEngine_ReadBytes(numBytes, pntrVar, debugName) do                \
	{                                                                           \
		pntrVar = StreamDeser_ReadBytes(crl->inputStream, scratch, (numBytes)); \
		if (pntrVar == nullptr)                                                 \
		{                                                                       \
			LogPrintLine_E(crl->log,                                            \
				"Expected %u more bytes for %s before end of file.",            \
				(numBytes),                                                     \
				(debugName)                                                     \
			);                                                                  \
			LogExitFailure(crl->log, CrlError_IncompleteData);                  \
			FreeScratchArena(scratch);                                          \
			return false;                                                       \
		}                                                                       \
	} while(0)
	
	MemArena_t* scratch = GetScratchArena(crl->allocArena, (crl->isDeserializing ? crl->deserOutputArena : crl->serializedOutputArena));
	
	for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
	{
		CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
		if (entry->isFilled) { entry->keepForSecondPass = true; }
	}
	
	u8 numPasses = (crl->isDeserializing ? 1 : 2);
	for (u8 pass = 0; pass < numPasses; pass++)
	{
		crl->writeIndex = 0;
		crl->onSecondSerializationPass = (pass > 0);
		
		if (!crl->isDeserializing)
		{
			CrlPushSingleTaskSer(crl, firstTaskTypeIndex, firstTaskRuntimeItemPntr, firstTaskRuntimeItemSize);
		}
		else
		{
			CrlPushSingleTaskDeser(crl, firstTaskTypeIndex, firstTaskPredeclaredSize);
		}
		
		while (crl->taskStack.length > 0)
		{
			u64 nextTaskIndex = crl->taskStack.length-1;
			CrlTask_t* nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
			crl->currentTask = nextTask;
			
			if (!nextTask->started)
			{
				nextTask->taskStartFileOffset = (crl->isDeserializing ? crl->inputStream->readIndex : crl->writeIndex);
				if (nextTask->isArray)
				{
					if (!crl->isDeserializing)
					{
						BinSer_WriteU64(crl->outputPntr, crl->outputSize, &crl->writeIndex, nextTask->arraySize);
					}
					else
					{
						CrlEngine_ReadTyped(u64, &nextTask->arraySize, "Array Length");
					}
				}
				if (!nextTask->typeVersion->customReadingLogic)
				{
					if (!crl->isDeserializing)
					{
						Assert(nextTask->typeVersion->serializedSize >= nextTask->typeVersion->minimumSize);
						Assert(nextTask->typeVersion->serializedSize <= UINT16_MAX);
						BinSer_WriteU16(crl->outputPntr, crl->outputSize, &crl->writeIndex, (u16)nextTask->typeVersion->serializedSize);
					}
					else
					{
						CrlEngine_ReadTyped(u16, &nextTask->deserStructSize, "Struct Size");
						if (nextTask->deserStructSize < nextTask->typeVersion->minimumSize)
						{
							MyStr_t debugName = NewStr(CRL_TYPE_DESIGNATION_SIZE, &nextTask->typeVersion->designation[0]);
							#if DEBUG_BUILD
							debugName = nextTask->typeVersion->debugName;
							#endif
							LogPrintLine_E(crl->log,
								"Declared size of struct is too small: %llu (expected at least %llu for \"%.*s\")",
								nextTask->deserStructSize,
								nextTask->typeVersion->minimumSize,
								StrPrint(debugName)
							);
							LogExitFailure(crl->log, CrlError_StructSizeTooSmall);
							FreeScratchArena(scratch);
							return false;
						}
						else if (nextTask->deserStructSize > nextTask->typeVersion->serializedSize)
						{
							MyStr_t debugName = NewStr(CRL_TYPE_DESIGNATION_SIZE, &nextTask->typeVersion->designation[0]);
							#if DEBUG_BUILD
							debugName = nextTask->typeVersion->debugName;
							#endif
							LogPrintLine_E(crl->log,
								"Declared size of struct is too large: %llu (expected at most %llu for \"%.*s\")",
								nextTask->deserStructSize,
								nextTask->typeVersion->serializedSize,
								StrPrint(debugName)
							);
							LogExitFailure(crl->log, CrlError_StructSizeTooLarge);
							FreeScratchArena(scratch);
							return false;
						}
					}
				}
				nextTask->started = true;
			}
			nextTask->itemStartFileOffset = (crl->isDeserializing ? crl->inputStream->readIndex : crl->writeIndex);
			
			// +==============================+
			// |        Serialization         |
			// +==============================+
			if (!crl->isDeserializing)
			{
				const void* runtimeItemPntr = nextTask->runtimeItemPntr;
				if (nextTask->isArray)
				{
					if (nextTask->isRuntimeVarArray)
					{
						const VarArray_t* varArray = (const VarArray_t*)nextTask->runtimeItemPntr;
						runtimeItemPntr = VarArrayGet_(varArray, nextTask->progressIndex, nextTask->runtimeItemSize, true);
					}
					else if (nextTask->isAllocated)
					{
						NotNull(nextTask->runtimeItemPntr);
						runtimeItemPntr = ((void**)nextTask->runtimeItemPntr)[nextTask->progressIndex];
					}
					else if (nextTask->runtimeItemPntr != nullptr)
					{
						runtimeItemPntr = (void*)(((u8*)nextTask->runtimeItemPntr) + (nextTask->runtimeItemSize * nextTask->progressIndex));
					}
				}
				
				if (nextTask->typeVersion->designation[0] != '\0')
				{
					BinSer_WriteBytes(crl->outputPntr, crl->outputSize, &crl->writeIndex, CRL_TYPE_DESIGNATION_SIZE, &nextTask->typeVersion->designation[0]);
				}
				
				u64 structSize = 0;
				u8* structPntr = nullptr;
				if (!nextTask->typeVersion->customReadingLogic && nextTask->typeVersion->serializedSize > 0)
				{
					structSize = nextTask->typeVersion->serializedSize;
					structPntr = (u8*)BinSer_WriteStructure_(crl->outputPntr, crl->outputSize, &crl->writeIndex, nextTask->typeVersion->serializedSize);
				}
				
				u64 numTasksBeforeSerialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Serialize(crl, nextTask->progressIndex, nextTask->runtimeItemSize, runtimeItemPntr, structSize, structPntr))
				{
					AssertIf(crl->log != nullptr, crl->log->errorCode != 0);
					FreeScratchArena(scratch);
					return false;
				}
				if (numTasksBeforeSerialize < crl->taskStack.length)
				{
					//To make it easy on the implementations, we reverse the items pushed onto the stack during
					//Serialize/Deserialize so they will get carried out in the order in which they were pushed
					VarArrayReverse(&crl->taskStack, numTasksBeforeSerialize, crl->taskStack.length);
					
					//Re-acquire the nextTask pntr into the VarArray because Deserialize might have pushed new items into the array, causing a realloc
					nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
					crl->currentTask = nextTask;
				}
				
				if (nextTask->isArray)
				{
					nextTask->progressIndex++;
					if (nextTask->progressIndex >= nextTask->arraySize)
					{
						FreeCrlTask(crl, nextTask);
						VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
					}
				}
				else
				{
					FreeCrlTask(crl, nextTask);
					VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
				}
			}
			// +==============================+
			// |       Deserialization        |
			// +==============================+
			else
			{
				PushMemMark(scratch);
				u64 numBytesRead = 0;
				
				#if DEBUG_BUILD
				MyStr_t taskStackDebugStr = CrlGetDebugStackString(crl, scratch);
				#endif
				
				if (nextTask->typeVersion->designation[0] != '\0')
				{
					u8* designationBytes = nullptr;
					if (IsFlagSet(crl->inputStream->capabilities, StreamCapability_StaticRead))
					{
						designationBytes = (u8*)StreamRead(crl->inputStream, CRL_TYPE_DESIGNATION_SIZE, &numBytesRead);
					}
					else
					{
						designationBytes = (u8*)StreamReadInArena(crl->inputStream, CRL_TYPE_DESIGNATION_SIZE, scratch, &numBytesRead);
					}
					if (designationBytes == nullptr || numBytesRead < CRL_TYPE_DESIGNATION_SIZE)
					{
						if (designationBytes == nullptr) { LogPrintLine_E(crl->log, "Failed to read %u byte designation from stream (nullptr return)", CRL_TYPE_DESIGNATION_SIZE); }
						else { LogPrintLine_E(crl->log, "Failed to read %u byte designation from stream. Only %llu bytes read", CRL_TYPE_DESIGNATION_SIZE, numBytesRead); }
						LogExitFailure(crl->log, CrlError_IncompleteData);
						PopMemMark(scratch);
						FreeScratchArena(scratch);
						return false;
					}
					if (MyMemCompare(designationBytes, &nextTask->typeVersion->designation[0], CRL_TYPE_DESIGNATION_SIZE) != 0)
					{
						MyStr_t expectedDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)&nextTask->typeVersion->designation[0]);
						MyStr_t readDesignation = NewStr(CRL_TYPE_DESIGNATION_SIZE, (char*)designationBytes);
						LogPrintLine_E(crl->log, "Invalid type designation found in file. Expected \"%.*s\", found \"%.*s\" at offset 0x%X", StrPrint(expectedDesignation), StrPrint(readDesignation), crl->inputStream->readIndex - CRL_TYPE_DESIGNATION_SIZE);
						#if DEBUG_BUILD
						LogPrintLine_E(crl->log, "Task Stack: %.*s", StrPrint(taskStackDebugStr));
						#endif
						LogExitFailure(crl->log, CrlError_InvalidDesignation);
						PopMemMark(scratch);
						FreeScratchArena(scratch);
						return false;
					}
				}
				
				u64 structSize = 0;
				u8* structPntr = nullptr;
				if (!nextTask->typeVersion->customReadingLogic)
				{
					structSize = nextTask->deserStructSize;
					CrlEngine_ReadBytes(nextTask->deserStructSize, structPntr, "Structure"); //TODO: Make the debugName for the error log line better!
				}
				
				u64 numTasksBeforeDeserialize = crl->taskStack.length;
				if (!nextTask->typeVersion->Deserialize(crl, crl->deserOutputArena, scratch, nextTask->progressIndex, structSize, structPntr))
				{
					Assert(crl->log->errorCode != 0);
					PopMemMark(scratch);
					FreeScratchArena(scratch);
					return false;
				}
				PopMemMark(scratch);
				
				if (numTasksBeforeDeserialize < crl->taskStack.length)
				{
					//To make it easy on the implementations, we reverse the items pushed onto the stack during
					//Serialize/Deserialize so they will get carried out in the order in which they were pushed
					VarArrayReverse(&crl->taskStack, numTasksBeforeDeserialize, crl->taskStack.length);
					
					//Re-acquire the nextTask pntr into the VarArray because Deserialize might have pushed new items into the array, causing a realloc
					nextTask = VarArrayGetHard(&crl->taskStack, nextTaskIndex, CrlTask_t);
					crl->currentTask = nextTask;
				}
				
				if (nextTask->isArray)
				{
					nextTask->progressIndex++;
					if (nextTask->progressIndex >= nextTask->arraySize)
					{
						FreeCrlTask(crl, nextTask);
						VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
					}
				}
				else
				{
					FreeCrlTask(crl, nextTask);
					VarArrayRemove(&crl->taskStack, nextTaskIndex, CrlTask_t);
				}
			}
		}
		
		if (!crl->isDeserializing)
		{
			if (pass == 0)
			{
				crl->outputSize = crl->writeIndex;
				if (crl->outputSize == 0)
				{
					if (crl->log != nullptr)
					{
						LogWriteLine_E(crl->log, "Serialization produced no data");
						LogExitFailure(crl->log, CrlError_EmptySerialization);
					}
					FreeScratchArena(scratch);
					return false;
				}
				MemArena_t* outputArena = ((crl->serializedOutputArena != nullptr) ? crl->serializedOutputArena : crl->allocArena);
				crl->outputPntr = AllocArray(outputArena, u8, crl->outputSize);
				NotNull(crl->outputPntr);
				#if DEBUG_BUILD
				MyMemSet(crl->outputPntr, 0xDD, crl->outputSize);
				#endif
				
				for (u64 cIndex = 0; cIndex < crl->numContextEntries; cIndex++)
				{
					CrlContextEntry_t* entry = &crl->contextEntries[cIndex];
					if (entry->isFilled && !entry->keepForSecondPass)
					{
						if (entry->isAllocated) { FreeMem(crl->allocArena, entry->pntr, entry->size * entry->count); }
						MyMemSet(&entry->valueBytes[0], 0x00, CRL_CONTEXT_MAX_VALUE_SIZE);
						entry->isFilled = false;
					}
				}
			}
			else if (pass == 1)
			{
				Assert(crl->writeIndex == crl->outputSize);
			}
		}
	}
	
	FreeScratchArena(scratch);
	if (crl->log != nullptr && !crl->log->hadErrors) { LogExitSuccess(crl->log); }
	return (crl->log != nullptr ? !crl->log->hadErrors : true);
}

bool CrlEngineSerialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr)
{
	return CrlEngineRun(crl, firstTaskTypeIndex, firstTaskRuntimeItemSize, firstTaskRuntimeItemPntr, 0);
}

bool CrlEngineDeserialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskPredeclaredSize = 0)
{
	return CrlEngineRun(crl, firstTaskTypeIndex, 0, nullptr, firstTaskPredeclaredSize);
}

// Ownership of this data is passed to the caller at this point
MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl)
{
	NotNull(crl);
	Assert(!crl->isDeserializing);
	Assert(crl->outputPntr != nullptr);
	MyStr_t result = NewStr(crl->outputSize, (char*)crl->outputPntr);
	crl->outputPntr = nullptr;
	crl->outputSize = 0;
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

#endif //  _GY_CEREAL_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
CRL_TYPE_DESIGNATION_SIZE
CRL_CONTEXT_MAX_VALUE_SIZE
CrlError_None
CrlError_IncompleteData
CrlError_InvalidDesignation
CrlError_EmptySerialization
CrlError_NumErrors
CrlVersion_Zero
CrlVersion_Zero_Const
CrlVersion_Max
CrlVersion_Max_Const
@Types
CrlVersion_t
CrlError_t
CrlSerialize_f
CrlDeserialize_f
CrlRegisteredTypeVersion_t
CrlRegisteredType_t
CrlContextEntry_t
CrlTask_t
CrlEngine_t
@Functions
const char* GetCrlErrorStr(CrlError_t enumValue)
bool CRL_SERIALIZE_DEF(CrlEngine_t* crl, u64 arrayIndex, u64 runtimeItemSize, const void* runtimeItemPntr, u64 structSize, u8* structPntr)
bool CRL_DESERIALIZE_DEF(CrlEngine_t* crl, MemArena_t* memArena, MemArena_t* scratch, u64 arrayIndex, u64 structSize, const u8* structPntr)
CrlVersion_t NewCrlVersion(u8 major, u8 minor)
bool IsCrlVersionGreaterThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionLessThan(CrlVersion_t left, CrlVersion_t right, bool allowEqual = false)
bool IsCrlVersionEqual(CrlVersion_t left, CrlVersion_t right)
MyStr_t CrlGetDebugStackString(CrlEngine_t* crl, MemArena_t* memArena)
void FreeCrlTask(CrlEngine_t* crl, CrlTask_t* task)
void FreeCrlEngine(CrlEngine_t* crl)
void CreateCrlEngine(CrlEngine_t* crl, bool deserializing, CrlVersion_t version, MemArena_t* memArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineDeser(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* deserOutputArena, u64 numTypes, u64 numContextEntries, ProcessLog_t* processLog, Stream_t* stream)
void CreateCrlEngineSer(CrlEngine_t* crl, CrlVersion_t version, MemArena_t* memArena, MemArena_t* serializedOutputArena, u64 numTypes, u64 numContextEntries)
CrlRegisteredType_t* CrlGetType(CrlEngine_t* crl, u64 index)
CrlRegisteredTypeVersion_t* CrlGetTypeVersion(CrlEngine_t* crl, u64 index, CrlVersion_t version, bool allowLowerVersions = true)
CrlRegisteredType_t* CrlRegisterType(CrlEngine_t* crl, const char* debugName, u64 index, MyStr_t designation, bool customReadingLogic, CrlVersion_t version, u64 serializedSize, u64 minimumSize, CrlSerialize_f* serializeFunc, CrlDeserialize_f* deserializeFunc)
void CrlPushContext_(CrlEngine_t* crl, u64 index, u64 size, void* pntr, bool allowOverwrite = false, bool allocate = false, bool keepForSecondPass = false)
void* CrlPushContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64 itemCount, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextValue_(CrlEngine_t* crl, u64 index, u64 valueSize, const void* valuePntr, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContext(CrlEngine_t* crl, u64 index, T* typedPntr, bool allowOverwrite = false, bool allocate = false, bool keepForSecondPass = false)
T* CrlPushContextArray(CrlEngine_t* crl, u64 index, T type, u64 count, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI8(CrlEngine_t* crl, u64 index, i8 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI16(CrlEngine_t* crl, u64 index, i16 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI32(CrlEngine_t* crl, u64 index, i32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextI64(CrlEngine_t* crl, u64 index, i64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU8(CrlEngine_t* crl, u64 index, u8 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU16(CrlEngine_t* crl, u64 index, u16 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU32(CrlEngine_t* crl, u64 index, u32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextU64(CrlEngine_t* crl, u64 index, u64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextR32(CrlEngine_t* crl, u64 index, r32 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextR64(CrlEngine_t* crl, u64 index, r64 value, bool allowOverwrite = false, bool keepForSecondPass = false)
void CrlPushContextBool(CrlEngine_t* crl, u64 index, bool value, bool allowOverwrite = false, bool keepForSecondPass = false)
void* CrlGetContext_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
void* CrlGetContextArray_(CrlEngine_t* crl, u64 index, u64 itemSize, u64* countOut, bool assertOnFailure)
CrlContextEntry_t* CrlGetContextRaw_(CrlEngine_t* crl, u64 index, u64 size, bool assertOnFailure)
T* CrlGetContextHard(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContextSoft(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContext(CrlEngine_t* crl, u64 index, Type T)
T* CrlGetContextArrayHard(CrlEngine_t* crl, u64 index, Type T, u64* countOut)
T* CrlGetContextArraySoft(CrlEngine_t* crl, u64 index, Type T, u64* countOut)
T* CrlGetContextArray(CrlEngine_t* crl, u64 index, Type T, u64* countOut)
i8 CrlGetContextI8(CrlEngine_t* crl, u64 index)
i16 CrlGetContextI16(CrlEngine_t* crl, u64 index)
i32 CrlGetContextI32(CrlEngine_t* crl, u64 index)
i64 CrlGetContextI64(CrlEngine_t* crl, u64 index)
u8 CrlGetContextU8(CrlEngine_t* crl, u64 index)
u16 CrlGetContextU16(CrlEngine_t* crl, u64 index)
u32 CrlGetContextU32(CrlEngine_t* crl, u64 index)
u64 CrlGetContextU64(CrlEngine_t* crl, u64 index)
r32 CrlGetContextR32(CrlEngine_t* crl, u64 index)
r64 CrlGetContextR64(CrlEngine_t* crl, u64 index)
bool CrlGetContextBool(CrlEngine_t* crl, u64 index)
CrlTask_t* CrlPushTask(CrlEngine_t* crl, u64 typeIndex, bool isArray, bool isVarArray, u64 arraySize, const void* runtimeItemPntr, u64 runtimeItemSize, u64 predeclaredSize)
CrlTask_t* CrlPushSingleTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize)
CrlTask_t* CrlPushArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const void* runtimeItemPntr, u64 runtimeItemSize, u64 arraySize)
CrlTask_t* CrlPushVarArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, const VarArray_t* runtimeVarArray)
CrlTask_t* CrlPushExplicitArrayTaskSer(CrlEngine_t* crl, u64 typeIndex, u64 runtimeItemSize, u64 arraySize)
void CrlPushExplicitArrayItemSer(CrlEngine_t* crl, CrlTask_t* explicitArrayTask, void* runtimeItemPntr)
CrlTask_t* CrlPushSingleTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredSize = 0)
CrlTask_t* CrlPushArrayTaskDeser(CrlEngine_t* crl, u64 typeIndex, u64 predeclaredArraySize = 0, u64 predeclaredSize = 0)
bool CrlEngineRun(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr, u64 firstTaskPredeclaredSize)
bool CrlEngineSerialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskRuntimeItemSize, const void* firstTaskRuntimeItemPntr)
bool CrlEngineDeserialize(CrlEngine_t* crl, u64 firstTaskTypeIndex, u64 firstTaskPredeclaredSize = 0)
MyStr_t CrlEngineTakeSerializedData(CrlEngine_t* crl)
*/