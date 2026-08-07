/*
File:   gy_string.h
Author: Taylor Robbins
Date:   09\24\2021
*/

#ifndef _GY_STRING_H
#define _GY_STRING_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"
#include "gy_assert.h"
#include "gy_intrinsics.h"
#include "gy_memory.h"
#include "gy_unicode.h"
#include "gy_hash.h"

struct MyStr_t
{
	u64 length;
	union
	{
		char* pntr;
		char* chars;
		u8* bytes;
	};
};

struct MyWideStr_t
{
	u64 length;
	union
	{
		wchar_t* pntr;
		wchar_t* chars;
		u16* words;
	};
};

union MyStrPair_t
{
	MyStr_t strs[2];
	struct 
	{
		union
		{
			MyStr_t key;
			MyStr_t left;
			MyStr_t first;
		};
		union
		{
			MyStr_t value;
			MyStr_t right;
			MyStr_t second;
		};
	};
};

enum WordBreakCharClass_t
{
	WordBreakCharClass_AlphabeticLower,
	WordBreakCharClass_AlphabeticUpper,
	WordBreakCharClass_WordPunctuation,
	WordBreakCharClass_Numeric,
	WordBreakCharClass_PunctuationOpen,
	WordBreakCharClass_PunctuationClose,
	WordBreakCharClass_Whitespace,
	WordBreakCharClass_Punctuation,
	WordBreakCharClass_Other,
	WordBreakCharClass_NumClasses,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetWordBreakCharClassStr(WordBreakCharClass_t enumValue);
#else
const char* GetWordBreakCharClassStr(WordBreakCharClass_t enumValue)
{
	switch (enumValue)
	{
		case WordBreakCharClass_AlphabeticLower:  return "AlphabeticLower";
		case WordBreakCharClass_AlphabeticUpper:  return "AlphabeticUpper";
		case WordBreakCharClass_WordPunctuation:  return "WordPunctuation";
		case WordBreakCharClass_PunctuationOpen:  return "PunctuationOpen";
		case WordBreakCharClass_PunctuationClose: return "PunctuationClose";
		case WordBreakCharClass_Whitespace:       return "Whitespace";
		case WordBreakCharClass_Punctuation:      return "Punctuation";
		case WordBreakCharClass_Other:            return "Other";
		default: return "Unknown";
	}
}
#endif

struct SplitStringContext_t
{
	u64 lastSeparatorIndex;
	MyStr_t piece;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define MyStr_Empty_Const { 0, nullptr }
#define MyStr_Empty NewStr((u64)0, (char*)nullptr)

// +==============================+
// |       Assertion Macros       |
// +==============================+
#define NotNullStr(strPntr)      Assert (!IsNullStr(strPntr))
#define NotNullStr_(strPntr)     Assert_(!IsNullStr(strPntr))
#define NotEmptyStr(strPntr)     Assert ((strPntr) != nullptr && !IsEmptyStr(strPntr))
#define NotEmptyStr_(strPntr)    Assert_((strPntr) != nullptr && !IsEmptyStr(strPntr))
#define AssertNullTerm(strPntr)  Assert ((strPntr) != nullptr && IsStrNullTerminated(strPntr))
#define AssertNullTerm_(strPntr) Assert_((strPntr) != nullptr && IsStrNullTerminated(strPntr))

// +==============================+
// |       MemArena Macros        |
// +==============================+
#define AllocString(arena, strPntr)               NewStr((strPntr)->length,          AllocCharsAndFill  ((arena), (strPntr)->length, (strPntr)->pntr))
#define NewStringInArena(arena, length, charPntr) NewStr((length),                   AllocCharsAndFill  ((arena), (length),          (charPntr)))
#define NewStringInArenaNt(arena, nullTermStr)    NewStr(MyStrLength64(nullTermStr), AllocCharsAndFillNt((arena), (nullTermStr)))

#define FreeString(arena, strPntr) do { NotNullStr(strPntr); if ((strPntr)->pntr != nullptr) { FreeMem((arena), (strPntr)->pntr, (strPntr)->length+1); (strPntr)->pntr = nullptr; (strPntr)->length = 0; } } while(0)

// +==============================+
// |         Print Macros         |
// +==============================+
// When using the %.*s format specifier, we need to be careful about the size of the length argument being passed
// On 32-bit platforms, the u64 in the MyStr_t structure is too large, so we need to cast to u32.
// We should use one of these macros in ALL location where we are using %.*s in a format string and sourcing from MyStr_t
#if PLATFORM_32BIT
#define StrPrint(myStrStruct)   (u32)(myStrStruct).length, (myStrStruct).chars
#define StrPntrPrint(myStrPntr) (u32)(myStrPntr)->length, (myStrPntr)->chars
#else
#define StrPrint(myStrStruct)   (myStrStruct).length, (myStrStruct).chars
#define StrPntrPrint(myStrPntr) (myStrPntr)->length, (myStrPntr)->chars
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	MyStr_t NewStrLengthOnly(u64 length);
	MyStr_t NewStr(u64 length, char* pntr);
	MyStr_t NewStr(u64 length, const char* pntr);
	MyStr_t NewStr(char* nullTermStr);
	MyStr_t NewStr(const char* nullTermStr);
	MyStrPair_t NewStrPair(const char* keyStrNullTerm, const char* valueStrNullTerm);
	MyStrPair_t NewStrPair(MyStr_t keyStr, const char* valueStrNullTerm);
	MyStrPair_t NewStrPair(const char* keyStrNullTerm, MyStr_t valueStr);
	MyStrPair_t NewStrPair(MyStr_t keyStr, MyStr_t valueStr);
	bool IsNullStr(MyStr_t target);
	bool IsNullStr(const MyStr_t* targetPntr);
	bool IsEmptyStr(MyStr_t target);
	bool IsEmptyStr(const MyStr_t* targetPntr);
	bool IsStrNullTerminated(MyStr_t target);
	bool IsStrNullTerminated(const MyStr_t* targetPntr);
	bool BufferIsNullTerminated(u64 bufferSize, const char* bufferPntr);
	MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...);
	#ifdef _GY_UNICODE_H
	u8 GetCodepointForUtf8Str(MyStr_t str, u64 index, u32* codepointOut = nullptr);
	#if WINDOWS_COMPILATION
	MyStr_t ConvertUcs2StrToUtf8(MemArena_t* memArena, const wchar_t* wideStrPntr, u64 wideStrLength);
	MyStr_t ConvertUcs2StrToUtf8Nt(MemArena_t* memArena, const wchar_t* nullTermWideStr);
	MyWideStr_t ConvertUtf8StrToUcs2(MemArena_t* memArena, MyStr_t utf8Str);
	#endif // WINDOWS_COMPILATION
	bool DoesStrContainMultibyteUtf8Characters(MyStr_t str);
	#endif //_GY_UNICODE_H
	u64 TrimLeadingWhitespace(MyStr_t* target, bool trimNewLines = false);
	u64 TrimTrailingWhitespace(MyStr_t* target, bool trimNewLines = false);
	u64 TrimWhitespace(MyStr_t* target, bool trimNewLines = false);
	bool FindNextCharInStr(MyStr_t target, u64 startIndex, MyStr_t searchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false);
	bool FindNextCharInStr(MyStr_t target, u64 startIndex, const char* nullTermSearchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false);
	bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, MyStr_t knownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false);
	bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, const char* nullTermKnownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false);
	bool FindNextWhitespaceInStr(MyStr_t target, u64 startIndex, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false);
	MyStr_t StrSubstring(MyStr_t* target, u64 startIndex);
	MyStr_t StrSubstring(MyStr_t* target, u64 startIndex, u64 endIndex);
	MyStr_t StrSubstringLength(MyStr_t* target, u64 startIndex, u64 length);
	MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2);
	MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3);
	MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3, MyStr_t str4);
	bool StrEquals(MyStr_t target, MyStr_t comparison);
	bool StrEquals(MyStr_t target, const char* comparisonNt);
	bool StrEquals(MyStr_t target, u64 comparisonLength, const char* comparisonPntr);
	bool StrEquals(const char* comparisonNt, MyStr_t target);
	bool StrEquals(u64 comparisonLength, const char* comparisonPntr, MyStr_t target);
	i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2, u64 compareLength);
	i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2);
	i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr, u64 compareLength);
	i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr);
	i32 StrCompareIgnoreCase(const char* str1, const char* str2, u64 compareLength);
	bool StrEqualsIgnoreCase(MyStr_t target, MyStr_t comparison);
	bool StrEqualsIgnoreCase(MyStr_t target, const char* comparisonNt);
	bool StrEqualsIgnoreCase(MyStr_t target, u64 comparisonLength, const char* comparisonPntr);
	bool StrEqualsIgnoreCase(const char* comparisonNt, MyStr_t target);
	bool StrEqualsIgnoreCase(u64 comparisonLength, const char* comparisonPntr, MyStr_t target);
	bool StrStartsWith(MyStr_t str, MyStr_t prefix, bool ignoreCase = false);
	bool StrStartsWith(MyStr_t str, const char* nullTermPrefixStr, bool ignoreCase = false);
	bool StrStartsWith(const char* nullTermStr, MyStr_t prefix, bool ignoreCase = false);
	bool StrStartsWith(const char* nullTermStr, const char* nullTermPrefixStr, bool ignoreCase = false);
	bool StrEndsWith(MyStr_t str, MyStr_t suffix, bool ignoreCase = false);
	bool StrEndsWith(MyStr_t str, const char* nullTermSuffix, bool ignoreCase = false);
	bool StrEndsWith(const char* nullTermStr, MyStr_t suffix, bool ignoreCase = false);
	bool StrEndsWith(const char* nullTermStr, const char* nullTermSuffix, bool ignoreCase = false);
	bool StrStartsWithSlash(MyStr_t str);
	bool StrStartsWithSlash(const char* nullTermStr);
	bool StrEndsWithSlash(MyStr_t str);
	bool StrEndsWithSlash(const char* nullTermStr);
	bool SplitStringFixed(MyStr_t target, MyStr_t delineator, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false);
	bool SplitStringFixed(MyStr_t target, const char* delineatorNullTerm, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false);
	bool SplitStringFixed(const char* targetNullTerm, MyStr_t delineator, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false);
	bool SplitStringFixed(const char* targetNullTerm, const char* delineatorNullTerm, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false);
	MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false);
	MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false);
	MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false);
	MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false);
	bool SplitStringFast(SplitStringContext_t* context, MyStr_t target, char separatorChar, bool includeEmptyPieces = false);
	MyStr_t* SplitStringBySpacesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut);
	MyStr_t* SplitStringBySlashesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut);
	u64 UnescapeQuotedStringInPlace(MyStr_t* target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false);
	MyStr_t UnescapeQuotedStringInArena(MemArena_t* memArena, MyStr_t target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false);
	MyStr_t* SplitStringBySpacesWithQuotesAndUnescape(MemArena_t* memArena, MyStr_t target, u64* numPiecesOut);
	void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr);
	MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true);
	MyStr_t GetDirectoryPart(MyStr_t filePath);
	const char* GetFileNamePartNt(const char* filePath);
	void StrSpliceInPlace(MyStr_t target, u64 startIndex, MyStr_t replacement);
	void StrSpliceInPlace(MyStr_t target, u64 startIndex, const char* replacementNullTerm);
	void StrSpliceInPlace(char* targetNullTermStr, u64 startIndex, MyStr_t replacement);
	void StrSpliceInPlace(char* targetNullTermStr, u64 startIndex, const char* replacementNullTerm);
	MyStr_t StrSplice(MyStr_t target, u64 startIndex, u64 endIndex, MyStr_t replacement, MemArena_t* memArena);
	MyStr_t StrSplice(MyStr_t target, u64 startIndex, u64 endIndex, const char* replacementNullTerm, MemArena_t* memArena);
	MyStr_t StrSplice(char* targetNullTermStr, u64 startIndex, u64 endIndex, MyStr_t replacement, MemArena_t* memArena);
	MyStr_t StrSplice(char* targetNullTermStr, u64 startIndex, u64 endIndex, const char* replacement, MemArena_t* memArena);
	u64 StrReplaceInPlace(MyStr_t str, MyStr_t target, MyStr_t replacement, bool ignoreCase = false, bool allowShrinking = false);
	u64 StrReplaceInPlace(MyStr_t str, const char* target, const char* replacement, bool ignoreCase = false, bool allowShrinking = false);
	MyStr_t StrReplace(MyStr_t str, MyStr_t target, MyStr_t replacement, MemArena_t* memArena);
	MyStr_t StrReplace(MyStr_t str, const char* target, const char* replacement, MemArena_t* memArena);
	MyStr_t StrReplaceMultiple(MyStr_t str, u64 numReplacements, const MyStrPair_t* replacements, MemArena_t* memArena);
	bool FindSubstring(MyStr_t target, MyStr_t substring, u64* indexOut = nullptr, bool ignoreCase = false, u64 startIndex = 0);
	bool FindSubstring(MyStr_t target, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0);
	bool FindSubstring(const char* nullTermTarget, MyStr_t substring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0);
	bool FindSubstring(const char* nullTermTarget, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0);
	MyStr_t FindStrParensPart(MyStr_t target, char openParensChar = '(', char closeParensChar = ')');
	MyStr_t FindStrParensPart(const char* nullTermTarget, char openParensChar = '(', char closeParensChar = ')');
	MyStr_t StringRepeat(MemArena_t* memArena, MyStr_t str, u64 numRepetitions);
	MyStr_t StringRepeat(MemArena_t* memArena, const char* nullTermStr, u64 numRepetitions);
	MyStr_t FormatBytes(u64 numBytes, MemArena_t* memArena);
	const char* FormatBytesNt(u64 numBytes, MemArena_t* memArena);
	MyStr_t FormatNumberWithCommas(u64 number, MemArena_t* memArena = nullptr);
	const char* FormatNumberWithCommasNt(u64 number, MemArena_t* memArena = nullptr);
	u64 FnvHashStr(MyStr_t str);
	u64 FnvHashStr(const char* nullTermStr);
	bool IsStringValidIdentifier(MyStr_t str, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false);
	void StrReallocAppend(MyStr_t* baseStr, MyStr_t appendStr, MemArena_t* memArena);
	void StrReallocAppend(MyStr_t* baseStr, const char* appendNullTermStr, MemArena_t* memArena);
	WordBreakCharClass_t GetWordBreakCharClass(u32 codepoint);
	bool IsCharPairWordBreak(u32 prevCodepoint, u32 nextCodepoint, bool forward, bool subwords);
	u64 FindNextWordBreakInString(MyStr_t str, u64 startIndex, bool forward, bool subwords, bool includeBreakAtStartIndex = false);
	#ifdef _GY_TIME_H
	MyStr_t FormatRealTime(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true);
	const char* FormatRealTimeNt(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true);
	MyStr_t FormatMilliseconds(u64 milliseconds, MemArena_t* memArena);
	const char* FormatMillisecondsNt(u64 milliseconds, MemArena_t* memArena);
	#endif //_GY_TIME_H
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
MyStr_t NewStrLengthOnly(u64 length)
{
	MyStr_t result;
	result.length = length;
	result.pntr = nullptr;
	return result;
}
MyStr_t NewStr(u64 length, char* pntr)
{
	MyStr_t result;
	result.length = length;
	result.pntr = pntr;
	return result;
}
MyStr_t NewStr(u64 length, const char* pntr)
{
	MyStr_t result;
	result.length = length;
	result.pntr = (char*)pntr;
	return result;
}
MyStr_t NewStr(char* nullTermStr)
{
	MyStr_t result;
	result.length = ((nullTermStr != nullptr) ? MyStrLength64(nullTermStr) : 0);
	result.pntr = nullTermStr;
	return result;
}
MyStr_t NewStr(const char* nullTermStr)
{
	MyStr_t result;
	result.length = ((nullTermStr != nullptr) ? MyStrLength64(nullTermStr) : 0);
	result.pntr = (char*)nullTermStr;
	return result;
}

