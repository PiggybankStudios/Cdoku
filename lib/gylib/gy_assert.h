/*
File:   gy_assert.h
Author: Taylor Robbins
Date:   09\23\2021
Description:
	** This file contains definitions for the Assert and related macros
	
	If @GYLIB_ASSERTIONS_ENABLED is not defined it will default to true
	otherwise the truthiness of this #define will be considered in order
	to decide whether assertions should be checked or compiled out
	
	If @GYLIB_USE_ASSERT_FAILURE_FUNC is defined then the file will declare
	a function GyLibAssertFailure that you must implement somewhere
	
	Assertion macros ending in _ will skip the call to GyLibAssertFailure.
	If GYLIB_USE_ASSERT_FAILURE_FUNC isn't enabled then both assertion types are the same.
	This is to help in scenarios where the GyLibAssertFailure could accidentally perform an action that causes another Assert
	
	Assertions with the suffix Msg allow a message string that will be passed to the GyLibAssertFailure
	function to help describe what this assertion means beyond the Expression itself
*/

#ifndef _GY_ASSERT_H
#define _GY_ASSERT_H

#include "gy_defines_check.h"
#include "gy_std.h"
#include "gy_types.h"

#ifndef GYLIB_ASSERTIONS_ENABLED
#define GYLIB_ASSERTIONS_ENABLED 1
#endif

#ifdef GYLIB_USE_ASSERT_FAILURE_FUNC
#undef GYLIB_USE_ASSERT_FAILURE_FUNC
#define GYLIB_USE_ASSERT_FAILURE_FUNC 1
#else
#define GYLIB_USE_ASSERT_FAILURE_FUNC 0
#endif

// +--------------------------------------------------------------+
// |                Compile-Time Assertion Macros                 |
// +--------------------------------------------------------------+
#define CompileAssertMsg(condition, message) static_assert(condition, message)
#define CompileAssert(condition)             static_assert(condition, #condition)

// +--------------------------------------------------------------+
// |                   MyBreak and MyDebugBreak                   |
// +--------------------------------------------------------------+
#if WINDOWS_COMPILATION || WASM_COMPILATION
#define MyBreak() __debugbreak()
#define MyBreakEx(message) __debugbreak()
#elif OSX_COMPILATION
#define MyBreak() raise(SIGINT)
#define MyBreakEx(message) raise(SIGINT)
#elif LINUX_COMPILATION
#define MyBreak() raise(SIGINT)
#define MyBreakEx(message) raise(SIGINT)
#elif PLAYDATE_COMPILATION
#define MyBreak() pd->system->error("MyBreak()")
#define MyBreakEx(message) pd->system->error(message)
#elif ORCA_COMPILATION
#define MyBreak() oc_abort_ext(__FILE__, __FUNCTION__, __LINE__, "MyBreak()")
#define MyBreakEx(message) oc_abort_ext(__FILE__, __FUNCTION__, __LINE__, message)
#else
#error Platform not supported in gy_assert.h
#endif

#if DEBUG_BUILD
#define MyDebugBreak() MyBreak()
#else
#define MyDebugBreak() //nothing
#endif

// +--------------------------------------------------------------+
// |                    Actual Assertion Macro                    |
// +--------------------------------------------------------------+
#if GYLIB_ASSERTIONS_ENABLED

#if WASM_COMPILATION
#define AssertMsg_(Expression, message) do { if (!(Expression)) { jsStdAssertFailure(__FILE__, __LINE__, __FUNCTION__, #Expression, (message)); } } while(0)
#else
#define AssertMsg_(Expression, message) do { if (!(Expression)) { MyBreakEx(((message) != nullptr) ? (message) : #Expression); } } while(0)
#endif

#if GYLIB_USE_ASSERT_FAILURE_FUNC
extern void GyLibAssertFailure(const char* filePath, int lineNumber, const char* funcName, const char* expressionStr, const char* messageStr);
#define AssertMsg(Expression, message) do { if (!(Expression)) { GyLibAssertFailure(__FILE__, __LINE__, __func__, #Expression, (message)); } } while(0)
#else
#define AssertMsg(Expression, message) AssertMsg_(Expression, (message))
#endif

#else //!GYLIB_ASSERTIONS_ENABLED

#define AssertMsg_(Expression, message) //nothing
#define AssertMsg(Expression, message)  //nothing

#endif //GYLIB_ASSERTIONS_ENABLED

// +--------------------------------------------------------------+
// |                      Assertion Aliases                       |
// +--------------------------------------------------------------+
#define Assert(Expression)  AssertMsg((Expression), nullptr)
#define Assert_(Expression) AssertMsg_((Expression), nullptr)

//Compiled out precondition if statement
#if GYLIB_ASSERTIONS_ENABLED
#define AssertIf(Precondition, Expression)  if (Precondition) { Assert(Expression);  }
#define AssertIf_(Precondition, Expression) if (Precondition) { Assert_(Expression); }
#define AssertIfMsg(Precondition, Expression, message)  if (Precondition) { AssertMsg(Expression, message);  }
#define AssertIfMsg_(Precondition, Expression, message) if (Precondition) { AssertMsg_(Expression, message); }
#else
#define AssertIf(Precondition, Expression)              //nothing
#define AssertIf_(Precondition, Expression)             //nothing
#define AssertIfMsg(Precondition, Expression, message)  //nothing
#define AssertIfMsg_(Precondition, Expression, message) //nothing
#endif

