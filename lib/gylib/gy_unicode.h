/*
File:   gy_unicode.h
Author: Taylor Robbins
Date:   09\24\2021
Description:
	** Holds a bunch of functions that help us deal with characters, utf-8, and unicode in general
*/

#ifndef _GY_UNICODE_H
#define _GY_UNICODE_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"

#define UTF8_MAX_CODEPOINT 0x10FFFFUL
#define UTF8_MAX_CHAR_SIZE 4 //bytes

#define NUM_LETTERS_IN_ENGLISH_ALPHABET 26

#define UNICODE_LATIN_EXT_START 0x000000A0UL
#define UNICODE_LATIN_EXT_COUNT 96
#define UNICODE_LATIN_EXT_END   (UNICODE_LATIN_EXT_START + UNICODE_LATIN_EXT_COUNT)

#define UNICODE_CYRILLIC_START 0x00000400UL
#define UNICODE_CYRILLIC_COUNT 256
#define UNICODE_CYRILLIC_END   (UNICODE_CYRILLIC_START + UNICODE_CYRILLIC_COUNT)

#define UNICODE_HIRAGANA_START 0x000003041UL
#define UNICODE_HIRAGANA_COUNT 95
#define UNICODE_HIRAGANA_END   (UNICODE_HIRAGANA_START + UNICODE_HIRAGANA_COUNT)

#define UNICODE_KATAKANA_START 0x0000030A0UL
#define UNICODE_KATAKANA_COUNT 96
#define UNICODE_KATAKANA_END   (UNICODE_KATAKANA_START + UNICODE_KATAKANA_COUNT)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	char GetLowercaseAnsiiChar(char c);
	char GetUppercaseAnsiiChar(char c);
	u32 GetLowercaseCodepoint(u32 codepoint);
	u32 GetUppercaseCodepoint(u32 codepoint);
	u8 CharToU8(char c);
	u32 CharToU32(char c);
	char GetHexChar(u8 hexValue, bool upperCase = true);
	u8 GetUpperNibble(u8 value);
	u8 GetLowerNibble(u8 value);
	u8 GetHexCharValue(char hexChar);
	bool IsCharNumeric(u32 codepoint);
	bool IsCharHexadecimal(u32 codepoint);
	bool AreCharsHexidecimal(u64 numChars, char* charsPntr);
	bool IsCharAlphabetic(u32 codepoint);
	bool IsCharLowercaseAlphabet(u32 codepoint);
	bool IsCharUppercaseAlphabet(u32 codepoint);
	bool IsCharWhitespace(u32 codepoint, bool includeNewLines = false);
	bool IsCharAnsii(u32 codepoint);
	bool IsCharRenderableAnsii(u32 codepoint);
	bool IsCharExtendedAnsii(u32 codepoint);
	bool IsCharAlphaNumeric(u32 codepoint);
	bool IsCharSyntax(u32 codepoint);
	bool IsCharPunctuationStart(u32 codepoint);
	bool IsCharPunctuationEnd(u32 codepoint);
	u8 GetUtf8BytesForCode(u32 codepoint, u8* byteBufferOut = nullptr, bool doAssertions = true);
	u8 GetCodepointForUtf8(u64 maxNumBytes, const char* strPntr, u32* codepointOut = nullptr);
	u8 GetCodepointBeforeIndex(const char* strPntr, u64 startIndex, u32* codepointOut = nullptr);
	i32 CompareCodepoints(u32 codepoint1, u32 codepoint2);
	u8 GetUcs2WordsForCode(u32 codepoint, u16* wordBufferOut, bool doAssertions = true);
	u8 GetCodepointForUcs2(u64 maxNumWords, const u16* strPntr, u32* codepointOut = nullptr);
	bool IsStringValidIdentifier(u64 strLength, const char* strPntr, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false);
	u32 GetMonospaceCodepointFor(u32 codepoint);
	u32 GetRegularCodepointForMonospace(u32 monospaceCodepoint);
#else