//TODO: Is our idea of an empty string somewhat flawed because I could have a 0 length string that is still allocated?? Like when I go to deallocate a string should I check if it's pntr is nullptr or if it's length is 0??

MyStrPair_t NewStrPair(const char* keyStrNullTerm, const char* valueStrNullTerm)
{
	MyStrPair_t result;
	result.key.length = ((keyStrNullTerm != nullptr) ? MyStrLength64(keyStrNullTerm) : 0);
	result.key.pntr = (char*)keyStrNullTerm;
	result.value.length = ((valueStrNullTerm != nullptr) ? MyStrLength64(valueStrNullTerm) : 0);
	result.value.pntr = (char*)valueStrNullTerm;
	return result;
}
MyStrPair_t NewStrPair(MyStr_t keyStr, const char* valueStrNullTerm)
{
	MyStrPair_t result;
	result.key = keyStr;
	result.value.length = ((valueStrNullTerm != nullptr) ? MyStrLength64(valueStrNullTerm) : 0);
	result.value.pntr = (char*)valueStrNullTerm;
	return result;
}
MyStrPair_t NewStrPair(const char* keyStrNullTerm, MyStr_t valueStr)
{
	MyStrPair_t result;
	result.key.length = ((keyStrNullTerm != nullptr) ? MyStrLength64(keyStrNullTerm) : 0);
	result.key.pntr = (char*)keyStrNullTerm;
	result.value = valueStr;
	return result;
}
MyStrPair_t NewStrPair(MyStr_t keyStr, MyStr_t valueStr)
{
	MyStrPair_t result;
	result.key = keyStr;
	result.value = valueStr;
	return result;
}

//A "Null Str" is one that has length but pntr is nullptr
bool IsNullStr(MyStr_t target)
{
	return (target.length > 0 && target.pntr == nullptr);
}
bool IsNullStr(const MyStr_t* targetPntr)
{
	return (targetPntr == nullptr || (targetPntr->length > 0 && targetPntr->pntr == nullptr));
}
bool IsEmptyStr(MyStr_t target)
{
	return (target.length == 0);
}
bool IsEmptyStr(const MyStr_t* targetPntr)
{
	NotNull(targetPntr);
	return (targetPntr->length == 0);
}
bool IsStrNullTerminated(MyStr_t target)
{
	if (target.pntr == nullptr) { return false; }
	return (target.pntr[target.length] == '\0');
}
bool IsStrNullTerminated(const MyStr_t* targetPntr)
{
	NotNull(targetPntr);
	if (targetPntr->pntr == nullptr) { return false; }
	return (targetPntr->pntr[targetPntr->length] == '\0');
}
bool BufferIsNullTerminated(u64 bufferSize, const char* bufferPntr)
{
	AssertIf(bufferSize > 0, bufferPntr != nullptr);
	for (u64 cIndex = 0; cIndex < bufferSize; cIndex++)
	{
		if (bufferPntr[cIndex] == '\0') { return true; }
	}
	return false;
}

// +--------------------------------------------------------------+
// |                         Print Helper                         |
// +--------------------------------------------------------------+
MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...)
{
	NotNull(arena);
	NotNull(formatString);
	
	char* result = nullptr;
	va_list args;
	
	va_start(args, formatString);
	int length = MyVaListPrintf(result, 0, formatString, args); //Measure first
	Assert(length >= 0);
	va_end(args);
	
	result = AllocArray(arena, char, length+1); //Allocate
	if (result == nullptr) { return MyStr_Empty; }
	
	va_start(args, formatString);
	MyVaListPrintf(result, (size_t)(length+1), formatString, args); //Real printf
	va_end(args);
	
	result[length] = '\0';
	
	return NewStr((u64)length, result);
}

// +--------------------------------------------------------------+
// |                   Unicode String Functions                   |
// +--------------------------------------------------------------+
#ifdef _GY_UNICODE_H
u8 GetCodepointForUtf8Str(MyStr_t str, u64 index, u32* codepointOut = nullptr)
{
	Assert(index <= str.length);
	return GetCodepointForUtf8(str.length - index, str.pntr + index, codepointOut);
}

#if WINDOWS_COMPILATION

MyStr_t ConvertUcs2StrToUtf8(MemArena_t* memArena, const wchar_t* wideStrPntr, u64 wideStrLength)
{
	Assert(wideStrPntr != nullptr || wideStrLength == 0);
	MyStr_t result = MyStr_Empty;
	for (u8 pass = 0; pass < 2; pass++)
	{
		u64 byteIndex = 0;
		
		u8 encodeBuffer[UTF8_MAX_CHAR_SIZE];
		for (u64 cIndex = 0; cIndex < wideStrLength; cIndex++)
		{
			wchar_t wideChar = wideStrPntr[cIndex];
			//TODO: Should we just straight convert wide characters to unicode points by casting to u32!?
			u8 encodeSize = GetUtf8BytesForCode((u32)wideChar, &encodeBuffer[0], false);
			if (encodeSize == 0)
			{
				return result;
			}
			if (result.pntr != nullptr)
			{
				Assert(result.length >= byteIndex + encodeSize);
				MyMemCopy(&result.pntr[byteIndex], &encodeBuffer[0], encodeSize);
			}
			byteIndex += encodeSize;
		}
		
		if (pass == 0)
		{
			result.length = byteIndex;
			if (memArena == nullptr) { return result; }
			result.pntr = AllocArray(memArena, char, result.length+1);
			NotNull(result.pntr);
		}
		else
		{
			Assert(byteIndex == result.length);
			result.pntr[result.length] = '\0';
		}
	}
	return result;
}
MyStr_t ConvertUcs2StrToUtf8Nt(MemArena_t* memArena, const wchar_t* nullTermWideStr)
{
	NotNull(nullTermWideStr);
	return ConvertUcs2StrToUtf8(memArena, nullTermWideStr, MyWideStrLength(nullTermWideStr));
}

