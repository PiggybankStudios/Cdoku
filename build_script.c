/*
File:   build_script.c
Author: Taylor Robbins
Date:   08\06\2026
Description: 
	** This script handles building C-Doku app for the Playdate Device (ARM Cortex)
	** As well as the Windows and OSX Simulators (Windows .dll or OSX .dylib)
*/

#include "pig_build.h"
#include "pig_build_optional.h"

#define BUILD_FOR_DEVICE     0
#define BUILD_FOR_SIMULATOR  1
#define DEBUG_BUILD          1

#if !BUILDING_ON_WINDOWS && !BUILDING_ON_OSX
#error This build script only works on Windows and MacOS
#endif

#if DEBUG_BUILD
#define IF_DEBUG(...)   __VA_ARGS__
#define IF_RELEASE(...) //nothing
#else
#define IF_DEBUG(...)   //nothing
#define IF_RELEASE(...) __VA_ARGS__
#endif

int main(int argc, char* argv[])
{
	PigBuildDebugMode = false;
	RecompileIfNeeded(StrArray_Empty);
	IF_WINDOWS(bool isMsvcInitialized = WasMsvcDevBatchRun());
	
	Str playdateSdkDir = GetPlaydateSdkPath();
	Str playdateSdkDir_C_API = JoinPaths(playdateSdkDir, StrLit("C_API"));
	
	CliArgs commonCompilerFlags = EMPTY;
	CliArgs commonLinkerFlags = EMPTY;
	
	AddTaggedArg(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_NO_LOGO);
	AddTaggedArg(&commonCompilerFlags, T_MSVC_CL, CL_FULL_FILE_PATHS);
	AddTaggedArg(&commonCompilerFlags, T_CLANG,   CLANG_FULL_FILE_PATHS);
	
	// AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_WARNING_LEVEL, "3");
	// AddTaggedArg(&commonCompilerFlags,   T_MSVC_CL T_SIMULATOR, CL_NO_WARNINGS_AS_ERRORS);
	
	AddTaggedArg(&commonCompilerFlags,   T_MSVC_CL T_SIMULATOR T_DEBUG_BUILD,   CL_STD_LIB_DYNAMIC_DBG);
	AddTaggedArg(&commonCompilerFlags,   T_MSVC_CL T_SIMULATOR T_RELEASE_BUILD, CL_STD_LIB_DYNAMIC);
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR T_DEBUG_BUILD,   CL_OPTIMIZATION_LEVEL, "d");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR T_RELEASE_BUILD, CL_OPTIMIZATION_LEVEL, "2");
	AddTaggedArg(&commonCompilerFlags,   T_MSVC_CL T_SIMULATOR T_DEBUG_BUILD, CL_DEBUG_INFO);
	
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL, CL_INCLUDE_DIR,    "[ROOT]/game");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/game");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL, CL_INCLUDE_DIR,    "[ROOT]/lib");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/lib");
	AddTaggedArgStr(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_INCLUDE_DIR,    playdateSdkDir_C_API);
	AddTaggedArgStr(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_INCLUDE_DIR, playdateSdkDir_C_API);
	
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PROJECT_NAME=\"C-doku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PROJECT_NAME=\"C-doku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PROJECT_NAME_SAFE=\"Cdoku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PROJECT_NAME_SAFE=\"Cdoku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_DEVICE,    CL_DEFINE,    "PLAYDATE_DEVICE");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_DEVICE,    CLANG_DEFINE, "PLAYDATE_DEVICE");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "PLAYDATE_SIMULATOR");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "PLAYDATE_SIMULATOR");
	
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "TARGET_SIMULATOR=1");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "TARGET_SIMULATOR=1");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "TARGET_EXTENSION=1");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "TARGET_EXTENSION=1");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "__HEAP_SIZE=8388208");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "__HEAP_SIZE=8388208");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "__STACK_SIZE=61800");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "__STACK_SIZE=61800");
	
	StrArray commonTags = EMPTY;
	AddTag(&commonTags, T_LANG_CPP);
	AddTag(&commonTags, T_PLAYDATE);
	IF_DEBUG(AddTag(&commonTags, T_DEBUG_BUILD));
	IF_WINDOWS(AddTag(&commonTags, T_WINDOWS));
	IF_OSX(AddTag(&commonTags, T_OSX));
	
	// +--------------------------------------------------------------+
	// |                            Device                            |
	// +--------------------------------------------------------------+
	if (BUILD_FOR_DEVICE)
	{
		//TODO: Implement me!
	}
	
	if (BUILD_FOR_SIMULATOR)
	{
		StrArray simulatorTags = EMPTY;
		AddTag(&simulatorTags, T_CLANG);
		AddTag(&simulatorTags, T_PLAYDATE);
		AddTag(&simulatorTags, T_SIMULATOR);
		
		// +--------------------------------------------------------------+
		// |                      Windows Simulator                       |
		// +--------------------------------------------------------------+
		#if BUILDING_ON_WINDOWS
		InitializeMsvcIf(&isMsvcInitialized);
		{
			//TODO: Implement me!
		}
		{
			//TODO: Implement me!
		}
		// +--------------------------------------------------------------+
		// |                        OSX Simulator                         |
		// +--------------------------------------------------------------+
		#elif BUILDING_ON_OSX
		{
			WriteLine("[Compiling for OSX Simulator...]");
			CliArgs args = EMPTY;
			AddArg(&args, CLANG_COMPILE);
			AddArgNt(&args, CLI_QUOTED_ARG, "[ROOT]/lib/engine/pig_main.cpp");
			AddArgNt(&args, CLANG_OUTPUT_FILE, "sim_pig_main.o");
			AddArgList(&args, &commonCompilerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddStrArray(&tags, &simulatorTags);
			AddTag(&tags, T_OBJECT);
			
			RunCliProgramAndExitOnFailureTags(StrLit("clang"), tags, &args, StrLit("Failed to link for OSX Simulator!"));
			AssertFileExist(StrLit("sim_pig_main.o"), true);
		}
		{
			WriteLine("[Linking for OSX Simulator]");
			
			CliArgs args = EMPTY;
			AddArg(&args, CLANG_BUILD_SHARED_LIB);
			AddArg(&args, CLANG_fPIC);
			AddArgNt(&args, CLI_QUOTED_ARG, "sim_pig_main.o");
			AddArgNt(&args, CLANG_OUTPUT_FILE, "pdex.dylib");
			AddArgList(&args, &commonLinkerFlags);
			
			StrArray tags = EMPTY;
			AddStrArray(&tags, &commonTags);
			AddStrArray(&tags, &simulatorTags);
			AddTag(&tags, T_LIBRARY);
			
			RunCliProgramAndExitOnFailureTags(StrLit("clang"), tags, &args, StrLit("Failed to link for OSX Simulator!"));
			AssertFileExist(StrLit("pdex.dylib"), true);
		}
		#endif
		
		WriteLine("[Finished OSX Simulator Build]");
	}
	
	// +--------------------------------------------------------------+
	// |                         Package Game                         |
	// +--------------------------------------------------------------+
	{
		#if BUILD_FOR_DEVICE
		CopyFileToFolder(StrLit("pdex.elf"), StrLit("../data"), true);
		#endif
		#if BUILD_FOR_SIMULATOR
		IF_WINDOWS(CopyFileToFolder(StrLit("pdex.dll"), StrLit("../data"), true));
		IF_OSX(CopyFileToFolder(StrLit("pdex.dylib"), StrLit("../data"), true));
		#endif
		
		WriteLine("[Packaging game...]");
		CliArgs args = EMPTY;
		AddArg(&args, "-q"); //Quiet mode, suppress non-error output (otherwise it prints the name of every single .txt file in our resources)
		AddArgStr(&args, "-sdkpath \"[VAL]\"", playdateSdkDir);
		AddArgNt(&args, CLI_QUOTED_ARG, "[ROOT]/data");
		AddArgNt(&args, CLI_QUOTED_ARG, "Cdoku.pdx");
		
		Str pdcPath = JoinPaths(playdateSdkDir, StrLit("bin/pdc"));
		RunCliProgramAndExitOnFailure(pdcPath, &args, StrLit("Failed to link for OSX Simulator!"));
		Assert(DoesFolderExist(StrLit("Cdoku.pdx")));
		
		WriteLine("[Game packaged!]");
	}
}