// +--------------------------------------------------------------+
// |                   Simple Char Manipulation                   |
// +--------------------------------------------------------------+
char GetLowercaseAnsiiChar(char c)
{
	if (c >= 'A' && c <= 'Z') { return 'a' + (c - 'A'); }
	return c;
}
char GetUppercaseAnsiiChar(char c)
{
	if (c >= 'a' && c <= 'z') { return 'A' + (c - 'a'); }
	return c;
}

u32 GetLowercaseCodepoint(u32 codepoint)
{
	if (codepoint >= 'A' && codepoint <= 'Z') { return 'a' + (codepoint - 'A'); }
	return codepoint;
}
u32 GetUppercaseCodepoint(u32 codepoint)
{
	if (codepoint >= 'a' && codepoint <= 'z') { return 'A' + (codepoint - 'z'); }
	return codepoint;
}

u8 CharToU8(char c)
{
	return *((u8*)&c);
}
u32 CharToU32(char c)
{
	return (u32)(*((u8*)&c));
}

char GetHexChar(u8 hexValue, bool upperCase = true)
{
	if (hexValue <= 9) { return '0' + hexValue; }
	else if (hexValue < 16) { return (upperCase ? 'A' : 'a') + (hexValue - 10); }
	else { return '?'; }
}
u8 GetUpperNibble(u8 value)
{
	return ((value & 0xF0) >> 4);
}
u8 GetLowerNibble(u8 value)
{
	return ((value & 0x0F) >> 0);
}

u8 GetHexCharValue(char hexChar)
{
	if (hexChar >= '0' && hexChar <= '9')
	{
		return (u8)(hexChar - '0');
	}
	else if (hexChar >= 'A' && hexChar <= 'F')
	{
		return (u8)(hexChar - 'A') + 10;
	}
	else if (hexChar >= 'a' && hexChar <= 'f')
	{
		return (u8)(hexChar - 'a') + 10;
	}
	else { return 0; }
}

// +--------------------------------------------------------------+
// |                         Char Classes                         |
// +--------------------------------------------------------------+
bool IsCharNumeric(u32 codepoint)
{
	return (codepoint >= '0' && codepoint <= '9');
}
bool IsCharHexadecimal(u32 codepoint)
{
	return ((codepoint >= '0' && codepoint <= '9') ||
		(codepoint >= 'A' && codepoint <= 'F') ||
		(codepoint >= 'a' && codepoint <= 'f'));
}
bool AreCharsHexidecimal(u64 numChars, char* charsPntr)
{
	AssertIf(numChars > 0, charsPntr != nullptr);
	for (u64 cIndex = 0; cIndex < numChars; cIndex++)
	{
		if (!IsCharHexadecimal(CharToU32(charsPntr[cIndex]))) { return false; }
	}
	return true;
}
bool IsCharAlphabetic(u32 codepoint)
{
	if (codepoint >= 'a' && codepoint <= 'z') { return true; }
	else if (codepoint >= 'A' && codepoint <= 'Z') { return true; }
	else { return false; }
}
bool IsCharLowercaseAlphabet(u32 codepoint)
{
	return (codepoint >= 'a' && codepoint <= 'z');
}
bool IsCharUppercaseAlphabet(u32 codepoint)
{
	return (codepoint >= 'A' && codepoint <= 'Z');
}
bool IsCharWhitespace(u32 codepoint, bool includeNewLines = false)
{
	if (codepoint == ' ') { return true; }
	else if (codepoint == '\t') { return true; }
	else if (codepoint == '\n' && includeNewLines) { return true; }
	else if (codepoint == '\r' && includeNewLines) { return true; }
	else { return false; }
}
bool IsCharAnsii(u32 codepoint)
{
	return (codepoint < 128);
}
bool IsCharRenderableAnsii(u32 codepoint)
{
	return (codepoint >= ' ' && codepoint <= '~');
}
bool IsCharExtendedAnsii(u32 codepoint)
{
	return (codepoint <= 255);
}
bool IsCharAlphaNumeric(u32 codepoint)
{
	return (IsCharAlphabetic(codepoint) || IsCharNumeric(codepoint));
}
bool IsCharSyntax(u32 codepoint)
{
	return (IsCharRenderableAnsii(codepoint) && !IsCharAlphaNumeric(codepoint));
}
bool IsCharPunctuationStart(u32 codepoint)
{
	return (codepoint == '(' || codepoint == '[' || codepoint == '<' || codepoint == '{' || codepoint == '"');
}
bool IsCharPunctuationEnd(u32 codepoint)
{
	return (codepoint == ')' || codepoint == ']' || codepoint == '>' || codepoint == '}' || codepoint == '"');
}

