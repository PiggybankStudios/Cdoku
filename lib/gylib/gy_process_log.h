/*
File:   gy_process_log.h
Author: Taylor Robbins
Date:   05\09\2023
*/

#ifndef _GY_PROCESS_LOG_H
#define _GY_PROCESS_LOG_H

// +--------------------------------------------------------------+
// |                         Enumerations                         |
// +--------------------------------------------------------------+
//NOTE: We moved the declaration of this enumeration up to this file so the ProcessLog_t could hold a properly types error code for XML parsing
enum XmlParsingError_t
{
	XmlParsingError_None = 0,
	XmlParsingError_UnexpectedEol,
	XmlParsingError_ExpectedClosingAngleBracket,
	XmlParsingError_UnexpectedClosingToken,
	XmlParsingError_ClosingTokenMismatch,
	XmlParsingError_InvalidCharacterOutsideToken,
	XmlParsingError_InvalidUtf8,
	XmlParsingError_NoTypeFoundForToken,
	XmlParsingError_InvalidCharInIdentifier,
	XmlParsingError_NoEqualsForProperty,
	XmlParsingError_InvalidPropertyFirstChar,
	XmlParsingError_MissingPropertyValue,
	XmlParsingError_MissingClosingTokens,
	XmlParsingError_NumErrors,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetXmlParsingErrorStr(XmlParsingError_t error);
#else
const char* GetXmlParsingErrorStr(XmlParsingError_t error)
{
	switch (error)
	{
		case XmlParsingError_None:                         return "None";
		case XmlParsingError_UnexpectedEol:                return "UnexpectedEol";
		case XmlParsingError_ExpectedClosingAngleBracket:  return "ExpectedClosingAngleBracket";
		case XmlParsingError_UnexpectedClosingToken:       return "UnexpectedClosingToken";
		case XmlParsingError_ClosingTokenMismatch:         return "ClosingTokenMismatch";
		case XmlParsingError_InvalidCharacterOutsideToken: return "InvalidCharacterOutsideToken";
		case XmlParsingError_InvalidUtf8:                  return "InvalidUtf8";
		case XmlParsingError_NoTypeFoundForToken:          return "NoTypeFoundForToken";
		case XmlParsingError_InvalidCharInIdentifier:      return "InvalidCharInIdentifier";
		case XmlParsingError_NoEqualsForProperty:          return "NoEqualsForProperty";
		case XmlParsingError_InvalidPropertyFirstChar:     return "InvalidPropertyFirstChar";
		case XmlParsingError_MissingPropertyValue:         return "MissingPropertyValue";
		case XmlParsingError_MissingClosingTokens:         return "MissingClosingTokens";
		default: return "Unknown";
	}
}
#endif

//NOTE: ProcessLog relies on scratch arenas for print formatting AND for concattenating the file and function names
#if GYLIB_SCRATCH_ARENA_AVAILABLE

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define DEFAULT_PROCESS_LOG_FIFO_SIZE Kilobytes(8)
#define DBG_FILEPATH_AND_FUNCNAME_SEP_CHAR '|'

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct ProcessLogLine_t
{
	u8 flags;
	u64 timestamp;
	u64 programTime;
	u64 preciseProgramTime;
	i64 threadNumber;
	u64 fileLineNumber;
	DbgLevel_t dbgLevel;
};

struct ProcessLog_t
{
	bool isInitilized;
	bool hadErrors;
	bool hadWarnings;
	bool debugBreakOnWarningsAndErrors;
	u32 errorCode;
	TryParseFailureReason_t parseFailureReason;
	XmlParsingError_t xmlParsingError;
	
