/*
File:   gy_debug.h
Author: Taylor Robbins
Date:   09\23\2021
Description:
	** Holds types and macros for the gy library to send debug output.
	** In order to get debug output set GyLibDebugOutputFunc and GyLibDebugPrintFunc to functions defined by GYLIB_DEBUG_OUTPUT_HANDLER_DEF and GYLIB_DEBUG_PRINT_HANDLER_DEF
*/

#ifndef _GY_DEBUG_H
#define _GY_DEBUG_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

enum DbgLevel_t
{
	DbgLevel_None = 0,
	DbgLevel_Debug,
	DbgLevel_Regular,
	DbgLevel_Info,
	DbgLevel_Notify,
	DbgLevel_Other,
	DbgLevel_Warning,
	DbgLevel_Error,
	DbgLevel_NumLevels,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetDbgLevelStr(DbgLevel_t enumValue);
#else
const char* GetDbgLevelStr(DbgLevel_t enumValue)
{
	switch (enumValue)
	{
		case DbgLevel_None:    return "None";
		case DbgLevel_Debug:   return "Debug";
		case DbgLevel_Regular: return "Regular";
		case DbgLevel_Info:    return "Info";
		case DbgLevel_Notify:  return "Notify";
		case DbgLevel_Other:   return "Other";
		case DbgLevel_Warning: return "Warning";
		case DbgLevel_Error:   return "Error";
		default: return "Unknown";
	}
}
#endif

#define GYLIB_DEBUG_OUTPUT_HANDLER_DEF(functionName) void functionName(const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t level, bool newLine, const char* message)
typedef GYLIB_DEBUG_OUTPUT_HANDLER_DEF(GyLibDebugOutput_f);
#define GYLIB_DEBUG_PRINT_HANDLER_DEF(functionName) void functionName(const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t level, bool newLine, const char* formatString, ...)
typedef GYLIB_DEBUG_PRINT_HANDLER_DEF(GyLibDebugPrint_f);

#ifdef GYLIB_HEADER_ONLY
GYLIB_DEBUG_OUTPUT_HANDLER_DEF(GyLibDebugOutputHandler_Stub);
GYLIB_DEBUG_PRINT_HANDLER_DEF(GyLibDebugPrintHandler_Stub);
extern GyLibDebugOutput_f* GyLibDebugOutputFunc;
extern GyLibDebugPrint_f*  GyLibDebugPrintFunc;
#else
GYLIB_DEBUG_OUTPUT_HANDLER_DEF(GyLibDebugOutputHandler_Stub)
{
	UNUSED(filePath);
	UNUSED(lineNumber);
	UNUSED(funcName);
	UNUSED(level);
	UNUSED(newLine);
	UNUSED(message);
	//do nothing
}
GYLIB_DEBUG_PRINT_HANDLER_DEF(GyLibDebugPrintHandler_Stub)
{
	UNUSED(filePath);
	UNUSED(lineNumber);
	UNUSED(funcName);
	UNUSED(level);
	UNUSED(newLine);
	UNUSED(formatString);
	//do nothing
}
GyLibDebugOutput_f* GyLibDebugOutputFunc = GyLibDebugOutputHandler_Stub;
GyLibDebugPrint_f*  GyLibDebugPrintFunc  = GyLibDebugPrintHandler_Stub;
#endif

#define GyLibWriteAt(level, message)               GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, (level),          false, (message))
#define GyLibWriteLineAt(level, message)           GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, (level),          true,  (message))
#define GyLibPrintAt(level, formatString, ...)     GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, (level),          false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLineAt(level, formatString, ...) GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, (level),          true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_D(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Debug,   false, (message))
#define GyLibWriteLine_D(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Debug,   true,  (message))
#define GyLibPrint_D(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Debug,   false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_D(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Debug,   true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_R(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Regular, false, (message))
#define GyLibWriteLine_R(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Regular, true,  (message))
#define GyLibPrint_R(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Regular, false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_R(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Regular, true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_I(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Info,    false, (message))
#define GyLibWriteLine_I(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Info,    true,  (message))
#define GyLibPrint_I(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Info,    false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_I(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Info,    true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_N(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Notify,  false, (message))
#define GyLibWriteLine_N(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Notify,  true,  (message))
#define GyLibPrint_N(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Notify,  false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_N(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Notify,  true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_O(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Other,   false, (message))
#define GyLibWriteLine_O(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Other,   true,  (message))
#define GyLibPrint_O(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Other,   false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_O(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Other,   true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_W(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Warning, false, (message))
#define GyLibWriteLine_W(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Warning, true,  (message))
#define GyLibPrint_W(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Warning, false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_W(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Warning, true,  (formatString), ##__VA_ARGS__)

#define GyLibWrite_E(message)                      GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Error,   false, (message))
#define GyLibWriteLine_E(message)                  GyLibDebugOutputFunc(__FILE__, __LINE__, __func__, DbgLevel_Error,   true,  (message))
#define GyLibPrint_E(formatString, ...)            GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Error,   false, (formatString), ##__VA_ARGS__)
#define GyLibPrintLine_E(formatString, ...)        GyLibDebugPrintFunc (__FILE__, __LINE__, __func__, DbgLevel_Error,   true,  (formatString), ##__VA_ARGS__)

#endif //  _GY_DEBUG_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
DbgLevel_None
DbgLevel_Debug
DbgLevel_Regular
DbgLevel_Info
DbgLevel_Notify
DbgLevel_Other
DbgLevel_Warning
DbgLevel_Error
DbgLevel_NumLevels
@Types
DbgLevel_t
GyLibDebugOutput_f
GyLibDebugPrint_f
@Functions
const char* GetDbgLevelStr(DbgLevel_t gyDbgLevel)
#define GYLIB_DEBUG_OUTPUT_HANDLER_DEF(functionName)
#define GYLIB_DEBUG_PRINT_HANDLER_DEF(functionName)
#define GyLibWriteAt(level, message)
#define GyLibWriteLineAt(level, message)
#define GyLibPrintAt(level, formatString, ...)
#define GyLibPrintLineAt(level, formatString, ...)
#define GyLibWrite_D(message)
#define GyLibWriteLine_D(message)
#define GyLibPrint_D(formatString, ...)
#define GyLibPrintLine_D(formatString, ...)
#define GyLibWrite_R(message)
#define GyLibWriteLine_R(message)
#define GyLibPrint_R(formatString, ...)
#define GyLibPrintLine_R(formatString, ...)
#define GyLibWrite_I(message)
#define GyLibWriteLine_I(message)
#define GyLibPrint_I(formatString, ...)
#define GyLibPrintLine_I(formatString, ...)
#define GyLibWrite_N(message)
#define GyLibWriteLine_N(message)
#define GyLibPrint_N(formatString, ...)
#define GyLibPrintLine_N(formatString, ...)
#define GyLibWrite_O(message)
#define GyLibWriteLine_O(message)
#define GyLibPrint_O(formatString, ...)
#define GyLibPrintLine_O(formatString, ...)
#define GyLibWrite_W(message)
#define GyLibWriteLine_W(message)
#define GyLibPrint_W(formatString, ...)
#define GyLibPrintLine_W(formatString, ...)
#define GyLibWrite_E(message)
#define GyLibWriteLine_E(message)
#define GyLibPrint_E(formatString, ...)
#define GyLibPrintLine_E(formatString, ...)
*/