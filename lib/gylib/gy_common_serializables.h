/*
File:   gy_common_serializables.h
Author: Taylor Robbins
Date:   07\22\2023
Description: 
	** This is a bunch of functions for common types that we want to serialize/deserialize
*/

#ifndef _GY_COMMON_SERIALIZABLES_H
#define _GY_COMMON_SERIALIZABLES_H

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	SERIALIZE_FUNC_DEFINITION(Serialize_BinaryCopy);
	DESERIALIZE_FUNC_DEFINITION(Deserialize_BinaryCopy);
	extern SerializableFuncs_t SzFuncs_BinaryCopy;
	Serializable_t NewSerializable_U8(u8*      u8Pntr);
	Serializable_t NewSerializable_U16(u16*    u16Pntr);
	Serializable_t NewSerializable_U32(u32*    u32Pntr);
	Serializable_t NewSerializable_U64(u64*    u64Pntr);
	Serializable_t NewSerializable_I8(i8*      i8Pntr);
	Serializable_t NewSerializable_I16(i16*    i16Pntr);
	Serializable_t NewSerializable_I32(i32*    i32Pntr);
	Serializable_t NewSerializable_I64(i64*    i64Pntr);
	Serializable_t NewSerializable_V2i(v2i*    v2iPntr);
	Serializable_t NewSerializable_V3i(v3i*    v3iPntr);
	Serializable_t NewSerializable_V4i(v4i*    v4iPntr);
	Serializable_t NewSerializable_Reci(reci*  reciPntr);
	Serializable_t NewSerializable_Boxi(boxi*  boxiPntr);
	Serializable_t NewSerializable_R32(r32*    r32Pntr);
	Serializable_t NewSerializable_R64(r64*    r64Pntr);
	Serializable_t NewSerializable_V2(v2*      v2Pntr);
	Serializable_t NewSerializable_V3(v3*      v3Pntr);
	Serializable_t NewSerializable_V4(v4*      v4Pntr);
	Serializable_t NewSerializable_Rec(rec*    recPntr);
	Serializable_t NewSerializable_Box(box*    boxPntr);
	Serializable_t NewSerializable_Obb2D(obb2* obb2Pntr);
	Serializable_t NewSerializable_Obb3D(obb3* obb3Pntr);
	SERIALIZE_FUNC_DEFINITION(Serialize_MyStr);
	DESERIALIZE_FUNC_DEFINITION(Deserialize_MyStr);
	extern SerializableFuncs_t SzFuncs_MyStr;
	Serializable_t NewSerializable_MyStr(MyStr_t* strPntr);
	SERIALIZE_FUNC_DEFINITION(Serialize_VarArrayBinaryCopy);
	DESERIALIZE_FUNC_DEFINITION(Deserialize_VarArrayBinaryCopy);
	extern SerializableFuncs_t SzFuncs_VarArrayBinaryCopy;
	Serializable_t NewSerializable_VarArrayBinaryCopy(VarArray_t* arrayPntr);
#else

// +--------------------------------------------------------------+
// |                BinaryCopy Serializable Funcs                 |
// +--------------------------------------------------------------+
// +==============================+
// |     BinaryCopySerialize      |
// +==============================+
SERIALIZE_FUNC_DEFINITION(Serialize_BinaryCopy)
{
	UNUSED(contextPntr);
	Assert(structSize > 0);
	NotNull(structPntr);
	if (memArena == nullptr) { return NewStrLengthOnly(structSize); }
	void* serializedDataPntr = AllocMem(memArena, structSize);
	NotNull(serializedDataPntr);
	MyMemCopy(serializedDataPntr, structPntr, structSize);
	return NewStr(structSize, (char*)serializedDataPntr);
}
// +==============================+
// |    BinaryCopyDeserialize     |
// +==============================+
DESERIALIZE_FUNC_DEFINITION(Deserialize_BinaryCopy)
{
	UNUSED(contextPntr);
	UNUSED(memArena);
	Assert(structOutSize > 0);
	NotNull(structOutPntr);
	if (serializedData.length != structOutSize) { return false; }
	NotNull(serializedData.bytes);
	MyMemCopy(structOutPntr, serializedData.bytes, structOutSize);
	return true;
}