	MemArena_t* allocArena;
	GyLibDebugOutput_f* immediateOutputFuncPntr;
	MyStr_t processName;
	MyStr_t filePath;
	StringFifo_t fifo;
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
typedef u64 GetPreciseProgramTime_f();
typedef u64 GetThreadNumber_f();

struct LogGlobals_t
{
	u64 timestamp;
	u64 programTime;
	GetPreciseProgramTime_f* getPreciseProgramTime;
	GetThreadNumber_f* getThreadNumber;
};

//The global variable "logGlobals" is how ProcessLogs (and any other log who wants to)
//can get implicit information to shove into the metadata of every logged line
#ifdef GYLIB_HEADER_ONLY
extern LogGlobals_t* logGlobals;
#else
LogGlobals_t* logGlobals = nullptr;
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	bool IsInitialized(const ProcessLog_t* log);
	void FreeProcessLog(ProcessLog_t* log);
	void CreateProcessLog(ProcessLog_t* logOut, u64 fifoSize, MemArena_t* fifoArena, MemArena_t* logArena);
	void CreateProcessLogStub(ProcessLog_t* logOut);
	void ProcessLogRouteToDebugOutput(ProcessLog_t* log, GyLibDebugOutput_f* outputFuncPntr);
	void SetProcessLogFilePath(ProcessLog_t* log, MyStr_t filePath);
	void SetProcessLogName(ProcessLog_t* log, MyStr_t processName);
	void LogOutput_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* message);
	void LogPrint_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* formatString, ...);
	void LogExit_(ProcessLog_t* log, bool success, u32 errorCode, const char* filePath, u32 lineNumber, const char* funcName);
#else

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
bool IsInitialized(const ProcessLog_t* log)
{
	NotNull(log);
	return log->isInitilized;
}

void FreeProcessLog(ProcessLog_t* log)
{
	NotNull_(log);
	DestroyStringFifo(&log->fifo);
	if (!IsEmptyStr(log->filePath))
	{
		NotNull(log->allocArena);
		FreeString(log->allocArena, &log->filePath);
	}
	if (!IsEmptyStr(log->processName))
	{
		NotNull(log->allocArena);
		FreeString(log->allocArena, &log->processName);
	}
	ClearPointer(log);
}

//The fifoArena is where we do a 1 time allocation from NOW, the logArena is where we allocate things that get stored in ProcessLog_t (like filePath and processName)
void CreateProcessLog(ProcessLog_t* logOut, u64 fifoSize, MemArena_t* fifoArena, MemArena_t* logArena)
{
	NotNull(logOut);
	NotNull(logArena);
	ClearPointer(logOut);
	logOut->isInitilized = true;
	logOut->allocArena = logArena;
	if (fifoSize > 0)
	{
		NotNull(fifoArena);
		CreateStringFifoInArena(&logOut->fifo, fifoArena, fifoSize);
	}
	logOut->hadErrors = false;
	logOut->hadWarnings = false;
	logOut->debugBreakOnWarningsAndErrors = false;
	logOut->errorCode = 0;
	logOut->filePath = NewStringInArenaNt(logOut->allocArena, "[UnspecifiedPath]");
	logOut->processName = NewStringInArenaNt(logOut->allocArena, "[UnspecifiedName]");
}

void CreateProcessLogStub(ProcessLog_t* logOut)
{
	NotNull(logOut);
	ClearPointer(logOut);
	logOut->isInitilized = true;
	logOut->allocArena = nullptr;
	logOut->hadErrors = false;
	logOut->hadWarnings = false;
	logOut->debugBreakOnWarningsAndErrors = false;
	logOut->errorCode = 0;
	logOut->filePath = MyStr_Empty;
	logOut->processName = MyStr_Empty;
}

void ProcessLogRouteToDebugOutput(ProcessLog_t* log, GyLibDebugOutput_f* outputFuncPntr)
{
	NotNull(log);
	log->immediateOutputFuncPntr = outputFuncPntr;
}

//TODO: Create a CopyProcessLog function?

// +--------------------------------------------------------------+
// |                   FilePath and ProcessName                   |
// +--------------------------------------------------------------+
void SetProcessLogFilePath(ProcessLog_t* log, MyStr_t filePath)
{
	NotNull(log);
	NotNullStr(&filePath);
	if (log->allocArena != nullptr)
	{
		if (!IsEmptyStr(log->filePath))
		{
			FreeString(log->allocArena, &log->filePath);
		}
		log->filePath = AllocString(log->allocArena, &filePath);
	}
}
void SetProcessLogName(ProcessLog_t* log, MyStr_t processName)
{
	NotNull(log);
	NotNullStr(&processName);
	if (log->allocArena != nullptr)
	{
		if (!IsEmptyStr(log->processName))
		{
			if (DoesMemArenaSupportFreeing(log->allocArena)) { FreeString(log->allocArena, &log->processName); }
		}
		log->processName = AllocString(log->allocArena, &processName);
	}
}

