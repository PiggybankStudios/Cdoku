/*
File:   gy_func_table.h
Author: Taylor Robbins
Date:   03\05\2024
Description:
	** When we reload a dll, all the functions in that DLL might change their address
	** To get around this we've been avoiding storing function pointers wherever possible
	** and in the cases where we want to store a pointer, we have to write fixup code
	** that runs on reload to go find and overwrite the old pointer.
	** Now that we have PigGen.exe we can start doing something a little smarter
	** Instead of storing a raw function pointer, we'll store an index into a table
	** and we'll generate code to fill that table with all the function pointers
	** on startup and reload. This way we don't have to write the code that finds
	** the stored pointers, we only need to generate the code that fills the table
	** properly so those pointers point to the most up to date address for any function.
	
NOTE: In order for this file to work, you must define FuncTable_NumFunctions before including
//    Then you need to fill implement and fill out GlobalFuncTable with all the pointers
*/

#ifndef _GY_FUNC_TABLE_H
#define _GY_FUNC_TABLE_H

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern const void* GlobalFuncTable[FuncTable_NumFunctions];

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct Func_t
{
	u64 tableIndex;
};

#define Func_Nullptr_Const { UINT64_MAX }
#define Func_Nullptr MakeFunc_(UINT64_MAX)

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define CallFunc(functionType, func, ...) ((functionType*)(GetRawFuncPntr(func)))(##__VA_ARGS__)
#define PIGGEN_RegisterFunc(functionName) //Compile to nothing, used by PigGen

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void* GetRawFuncPntr(Func_t func);
	bool IsValidFunc(Func_t func);
	bool IsValidFunc(Func_t* func);
	Func_t MakeFunc_(u64 tableIndex);
	Func_t MakeFunc(const void* functionPntr);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
const void* GetRawFuncPntr(Func_t func)
{
	DebugAssert(func.tableIndex < ArrayCount(GlobalFuncTable));
	return GlobalFuncTable[func.tableIndex];
}

bool IsValidFunc(Func_t func)
{
	return (func.tableIndex < ArrayCount(GlobalFuncTable));
}

bool IsValidFunc(Func_t* func)
{
	return (func->tableIndex < ArrayCount(GlobalFuncTable));
}

Func_t MakeFunc_(u64 tableIndex)
{
	Func_t result = {};
	result.tableIndex = tableIndex;
	return result;
}

Func_t MakeFunc(const void* functionPntr)
{
	Func_t result = {};
	for (u64 fIndex = 0; fIndex < ArrayCount(GlobalFuncTable); fIndex++)
	{
		if (GlobalFuncTable[fIndex] == functionPntr) { result.tableIndex = fIndex; return result; }
	}
	AssertMsg(false, "Tried to MakeFunc from a funciton pointer that isn't registered in the GlobalFuncTable");
	return result;
}

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_FUNC_TABLE_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
FuncTable_NumFunctions
Func_Nullptr_Const
Func_Nullptr
@Types
Func_t
@Globals
GlobalFuncTable
@Functions
#define CallFunc(functionType, func, ...)
#define PIGGEN_RegisterFunc(functionName)
const void* GetRawFuncPntr(Func_t func)
bool IsValidFunc(Func_t func);
Func_t MakeFunc_(u64 tableIndex)
Func_t MakeFunc(const void* functionPntr)
*/