//Assertions that only happen in debug builds
#if DEBUG_BUILD
#define DebugAssert(Expression)              Assert((Expression))
#define DebugAssert_(Expression)             Assert_((Expression))
#define DebugAssertMsg(Expression, message)  AssertMsg((Expression), (message))
#define DebugAssertMsg_(Expression, message) AssertMsg_((Expression), (message))
#define DebugAssertAndUnused(Expression, unusedVarInRelease)              DebugAssert(Expression)
#define DebugAssertAndUnused_(Expression, unusedVarInRelease)             DebugAssert_(Expression)
#define DebugAssertAndUnusedMsg(Expression, unusedVarInRelease, message)  DebugAssertMsg((Expression), message)
#define DebugAssertAndUnusedMsg_(Expression, unusedVarInRelease, message) DebugAssertMsg_((Expression), message)
#else
#define DebugAssert(Expression)              //null
#define DebugAssert_(Expression)             //null
#define DebugAssertMsg(Expression, message)  //null
#define DebugAssertMsg_(Expression, message) //null
#define DebugAssertAndUnused(Expression, unusedVarInRelease)              UNUSED(unusedVarInRelease)
#define DebugAssertAndUnused_(Expression, unusedVarInRelease)             UNUSED(unusedVarInRelease)
#define DebugAssertAndUnusedMsg(Expression, unusedVarInRelease, message)  UNUSED(unusedVarInRelease)
#define DebugAssertAndUnusedMsg_(Expression, unusedVarInRelease, message) UNUSED(unusedVarInRelease)
#endif

//Quick-hand for something != nullptr && something != nullptr && ...
#define NotNull(variable)                                    Assert((variable) != nullptr)
#define NotNull2(variable1, variable2)                       Assert((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3(variable1, variable2, variable3)            Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4(variable1, variable2, variable3, variable4) Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNull_(variable)                                    Assert_((variable) != nullptr)
#define NotNull2_(variable1, variable2)                       Assert_((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3_(variable1, variable2, variable3)            Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4_(variable1, variable2, variable3, variable4) Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNullMsg(variable, message)                                    AssertMsg((variable) != nullptr, (message))
#define NotNull2Msg(variable1, variable2, message)                       AssertMsg((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg(variable1, variable2, variable3, message)            AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg(variable1, variable2, variable3, variable4, message) AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))

#define NotNullMsg_(variable, message)                                    AssertMsg_((variable) != nullptr, (message))
#define NotNull2Msg_(variable1, variable2, message)                       AssertMsg_((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg_(variable1, variable2, variable3, message)            AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg_(variable1, variable2, variable3, variable4, message) AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))

#define Unimplemented()  AssertMsg(false, "This code path is unimplemented!")
#define Unimplemented_() AssertMsg_(false, "This code path is unimplemented!")

#endif //  _GY_ASSERT_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
GYLIB_ASSERTIONS_ENABLED
GYLIB_USE_ASSERT_FAILURE_FUNC
@Types
@Functions
#define CompileAssertMsg(condition, message)
#define CompileAssert(condition)
#define MyBreak()
#define MyBreakEx(message)
#define MyDebugBreak()
void GyLibAssertFailure(const char* filePath, int lineNumber, const char* funcName, const char* expressionStr, const char* messageStr)
#define AssertMsg_(Expression, message)
#define AssertMsg(Expression, message)
#define Assert(Expression)
#define Assert_(Expression)
#define AssertIf(Precondition, Expression)
#define AssertIf_(Precondition, Expression)
#define AssertIfMsg(Precondition, Expression, message)
#define AssertIfMsg_(Precondition, Expression, message)
#define DebugAssert(Expression)
#define DebugAssert_(Expression)
#define DebugAssertMsg(Expression, message)
#define DebugAssertMsg_(Expression, message)
#define DebugAssertAndUnused(Expression, unusedVarInRelease)
#define DebugAssertAndUnused_(Expression, unusedVarInRelease)
#define DebugAssertAndUnusedMsg(Expression, unusedVarInRelease, message)
#define DebugAssertAndUnusedMsg_(Expression, unusedVarInRelease, message)
#define NotNull(variable)
#define NotNull2(variable1, variable2)
#define NotNull3(variable1, variable2, variable3)
#define NotNull4(variable1, variable2, variable3, variable4)
#define NotNull_(variable)
#define NotNull2_(variable1, variable2)
#define NotNull3_(variable1, variable2, variable3)
#define NotNull4_(variable1, variable2, variable3, variable4)
#define NotNullMsg(variable, message)
#define NotNull2Msg(variable1, variable2, message)
#define NotNull3Msg(variable1, variable2, variable3, message)
#define NotNull4Msg(variable1, variable2, variable3, variable4, message)
#define NotNullMsg_(variable, message)
#define NotNull2Msg_(variable1, variable2, message)
#define NotNull3Msg_(variable1, variable2, variable3, message)
#define NotNull4Msg_(variable1, variable2, variable3, variable4, message)
#define Unimplemented()
#define Unimplemented_()
*/