// +--------------------------------------------------------------+
// |                       Output and Print                       |
// +--------------------------------------------------------------+
void LogOutput_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* message)
{
	NotNull_(log);
	UNUSED(addNewLine); //TODO: We should probably take this into account
	
	if (log->debugBreakOnWarningsAndErrors && (dbgLevel == DbgLevel_Warning || dbgLevel == DbgLevel_Error)) { MyDebugBreak(); }
	
	if (log->immediateOutputFuncPntr != nullptr)
	{
		log->immediateOutputFuncPntr(filePath, lineNumber, funcName, dbgLevel, addNewLine, message);
	}
	
	if (log->fifo.bufferSize > 0)
	{
		MemArena_t* scratch = GetScratchArena();
		
		MyStr_t text = NewStr(message);
		MyStr_t filePathAndFuncName = PrintInArenaStr(scratch, "%s%c%s", filePath, DBG_FILEPATH_AND_FUNCNAME_SEP_CHAR, funcName);
		ProcessLogLine_t metaInfo = {};
		metaInfo.flags = flags;
		metaInfo.fileLineNumber = lineNumber;
		metaInfo.dbgLevel = dbgLevel;
		metaInfo.timestamp = (logGlobals != nullptr) ? logGlobals->timestamp : 0;
		metaInfo.programTime = (logGlobals != nullptr) ? logGlobals->programTime : 0;
		metaInfo.preciseProgramTime = (logGlobals != nullptr && logGlobals->getPreciseProgramTime != nullptr) ? logGlobals->getPreciseProgramTime() : 0;
		metaInfo.threadNumber = (logGlobals != nullptr && logGlobals->getThreadNumber != nullptr) ? logGlobals->getThreadNumber() : 0;
		
		StringFifoLine_t* newLine = StringFifoPushLineExt(&log->fifo, text, sizeof(metaInfo), &metaInfo, filePathAndFuncName);
		DebugAssertAndUnused_(newLine != nullptr, newLine);
		
		FreeScratchArena(scratch);
	}
}

void LogPrint_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* formatString, ...)
{
	NotNull_(log);
	if (log->debugBreakOnWarningsAndErrors && (dbgLevel == DbgLevel_Warning || dbgLevel == DbgLevel_Error)) { MyDebugBreak(); }
	if (log->fifo.bufferSize > 0 || log->immediateOutputFuncPntr != nullptr)
	{
		MemArena_t* scratch = GetScratchArena();
		va_list args;
		va_start(args, formatString);
		int formattedStrLength = PrintVa_Measure((formatString), args);
		va_end(args);
		if (formattedStrLength >= 0)
		{
			char* formattedStr = AllocArray(scratch, char, formattedStrLength+1); //Allocate
			if (formattedStr != nullptr)
			{
				va_start(args, formatString);
				PrintVa_Print(formatString, args, formattedStr, formattedStrLength);
				va_end(args);
				formattedStr[formattedStrLength] = '\0';
				
				LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formattedStr);
			}
			else
			{
				//failed print, just send out the formatString
				LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formatString);
			}
		}
		else
		{
			//failed print, just send out the formatString
			LogOutput_(log, flags, filePath, lineNumber, funcName, dbgLevel, addNewLine, formatString);
		}
		FreeScratchArena(scratch);
	}
}

void LogExit_(ProcessLog_t* log, bool success, u32 errorCode, const char* filePath, u32 lineNumber, const char* funcName)
{
	NotNull_(log);
	if (success)
	{
		Assert_(errorCode == 0);
		LogOutput_(log, 0x00, filePath, lineNumber, funcName, DbgLevel_Info, true, "Process succeeded");
	}
	else
	{
		Assert_(errorCode != 0);
		if (log->debugBreakOnWarningsAndErrors) { MyDebugBreak(); }
		log->hadErrors = true;
		log->errorCode = errorCode;
		LogPrint_(log, 0x00, filePath, lineNumber, funcName, DbgLevel_Error, true, "Exiting with error code %u(0x%08X)", errorCode, errorCode);
		
	}
}

#endif //GYLIB_HEADER_ONLY

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define LogWriteAt(log, level, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, message)
#define LogWriteLineAt(log, level, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  message)
#define LogPrintAt(log, level, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, formatString, ##__VA_ARGS__)
#define LogPrintLineAt(log, level, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  formatString, ##__VA_ARGS__)
#define LogWriteAtx(log, level, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, message)
#define LogWriteLineAtx(log, level, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  message)
#define LogPrintAtx(log, level, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), false, formatString, ##__VA_ARGS__)
#define LogPrintLineAtx(log, level, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, (DbgLevel_t)(level), true,  formatString, ##__VA_ARGS__)