// +--------------------------------------------------------------+
// |                       UTF-8 Functions                        |
// +--------------------------------------------------------------+
//returns number of bytes needed to store this codepoint in UTF-8 and stores the values in byteBufferOut if not nullptr
//NOTE: byteBufferOut is assumed to be 4 bytes or greater and no null-terminating character is written to the buffer
u8 GetUtf8BytesForCode(u32 codepoint, u8* byteBufferOut = nullptr, bool doAssertions = true)
{
	if (codepoint <= 0x7F)
	{
		//0xxx xxxx
		if (byteBufferOut != nullptr) { byteBufferOut[0] = (u8)codepoint; }
		return 1;
	}
	else if (codepoint <= 0x7FF)
	{
		//110x xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xC0 | ((codepoint >> 6) & 0x1F));
			byteBufferOut[1] = (0x80 | (codepoint & 0x3F));
		}
		return 2;
	}
	else if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
	{
		//invalid block
		Assert(!doAssertions || (false && "Invalid codepoint for UTF8"));
		if (byteBufferOut != nullptr) { byteBufferOut[0] = '\0'; }
		return 0;
	}
	else if (codepoint <= 0xFFFF)
	{
		//1110 xxxx 10xx xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xE0 | ((codepoint >> 12) & 0x0F));
			byteBufferOut[1] = (0x80 | ((codepoint>>6) & 0x3F));
			byteBufferOut[2] = (0x80 | (codepoint & 0x3F));
		}
		return 3;
	}
	else if (codepoint <= UTF8_MAX_CODEPOINT)
	{
		//1111 0xxx 10xx xxxx 10xx xxxx 10xx xxxx
		if (byteBufferOut != nullptr)
		{
			byteBufferOut[0] = (0xF0 | ((codepoint >> 18) & 0x07));
			byteBufferOut[1] = (0x80 | ((codepoint>>12) & 0x3F));
			byteBufferOut[2] = (0x80 | ((codepoint>>6) & 0x3F));
			byteBufferOut[3] = (0x80 | (codepoint & 0x3F));
		}
		return 4;
	}
	else
	{
		//everything above this point is also invalid
		Assert(!doAssertions || (false && "Invalid codepoint value for UTF8"));
		if (byteBufferOut != nullptr) { byteBufferOut[0] = '\0'; }
		return 0;
	}
}

//Returns the number of bytes consumed to complete the UTF-8 encoded character pointed to by strPntr
//If more bytes are needed for a full UTF-8 character than is specified by maxNumBytes then we return 0
//If an invalid encoding is detected then we return 0. If maxNumBytes == 0 then we return 0.
//TODO: Should we accept characters that are technically valid but encoded in a larger number of bytes than needed?
//      Like a 4-byte encoding of \0 would be dumb because it could cause non-null-terminated UTF-8 strings to become null terminated collapsed ASCII strings
u8 GetCodepointForUtf8(u64 maxNumBytes, const char* strPntr, u32* codepointOut = nullptr) //somewhat tested
{
	Assert(strPntr != nullptr || maxNumBytes == 0);
	if (codepointOut != nullptr) { *codepointOut = 0; }
	if (maxNumBytes == 0) { return 0; }
	const u8* bytePntr = (const u8*)strPntr;
	if (bytePntr[0] <= 127)
	{
		if (codepointOut != nullptr) { *codepointOut = (u32)bytePntr[0]; }
		return 1;
	}
	else if (bytePntr[0] < 0xC0)
	{
		//An 10xx xxxx format for the first byte of a character is invalid
		return 0;
	}
	else if (bytePntr[0] < 0xE0)
	{
		if (maxNumBytes < 2) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x1F) << 6) | ((u32)(bytePntr[1] & 0x3F) << 0); }
		return 2;
	}
	else if (bytePntr[0] < 0xF0)
	{
		if (maxNumBytes < 3) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (bytePntr[2] < 0x80 || bytePntr[2] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x0F) << 12) | ((u32)(bytePntr[1] & 0x3F) << 6) | ((u32)(bytePntr[2] & 0x3F) << 0); }
		return 3;
	}
	else if (bytePntr[0] < 0xF8)
	{
		if (maxNumBytes < 4) { return 0; }
		if (bytePntr[1] < 0x80 || bytePntr[1] >= 0xC0) { return 0; }
		if (bytePntr[2] < 0x80 || bytePntr[2] >= 0xC0) { return 0; }
		if (bytePntr[3] < 0x80 || bytePntr[3] >= 0xC0) { return 0; }
		if (codepointOut != nullptr) { *codepointOut = ((u32)(bytePntr[0] & 0x07) << 18) | ((u32)(bytePntr[1] & 0x3F) << 12) | ((u32)(bytePntr[2] & 0x3F) << 6) | ((u32)(bytePntr[3] & 0x3F) << 0); }
		return 4;
	}
	else
	{
		//Everything above this point is considered an invalid character to exist in UTF-8 encoded strings
		return 0;
	}
}

