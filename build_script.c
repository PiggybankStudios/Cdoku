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

#define BUILD_FOR_DEVICE     1
#define BUILD_FOR_SIMULATOR  0
#define DEBUG_BUILD          1

#if !BUILDING_ON_WINDOWS && !BUILDING_ON_OSX
#error This build script only works on Windows and MacOS
#endif

#if BUILDING_ON_OSX
#define EXE_ARM_GCC_PATH EXE_ARM_GCC_OSX_PATH //hardcoded to a specific install path
#else
#define EXE_ARM_GCC_PATH EXE_ARM_GCC //Let it get resolved by %PATH%
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
	PigBuildDebugMode = true;
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
	
	AddTaggedArgNt(&commonCompilerFlags,  T_MSVC_CL, CL_INCLUDE_DIR,    "[ROOT]/game");
	AddTaggedArgNt(&commonCompilerFlags,  T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/game");
	AddTaggedArgNt(&commonCompilerFlags,  T_GCC,     GCC_INCLUDE_DIR,   "[ROOT]/game");
	AddTaggedArgNt(&commonCompilerFlags,  T_MSVC_CL, CL_INCLUDE_DIR,    "[ROOT]/lib");
	AddTaggedArgNt(&commonCompilerFlags,  T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/lib");
	AddTaggedArgNt(&commonCompilerFlags,  T_GCC,     GCC_INCLUDE_DIR,   "[ROOT]/lib");
	AddTaggedArgNt(&commonCompilerFlags,  T_MSVC_CL, CL_INCLUDE_DIR,    "[ROOT]/lib/engine");
	AddTaggedArgNt(&commonCompilerFlags,  T_CLANG,   CLANG_INCLUDE_DIR, "[ROOT]/lib/engine");
	AddTaggedArgNt(&commonCompilerFlags,  T_GCC,     GCC_INCLUDE_DIR,   "[ROOT]/lib/engine");
	AddTaggedArgStr(&commonCompilerFlags, T_MSVC_CL, CL_INCLUDE_DIR,    playdateSdkDir_C_API);
	AddTaggedArgStr(&commonCompilerFlags, T_CLANG,   CLANG_INCLUDE_DIR, playdateSdkDir_C_API);
	AddTaggedArgStr(&commonCompilerFlags, T_GCC,     GCC_INCLUDE_DIR,   playdateSdkDir_C_API);
	
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PROJECT_NAME=\"C-doku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PROJECT_NAME=\"C-doku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "PROJECT_NAME=\"C-doku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PROJECT_NAME_SAFE=\"Cdoku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PROJECT_NAME_SAFE=\"Cdoku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "PROJECT_NAME_SAFE=\"Cdoku\"");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "PLAYDATE_COMPILATION");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_DEVICE,    CL_DEFINE,    "PLAYDATE_DEVICE");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_DEVICE,    CLANG_DEFINE, "PLAYDATE_DEVICE");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC     T_DEVICE,    GCC_DEFINE,   "PLAYDATE_DEVICE");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "PLAYDATE_SIMULATOR");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "PLAYDATE_SIMULATOR");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC     T_SIMULATOR, GCC_DEFINE,   "PLAYDATE_SIMULATOR");
	
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_SIMULATOR, CL_DEFINE,    "TARGET_SIMULATOR=1");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_SIMULATOR, CLANG_DEFINE, "TARGET_SIMULATOR=1");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC     T_SIMULATOR, GCC_DEFINE,   "TARGET_SIMULATOR=1");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL T_DEVICE,    CL_DEFINE,    "TARGET_PLAYDATE=1");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG   T_DEVICE,    CLANG_DEFINE, "TARGET_PLAYDATE=1");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC     T_DEVICE,    GCC_DEFINE,   "TARGET_PLAYDATE=1");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "TARGET_EXTENSION=1");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "TARGET_EXTENSION=1");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "TARGET_EXTENSION=1");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "__HEAP_SIZE=8388208");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "__HEAP_SIZE=8388208");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "__HEAP_SIZE=8388208");
	AddTaggedArgNt(&commonCompilerFlags, T_MSVC_CL,             CL_DEFINE,    "__STACK_SIZE=61800");
	AddTaggedArgNt(&commonCompilerFlags, T_CLANG,               CLANG_DEFINE, "__STACK_SIZE=61800");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC,                 GCC_DEFINE,   "__STACK_SIZE=61800");
	
	// -fverbose-asm -fno-common -falign-functions=16 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-exceptions -mword-relocations
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DEFINE, "__FPU_USED=1");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_USE_SPEC_FILE, "nano.specs"); //Required for things like _read, _write, _exit, etc. to not be pulled in as requirements from standard library
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_USE_SPEC_FILE, "nosys.specs"); //TODO: Is this helping?
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_TARGET_THUMB);
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_TARGET_CPU, "cortex-m7");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_FLOAT_ABI_MODE, "hard"); //Use hardware for floating-point operations
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_TARGET_FPU, "fpv5-sp-d16");
	
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DEBUG_INFO_EX, "3");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DEBUG_INFO_EX, "dwarf-2");
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_STD_LIB_DYNAMIC);
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DEPENDENCY_FILE, "tests.d"); //TODO: This should really move down below inside the tests.exe block
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_ALIGN_FUNCS_TO, "16");
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_SEP_DATA_SECTIONS);
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_SEP_FUNC_SECTIONS);
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_DISABLE_EXCEPTIONS);
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_OMIT_FRAME_PNTR);
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_GLOBAL_VAR_NO_COMMON);
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_VERBOSE_ASSEMBLY); //TODO: Should this only be on when DEBUG_BUILD?
	AddTaggedArg(&commonCompilerFlags,   T_GCC T_DEVICE, GCC_ONLY_RELOC_WORD_SIZE);
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_WARNING_LEVEL, "all");
	// AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_ENABLE_WARNING, "double-promotion");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "unknown-pragmas");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "comment");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "switch");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "nonnull");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "unused");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "missing-braces");
	AddTaggedArgNt(&commonCompilerFlags, T_GCC T_DEVICE, GCC_DISABLE_WARNING, "char-subscripts");
	
	//-nostartfiles --entry eventHandlerShim -Wl,-Map=%OutputMapName%,--cref,--gc-sections,--no-warn-mismatch,--emit-relocs
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_NO_STD_STARTUP);
	AddTaggedArgNt(&commonLinkerFlags,  T_GCC T_DEVICE, GCC_ENTRYPOINT_NAME, "eventHandler");
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_DISABLE_RWX_WARNING);
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_CREF);
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_GC_SECTIONS);
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_DISABLE_MISMATCH_WARNING);
	AddTaggedArg(&commonLinkerFlags,    T_GCC T_DEVICE, GCC_EMIT_RELOCATIONS);
	AddTaggedArgStr(&commonLinkerFlags, T_GCC T_DEVICE, GCC_LINKER_SCRIPT, JoinStrings2(playdateSdkDir_C_API, StrLit("/buildsupport/link_map.ld")));
	
	//-T%PlaydateSdkDirectory%\C_API\buildsupport\link_map.ld
	// AddTaggedArgStr(&commonLinkerFlags, T_GCC T_DEVICE, GCC_LINKER_SCRIPT, JoinStrings2(playdateSdkDir, StrLit("/C_API/buildsupport/link_map.ld")));
	
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
		WriteLine("[Compiling for Device...]");
		CliArgs args = EMPTY;
		AddArgNt(&args, CLI_QUOTED_ARG, "[ROOT]/lib/engine/pig_main.cpp");
		AddArgNt(&args, GCC_OUTPUT_FILE, "pdex.elf");
		AddArgList(&args, &commonCompilerFlags);
		AddArgList(&args, &commonLinkerFlags);
		
		StrArray tags = EMPTY;
		AddStrArray(&tags, &commonTags);
		AddTag(&tags, T_GCC);
		AddStrNt(&tags, EXE_ARM_GCC);
		AddTag(&tags, T_DEVICE);
		AddTag(&tags, T_LIBRARY);
		
		RunCliProgramAndExitOnFailureTags(StrLit(EXE_ARM_GCC_PATH), tags, &args, StrLit("Failed to compile for device!"));
		AssertFileExist(StrLit("pdex.elf"), true);
		
		WriteLine("[Finished Device Build]");
	}
	
	if (BUILD_FOR_SIMULATOR)
	{
		StrArray simulatorTags = EMPTY;
		AddTag(&simulatorTags, T_CLANG);
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
		Str elfDataPath = StrLit("../data/pdex.elf");
		Str dllDataPath = StrLit("../data/pdex" DLL_EXT);
		
		#if BUILD_FOR_DEVICE
		CopyFileToFolder(StrLit("pdex.elf"), StrLit("../data"), true);
		#else
		if (DoesFileExist(elfDataPath)) { RemoveFile(elfDataPath); }
		#endif
		
		#if BUILD_FOR_SIMULATOR
		IF_WINDOWS(CopyFileToFolder(StrLit("pdex.dll"), StrLit("../data"), true));
		IF_OSX(CopyFileToFolder(StrLit("pdex.dylib"), StrLit("../data"), true));
		#else
		if (DoesFileExist(dllDataPath)) { RemoveFile(dllDataPath); }
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