#define LogWrite_D(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, false, message)
#define LogWriteLine_D(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  message)
#define LogPrint_D(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_D(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Dx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, false, message)
#define LogWriteLine_Dx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  message)
#define LogPrint_Dx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Dx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Debug, true,  formatString, ##__VA_ARGS__)

#define LogWrite_R(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, false, message)
#define LogWriteLine_R(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  message)
#define LogPrint_R(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_R(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Rx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, false, message)
#define LogWriteLine_Rx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  message)
#define LogPrint_Rx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Rx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Regular, true,  formatString, ##__VA_ARGS__)

#define LogWrite_I(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, false, message)
#define LogWriteLine_I(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, true,  message)
#define LogPrint_I(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_I(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Info, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ix(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, false, message)
#define LogWriteLine_Ix(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, true,  message)
#define LogPrint_Ix(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ix(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Info, true,  formatString, ##__VA_ARGS__)

#define LogWrite_N(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, false, message)
#define LogWriteLine_N(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  message)
#define LogPrint_N(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_N(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Nx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, false, message)
#define LogWriteLine_Nx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  message)
#define LogPrint_Nx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Nx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Notify, true,  formatString, ##__VA_ARGS__)

#define LogWrite_O(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, false, message)
#define LogWriteLine_O(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, true,  message)
#define LogPrint_O(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_O(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Other, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ox(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, false, message)
#define LogWriteLine_Ox(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, true,  message)
#define LogPrint_Ox(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ox(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Other, true,  formatString, ##__VA_ARGS__)

#define LogWrite_W(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, false, message)
#define LogWriteLine_W(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  message)
#define LogPrint_W(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_W(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Wx(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, false, message)
#define LogWriteLine_Wx(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  message)
#define LogPrint_Wx(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Wx(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Warning, true,  formatString, ##__VA_ARGS__)

#define LogWrite_E(log, message)                       LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, false, message)
#define LogWriteLine_E(log, message)                   LogOutput_((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, true,  message)
#define LogPrint_E(log, formatString, ...)             LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_E(log, formatString, ...)         LogPrint_ ((log), 0x00,    __FILE__, __LINE__, __func__, DbgLevel_Error, true,  formatString, ##__VA_ARGS__)
#define LogWrite_Ex(log, flags, message)               LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, false, message)
#define LogWriteLine_Ex(log, flags, message)           LogOutput_((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, true,  message)
#define LogPrint_Ex(log, flags, formatString, ...)     LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, false, formatString, ##__VA_ARGS__)
#define LogPrintLine_Ex(log, flags, formatString, ...) LogPrint_ ((log), (flags), __FILE__, __LINE__, __func__, DbgLevel_Error, true,  formatString, ##__VA_ARGS__)

#define LogExitSuccess(log)            LogExit_((log), true,  0,           __FILE__, __LINE__, __func__)
#define LogExitFailure(log, errorCode) LogExit_((log), false, (errorCode), __FILE__, __LINE__, __func__)

#endif // GYLIB_SCRATCH_ARENA_AVAILABLE