SerializableFuncs_t SzFuncs_BinaryCopy = { Serialize_BinaryCopy, Deserialize_BinaryCopy };
Serializable_t NewSerializable_U8(u8*      u8Pntr)   { return NewSerializable(SzFuncs_BinaryCopy, u8Pntr);   }
Serializable_t NewSerializable_U16(u16*    u16Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, u16Pntr);  }
Serializable_t NewSerializable_U32(u32*    u32Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, u32Pntr);  }
Serializable_t NewSerializable_U64(u64*    u64Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, u64Pntr);  }
Serializable_t NewSerializable_I8(i8*      i8Pntr)   { return NewSerializable(SzFuncs_BinaryCopy, i8Pntr);   }
Serializable_t NewSerializable_I16(i16*    i16Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, i16Pntr);  }
Serializable_t NewSerializable_I32(i32*    i32Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, i32Pntr);  }
Serializable_t NewSerializable_I64(i64*    i64Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, i64Pntr);  }
Serializable_t NewSerializable_V2i(v2i*    v2iPntr)  { return NewSerializable(SzFuncs_BinaryCopy, v2iPntr);  }
Serializable_t NewSerializable_V3i(v3i*    v3iPntr)  { return NewSerializable(SzFuncs_BinaryCopy, v3iPntr);  }
Serializable_t NewSerializable_V4i(v4i*    v4iPntr)  { return NewSerializable(SzFuncs_BinaryCopy, v4iPntr);  }
Serializable_t NewSerializable_Reci(reci*  reciPntr) { return NewSerializable(SzFuncs_BinaryCopy, reciPntr); }
Serializable_t NewSerializable_Boxi(boxi*  boxiPntr) { return NewSerializable(SzFuncs_BinaryCopy, boxiPntr); }
//	TODO: Do floating point numbers work with BinaryCopy when sending between computers? (i.e. over the network) Do any computers that we want to run on not support IEEE 754? Maybe floating point calculations differ on CPUs and determinism of simulation could be broken but serialization/deserialization isn't the problem?
Serializable_t NewSerializable_R32(r32*    r32Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, r32Pntr);  }
Serializable_t NewSerializable_R64(r64*    r64Pntr)  { return NewSerializable(SzFuncs_BinaryCopy, r64Pntr);  }
Serializable_t NewSerializable_V2(v2*      v2Pntr)   { return NewSerializable(SzFuncs_BinaryCopy, v2Pntr);   }
Serializable_t NewSerializable_V3(v3*      v3Pntr)   { return NewSerializable(SzFuncs_BinaryCopy, v3Pntr);   }
Serializable_t NewSerializable_V4(v4*      v4Pntr)   { return NewSerializable(SzFuncs_BinaryCopy, v4Pntr);   }
Serializable_t NewSerializable_Rec(rec*    recPntr)  { return NewSerializable(SzFuncs_BinaryCopy, recPntr);  }
Serializable_t NewSerializable_Box(box*    boxPntr)  { return NewSerializable(SzFuncs_BinaryCopy, boxPntr);  }
Serializable_t NewSerializable_Obb2D(obb2* obb2Pntr) { return NewSerializable(SzFuncs_BinaryCopy, obb2Pntr); }
Serializable_t NewSerializable_Obb3D(obb3* obb3Pntr) { return NewSerializable(SzFuncs_BinaryCopy, obb3Pntr); }

// +--------------------------------------------------------------+
// |                  MyStr_t Serializable Funcs                  |
// +--------------------------------------------------------------+
// +==============================+
// |       Serialize_MyStr        |
// +==============================+
SERIALIZE_FUNC_DEFINITION(Serialize_MyStr)
{
	UNUSED(contextPntr);
	Assert(structSize == sizeof(MyStr_t));
	const MyStr_t* strPntr = (const MyStr_t*)structPntr;
	NotNullStr(strPntr);
	if (memArena == nullptr) { return NewStrLengthOnly(sizeof(u64) + strPntr->length); }
	MyStr_t result;
	result.length = sizeof(u64) + strPntr->length;
	result.bytes = AllocArray(memArena, u8, result.length);
	NotNull(result.bytes);
	u64 writeIndex = 0;
	BinSer_WriteU64(result.bytes, result.length, &writeIndex, strPntr->length);
	BinSer_WriteValue_(result.bytes, result.length, &writeIndex, strPntr->length, strPntr->bytes);
	return result;
}
// +==============================+
// |      Deserialize_MyStr       |
// +==============================+
DESERIALIZE_FUNC_DEFINITION(Deserialize_MyStr)
{
	UNUSED(contextPntr);
	NotNull(memArena);
	Assert(structOutSize == sizeof(MyStr_t));
	NotNull(structOutPntr);
	MyStr_t* strOut = (MyStr_t*)structOutPntr;
	u64 readIndex = 0;
	if (!BinDeser_ReadU64(serializedData.bytes, serializedData.length, &readIndex, &strOut->length)) { return false; }
	if (readIndex + strOut->length != serializedData.length) { return false; }
	strOut->bytes = AllocArray(memArena, u8, strOut->length+1);
	NotNull(strOut->bytes);
	MyMemCopy(strOut->bytes, &serializedData.bytes[readIndex], strOut->length);
	strOut->chars[strOut->length] = '\0';
	return true;
}

SerializableFuncs_t SzFuncs_MyStr = { Serialize_MyStr, Deserialize_MyStr };
Serializable_t NewSerializable_MyStr(MyStr_t* strPntr) { return NewSerializable(SzFuncs_MyStr, strPntr); }