MyWideStr_t ConvertUtf8StrToUcs2(MemArena_t* memArena, MyStr_t utf8Str)
{
	NotNull(memArena);
	NotNullStr(&utf8Str);
	u16 encodedWords[2];
	
	u64 numWordsNeeded = 0;
	for (u64 byteIndex = 0; byteIndex < utf8Str.length; )
	{
		u32 codepoint = 0;
		u8 charByteSize = GetCodepointForUtf8(utf8Str.length - byteIndex, &utf8Str.chars[byteIndex], &codepoint);
		if (charByteSize == 0)
		{
			//TODO: Should we print out a warning or error or something for invalid UTF-8 encoding?
			codepoint = CharToU32(utf8Str.chars[byteIndex]);
			charByteSize = 1;
		}
		
		u8 numWords = GetUcs2WordsForCode(codepoint, encodedWords);
		Assert(numWords != 0);
		numWordsNeeded += numWords;
		byteIndex += charByteSize;
	}
	
	MyWideStr_t result;
	result.words = AllocArray(memArena, u16, numWordsNeeded);
	result.length = 0;
	if (result.words == nullptr) { return result; }
	
	for (u64 byteIndex = 0; byteIndex < utf8Str.length; )
	{
		u32 codepoint = 0;
		u8 charByteSize = GetCodepointForUtf8(utf8Str.length - byteIndex, &utf8Str.chars[byteIndex], &codepoint);
		if (charByteSize == 0)
		{
			//TODO: Should we print out a warning or error or something for invalid UTF-8 encoding?
			codepoint = CharToU32(utf8Str.chars[byteIndex]);
			charByteSize = 1;
		}
		
		u8 numWords = GetUcs2WordsForCode(codepoint, encodedWords);
		Assert(numWords != 0);
		result.words[result.length] = encodedWords[0]; result.length++;
		if (numWords > 1) { result.words[result.length] = encodedWords[1]; result.length++; }
		
		byteIndex += charByteSize;
	}
	
	Assert(result.length == numWordsNeeded);
	return result;
}

#endif // WINDOWS_COMPILATION

bool DoesStrContainMultibyteUtf8Characters(MyStr_t str)
{
	for (u64 bIndex = 0; bIndex < str.length; bIndex++)
	{
		if (GetCodepointForUtf8Str(str, bIndex) > 0) { return true; }
	}
	return false;
}

#endif //_GY_UNICODE_H

// +--------------------------------------------------------------+
// |                Helpful Manipulation Functions                |
// +--------------------------------------------------------------+
u64 TrimLeadingWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	while (target->length > 0)
	{
		if (target->pntr[0] == ' ' || target->pntr[0] == '\t' ||
			(trimNewLines && (target->pntr[0] == '\n' || target->pntr[0] == '\r')))
		{
			target->length--;
			target->pntr++;
			result++;
		}
		else { break; }
	}
	return result;
}
u64 TrimTrailingWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	while (target->length > 0)
	{
		if (target->pntr[target->length-1] == ' ' || target->pntr[target->length-1] == '\t' ||
			(trimNewLines && (target->pntr[target->length-1] == '\n' || target->pntr[target->length-1] == '\r')))
		{
			target->length--;
			result++;
		}
		else { break; }
	}
	return result;
}
//TODO: We should make a version of this that doesn't take a pointer to a string, but rather just returns the MyStr_t without modifying the input MyStr_t
u64 TrimWhitespace(MyStr_t* target, bool trimNewLines = false)
{
	NotNullStr(target);
	u64 result = 0;
	result += TrimLeadingWhitespace(target, trimNewLines);
	result += TrimTrailingWhitespace(target, trimNewLines);
	return result;
}

bool FindNextCharInStr(MyStr_t target, u64 startIndex, MyStr_t searchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		for (u64 sIndex = 0; sIndex < searchCharsStr.length; )
		{
			u32 searchCodepoint = 0;
			u8 searchCodepointSize = GetCodepointForUtf8Str(searchCharsStr, sIndex, &searchCodepoint);
			DebugAssert(searchCodepointSize > 0);
			if (searchCodepointSize == 0) { return false; } //invalid utf-8 encoding in searchCharsStr
			if (codepoint == searchCodepoint && !inString)
			{
				if (indexOut != nullptr) { *indexOut = cIndex; }
				return true;
			}
			sIndex += searchCodepointSize;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}
bool FindNextCharInStr(MyStr_t target, u64 startIndex, const char* nullTermSearchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	return FindNextCharInStr(target, startIndex, NewStr(nullTermSearchCharsStr), indexOut, ignoreCharsInQuotes);
}
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, MyStr_t knownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		bool isUnknownChar = true;
		for (u64 sIndex = 0; sIndex < knownCharsStr.length; )
		{
			u32 knownCodepoint = 0;
			u8 knownCodepointSize = GetCodepointForUtf8Str(knownCharsStr, sIndex, &knownCodepoint);
			DebugAssert(knownCodepointSize > 0);
			if (knownCodepointSize == 0) { return false; } //invalid utf-8 encoding in knownCharsStr
			if (codepoint == knownCodepoint && !inString)
			{
				isUnknownChar = false;
				break;
			}
			sIndex += knownCodepointSize;
		}
		if (isUnknownChar)
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, const char* nullTermKnownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	return FindNextUnknownCharInStr(target, startIndex, NewStr(nullTermKnownCharsStr), indexOut, ignoreCharsInQuotes);
}
bool FindNextWhitespaceInStr(MyStr_t target, u64 startIndex, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
{
	NotNullStr(&target);
	bool inString = false;
	u32 previousCodepoint = 0;
	for (u64 cIndex = startIndex; cIndex < target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = GetCodepointForUtf8Str(target, cIndex, &codepoint);
		if (codepointSize == 0) { cIndex++; continue; } //invalid utf-8 encoding in target
		if (IsCharWhitespace(codepoint))
		{
			if (indexOut != nullptr) { *indexOut = cIndex; }
			return true;
		}
		if (ignoreCharsInQuotes && codepoint == '"' && !(inString && previousCodepoint == '\\'))
		{
			inString = !inString;
		}
		previousCodepoint = codepoint;
		cIndex += codepointSize;
	}
	return false;
}

//TODO: Change these so they don't take pointers?
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = target->length - startIndex;
	return result;
}
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex, u64 endIndex)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex <= target->length);
	Assert(endIndex >= startIndex);
	Assert(endIndex <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = endIndex - startIndex;
	return result;
}
MyStr_t StrSubstringLength(MyStr_t* target, u64 startIndex, u64 length)
{
	NotNullStr(target);
	MyStr_t result;
	Assert(startIndex + length <= target->length);
	result.pntr = &target->chars[startIndex];
	result.length = length;
	return result;
}

MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	
	MyStr_t result;
	result.length = str1.length + str2.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	result.pntr[result.length] = '\0';
	
	return result;
}
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	NotNullStr(&str3);
	
	MyStr_t result;
	result.length = str1.length + str2.length + str3.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	MyMemCopy(&result.pntr[str1.length + str2.length], str3.pntr, str3.length);
	result.pntr[result.length] = '\0';
	
	return result;
}
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2, MyStr_t str3, MyStr_t str4)
{
	NotNull(memArena);
	NotNullStr(&str1);
	NotNullStr(&str2);
	NotNullStr(&str3);
	NotNullStr(&str4);
	
	MyStr_t result;
	result.length = str1.length + str2.length + str3.length + str4.length;
	result.pntr = AllocArray(memArena, char, result.length + 1);
	NotNull(result.pntr);
	
	MyMemCopy(&result.pntr[0], str1.pntr, str1.length);
	MyMemCopy(&result.pntr[str1.length], str2.pntr, str2.length);
	MyMemCopy(&result.pntr[str1.length + str2.length], str3.pntr, str3.length);
	MyMemCopy(&result.pntr[str1.length + str2.length + str3.length], str4.pntr, str4.length);
	result.pntr[result.length] = '\0';
	
	return result;
}

bool StrEquals(MyStr_t target, MyStr_t comparison)
{
	NotNullStr(&target);
	NotNullStr(&comparison);
	if (target.pntr == nullptr && comparison.pntr == nullptr) { return true; }
	if (target.length != comparison.length) { return false; }
	if (target.length == 0 && comparison.length == 0) { return true; }
	if (target.pntr == nullptr || comparison.pntr == nullptr) { AssertMsg(false, "this case is only for malformed MyStr_t structs (size != 0 but pntr is nullptr)"); return false; }
	return (MyStrCompare(target.pntr, comparison.pntr, target.length) == 0);
}
bool StrEquals(MyStr_t target, const char* comparisonNt)
{
	MyStr_t comparisonStr = NewStr(comparisonNt);
	return StrEquals(target, comparisonStr);
}
bool StrEquals(MyStr_t target, u64 comparisonLength, const char* comparisonPntr)
{
	MyStr_t comparisonStr = NewStr(comparisonLength, comparisonPntr);
	return StrEquals(target, comparisonStr);
}
bool StrEquals(const char* comparisonNt, MyStr_t target)
{
	MyStr_t comparisonStr = NewStr(comparisonNt);
	return StrEquals(comparisonStr, target);
}
bool StrEquals(u64 comparisonLength, const char* comparisonPntr, MyStr_t target)
{
	MyStr_t comparisonStr = NewStr(comparisonLength, comparisonPntr);
	return StrEquals(comparisonStr, target);
}

//TODO: These currently don't support unicode's idea of "lowercase/uppercase" equivalent characters
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2, u64 compareLength)
{
	NotNullStr(&str1);
	NotNullStr(&str2);
	for (u64 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		if (cIndex >= str1.length && cIndex >= str2.length)
		{
			return 0; //they were equal all the way up until they both ended early
		}
		else if (cIndex >= str1.length)
		{
			return -1; //str1 ended earlier than str2
		}
		else if (cIndex >= str2.length)
		{
			return 1; //str2 ended earlier than str1
		}
		else
		{
			char char1 = GetLowercaseAnsiiChar(str1.pntr[cIndex]);
			char char2 = GetLowercaseAnsiiChar(str2.pntr[cIndex]);
			if (char1 > char2)
			{
				return 1; //str1 comes after str2 alphabetically (str1 > str2)
			}
			else if (char2 > char1)
			{
				return -1; //str2 comes after str1 alphabetically (str1 < str2)
			}
			else { continue; }
		}
	}
	return 0;
}
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2)
{
	i32 compareResult = StrCompareIgnoreCase(str1, str2, MinU64(str1.length, str2.length));
	if (compareResult != 0) { return compareResult; }
	if (str1.length > str2.length) { return 1; }
	if (str1.length < str2.length) { return -1; }
	return 0;
}
i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr, u64 compareLength)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, str2, compareLength);
}
i32 StrCompareIgnoreCase(MyStr_t str1, const char* nullTermStr)
{
	MyStr_t str2 = NewStr(nullTermStr);
	return StrCompareIgnoreCase(str1, str2);
}
i32 StrCompareIgnoreCase(const char* str1, const char* str2, u64 compareLength)
{
	NotNull(str1);
	NotNull(str2);
	for (u64 cIndex = 0; cIndex < compareLength; cIndex++)
	{
		char char1 = GetLowercaseAnsiiChar(str1[cIndex]);
		char char2 = GetLowercaseAnsiiChar(str2[cIndex]);
		if (char1 == '\0' && char2 == '\0')
		{
			return 0; //they were equal all the way up until they both ended
		}
		else if (char1 == '\0')
		{
			return -1; //str1 ended earlier than str2
		}
		else if (char2 == '\0')
		{
			return 1; //str2 ended earlier than str1
		}
		else
		{
			if (char1 > char2)
			{
				return 1; //str1 comes after str2 alphabetically (str1 > str2)
			}
			else if (char2 > char1)
			{
				return -1; //str2 comes after str1 alphabetically (str1 < str2)
			}
			else { continue; }
		}
	}
	return 0;
}

