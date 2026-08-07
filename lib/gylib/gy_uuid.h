/*
File:   gy_uuid.h
Author: Taylor Robbins
Date:   01\21\2023
Description:
	** A Uuid_t (Universally Unique Identifier) is a 128 bit (16 byte) identifier with 5 "blocks". In hex-string form: "12345678-1234-1234-1234-123456789012"
	** Bit sizes: 32-16-16-16-48 Byte sizes: 4-2-2-2-6 Char sizes: 8-4-4-4-12
	** https://en.wikipedia.org/wiki/Universally_unique_identifier
*/

#ifndef _GY_UUID_H
#define _GY_UUID_H

#include "gylib/gy_defines_check.h"
#include "gylib/gy_std.h"
#include "gylib/gy_basic_macros.h"
#include "gylib/gy_types.h"
#include "gylib/gy_assert.h"
#include "gylib/gy_memory.h"
#include "gylib/gy_unicode.h"
#include "gylib/gy_string.h"

#define UUID_BYTE_LENGTH            16
#define UUID_STR_LENGTH             ((UUID_BYTE_LENGTH * 2) + 4) //16 bytes, 2 hex chars per byte, and 4 hyphens = 36 chars
#define UUID_STR_LENGTH_NO_HYPHENS  (UUID_BYTE_LENGTH * 2) //16 bytes, 2 hex chars per byte = 32 chars

// +--------------------------------------------------------------+
// |                             Type                             |
// +--------------------------------------------------------------+
START_PACK()
union ATTR_PACKED Uuid_t
{
	u8 bytes[UUID_BYTE_LENGTH]; //NOTE: These are not in the order they show up in string form! (because little-endian)
	struct
	{
		u32 block0;
		u16 block1;
		u16 block2;
		u16 block3;
		u16 block4_High;
		u32 block4_Low;
	};
};
END_PACK()
CompileAssert(sizeof(Uuid_t) == UUID_BYTE_LENGTH); //make sure packing is working right

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define Uuid_Zero NewUuid(0x00000000, 0x0000, 0x0000, 0x0000, 0x000000000000)
#define Uuid_Full NewUuid(0xFFFFFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFFFFFFFFFF)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	Uuid_t NewUuid(u32 block0, u16 block1, u16 block2, u16 block3, u64 block4);
	bool operator == (Uuid_t left, Uuid_t right);
	bool operator != (Uuid_t left, Uuid_t right);
	bool UuidEquals(const Uuid_t* left, const Uuid_t* right);
	bool UuidEquals(const Uuid_t& left, const Uuid_t& right);
	void UuidToStr(const Uuid_t* uuid, char* buffer, bool addHyphens = true);
	MyStr_t UuidToStrArena(const Uuid_t* uuid, MemArena_t* memArena, bool addHyphens = true);
#else

// +--------------------------------------------------------------+
// |               New Functions and Simple Values                |
// +--------------------------------------------------------------+
Uuid_t NewUuid(u32 block0, u16 block1, u16 block2, u16 block3, u64 block4)
{
	Assert(block4 < (1ULL << 48));
	Uuid_t result;
	result.block0 = block0;
	result.block1 = block1;
	result.block2 = block2;
	result.block3 = block3;
	result.block4_High = (u16)(block4 & 0xFFFF00000000);
	result.block4_Low  = (u32)(block4 & 0x0000FFFFFFFF);
	return result;
}

// +--------------------------------------------------------------+
// |                      Operator Overloads                      |
// +--------------------------------------------------------------+
bool operator == (Uuid_t left, Uuid_t right) { return (left.block0 == right.block0 && left.block1 == right.block1 && left.block2 == right.block2 && left.block3 == right.block3 && left.block4_High == right.block4_High && left.block4_Low == right.block4_Low); }
bool operator != (Uuid_t left, Uuid_t right) { return (left.block0 != right.block0 || left.block1 != right.block1 || left.block2 != right.block2 || left.block3 != right.block3 || left.block4_High != right.block4_High || left.block4_Low != right.block4_Low); }

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
bool UuidEquals(const Uuid_t* left, const Uuid_t* right)
{
	return (left->block0 == right->block0 && left->block1 == right->block1 && left->block2 == right->block2 && left->block3 == right->block3 && left->block4_High == right->block4_High && left->block4_Low == right->block4_Low);
}
bool UuidEquals(const Uuid_t& left, const Uuid_t& right)
{
	return (left.block0 == right.block0 && left.block1 == right.block1 && left.block2 == right.block2 && left.block3 == right.block3 && left.block4_High == right.block4_High && left.block4_Low == right.block4_Low);
}

// +--------------------------------------------------------------+
// |                          To String                           |
// +--------------------------------------------------------------+
//NOTE: Expects a pointer to a buffer at least UUID_STR_LENGTH+1 long (or UUID_STR_LENGTH_NO_HYPHENS+1 if addHyphens=false)
void UuidToStr(const Uuid_t* uuid, char* buffer, bool addHyphens = true)
{
	NotNull2(uuid, buffer);
	u8 charIndex = 0;
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[3])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[3])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[2])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[2])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[1])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[1])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[0])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[0])); charIndex++;
	
	if (addHyphens) { buffer[charIndex] = '-'; charIndex++; }
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[5])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[5])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[4])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[4])); charIndex++;
	
	if (addHyphens) { buffer[charIndex] = '-'; charIndex++; }
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[7])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[7])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[6])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[6])); charIndex++;
	
	if (addHyphens) { buffer[charIndex] = '-'; charIndex++; }
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[9])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[9])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[8])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[8])); charIndex++;
	
	if (addHyphens) { buffer[charIndex] = '-'; charIndex++; }
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[11])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[11])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[10])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[10])); charIndex++;
	
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[15])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[15])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[14])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[14])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[13])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[13])); charIndex++;
	buffer[charIndex] = GetHexChar(GetUpperNibble(uuid->bytes[12])); charIndex++;
	buffer[charIndex] = GetHexChar(GetLowerNibble(uuid->bytes[12])); charIndex++;
	
	buffer[charIndex] = '\0';
}

MyStr_t UuidToStrArena(const Uuid_t* uuid, MemArena_t* memArena, bool addHyphens = true)
{
	NotNull2(uuid, memArena);
	MyStr_t result;
	result.length = (addHyphens ? UUID_STR_LENGTH : UUID_STR_LENGTH_NO_HYPHENS);
	result.chars = AllocArray(memArena, char, result.length + 1);
	NotNull(result.chars);
	UuidToStr(uuid, result.chars, addHyphens);
	return result;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_UUID_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
UUID_BYTE_LENGTH
UUID_STR_LENGTH
UUID_STR_LENGTH_NO_HYPHENS
Uuid_Zero
Uuid_Full
@Types
Uuid_t
@Functions
Uuid_t NewUuid(u32 block0, u16 block1, u16 block2, u16 block3, u64 block4)
bool UuidEquals(const Uuid_t* left, const Uuid_t* right)
void UuidToStr(const Uuid_t* uuid, char* buffer, bool addHyphens = true)
MyStr_t UuidToStrArena(const Uuid_t* uuid, MemArena_t* memArena, bool addHyphens = true)
*/