// +--------------------------------------------------------------+
// |                VarArray_t Serializable Funcs                 |
// +--------------------------------------------------------------+
// +==============================+
// | Serialize_VarArrayBinaryCopy |
// +==============================+
SERIALIZE_FUNC_DEFINITION(Serialize_VarArrayBinaryCopy)
{
	UNUSED(contextPntr);
	Assert(structSize == sizeof(VarArray_t));
	const VarArray_t* arrayPntr = (const VarArray_t*)structPntr;
	AssertIf(arrayPntr->length > 0, arrayPntr->items != nullptr);
	u64 numBytesNeeded = sizeof(u64) + sizeof(u64) + arrayPntr->itemSize * arrayPntr->length;
	if (memArena == nullptr) { return NewStrLengthOnly(numBytesNeeded); }
	MyStr_t result;
	result.length = numBytesNeeded;
	result.bytes = AllocArray(memArena, u8, numBytesNeeded);
	NotNull(result.bytes);
	u64 writeIndex = 0;
	BinSer_WriteU64(result.bytes, result.length, &writeIndex, arrayPntr->itemSize);
	BinSer_WriteU64(result.bytes, result.length, &writeIndex, arrayPntr->length);
	for (u64 iIndex = 0; iIndex < arrayPntr->length; iIndex++)
	{
		u8* itemPntr = ((u8*)arrayPntr->items) + (arrayPntr->itemSize * iIndex);
		BinSer_WriteValue_(result.bytes, result.length, &writeIndex, arrayPntr->itemSize, itemPntr);
	}
	return result;
}
// +================================+
// | Deserialize_VarArrayBinaryCopy |
// +================================+
DESERIALIZE_FUNC_DEFINITION(Deserialize_VarArrayBinaryCopy)
{
	UNUSED(contextPntr);
	NotNull(memArena);
	Assert(structOutSize == sizeof(VarArray_t));
	NotNull(structOutPntr);
	VarArray_t* arrayPntr = (VarArray_t*)structOutPntr;
	u64 readIndex = 0;
	u64 itemSize = 0;
	if (!BinDeser_ReadU64(serializedData.bytes, serializedData.length, &readIndex, &itemSize)) { return false; }
	u64 numItems = 0;
	if (!BinDeser_ReadU64(serializedData.bytes, serializedData.length, &readIndex, &numItems)) { return false; }
	CreateVarArray(arrayPntr, memArena, itemSize, numItems);
	for (u64 iIndex = 0; iIndex < numItems; iIndex++)
	{
		void* newItem = VarArrayAdd_(arrayPntr, itemSize);
		NotNull(newItem);
		if (!BinDeser_ReadVariable_(serializedData.bytes, serializedData.length, &readIndex, itemSize, newItem)) { FreeVarArray(arrayPntr); return false; }
	}
	return true;
}

SerializableFuncs_t SzFuncs_VarArrayBinaryCopy = { Serialize_VarArrayBinaryCopy, Deserialize_VarArrayBinaryCopy };
Serializable_t NewSerializable_VarArrayBinaryCopy(VarArray_t* arrayPntr) { return NewSerializable(SzFuncs_VarArrayBinaryCopy, arrayPntr); }

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_COMMON_SERIALIZABLES_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Globals
SzFuncs_BinaryCopy
SzFuncs_MyStr
SzFuncs_VarArrayBinaryCopy
@Functions
inline Serializable_t NewSerializable_U8(u8*      u8Pntr)
inline Serializable_t NewSerializable_U16(u16*    u8Pntr)
inline Serializable_t NewSerializable_U32(u32*    u8Pntr)
inline Serializable_t NewSerializable_U64(u64*    u8Pntr)
inline Serializable_t NewSerializable_I8(u8*      u8Pntr)
inline Serializable_t NewSerializable_I16(u16*    u8Pntr)
inline Serializable_t NewSerializable_I32(u32*    u8Pntr)
inline Serializable_t NewSerializable_I64(u64*    u8Pntr)
inline Serializable_t NewSerializable_V2i(v2i*    v2iPntr)
inline Serializable_t NewSerializable_V3i(v3i*    v2iPntr)
inline Serializable_t NewSerializable_V4i(v4i*    v2iPntr)
inline Serializable_t NewSerializable_Reci(reci*  reciPntr)
inline Serializable_t NewSerializable_Boxi(boxi*  boxiPntr)
inline Serializable_t NewSerializable_R32(r32*    r32Pntr)
inline Serializable_t NewSerializable_R64(r64*    r64Pntr)
inline Serializable_t NewSerializable_V2(v2*      v2Pntr)
inline Serializable_t NewSerializable_V3(v3*      v3Pntr)
inline Serializable_t NewSerializable_V4(v4*      v4Pntr)
inline Serializable_t NewSerializable_Rec(rec*    recPntr)
inline Serializable_t NewSerializable_Box(box*    boxPntr)
inline Serializable_t NewSerializable_Obb2D(obb2* obb2Pntr)
inline Serializable_t NewSerializable_Obb3D(obb3* obb3Pntr)
inline Serializable_t NewSerializable_MyStr(MyStr_t* strPntr)
inline Serializable_t NewSerializable_VarArrayBinaryCopy(VarArray_t* arrayPntr)
*/