#endif //  _GY_PROCESS_LOG_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
DEFAULT_PROCESS_LOG_FIFO_SIZE
DBG_FILEPATH_AND_FUNCNAME_SEP_CHAR
XmlParsingError_None
XmlParsingError_UnexpectedEol
XmlParsingError_ExpectedClosingAngleBracket
XmlParsingError_UnexpectedClosingToken
XmlParsingError_ClosingTokenMismatch
XmlParsingError_InvalidCharacterOutsideToken
XmlParsingError_InvalidUtf8
XmlParsingError_NoTypeFoundForToken
XmlParsingError_InvalidCharInIdentifier
XmlParsingError_NoEqualsForProperty
XmlParsingError_InvalidPropertyFirstChar
XmlParsingError_MissingPropertyValue
XmlParsingError_MissingClosingTokens
XmlParsingError_NumErrors
@Types
XmlParsingError_t
ProcessLogLine_t
ProcessLog_t
GetPreciseProgramTime_f
GetThreadNumber_f
LogGlobals_t
@Globals
logGlobals
@Functions
void FreeProcessLog(ProcessLog_t* log)
void CreateProcessLog(ProcessLog_t* logOut, u64 fifoSize, MemArena_t* fifoArena, MemArena_t* logArena)
void CreateProcessLogStub(ProcessLog_t* logOut)
void ProcessLogRouteToDebugOutput(ProcessLog_t* log, GyLibDebugOutput_f* outputFuncPntr)
void SetProcessLogFilePath(ProcessLog_t* log, MyStr_t filePath)
void SetProcessLogName(ProcessLog_t* log, MyStr_t processName)
void LogOutput_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* message)
void LogPrint_(ProcessLog_t* log, u8 flags, const char* filePath, u32 lineNumber, const char* funcName, DbgLevel_t dbgLevel, bool addNewLine, const char* formatString, ...)
void LogExit_(ProcessLog_t* log, bool success, u32 errorCode, const char* filePath, u32 lineNumber, const char* funcName)
#define LogWriteAt(log, level, message)
#define LogWriteLineAt(log, level, message)
#define LogPrintAt(log, level, formatString, ...)
#define LogPrintLineAt(log, level, formatString, ...)
#define LogWriteAtx(log, level, flags, message)
#define LogWriteLineAtx(log, level, flags, message)
#define LogPrintAtx(log, level, flags, formatString, ...)
#define LogPrintLineAtx(log, level, flags, formatString, ...)
#define LogWrite_D(log, message)
#define LogWriteLine_D(log, message)
#define LogPrint_D(log, formatString, ...)
#define LogPrintLine_D(log, formatString, ...)
#define LogWrite_Dx(log, flags, message)
#define LogWriteLine_Dx(log, flags, message)
#define LogPrint_Dx(log, flags, formatString, ...)
#define LogPrintLine_Dx(log, flags, formatString, ...)
#define LogWrite_R(log, message)
#define LogWriteLine_R(log, message)
#define LogPrint_R(log, formatString, ...)
#define LogPrintLine_R(log, formatString, ...)
#define LogWrite_Rx(log, flags, message)
#define LogWriteLine_Rx(log, flags, message)
#define LogPrint_Rx(log, flags, formatString, ...)
#define LogPrintLine_Rx(log, flags, formatString, ...)
#define LogWrite_I(log, message)
#define LogWriteLine_I(log, message)
#define LogPrint_I(log, formatString, ...)
#define LogPrintLine_I(log, formatString, ...)
#define LogWrite_Ix(log, flags, message)
#define LogWriteLine_Ix(log, flags, message)
#define LogPrint_Ix(log, flags, formatString, ...)
#define LogPrintLine_Ix(log, flags, formatString, ...)
#define LogWrite_N(log, message)
#define LogWriteLine_N(log, message)
#define LogPrint_N(log, formatString, ...)
#define LogPrintLine_N(log, formatString, ...)
#define LogWrite_Nx(log, flags, message)
#define LogWriteLine_Nx(log, flags, message)
#define LogPrint_Nx(log, flags, formatString, ...)
#define LogPrintLine_Nx(log, flags, formatString, ...)
#define LogWrite_O(log, message)
#define LogWriteLine_O(log, message)
#define LogPrint_O(log, formatString, ...)
#define LogPrintLine_O(log, formatString, ...)
#define LogWrite_Ox(log, flags, message)
#define LogWriteLine_Ox(log, flags, message)
#define LogPrint_Ox(log, flags, formatString, ...)
#define LogPrintLine_Ox(log, flags, formatString, ...)
#define LogWrite_W(log, message)
#define LogWriteLine_W(log, message)
#define LogPrint_W(log, formatString, ...)
#define LogPrintLine_W(log, formatString, ...)
#define LogWrite_Wx(log, flags, message)
#define LogWriteLine_Wx(log, flags, message)
#define LogPrint_Wx(log, flags, formatString, ...)
#define LogPrintLine_Wx(log, flags, formatString, ...)
#define LogWrite_E(log, message)
#define LogWriteLine_E(log, message)
#define LogPrint_E(log, formatString, ...)
#define LogPrintLine_E(log, formatString, ...)
#define LogWrite_Ex(log, flags, message)
#define LogWriteLine_Ex(log, flags, message)
#define LogPrint_Ex(log, flags, formatString, ...)
#define LogPrintLine_Ex(log, flags, formatString, ...)
#define LogExitSuccess(log)
#define LogExitFailure(log, errorCode)
*/