bool StrEqualsIgnoreCase(MyStr_t target, MyStr_t comparison)
{
	return (StrCompareIgnoreCase(target, comparison) == 0);
}
bool StrEqualsIgnoreCase(MyStr_t target, const char* comparisonNt)
{
	return (StrCompareIgnoreCase(target, comparisonNt) == 0);
}
bool StrEqualsIgnoreCase(MyStr_t target, u64 comparisonLength, const char* comparisonPntr)
{
	return (StrCompareIgnoreCase(target, comparisonPntr, comparisonLength) == 0);
}
bool StrEqualsIgnoreCase(const char* comparisonNt, MyStr_t target)
{
	return (StrCompareIgnoreCase(target, comparisonNt) == 0);
}
bool StrEqualsIgnoreCase(u64 comparisonLength, const char* comparisonPntr, MyStr_t target)
{
	return (StrCompareIgnoreCase(target, comparisonPntr, comparisonLength) == 0);
}

bool StrStartsWith(MyStr_t str, MyStr_t prefix, bool ignoreCase = false)
{
	NotNullStr_(&str);
	NotNullStr_(&prefix);
	if (prefix.length == 0) { return true; }
	if (str.length < prefix.length) { return false; }
	if (ignoreCase)
	{
		if (StrEqualsIgnoreCase(NewStr(prefix.length, str.pntr), prefix)) { return true; }
	}
	else
	{
		if (StrEquals(NewStr(prefix.length, str.pntr), prefix)) { return true; }
	}
	return false;
}
bool StrStartsWith(MyStr_t str, const char* nullTermPrefixStr, bool ignoreCase = false)
{
	NotNull_(nullTermPrefixStr);
	return StrStartsWith(str, NewStr(nullTermPrefixStr), ignoreCase);
}
bool StrStartsWith(const char* nullTermStr, MyStr_t prefix, bool ignoreCase = false)
{
	NotNull_(nullTermStr);
	return StrStartsWith(NewStr(nullTermStr), prefix, ignoreCase);
}
bool StrStartsWith(const char* nullTermStr, const char* nullTermPrefixStr, bool ignoreCase = false)
{
	NotNull_(nullTermStr);
	NotNull_(nullTermPrefixStr);
	return StrStartsWith(NewStr(nullTermStr), NewStr(nullTermPrefixStr), ignoreCase);
}

bool StrEndsWith(MyStr_t str, MyStr_t suffix, bool ignoreCase = false)
{
	NotNullStr_(&str);
	NotNullStr_(&suffix);
	if (suffix.length == 0) { return true; }
	if (str.length < suffix.length) { return false; }
	if (ignoreCase)
	{
		if (StrEqualsIgnoreCase(StrSubstring(&str, str.length - suffix.length), suffix)) { return true; }
	}
	else
	{
		if (StrEquals(StrSubstring(&str, str.length - suffix.length), suffix)) { return true; }
	}
	return false;
}
bool StrEndsWith(MyStr_t str, const char* nullTermSuffix, bool ignoreCase = false)
{
	return StrEndsWith(str, NewStr(nullTermSuffix), ignoreCase);
}
bool StrEndsWith(const char* nullTermStr, MyStr_t suffix, bool ignoreCase = false)
{
	return StrEndsWith(NewStr(nullTermStr), suffix, ignoreCase);
}
bool StrEndsWith(const char* nullTermStr, const char* nullTermSuffix, bool ignoreCase = false)
{
	return StrEndsWith(NewStr(nullTermStr), NewStr(nullTermSuffix), ignoreCase);
}

bool StrStartsWithSlash(MyStr_t str)
{
	NotNullStr(&str);
	return (str.length > 0 && (str.chars[0] == '/' || str.chars[0] == '\\'));
}
bool StrStartsWithSlash(const char* nullTermStr)
{
	return StrStartsWithSlash(NewStr(nullTermStr));
}

bool StrEndsWithSlash(MyStr_t str)
{
	NotNullStr(&str);
	return (str.length > 0 && (str.chars[str.length-1] == '/' || str.chars[str.length-1] == '\\'));
}
bool StrEndsWithSlash(const char* nullTermStr)
{
	return StrEndsWithSlash(NewStr(nullTermStr));
}