//Using the startIndex as a known max length to walk backwards this function will look backwards through a string until it finds a full encoded character
//Returns the number of bytes that encoded character took up and stores the codepoint for it in codepointOut
u8 GetCodepointBeforeIndex(const char* strPntr, u64 startIndex, u32* codepointOut = nullptr)
{
	if (startIndex == 0) { return 0; }
	NotNull(strPntr);
	for (u8 encodedSize = 1; encodedSize <= UTF8_MAX_CHAR_SIZE && encodedSize <= startIndex; encodedSize++)
	{
		char thisChar = strPntr[startIndex - encodedSize];
		u8 thisByte = CharToU8(thisChar);
		if (thisByte <= 127)
		{
			if (codepointOut != nullptr) { *codepointOut = (u32)thisByte; }
			return 1;
		}
		else if (thisByte < 0xC0)
		{
			//The 10xx xxxx format means a second/third/fourth byte in a sequence, keep walking
		}
		else if (thisByte < 0xE0)
		{
			if (encodedSize != 2) { return 0; } //We walked either too many characters before finding this 2-byte encoding start, or not enough
			u8 nextByte = CharToU8(strPntr[startIndex - 1]);
			if (nextByte < 0x80 || nextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (codepointOut != nullptr) { *codepointOut = ((u32)(thisByte & 0x1F) << 6) | ((u32)(nextByte & 0x3F) << 0); }
			return 2;
		}
		else if (thisByte < 0xF0)
		{
			if (encodedSize != 3) { return 0; } //We walked either too many characters before finding this 2-byte encoding start, or not enough
			u8 nextByte = CharToU8(strPntr[startIndex - 2]);
			u8 nextNextByte = CharToU8(strPntr[startIndex - 1]);
			if (nextByte < 0x80 || nextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (nextNextByte < 0x80 || nextNextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (codepointOut != nullptr) { *codepointOut = ((u32)(thisByte & 0x0F) << 12) | ((u32)(nextByte & 0x3F) << 6) | ((u32)(nextNextByte & 0x3F) << 0); }
			return 3;
		}
		else if (thisByte < 0xF8)
		{
			if (encodedSize != 4) { return 0; } //We walked either too many characters before finding this 2-byte encoding start, or not enough
			u8 nextByte = CharToU8(strPntr[startIndex - 3]);
			u8 nextNextByte = CharToU8(strPntr[startIndex - 2]);
			u8 nextNextNextByte = CharToU8(strPntr[startIndex - 1]);
			if (nextByte < 0x80 || nextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (nextNextByte < 0x80 || nextNextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (nextNextNextByte < 0x80 || nextNextNextByte >= 0xC0) { return 0; } //not sure how this would happen?
			if (codepointOut != nullptr) { *codepointOut = ((u32)(thisByte & 0x07) << 18) | ((u32)(nextByte & 0x3F) << 12) | ((u32)(nextNextByte & 0x3F) << 6) | ((u32)(nextNextNextByte & 0x3F) << 0); }
			return 4;
		}
		else
		{
			//Everything above this point is considered an invalid character to exist in UTF-8 encoded strings
			return 0;
		}
	}
	//We had to walk farther than 4 bytes (or the length of the string). Something is wrong with this encoding
	return 0;
}

//Compares two characters for alphabetic order
i32 CompareCodepoints(u32 codepoint1, u32 codepoint2)
{
	if (codepoint1 == codepoint2) { return 0; }
	u32 majorClass1 = 0;
	if (IsCharNumeric(codepoint1)) { majorClass1 = 1; }
	if (IsCharAlphabetic(codepoint1)) { majorClass1 = 2; }
	u32 majorClass2 = 0;
	if (IsCharNumeric(codepoint2)) { majorClass2 = 1; }
	if (IsCharAlphabetic(codepoint2)) { majorClass2 = 2; }
	if (majorClass1 < majorClass2) { return -1; }
	else if (majorClass1 > majorClass2) { return 1; }
	if (majorClass1 == 2) //alphabet
	{
		u32 lower1 = GetLowercaseCodepoint(codepoint1);
		u32 lower2 = GetLowercaseCodepoint(codepoint2);
		if (lower1 < lower2) { return -1; }
		else if (lower1 > lower2) { return 1; }
		else { return 0; }
	}
	if (codepoint1 < codepoint2) { return -1; }
	else if (codepoint1 > codepoint2) { return 1; }
	else { return 0; }
}

// +--------------------------------------------------------------+
// |                       UCS-2 Functions                        |
// +--------------------------------------------------------------+
//returns number of 16-bit words needed to store this codepoint in UCS-2 and stores the values in wordBufferOut if not nullptr
//NOTE: wordBufferOut is assumed to be 2 words or greater and no null-terminating character is written to the buffer
u8 GetUcs2WordsForCode(u32 codepoint, u16* wordBufferOut, bool doAssertions = true) //untested
{
	if (codepoint >= 0x0000 && codepoint <= 0xD7FF)
	{
		wordBufferOut[0] = (u16)codepoint;
		return 1;
	}
	else if (codepoint >= 0xE000 && codepoint <= 0xFFFF)
	{
		wordBufferOut[0] = (u16)codepoint;
		return 1;
	}
	else if (codepoint >= 0x10000 && codepoint <= 0x10FFFF)
	{
		//0xD800-0xDFFF is the "surrogate" area where no unicode points exist so we use it to form "surrogate pairs" in UCS-2
		wordBufferOut[0] = 0xD800 + (u16)(((codepoint - 0x10000) >> 10) & 0x03FF);
		wordBufferOut[1] = 0xDC00 + (u16)(((codepoint - 0x10000) >>  0) & 0x03FF);
		return 2;
	}
	else
	{
		AssertIfMsg(doAssertions, false, "Codepoint can't be converted to UCS-2. Outside supported range of characters");
		return 0;
	}
}

u8 GetCodepointForUcs2(u64 maxNumWords, const u16* strPntr, u32* codepointOut = nullptr) //untested
{
	UNUSED(maxNumWords);
	UNUSED(strPntr);
	UNUSED(codepointOut);
	Unimplemented(); //TODO: Implement me!
	return 0;
}

// +--------------------------------------------------------------+
// |                       String Functions                       |
// +--------------------------------------------------------------+
//NOTE: This technically works 'okay' for unicode strings, it's a bit strict in which characters are valid
bool IsStringValidIdentifier(u64 strLength, const char* strPntr, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false)
{
	Assert(strPntr != nullptr || strLength == 0);
	if (strLength == 0)
	{
		return allowEmpty;
	}
	
	for (u64 cIndex = 0; cIndex < strLength; cIndex++)
	{
		char nextChar = strPntr[cIndex];
		if ((nextChar >= 'a' && nextChar <= 'z') ||
			(nextChar >= 'A' && nextChar <= 'Z'))
		{
			continue;
		}
		else if (nextChar >= '0' && nextChar <= '9' && allowNumbers)
		{
			if (cIndex == 0 && !allowLeadingNumbers) { return false; }
			continue;
		}
		else if (nextChar == '_' && allowUnderscores)
		{
			continue;
		}
		else if (nextChar == ' ' && allowSpaces)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

// +--------------------------------------------------------------+
// |                     Bespoke Conversions                      |
// +--------------------------------------------------------------+
// +==============================+
// |          Monospace           |
// +==============================+
// Works on all alphanumeric characters
u32 GetMonospaceCodepointFor(u32 codepoint)
{
	if (codepoint >= 'A' && codepoint <= 'Z') { return 0x1D670 + (codepoint - 'A'); }
	if (codepoint >= 'a' && codepoint <= 'z') { return 0x1D68A + (codepoint - 'a'); }
	if (codepoint >= '0' && codepoint <= '9') { return 0x1D7F6 + (codepoint - '0'); }
	return 0;
}
u32 GetRegularCodepointForMonospace(u32 monospaceCodepoint)
{
	if (monospaceCodepoint >= 0x1D670 && monospaceCodepoint <= 0x1D689) { return CharToU32('A') + (monospaceCodepoint - 0x1D670); }
	if (monospaceCodepoint >= 0x1D68A && monospaceCodepoint <= 0x1D6A3) { return CharToU32('a') + (monospaceCodepoint - 0x1D68A); }
	if (monospaceCodepoint >= 0x1D7F6 && monospaceCodepoint <= 0x1D7FF) { return CharToU32('0') + (monospaceCodepoint - 0x1D7F6); }
	return 0;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_UNICODE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
UTF8_MAX_CODEPOINT
UTF8_MAX_CHAR_SIZE
NUM_LETTERS_IN_ENGLISH_ALPHABET
UNICODE_LATIN_EXT_START
UNICODE_LATIN_EXT_COUNT
UNICODE_LATIN_EXT_END
UNICODE_CYRILLIC_START
UNICODE_CYRILLIC_COUNT
UNICODE_CYRILLIC_END
UNICODE_HIRAGANA_START
UNICODE_HIRAGANA_COUNT
UNICODE_HIRAGANA_END
UNICODE_KATAKANA_START
UNICODE_KATAKANA_COUNT
UNICODE_KATAKANA_END
@Types
@Functions
char GetLowercaseAnsiiChar(char c)
char GetUppercaseAnsiiChar(char c)
u32 GetLowercaseCodepoint(u32 codepoint)
u32 GetUppercaseCodepoint(u32 codepoint)
u8 CharToU8(char c)
u32 CharToU32(char c)
char GetHexChar(u8 hexValue, bool upperCase = true)
u8 GetUpperNibble(u8 value)
u8 GetLowerNibble(u8 value)
u8 GetHexCharValue(char hexChar)
bool IsCharNumeric(u32 codepoint)
bool IsCharHexadecimal(u32 codepoint)
bool AreCharsHexidecimal(u64 numChars, char* charsPntr)
bool IsCharAlphabetic(u32 codepoint)
bool IsCharLowercaseAlphabet(u32 codepoint)
bool IsCharUppercaseAlphabet(u32 codepoint)
bool IsCharWhitespace(u32 codepoint, bool includeNewLines = false)
bool IsCharAnsii(u32 codepoint)
bool IsCharRenderableAnsii(u32 codepoint)
bool IsCharExtendedAnsii(u32 codepoint)
bool IsCharAlphaNumeric(u32 codepoint)
bool IsCharSyntax(u32 codepoint)
bool IsCharPunctuationStart(u32 codepoint)
bool IsCharPunctuationEnd(u32 codepoint)
u8 GetUtf8BytesForCode(u32 codepoint, u8* byteBufferOut = nullptr, bool doAssertions = true)
u8 GetCodepointForUtf8(u64 maxNumBytes, const char* strPntr, u32* codepointOut = nullptr)
u8 GetCodepointBeforeIndex(const char* strPntr, u64 startIndex, u32* codepointOut = nullptr)
i32 CompareCodepoints(u32 codepoint1, u32 codepoint2)
u8 GetUcs2WordsForCode(u32 codepoint, u16* wordBufferOut, bool doAssertions = true)
u8 GetCodepointForUcs2(u64 maxNumWords, const u16* strPntr, u32* codepointOut = nullptr)
bool IsStringValidIdentifier(u64 strLength, const char* strPntr, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false)
*/