bool SplitStringFixed(MyStr_t target, MyStr_t delineator, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false)
{
	u64 pieceIndex = 0;
	u64 prevDelineator = 0;
	for (u64 cIndex = 0; cIndex + delineator.length <= target.length+1; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator.length > target.length) { match = true; }
		else if (ignoreCase && StrEqualsIgnoreCase(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		else if (!ignoreCase && StrEquals(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		if (match)
		{
			if (pieceIndex < numPieces)
			{
				piecesBuffer[pieceIndex] = StrSubstring(&target, prevDelineator, cIndex);
				pieceIndex++;
			}
			else { return false; }
			prevDelineator = cIndex + delineator.length;
			cIndex += delineator.length-1;
		}
	}
	if (pieceIndex != numPieces) { return false; }
	return true;
}
bool SplitStringFixed(MyStr_t target, const char* delineatorNullTerm, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false)
{
	return SplitStringFixed(target, NewStr(delineatorNullTerm), numPieces, piecesBuffer, ignoreCase);
}
bool SplitStringFixed(const char* targetNullTerm, MyStr_t delineator, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false)
{
	return SplitStringFixed(NewStr(targetNullTerm), delineator, numPieces, piecesBuffer, ignoreCase);
}
bool SplitStringFixed(const char* targetNullTerm, const char* delineatorNullTerm, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false)
{
	return SplitStringFixed(NewStr(targetNullTerm), NewStr(delineatorNullTerm), numPieces, piecesBuffer, ignoreCase);
}

//TODO: This has some kind of bug when the delineator is 2 or more characters long it chops off the last character (or more?) of the target!
MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	NotNull(memArena);
	NotNullStr(&target);
	NotEmptyStr(&delineator);
	
	u64 numPieces = 0;
	u64 prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target.length+1 - delineator.length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator.length > target.length) { match = true; }
		else if (ignoreCase && StrEqualsIgnoreCase(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		else if (!ignoreCase && StrEquals(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		if (match)
		{
			numPieces++;
			prevDelineator = cIndex + delineator.length;
			cIndex += delineator.length-1;
		}
	}
	AssertMsg(prevDelineator >= target.length, "We missed a piece at the end somehow? We should be going 1 past the end and considering it a match");
	
	if (numPieces == 0)
	{
		if (numPiecesOut != nullptr) { *numPiecesOut = 0; }
		return nullptr;
	}
	MyStr_t* results = AllocArray(memArena, MyStr_t, numPieces);
	NotNull(results);
	
	u64 pIndex = 0;
	prevDelineator = 0;
	for (u64 cIndex = 0; cIndex <= target.length+1 - delineator.length; cIndex++)
	{
		bool match = false;
		if (cIndex + delineator.length > target.length) { match = true; }
		else if (ignoreCase && StrEqualsIgnoreCase(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		else if (!ignoreCase && StrEquals(StrSubstringLength(&target, cIndex, delineator.length), delineator)) { match = true; }
		if (match)
		{
			Assert(pIndex < numPieces);
			Assert(cIndex >= prevDelineator);
			results[pIndex].pntr = &target.pntr[prevDelineator];
			results[pIndex].length = cIndex - prevDelineator;
			pIndex++;
			prevDelineator = cIndex + delineator.length;
			cIndex += delineator.length-1;
		}
	}
	AssertMsg(pIndex == numPieces, "Expected the same number of pieces in both runs through the loop");
	
	if (numPiecesOut != nullptr) { *numPiecesOut = numPieces; }
	return results;
}
MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}
MyStr_t* SplitString(MemArena_t* memArena, const char* targetNt, const char* delineatorNt, u64* numPiecesOut = nullptr, bool ignoreCase = false)
{
	MyStr_t target = NewStr(targetNt);
	MyStr_t delineator = NewStr(delineatorNt);
	return SplitString(memArena, target, delineator, numPiecesOut, ignoreCase);
}

bool SplitStringFast(SplitStringContext_t* context, MyStr_t target, char separatorChar, bool includeEmptyPieces = false)
{
	for (u64 cIndex = context->lastSeparatorIndex; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == separatorChar)
		{
			if (includeEmptyPieces || cIndex > context->lastSeparatorIndex)
			{
				context->piece.pntr = &target.pntr[context->lastSeparatorIndex];
				context->piece.length = cIndex - context->lastSeparatorIndex;
				context->lastSeparatorIndex = cIndex+1;
				return true;
			}
		}
	}
	context->lastSeparatorIndex = target.length;
	return false;
}

MyStr_t* SplitStringBySpacesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut)
{
	DebugAssert(tempArena != nullptr);
	DebugAssert(DoesMemArenaSupportPushAndPop(tempArena));
	DebugAssert(target.pntr != nullptr || target.length == 0);
	DebugAssert(numPiecesOut != nullptr);
	*numPiecesOut = 0;
	MyStr_t* result = nullptr;
	u64 previousSplitIndex = 0;
	for (u64 cIndex = 0; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == ' ')
		{
			if (cIndex > previousSplitIndex)
			{
				MyStr_t* newStr = AllocStruct(tempArena, MyStr_t);
				DebugAssert(newStr != nullptr);
				DebugAssert(result == nullptr || newStr == result + (*numPiecesOut));
				newStr->length = cIndex - previousSplitIndex;
				newStr->pntr = &target.pntr[previousSplitIndex];
				if (result == nullptr) { result = newStr; }
				*numPiecesOut = (*numPiecesOut) + 1;
			}
			previousSplitIndex = cIndex+1;
		}
	}
	return result;
}
MyStr_t* SplitStringBySlashesFastTemp(MemArena_t* tempArena, MyStr_t target, u64* numPiecesOut)
{
	DebugAssert(tempArena != nullptr);
	DebugAssert(DoesMemArenaSupportPushAndPop(tempArena));
	DebugAssert(target.pntr != nullptr || target.length == 0);
	DebugAssert(numPiecesOut != nullptr);
	*numPiecesOut = 0;
	MyStr_t* result = nullptr;
	u64 previousSplitIndex = 0;
	for (u64 cIndex = 0; cIndex <= target.length; cIndex++)
	{
		if (cIndex == target.length || target.pntr[cIndex] == '/')
		{
			if (cIndex > previousSplitIndex)
			{
				MyStr_t* newStr = AllocStruct(tempArena, MyStr_t);
				DebugAssert(newStr != nullptr);
				DebugAssert(result == nullptr || newStr == result + (*numPiecesOut));
				newStr->length = cIndex - previousSplitIndex;
				newStr->pntr = &target.pntr[previousSplitIndex];
				if (result == nullptr) { result = newStr; }
				*numPiecesOut = (*numPiecesOut) + 1;
			}
			previousSplitIndex = cIndex+1;
		}
	}
	return result;
}

//TODO: This doesn't play SUPER nice with unicode strings. It shouldn't really "fail" though
//TODO: Add support for hex or unicode sequences
//Returns the number of bytes that the string shrunk by after unescaping
u64 UnescapeQuotedStringInPlace(MyStr_t* target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
{
	NotNullStr(target);
	if (target->length == 0) { return 0; }
	u64 numBytesSmaller = 0;
	u64 writeIndex = 0;
	for (u64 readIndex = 0; readIndex < target->length; readIndex++)
	{
		char currChar = target->pntr[readIndex];
		char nextChar = (readIndex+1 < target->length) ? target->pntr[readIndex+1] : '\0';
		if (currChar == '\\')
		{
			if (nextChar == '\\')
			{
				target->pntr[writeIndex] = '\\';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '\"')
			{
				target->pntr[writeIndex] = '\"';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '\'')
			{
				target->pntr[writeIndex] = '\'';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 't')
			{
				target->pntr[writeIndex] = '\t';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'n' && allowNewLineEscapes)
			{
				target->pntr[writeIndex] = '\n';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'r' && allowNewLineEscapes)
			{
				target->pntr[writeIndex] = '\r';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'b' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\b';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'a' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\a';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'b' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\b';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'f' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\f';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == 'v' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\v';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else if (nextChar == '?' && allowOtherEscapeCodes)
			{
				target->pntr[writeIndex] = '\?';
				writeIndex++;
				numBytesSmaller++;
				readIndex++;
			}
			else
			{
				//Invalid escape code
				//Just leave this escape sequence
				target->pntr[writeIndex] = target->pntr[readIndex];
				writeIndex++;
			}
		}
		else if (currChar == '\"' && (readIndex == 0 || readIndex == target->length-1) && removeQuotes)
		{
			numBytesSmaller++;
			//don't increment writeIndex
		}
		else
		{
			target->pntr[writeIndex] = target->pntr[readIndex];
			writeIndex++;
		}
	}
	Assert(numBytesSmaller <= target->length);
	Assert(numBytesSmaller == target->length - writeIndex);
	target->length -= numBytesSmaller;
	if (numBytesSmaller > 0) 
	{
		//If we shrunk at all then we should have space to insert a new null-term char
		//Otherwise we rely on the null-term pre-existing if the string didn't change at all
		target->pntr[target->length] = '\0';
	}
	return numBytesSmaller;
}
//TODO: This is not quite right because the result length won't match the allocated size and deallocating that string with FreeString might Assert
MyStr_t UnescapeQuotedStringInArena(MemArena_t* memArena, MyStr_t target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
{
	NotNull(memArena);
	MyStr_t result = AllocString(memArena, &target);
	u64 numBytesSmaller = UnescapeQuotedStringInPlace(&result, removeQuotes, allowNewLineEscapes, allowOtherEscapeCodes);
	UNUSED(numBytesSmaller);
	return result;
}

//NOTE: Unlike the other split string functions, the string pieces are each allocated from the memArena rather than pointing at target (because we need to unescape them without modifying the original)
MyStr_t* SplitStringBySpacesWithQuotesAndUnescape(MemArena_t* memArena, MyStr_t target, u64* numPiecesOut)
{
	NotNullStr(&target);
	bool insideQuotes = false;
	bool prevCharWasBackslash = false;
	u64 pieceStart = 0;
	
	u64 numPieces = 0;
	for (u64 bIndex = 0; bIndex <= target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = 0;
		if (bIndex < target.length)
		{
			codepointSize = GetCodepointForUtf8Str(target, bIndex, &codepoint);
			if (codepointSize == 0)
			{
				codepoint = CharToU32(target.pntr[bIndex]);
				codepointSize = 1;
			}
		}
		else
		{
			codepoint = '\0';
			codepointSize = 1;
		}
		
		bool thisCharIsUnescapedBackslash = false;
		if (codepoint == '\"' && !prevCharWasBackslash)
		{
			insideQuotes = !insideQuotes;
		}
		else if (codepoint == '\\' && !prevCharWasBackslash)
		{
			thisCharIsUnescapedBackslash = insideQuotes;
		}
		else if ((codepoint == ' ' && !insideQuotes) || codepoint == '\0')
		{
			numPieces++;
			pieceStart = bIndex + codepointSize;
		}
		
		bIndex += codepointSize;
		prevCharWasBackslash = thisCharIsUnescapedBackslash;
	}
	Assert(numPieces >= 1);
	
	if (numPiecesOut != nullptr) { *numPiecesOut = numPieces; }
	if (memArena == nullptr) { return nullptr; }
	MyStr_t* pieces = AllocArray(memArena, MyStr_t, numPieces);
	NotNull(pieces);
	
	insideQuotes = false;
	prevCharWasBackslash = false;
	pieceStart = 0;
	u64 pieceIndex = 0;
	for (u64 bIndex = 0; bIndex <= target.length; )
	{
		u32 codepoint = 0;
		u8 codepointSize = 0;
		if (bIndex < target.length)
		{
			codepointSize = GetCodepointForUtf8Str(target, bIndex, &codepoint);
			if (codepointSize == 0)
			{
				codepoint = CharToU32(target.pntr[bIndex]);
				codepointSize = 1;
			}
		}
		else
		{
			codepoint = '\0';
			codepointSize = 1;
		}
		
		bool thisCharIsUnescapedBackslash = false;
		if (codepoint == '\"' && !prevCharWasBackslash)
		{
			insideQuotes = !insideQuotes;
		}
		else if (codepoint == '\\' && !prevCharWasBackslash)
		{
			thisCharIsUnescapedBackslash = insideQuotes;
		}
		else if ((codepoint == ' ' && !insideQuotes) || codepoint == '\0')
		{
			MyStr_t escapedPiece = StrSubstring(&target, pieceStart, bIndex);
			
			MyStr_t newPiece = MyStr_Empty;
			if (StrStartsWith(escapedPiece, "\""))
			{
				newPiece = UnescapeQuotedStringInArena(memArena, escapedPiece, true, true, true);
				NotNullStr(&newPiece);
			}
			else
			{
				newPiece = AllocString(memArena, &escapedPiece);
				NotNullStr(&newPiece);
			}
			
			Assert(pieceIndex < numPieces);
			pieces[pieceIndex] = newPiece;
			pieceIndex++;
			
			pieceStart = bIndex + codepointSize;
		}
		
		bIndex += codepointSize;
		prevCharWasBackslash = thisCharIsUnescapedBackslash;
	}
	Assert(pieceIndex == numPieces);
	
	return pieces;
}

// if extensionOut is not passed then the extension will be included in the fileNameOut
// Output MyStr_t are not reallocated so they are not all null-terminated
// extensionOut will include the '.' character
// If the path is actually a directory only we may interpret the last folder name as fileName w/ extension.
// Only use full file paths in order to avoid this, or have trailing "/"
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr)
{
	NotNullStr(&fullPath);
	if (fullPath.length == 0)
	{
		if (directoryOut != nullptr) { *directoryOut = MyStr_Empty; }
		if (fileNameOut != nullptr) { *fileNameOut = MyStr_Empty; }
		if (extensionOut != nullptr) { *extensionOut = MyStr_Empty; }
		return;
	}
	
	bool foundSlash = false;
	bool foundPeriod = false;
	u64 lastSlashIndex = 0; //index after char
	u64 lastPeriodIndex = fullPath.length; //index before char
	for (u64 cIndex = 0; cIndex < fullPath.length; cIndex++)
	{
		if (fullPath.pntr[cIndex] == '\\' || fullPath.pntr[cIndex] == '/')
		{
			foundSlash = true;
			lastSlashIndex = cIndex+1;
		}
		if (fullPath.pntr[cIndex] == '.')
		{
			foundPeriod = true;
			lastPeriodIndex = cIndex;
		}
	}
	if (foundPeriod && lastPeriodIndex < lastSlashIndex) //periods in the directory portion are relative directives like ".\..\" 
	{
		foundPeriod = false;
		lastPeriodIndex = fullPath.length;
	}
	Assert(lastPeriodIndex >= lastSlashIndex);
	
	if (directoryOut != nullptr)
	{
		*directoryOut = NewStr(lastSlashIndex, &fullPath.pntr[0]);
	}
	if (fileNameOut != nullptr)
	{
		if (extensionOut != nullptr)
		{
			*fileNameOut = NewStr(lastPeriodIndex - lastSlashIndex, &fullPath.pntr[lastSlashIndex]);
			*extensionOut = NewStr(fullPath.length - lastPeriodIndex, &fullPath.pntr[lastPeriodIndex]);
		}
		else
		{
			*fileNameOut = NewStr(fullPath.length - lastSlashIndex, &fullPath.pntr[lastSlashIndex]);
		}
	}
}
MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
{
	MyStr_t result;
	MyStr_t extensionThrowAway;
	SplitFilePath(filePath, nullptr, &result, (includeExtension ? nullptr : &extensionThrowAway));
	NotNullStr(&result);
	return result;
}

MyStr_t GetDirectoryPart(MyStr_t filePath)
{
	MyStr_t result;
	SplitFilePath(filePath, &result, nullptr, nullptr);
	NotNullStr(&result);
	return result;
}

//NOTE: Because this is expecting a null-terminated output, we can provide the option for includeExtension=false
const char* GetFileNamePartNt(const char* filePath)
{
	NotNull(filePath);
	MyStr_t result;
	SplitFilePath(NewStr(filePath), nullptr, &result, nullptr);
	NotNullStr(&result);
	return result.pntr;
}

void StrSpliceInPlace(MyStr_t target, u64 startIndex, MyStr_t replacement)
{
	NotNullStr(&target);
	NotNullStr(&replacement);
	Assert(startIndex <= target.length);
	Assert(startIndex + replacement.length <= target.length);
	if (replacement.length == 0) { return; }
	MyMemCopy(&target.chars[startIndex], &replacement.chars[0], replacement.length);
}
void StrSpliceInPlace(MyStr_t target, u64 startIndex, const char* replacementNullTerm)
{
	StrSpliceInPlace(target, startIndex, NewStr(replacementNullTerm));
}
void StrSpliceInPlace(char* targetNullTermStr, u64 startIndex, MyStr_t replacement)
{
	StrSpliceInPlace(NewStr(targetNullTermStr), startIndex, replacement);
}
void StrSpliceInPlace(char* targetNullTermStr, u64 startIndex, const char* replacementNullTerm)
{
	StrSpliceInPlace(NewStr(targetNullTermStr), startIndex, NewStr(replacementNullTerm));
}

MyStr_t StrSplice(MyStr_t target, u64 startIndex, u64 endIndex, MyStr_t replacement, MemArena_t* memArena)
{
	NotNull(memArena);
	NotNullStr(&target);
	NotNullStr(&replacement);
	Assert(startIndex <= target.length);
	Assert(endIndex <= target.length);
	Assert(endIndex >= startIndex);
	
	MyStr_t result;
	result.length = startIndex + replacement.length + (target.length - endIndex);
	result.chars = AllocArray(memArena, char, result.length+1);
	NotNull(result.chars);
	
	if (startIndex > 0) { MyMemCopy(&result.chars[0], &target.chars[0], startIndex); }
	if (replacement.length > 0) { MyMemCopy(&result.chars[startIndex], &replacement.chars[0], replacement.length); }
	if (endIndex < target.length) { MyMemCopy(&result.chars[startIndex + replacement.length], &target.chars[endIndex], target.length - endIndex); }
	result.chars[result.length] = '\0';
	
	return result;
}
MyStr_t StrSplice(MyStr_t target, u64 startIndex, u64 endIndex, const char* replacementNullTerm, MemArena_t* memArena)
{
	return StrSplice(target, startIndex, endIndex, NewStr(replacementNullTerm), memArena);
}
MyStr_t StrSplice(char* targetNullTermStr, u64 startIndex, u64 endIndex, MyStr_t replacement, MemArena_t* memArena)
{
	return StrSplice(NewStr(targetNullTermStr), startIndex, endIndex, replacement, memArena);
}
MyStr_t StrSplice(char* targetNullTermStr, u64 startIndex, u64 endIndex, const char* replacement, MemArena_t* memArena)
{
	return StrSplice(NewStr(targetNullTermStr), startIndex, endIndex, NewStr(replacement), memArena);
}

//TODO: This is really slow right now, when we are replacing "\r\n" with "\n" in win32_files.cpp when asked to convertNewLines
//      We should do some profiling and optimize this function a bit more
//Returns the number of instances replaced
u64 StrReplaceInPlace(MyStr_t str, MyStr_t target, MyStr_t replacement, bool ignoreCase = false, bool allowShrinking = false)
{
	NotNullStr(&str);
	NotNullStr(&target);
	NotNullStr(&replacement);
	AssertIf(!allowShrinking, target.length == replacement.length);
	AssertIf(allowShrinking, target.length >= replacement.length);
	if (target.length == 0) { return 0; } //nothing to replace
	
	u64 numReplacements = 0;
	if (!ignoreCase) //Faster code-path:
	{
		u64 writeIndex = 0;
		u64 prevMemMove = 0;
		u64 readIndex = 0;
		for (readIndex = 0; readIndex < str.length; readIndex++)
		{
			if (readIndex + target.length <= str.length &&
				MyMemCompare(&str.chars[readIndex], target.chars, target.length) == 0)
			{
				if (prevMemMove < writeIndex)
				{
					u64 amountToMove = writeIndex - prevMemMove;
					MyMemMove(&str.chars[prevMemMove], &str.chars[readIndex - amountToMove], amountToMove);
				}
				MyMemCopy(&str.chars[writeIndex], replacement.chars, replacement.length);
				writeIndex += replacement.length;
				readIndex += target.length-1;
				prevMemMove = writeIndex;
				numReplacements++;
			}
			else
			{
				//str.pntr[writeIndex] = str.pntr[readIndex];
				writeIndex++;
			}
		}
		if (prevMemMove < writeIndex)
		{
			u64 amountToMove = writeIndex - prevMemMove;
			MyMemMove(&str.chars[prevMemMove], &str.chars[readIndex - amountToMove], amountToMove);
		}
	}
	else
	{
		u64 writeIndex = 0;
		for (u64 readIndex = 0; readIndex < str.length; readIndex++)
		{
			if (readIndex + target.length <= str.length &&
				((ignoreCase && StrEqualsIgnoreCase(StrSubstringLength(&str, readIndex, target.length), target)) ||
				(!ignoreCase && StrEquals(StrSubstringLength(&str, readIndex, target.length), target))))
			{
				for (u64 cIndex = 0; cIndex < replacement.length; cIndex++)
				{
					str.pntr[writeIndex + cIndex] = replacement.pntr[cIndex];
				}
				writeIndex += replacement.length;
				readIndex += target.length-1;
				numReplacements++;
			}
			else
			{
				str.pntr[writeIndex] = str.pntr[readIndex];
				writeIndex++;
			}
		}
	}
	
	return numReplacements;
}
u64 StrReplaceInPlace(MyStr_t str, const char* target, const char* replacement, bool ignoreCase = false, bool allowShrinking = false)
{
	NotNullStr(&str);
	NotNull(target);
	NotNull(replacement);
	return StrReplaceInPlace(str, NewStr(target), NewStr(replacement), ignoreCase, allowShrinking);
}

//TODO: Add ignoreCase support to this implementation!
MyStr_t StrReplace(MyStr_t str, MyStr_t target, MyStr_t replacement, MemArena_t* memArena)
{
	NotNullStr(&str);
	NotNullStr(&target);
	NotNullStr(&replacement);
	Assert(target.length > 0);
	
	MyStr_t result = {};
	for (u8 pass = 0; pass < 2; pass++)
	{
		u64 numBytesNeeded = 0;
		
		for (u64 bIndex = 0; bIndex < str.length; )
		{
			if (bIndex + target.length <= str.length)
			{
				bool foundTarget = true;
				for (u64 tIndex = 0; tIndex < target.length; )
				{
					u32 strCodepoint = 0;
					u8 strByteSize = GetCodepointForUtf8Str(str, bIndex + tIndex, &strCodepoint);
					if (strByteSize == 0) { strByteSize = 1; strCodepoint = CharToU32(str.pntr[bIndex + tIndex]); } //invalid unicode
					u32 targetCodepoint = 0;
					u8 targetByteSize = GetCodepointForUtf8Str(target, tIndex, &targetCodepoint);
					if (targetByteSize == 0) { targetByteSize = 1; targetCodepoint = target.pntr[tIndex]; } //invalid unicode
					if (strCodepoint != targetCodepoint)
					{
						foundTarget = false;
						break;
					}
					Assert(targetByteSize == strByteSize); //TODO: Get rid of this assumption by having 2 iteration variables, not just tIndex
					tIndex += strByteSize;
				}
				
				if (foundTarget)
				{
					if (result.pntr != nullptr)
					{
						Assert(numBytesNeeded + replacement.length <= result.length);
						MyMemCopy(&result.pntr[numBytesNeeded], replacement.pntr, replacement.length);
					}
					numBytesNeeded += replacement.length;
					bIndex += target.length;
				}
				else
				{
					u32 codepoint = 0;
					u8 charByteSize = GetCodepointForUtf8Str(str, bIndex, &codepoint);
					if (charByteSize == 0) { charByteSize = 1; codepoint = CharToU32(str.pntr[bIndex]); } //invalid unicode
					if (result.pntr != nullptr)
					{
						Assert(numBytesNeeded + charByteSize <= result.length);
						MyMemCopy(&result.pntr[numBytesNeeded], &str.pntr[bIndex], charByteSize);
					}
					numBytesNeeded += charByteSize;
					bIndex += charByteSize;
				}
			}
			else
			{
				u32 codepoint = 0;
				u8 charByteSize = GetCodepointForUtf8Str(str, bIndex, &codepoint);
				if (charByteSize == 0) { charByteSize = 1; codepoint = CharToU32(str.pntr[bIndex]); } //invalid unicode
				if (result.pntr != nullptr)
				{
					Assert(numBytesNeeded + charByteSize <= result.length);
					MyMemCopy(&result.pntr[numBytesNeeded], &str.pntr[bIndex], charByteSize);
				}
				numBytesNeeded += charByteSize;
				bIndex += charByteSize;
			}
		}
		
		if (pass == 0)
		{
			result.length = numBytesNeeded;
			result.pntr = AllocArray(memArena, char, result.length+1);
			Assert(result.pntr);
		}
		else
		{
			Assert(numBytesNeeded == result.length);
			result.pntr[result.length] = '\0';
		}
	}
	return result;
}
MyStr_t StrReplace(MyStr_t str, const char* target, const char* replacement, MemArena_t* memArena)
{
	return StrReplace(str, NewStr(target), NewStr(replacement), memArena);
}

MyStr_t StrReplaceMultiple(MyStr_t str, u64 numReplacements, const MyStrPair_t* replacements, MemArena_t* memArena)
{
	NotNullStr(&str);
	
	MyStr_t result = MyStr_Empty;
	for (u8 pass = 0; pass < 2; pass++)
	{
		u64 writeIndex = 0;
		
		for (u64 readIndex = 0; readIndex < str.length; readIndex++)
		{
			bool foundReplacement = false;
			for (u64 rIndex = 0; rIndex < numReplacements; rIndex++)
			{
				const MyStrPair_t* replacement = &replacements[rIndex];
				NotNullStr(&replacement->key);
				NotNullStr(&replacement->value);
				Assert(replacement->key.length > 0);
				if (readIndex + replacement->key.length <= str.length)
				{
					if (MyMemCompare(&str.chars[readIndex], replacement->key.chars, replacement->key.length) == 0)
					{
						if (result.chars != nullptr)
						{
							Assert(writeIndex + replacement->value.length <= result.length);
							if (replacement->value.length > 0) { MyMemCopy(&result.chars[writeIndex], replacement->value.chars, replacement->value.length); }
						}
						writeIndex += replacement->value.length;
						readIndex += replacement->key.length-1;
						foundReplacement = true;
						break;
					}
				}
			}
			
			if (!foundReplacement)
			{
				if (result.chars != nullptr)
				{
					result.chars[writeIndex] = str.chars[readIndex];
				}
				writeIndex++;
			}
		}
		
		if (pass == 0)
		{
			result.length = writeIndex;
			if (memArena == nullptr) { return result; }
			result.chars = AllocArray(memArena, char, result.length+1);
			NotNull(result.chars);
		}
		else
		{
			Assert(writeIndex == result.length);
			result.chars[result.length] = '\0';
		}
	}
	
	return result;
}

//TODO: This should return true if target and substring are equal!
bool FindSubstring(MyStr_t target, MyStr_t substring, u64* indexOut = nullptr, bool ignoreCase = false, u64 startIndex = 0)
{
	NotNullStr(&target);
	NotNullStr(&substring);
	if (substring.length > target.length) { return false; }
	if (substring.length == 0) { return false; }
	
	if (!ignoreCase) //Faster code-path:
	{
		for (u64 cIndex = startIndex; cIndex + substring.length <= target.length; cIndex++)
		{
			if (MyMemCompare(&target.chars[cIndex], substring.chars, substring.length) == 0)
			{
				SetOptionalOutPntr(indexOut, cIndex);
				return true;
			}
		}
	}
	else
	{
		for (u64 cIndex = startIndex; cIndex + substring.length <= target.length; )
		{
			bool allMatched = true;
			u64 cSubIndex = 0;
			for (u64 subIndex = 0; subIndex < substring.length; )
			{
				u32 targetCodepoint = 0;
				u8 targetCodepointSize = GetCodepointForUtf8Str(target, cIndex + cSubIndex, &targetCodepoint);
				u32 subCodepoint = 0;
				u8 subCodepointSize = GetCodepointForUtf8Str(substring, subIndex, &subCodepoint);
				if (targetCodepointSize == 0 || subCodepointSize == 0)
				{
					return false;
				}
				
				if ((!ignoreCase && targetCodepoint != subCodepoint) ||
					(ignoreCase && GetLowercaseCodepoint(targetCodepoint) != GetLowercaseCodepoint(subCodepoint)))
				{
					allMatched = false;
					break;
				}
				
				subIndex += subCodepointSize;
				cSubIndex += targetCodepointSize;
			}
			if (allMatched)
			{
				SetOptionalOutPntr(indexOut, cIndex);
				return true;
			}
			else
			{
				u8 targetCodepointSize = GetCodepointForUtf8Str(target, cIndex, nullptr);
				if (targetCodepointSize == 0)
				{
					return false;
				}
				cIndex += targetCodepointSize;
			}
		}
	}
	
	return false;
}
bool FindSubstring(MyStr_t target, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0)
{
	return FindSubstring(target, NewStr(nullTermSubstring), indexOut, ignoreCase, startIndex);
}
bool FindSubstring(const char* nullTermTarget, MyStr_t substring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0)
{
	return FindSubstring(NewStr(nullTermTarget), substring, indexOut, ignoreCase, startIndex);
}
bool FindSubstring(const char* nullTermTarget, const char* nullTermSubstring, u64* indexOut= nullptr, bool ignoreCase = false, u64 startIndex = 0)
{
	return FindSubstring(NewStr(nullTermTarget), NewStr(nullTermSubstring), indexOut, ignoreCase, startIndex);
}

MyStr_t FindStrParensPart(MyStr_t target, char openParensChar = '(', char closeParensChar = ')')
{
	NotNullStr(&target);
	u64 openParensIndex = target.length;
	u64 parensLevel = 0;
	for (u64 cIndex = 0; cIndex < target.length; cIndex++)
	{
		if (target.pntr[cIndex] == openParensChar)
		{
			if (parensLevel == 0)
			{
				openParensIndex = cIndex;
			}
			parensLevel++;
		}
		else if (target.pntr[cIndex] == closeParensChar)
		{
			if (parensLevel > 1)
			{
				parensLevel--;
			}
			else if (parensLevel == 1)
			{
				return NewStr(cIndex+1 - openParensIndex, &target.pntr[openParensIndex]);
			}
		}
	}
	return MyStr_Empty;
}
MyStr_t FindStrParensPart(const char* nullTermTarget, char openParensChar = '(', char closeParensChar = ')')
{
	return FindStrParensPart(NewStr(nullTermTarget), openParensChar, closeParensChar);
}

//takes the str and returns a string that repeats that str some number of times
MyStr_t StringRepeat(MemArena_t* memArena, MyStr_t str, u64 numRepetitions)
{
	NotNull(memArena);
	NotNullStr(&str);
	MyStr_t result;
	result.length = str.length * numRepetitions;
	result.pntr = AllocArray(memArena, char, result.length+1);
	NotNull(result.pntr);
	for (u64 rIndex = 0; rIndex < numRepetitions; rIndex++)
	{
		MyMemCopy(&result.pntr[rIndex * str.length], str.pntr, str.length);
	}
	result.pntr[result.length] = '\0';
	return result;
}
MyStr_t StringRepeat(MemArena_t* memArena, const char* nullTermStr, u64 numRepetitions)
{
	return StringRepeat(memArena, NewStr(nullTermStr), numRepetitions);
}

MyStr_t FormatBytes(u64 numBytes, MemArena_t* memArena)
{
	u64 gigabytes = numBytes/Gigabytes(1);
	u64 megabytes = (numBytes%Gigabytes(1))/Megabytes(1);
	u64 kilobytes = (numBytes%Megabytes(1))/Kilobytes(1);
	u64 remainder = (numBytes % Kilobytes(1));
	if (numBytes >= Kilobytes(1))
	{
		if (numBytes >= Megabytes(1))
		{
			if (numBytes >= Gigabytes(1))
			{
				return PrintInArenaStr(memArena, "%lluG %lluM %lluk %llub", gigabytes, megabytes, kilobytes, remainder);
			}
			else
			{
				return PrintInArenaStr(memArena, "%lluM %lluk %llub", megabytes, kilobytes, remainder);
			}
		}
		else
		{
			return PrintInArenaStr(memArena, "%lluk %llub", kilobytes, remainder);
		}
	}
	else
	{
		return PrintInArenaStr(memArena, "%llub", remainder);
	}
}
const char* FormatBytesNt(u64 numBytes, MemArena_t* memArena)
{
	return FormatBytes(numBytes, memArena).pntr;
}

MyStr_t FormatNumberWithCommas(u64 number, MemArena_t* memArena = nullptr)
{
	//Max uint64: 18,446,744,073,709,551,615 (20 digits + 6 commas)
	static char printBuffer[20 + 6 + 1];
	int numberLength = MyBufferPrintf(printBuffer, ArrayCount(printBuffer), "%llu", number);
	if (numberLength >= 0 && numberLength < (int)ArrayCount(printBuffer)) { printBuffer[numberLength] = '\0'; }
	for (int cIndex = numberLength-3; cIndex > 0; cIndex -= 3)
	{
		//Shift all the characters up
		for (int cIndex2 = ArrayCount(printBuffer)-1; cIndex2 > cIndex; cIndex2--)
		{
			printBuffer[cIndex2] = printBuffer[cIndex2-1];
		}
		//Insert the comma
		printBuffer[cIndex] = ',';
	}
	MyStr_t result = NewStr(&printBuffer[0]);
	if (memArena != nullptr)
	{
		result = AllocString(memArena, &result);
	}
	return result;
}
const char* FormatNumberWithCommasNt(u64 number, MemArena_t* memArena = nullptr)
{
	return FormatNumberWithCommas(number, memArena).chars;
}

u64 FnvHashStr(MyStr_t str)
{
	return FnvHashU64(str.pntr, str.length);
}
u64 FnvHashStr(const char* nullTermStr)
{
	return FnvHashU64(nullTermStr, MyStrLength64(nullTermStr));
}

bool IsStringValidIdentifier(MyStr_t str, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false)
{
	return IsStringValidIdentifier(str.length, str.chars, allowUnderscores, allowNumbers, allowLeadingNumbers, allowEmpty, allowSpaces);
}

void StrReallocAppend(MyStr_t* baseStr, MyStr_t appendStr, MemArena_t* memArena)
{
	MyStr_t result = StrSplice(*baseStr, baseStr->length, baseStr->length, appendStr, memArena);
	FreeString(memArena, baseStr);
	*baseStr = result;
}
void StrReallocAppend(MyStr_t* baseStr, const char* appendNullTermStr, MemArena_t* memArena)
{
	StrReallocAppend(baseStr, NewStr(appendNullTermStr), memArena);
}

// +--------------------------------------------------------------+
// |                   Word Break Calculations                    |
// +--------------------------------------------------------------+
//TODO: Subword calculation should take into account capital letters and numbers in identifiers
//TODO: We should really dial this logic in so it matches Sublime Text logic perfectly. But for now, it's working pretty well
WordBreakCharClass_t GetWordBreakCharClass(u32 codepoint)
{
	if (codepoint >= 'a' && codepoint <= 'z') { return WordBreakCharClass_AlphabeticLower; }
	if (codepoint >= 'A' && codepoint <= 'Z') { return WordBreakCharClass_AlphabeticUpper; }
	if (codepoint >= '0' && codepoint <= '9') { return WordBreakCharClass_Numeric; }
	if (codepoint == '_') { return WordBreakCharClass_WordPunctuation; }
	if (codepoint == '(' || codepoint == '[' || codepoint == '{') { return WordBreakCharClass_PunctuationOpen; }
	if (codepoint == ')' || codepoint == ']' || codepoint == '}' || codepoint == ';') { return WordBreakCharClass_PunctuationClose; }
	if (codepoint == ' ' || codepoint == '\t' || codepoint == '\n' || codepoint == '\r') { return WordBreakCharClass_Whitespace; }
	if (codepoint >= 0x20 && codepoint <= 0x7F) { return WordBreakCharClass_Punctuation; }
	return WordBreakCharClass_Other;
}
bool IsCharPairWordBreak(u32 prevCodepoint, u32 nextCodepoint, bool forward, bool subwords)
{
	WordBreakCharClass_t prevCharClass = GetWordBreakCharClass(prevCodepoint);
	WordBreakCharClass_t nextCharClass = GetWordBreakCharClass(nextCodepoint);
	if (!forward)
	{
		if (prevCharClass == WordBreakCharClass_PunctuationOpen)  { prevCharClass = WordBreakCharClass_PunctuationClose; }
		if (prevCharClass == WordBreakCharClass_PunctuationClose) { prevCharClass = WordBreakCharClass_PunctuationOpen;  }
		if (nextCharClass == WordBreakCharClass_PunctuationOpen)  { nextCharClass = WordBreakCharClass_PunctuationClose; }
		if (nextCharClass == WordBreakCharClass_PunctuationClose) { nextCharClass = WordBreakCharClass_PunctuationOpen;  }
	}
	bool fromAlphabet = (
		prevCharClass == WordBreakCharClass_AlphabeticLower ||
		prevCharClass == WordBreakCharClass_AlphabeticUpper ||
		prevCharClass == WordBreakCharClass_Numeric ||
		(!subwords && prevCharClass == WordBreakCharClass_WordPunctuation)
	);
	bool toPunctuation = (
		nextCharClass == WordBreakCharClass_Punctuation ||
		nextCharClass == WordBreakCharClass_PunctuationOpen ||
		nextCharClass == WordBreakCharClass_PunctuationClose ||
		nextCharClass == WordBreakCharClass_Whitespace ||
		nextCharClass == WordBreakCharClass_Other ||
		(subwords && nextCharClass == WordBreakCharClass_WordPunctuation)
	);
	if (prevCharClass == nextCharClass) { return false; }
	if (prevCharClass == WordBreakCharClass_Other || nextCharClass == WordBreakCharClass_Other) { return true; }
	if (fromAlphabet && toPunctuation) { return true; }
	if (nextCharClass == WordBreakCharClass_PunctuationClose) { return true; }
	return false;
}
//This function stops at invalid UTF-8 encoding, treating them as single byte characters that always cause a word break
u64 FindNextWordBreakInString(MyStr_t str, u64 startIndex, bool forward, bool subwords, bool includeBreakAtStartIndex = false)
{
	NotNullStr(&str);
	Assert(startIndex <= str.length);
	u64 result = startIndex;
	u8 leftCodepointSize = 0;
	u8 rightCodepointSize = 0;
	for (u64 bIndex = startIndex; bIndex >= 0 && bIndex <= str.length; bIndex += (forward ? rightCodepointSize : -leftCodepointSize))
	{
		if ((!forward && bIndex == 0) || (forward && bIndex == str.length))
		{
			result = bIndex;
			break;
		}
		u32 leftCodepoint = 0;
		leftCodepointSize = GetCodepointBeforeIndex(str.pntr, bIndex, &leftCodepoint);
		if (leftCodepointSize == 0)
		{
			leftCodepointSize = 1;
			if (bIndex != startIndex) { result = bIndex; break; }
		}
		u32 rightCodepoint = 0;
		rightCodepointSize = GetCodepointForUtf8Str(str, bIndex, &rightCodepoint);
		if (rightCodepointSize == 0)
		{
			rightCodepointSize = 1;
			if (bIndex != startIndex) { result = bIndex; break; }
		}
		if (bIndex != startIndex || (includeBreakAtStartIndex && bIndex != 0 && bIndex != str.length))
		{
			bool isWordBreak = IsCharPairWordBreak(
				(forward ? leftCodepoint : rightCodepoint),
				(forward ? rightCodepoint : leftCodepoint),
				forward, subwords
			);
			if (isWordBreak)
			{
				result = bIndex;
				break;
			}
		}
	}
	return result;
}

// +--------------------------------------------------------------+
// |                    Time String Functions                     |
// +--------------------------------------------------------------+
#ifdef _GY_TIME_H
MyStr_t FormatRealTime(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	NotNull(realTime);
	NotNull(memArena);
	MyStr_t result;
	if (includeDayOfWeek)
	{
		if (includeHourMinuteSecond)
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%s %u:%02u:%02u%s (%s %s, %u)",
					GetDayOfWeekStr(realTime->dayOfWeek),
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am",
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena,
					"%s %u:%02u:%02u%s",
					GetDayOfWeekStr(realTime->dayOfWeek),
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am"
				);
			}
		}
		else
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%s (%s %s, %u)",
					GetDayOfWeekStr(realTime->dayOfWeek),
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena, "%s", GetDayOfWeekStr(realTime->dayOfWeek));
			}
		}
	}
	else
	{
		if (includeHourMinuteSecond)
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena,
					"%u:%02u:%02u%s (%s %s, %u)",
					Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second,
					IsPostMeridian(realTime->hour) ? "pm" : "am",
					GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year
				);
			}
			else
			{
				result = PrintInArenaStr(memArena, "%u:%02u:%02u%s", Convert24HourTo12Hour(realTime->hour), realTime->minute, realTime->second, IsPostMeridian(realTime->hour) ? "pm" : "am");
			}
		}
		else
		{
			if (includeMonthDayYear)
			{
				result = PrintInArenaStr(memArena, "(%s %s, %u)", GetMonthStr(realTime->month), GetDayOfMonthString(realTime->day), realTime->year);
			}
			else
			{
				return NewStringInArenaNt(memArena, "");
			}
		}
	}
	return result;
}
const char* FormatRealTimeNt(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
{
	return FormatRealTime(realTime, memArena, includeDayOfWeek, includeHourMinuteSecond, includeMonthDayYear).pntr;
}

MyStr_t FormatMilliseconds(u64 milliseconds, MemArena_t* memArena)
{
	NotNull(memArena);
	if (milliseconds < NUM_MS_PER_DAY)
	{
		if (milliseconds < NUM_MS_PER_HOUR)
		{
			if (milliseconds < NUM_MS_PER_MINUTE)
			{
				if (milliseconds < NUM_MS_PER_SECOND)
				{
					return PrintInArenaStr(memArena, "%llums", milliseconds);
				}
				else
				{
					return PrintInArenaStr(memArena, "%llus %llums",
						milliseconds/NUM_MS_PER_SECOND,
						milliseconds%NUM_MS_PER_SECOND
					);
				}
			}
			else
			{
				return PrintInArenaStr(memArena, "%llum %llus %llums",
					milliseconds/NUM_MS_PER_MINUTE,
					(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
					milliseconds%NUM_MS_PER_SECOND
				);
			}
		}
		else
		{
			return PrintInArenaStr(memArena, "%lluh %llum %llus %llums",
				milliseconds/NUM_MS_PER_HOUR,
				(milliseconds%NUM_MS_PER_HOUR)/NUM_MS_PER_MINUTE,
				(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
				milliseconds%NUM_MS_PER_SECOND
			);
		}
	}
	else
	{
		return PrintInArenaStr(memArena, "%llud %lluh %llum %llus %llums",
			milliseconds/NUM_MS_PER_DAY,
			(milliseconds%NUM_MS_PER_DAY)/NUM_MS_PER_HOUR,
			(milliseconds%NUM_MS_PER_HOUR)/NUM_MS_PER_MINUTE,
			(milliseconds%NUM_MS_PER_MINUTE)/NUM_MS_PER_SECOND,
			milliseconds%NUM_MS_PER_SECOND
		);
	}
}
const char* FormatMillisecondsNt(u64 milliseconds, MemArena_t* memArena)
{
	return FormatMilliseconds(milliseconds, memArena).pntr;
}
#endif //_GY_TIME_H

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_STRING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
MyStr_Empty_Const
MyStr_Empty
WordBreakCharClass_AlphabeticLower
WordBreakCharClass_AlphabeticUpper
WordBreakCharClass_WordPunctuation
WordBreakCharClass_Numeric
WordBreakCharClass_PunctuationOpen
WordBreakCharClass_PunctuationClose
WordBreakCharClass_Whitespace
WordBreakCharClass_Punctuation
WordBreakCharClass_Other
WordBreakCharClass_NumClasses
@Types
MyStr_t
MyWideStr_t
MyStrPair_t
WordBreakCharClass_t
@Functions
const char* GetWordBreakCharClassStr(WordBreakCharClass_t enumValue)
MyStr_t NewStrLengthOnly(u64 length)
MyStr_t NewStr(u64 length, char* pntr)
MyStrPair_t NewStrPair(MyStr_t keyStr, MyStr_t valueStr)
bool IsNullStr(MyStr_t target)
bool IsEmptyStr(MyStr_t target)
bool IsStrNullTerminated(MyStr_t target)
bool BufferIsNullTerminated(u64 bufferSize, const char* bufferPntr)
#define NotNullStr(strPntr)
#define NotNullStr_(strPntr)
#define NotEmptyStr(strPntr)
#define NotEmptyStr_(strPntr)
#define AssertNullTerm(strPntr)
#define AssertNullTerm_(strPntr)
#define AllocString(arena, strPntr)
#define NewStringInArena(arena, length, charPntr)
#define NewStringInArenaNt(arena, nullTermStr)
#define FreeString(arena, strPntr)
#define StrPrint(myStrStruct)
#define StrPntrPrint(myStrPntr)
MyStr_t PrintInArenaStr(MemArena_t* arena, const char* formatString, ...)
u64 TrimLeadingWhitespace(MyStr_t* target, bool trimNewLines = false)
u64 TrimTrailingWhitespace(MyStr_t* target, bool trimNewLines = false)
u64 TrimWhitespace(MyStr_t* target, bool trimNewLines = false)
bool FindNextCharInStr(MyStr_t target, u64 startIndex, MyStr_t searchCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
bool FindNextUnknownCharInStr(MyStr_t target, u64 startIndex, MyStr_t knownCharsStr, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
bool FindNextWhitespaceInStr(MyStr_t target, u64 startIndex, u64* indexOut = nullptr, bool ignoreCharsInQuotes = false)
MyStr_t StrSubstring(MyStr_t* target, u64 startIndex, u64 endIndex)
MyStr_t StrSubstringLength(MyStr_t* target, u64 startIndex, u64 length)
MyStr_t CombineStrs(MemArena_t* memArena, MyStr_t str1, MyStr_t str2)
bool StrEquals(MyStr_t target, MyStr_t comparison)
i32 StrCompareIgnoreCase(MyStr_t str1, MyStr_t str2)
bool StrEqualsIgnoreCase(MyStr_t target, MyStr_t comparison)
bool StrStartsWith(MyStr_t str, MyStr_t prefix, bool ignoreCase = false)
bool StrEndsWith(MyStr_t str, MyStr_t suffix, bool ignoreCase = false)
bool StrStartsWithSlash(MyStr_t str)
bool StrEndsWithSlash(MyStr_t str)
bool SplitStringFixed(MyStr_t target, MyStr_t delineator, u64 numPieces, MyStr_t* piecesBuffer, bool ignoreCase = false)
MyStr_t* SplitString(MemArena_t* memArena, MyStr_t target, MyStr_t delineator, u64* numPiecesOut = nullptr, bool ignoreCase = false)
u64 UnescapeQuotedStringInPlace(MyStr_t* target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
MyStr_t UnescapeQuotedStringInArena(MemArena_t* memArena, MyStr_t target, bool removeQuotes = true, bool allowNewLineEscapes = true, bool allowOtherEscapeCodes = false)
void SplitFilePath(MyStr_t fullPath, MyStr_t* directoryOut, MyStr_t* fileNameOut, MyStr_t* extensionOut = nullptr)
MyStr_t GetFileNamePart(MyStr_t filePath, bool includeExtension = true)
MyStr_t GetDirectoryPart(MyStr_t filePath)
const char* GetFileNamePartNt(const char* filePath)
void StrSpliceInPlace(MyStr_t target, u64 startIndex, MyStr_t replacement)
MyStr_t StrSplice(MyStr_t target, u64 startIndex, u64 endIndex, MyStr_t replacement, MemArena_t* memArena)
u64 StrReplaceInPlace(MyStr_t str, MyStr_t target, MyStr_t replacement, bool ignoreCase = false, bool allowShrinking = false)
MyStr_t StrReplace(MyStr_t str, MyStr_t target, MyStr_t replacement, MemArena_t* memArena)
MyStr_t StrReplaceMultiple(MyStr_t str, u64 numReplacements, const MyStrPair_t* replacements, MemArena_t* memArena)
bool FindSubstring(MyStr_t target, MyStr_t substring, u64* indexOut = nullptr, bool ignoreCase = false, u64 startIndex = 0)
MyStr_t FindStrParensPart(MyStr_t target, char openParensChar = '[', char closeParensChar = ']')
MyStr_t StringRepeat(MemArena_t* memArena, MyStr_t str, u64 numRepetitions)
u8 GetCodepointForUtf8Str(MyStr_t str, u64 index, u32* codepointOut = nullptr)
MyStr_t ConvertUcs2StrToUtf8(MemArena_t* memArena, const wchar_t* wideStrPntr, u64 wideStrLength)
MyStr_t ConvertUcs2StrToUtf8Nt(MemArena_t* memArena, const wchar_t* nullTermWideStr)
MyWideStr_t ConvertUtf8StrToUcs2(MemArena_t* memArena, MyStr_t utf8Str)
bool DoesStrContainMultibyteUtf8Characters(MyStr_t str)
MyStr_t FormatBytes(u64 numBytes, MemArena_t* memArena)
const char* FormatBytesNt(u64 numBytes, MemArena_t* memArena)
MyStr_t FormatNumberWithCommas(u64 number, MemArena_t* memArena = nullptr)
const char* FormatNumberWithCommasNt(u64 number, MemArena_t* memArena = nullptr)
u64 FnvHashStr(MyStr_t str)
bool IsStringValidIdentifier(MyStr_t str, bool allowUnderscores = true, bool allowNumbers = true, bool allowLeadingNumbers = false, bool allowEmpty = false, bool allowSpaces = false)
void StrReallocAppend(MyStr_t* baseStr, MyStr_t appendStr, MemArena_t* memArena)
WordBreakCharClass_t GetWordBreakCharClass(u32 codepoint)
bool IsCharPairWordBreak(u32 prevCodepoint, u32 nextCodepoint, bool forward, bool subwords)
u64 FindNextWordBreakInString(MyStr_t str, u64 startIndex, bool forward, bool subwords, bool includeBreakAtStartIndex = false)
MyStr_t FormatRealTime(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
const char* FormatRealTimeNt(const RealTime_t* realTime, MemArena_t* memArena, bool includeDayOfWeek = true, bool includeHourMinuteSecond = true, bool includeMonthDayYear = true)
MyStr_t FormatMilliseconds(u64 milliseconds, MemArena_t* memArena)
const char* FormatMillisecondsNt(u64 milliseconds, MemArena_t* memArena)
*/
