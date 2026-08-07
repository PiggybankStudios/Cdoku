/*
File:   gy_resource_manifest.h
Author: Taylor Robbins
Date:   10\24\2023
*/

#ifndef _GY_RESOURCE_MANIFEST_H
#define _GY_RESOURCE_MANIFEST_H

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct ResourceManifestEntry_t
{
	u64 id;
	MyStr_t path;
};

struct ResourceManifest_t
{
	MemArena_t* allocArena;
	u64 nextId;
	VarArray_t entries; //ResourceManifestEntry_t
	u64 stringsChunkUsed;
	MyStr_t stringsChunk;
};

#define RESOURCE_MANIFEST_FILE_PREFIX        "# Pig Engine Manifest"
#define RESOURCE_MANIFEST_FILE_PREFIX_LENGTH 21

enum TryDeserResourceManifestError_t
{
	TryDeserResourceManifestError_None = 0,
	TryDeserResourceManifestError_EmptyFile,
	TryDeserResourceManifestError_InvalidOrMissingHeader,
	TryDeserResourceManifestError_NumErrors,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetTryDeserResourceManifestErrorStr(TryDeserResourceManifestError_t enumValue);
#else
const char* GetTryDeserResourceManifestErrorStr(TryDeserResourceManifestError_t enumValue)
{
	switch (enumValue)
	{
		case TryDeserResourceManifestError_None:                   return "None";
		case TryDeserResourceManifestError_EmptyFile:              return "EmptyFile";
		case TryDeserResourceManifestError_InvalidOrMissingHeader: return "InvalidOrMissingHeader";
		default: return "Unknown";
	}
}
#endif

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	void FreeResourceManifestEntry(ResourceManifest_t* manifest, ResourceManifestEntry_t* entry);
	void FreeResourceManifest(ResourceManifest_t* manifest);
	void CreateResourceManifest(ResourceManifest_t* manifest, MemArena_t* memArena, u64 numEntriesExpected = 0, u64 stringsChunkSize = 0);
	ResourceManifestEntry_t* ResourceManifestFind(ResourceManifest_t* manifest, MyStr_t path, bool caseSensitive = false);
	ResourceManifestEntry_t* ResourceManifestAdd(ResourceManifest_t* manifest, MyStr_t path, bool allowDuplicates = false, bool caseSensitive = false, bool* addEntryOut = nullptr);
#else

// +--------------------------------------------------------------+
// |                       Create and Free                        |
// +--------------------------------------------------------------+
void FreeResourceManifestEntry(ResourceManifest_t* manifest, ResourceManifestEntry_t* entry)
{
	NotNull2(manifest, entry);
	NotNull(manifest->allocArena);
	if (!IsPntrInsideRange(entry->path.chars, manifest->stringsChunk.chars, manifest->stringsChunk.length))
	{
		FreeString(manifest->allocArena, &entry->path);
	}
	ClearPointer(entry);
}
void FreeResourceManifest(ResourceManifest_t* manifest)
{
	NotNull(manifest);
	if (manifest->allocArena != nullptr)
	{
		VarArrayLoop(&manifest->entries, eIndex)
		{
			VarArrayLoopGet(ResourceManifestEntry_t, entry, &manifest->entries, eIndex);
			FreeResourceManifestEntry(manifest, entry);
		}
		FreeVarArray(&manifest->entries);
	}
	if (manifest->stringsChunk.chars != nullptr)
	{
		FreeString(manifest->allocArena, &manifest->stringsChunk);
	}
	ClearPointer(manifest);
}

void CreateResourceManifest(ResourceManifest_t* manifest, MemArena_t* memArena, u64 numEntriesExpected = 0, u64 stringsChunkSize = 0)
{
	NotNull2(manifest, memArena);
	
	ClearPointer(manifest);
	manifest->allocArena = memArena;
	manifest->nextId = 1;
	
	CreateVarArray(&manifest->entries, memArena, sizeof(ResourceManifestEntry_t), numEntriesExpected);
	
	if (stringsChunkSize > 0)
	{
		manifest->stringsChunk.chars = AllocArray(memArena, char, stringsChunkSize);
		manifest->stringsChunk.length = stringsChunkSize;
		manifest->stringsChunkUsed = 0;
	}
}

// +--------------------------------------------------------------+
// |                        Find Function                         |
// +--------------------------------------------------------------+
ResourceManifestEntry_t* ResourceManifestFind(ResourceManifest_t* manifest, MyStr_t path, bool caseSensitive = false)
{
	NotNull(manifest);
	VarArrayLoop(&manifest->entries, eIndex)
	{
		VarArrayLoopGet(ResourceManifestEntry_t, entry, &manifest->entries, eIndex);
		if (( caseSensitive && StrEquals(entry->path, path)) ||
			(!caseSensitive && StrEqualsIgnoreCase(entry->path, path)))
		{
			return entry;
		}
	}
	return nullptr;
}

// +--------------------------------------------------------------+
// |                         Add Function                         |
// +--------------------------------------------------------------+
ResourceManifestEntry_t* ResourceManifestAdd(ResourceManifest_t* manifest, MyStr_t path, bool allowDuplicates = false, bool caseSensitive = false, bool* addEntryOut = nullptr)
{
	NotNull(manifest);
	NotNull(manifest->allocArena);
	NotNullStr(&path);
	
	if (!allowDuplicates)
	{
		ResourceManifestEntry_t* existingEntry = ResourceManifestFind(manifest, path, caseSensitive);
		if (existingEntry != nullptr)
		{
			SetOptionalOutPntr(addEntryOut, false);
			return existingEntry;
		}
	}
	
	SetOptionalOutPntr(addEntryOut, true);
	
	ResourceManifestEntry_t* result = VarArrayAdd(&manifest->entries, ResourceManifestEntry_t);
	NotNull(result);
	ClearPointer(result);
	result->id = manifest->nextId;
	
	if (manifest->stringsChunkUsed + path.length+1 <= manifest->stringsChunk.length)
	{
		result->path.length = path.length;
		result->path.chars = manifest->stringsChunk.chars + manifest->stringsChunkUsed;
		manifest->stringsChunkUsed += path.length+1;
		MyMemCopy(result->path.chars, path.chars, path.length);
		result->path.chars[path.length] = '\0';
	}
	else
	{
		result->path = AllocString(manifest->allocArena, &path);
	}
	
	return result;
}

// +--------------------------------------------------------------+
// |                   Deserialization Function                   |
// +--------------------------------------------------------------+
bool TryDeserResourceManifest(MyStr_t fileContents, ProcessLog_t* log, ResourceManifest_t* manifestOut, MemArena_t* memArena)
{
	NotNullStr(&fileContents);
	NotNull(log);
	NotNull(manifestOut);
	NotNull(memArena);
	
	SetProcessLogName(log, NewStr("TryDeserResourceManifest"));
	LogWriteLine_N(log, "Entering TryDeserResourceManifest...");
	
	if (fileContents.length < RESOURCE_MANIFEST_FILE_PREFIX_LENGTH)
	{
		LogWriteLine_E(log, "An empty file is not a valid Resource Manifest!");
		LogExitFailure(log, TryDeserResourceManifestError_EmptyFile);
		return false;
	}
	if (MyMemCompare(fileContents.chars, RESOURCE_MANIFEST_FILE_PREFIX, RESOURCE_MANIFEST_FILE_PREFIX_LENGTH) != 0)
	{
		LogPrintLine_E(log, "The resource manifest file did not start with the proper file prefix \"%s\"", RESOURCE_MANIFEST_FILE_PREFIX);
		LogExitFailure(log, TryDeserResourceManifestError_InvalidOrMissingHeader);
		return false;
	}
	fileContents.chars += RESOURCE_MANIFEST_FILE_PREFIX_LENGTH;
	fileContents.length -= RESOURCE_MANIFEST_FILE_PREFIX_LENGTH;
	
	TextParser_t textParser;
	ParsingToken_t token;
	
	// Initial Pass to Calculate Count and Size
	u64 numResourceEntries = 0;
	u64 totalNumCharsInPaths = 0;
	textParser = NewTextParser(fileContents);
	while (TextParserGetToken(&textParser, &token))
	{
		if (token.type == ParsingTokenType_KeyValuePair && StrEqualsIgnoreCase(token.key, "Resource"))
		{
			numResourceEntries++;
			totalNumCharsInPaths += token.value.length + 1;
		}
	}
	
	CreateResourceManifest(manifestOut, memArena, numResourceEntries, totalNumCharsInPaths);
	
	// Second pass to actually add all the entries
	textParser = NewTextParser(fileContents);
	while (TextParserGetToken(&textParser, &token))
	{
		if (token.type == ParsingTokenType_KeyValuePair)
		{
			if (StrEqualsIgnoreCase(token.key, "Resource"))
			{
				ResourceManifestEntry_t* newEntry = ResourceManifestAdd(manifestOut, token.value);
				UNUSED(newEntry);
			}
			else
			{
				LogPrintLine_W(log, "WARNING: Unknown key %.*s on line %llu", StrPrint(token.key), textParser.lineParser.lineIndex);
				log->hadWarnings = true;
			}
		}
		else if (token.type == ParsingTokenType_Comment)
		{
			//Do nothing
		}
		else if (token.type == ParsingTokenType_Unknown)
		{
			LogPrintLine_W(log, "WARNING: Invalid syntax found on line %llu: \"%.*s\"", textParser.lineParser.lineIndex, StrPrint(token.str));
			log->hadWarnings = true;
		}
		else
		{
			LogPrintLine_W(log, "WARNING: Unhandled token type %s on line %llu: \"%.*s\"", GetParsingTokenTypeStr(token.type), textParser.lineParser.lineIndex, StrPrint(token.str));
			log->hadWarnings = true;
		}
	}
	
	return true;
}

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_RESOURCE_MANIFEST_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
RESOURCE_MANIFEST_FILE_PREFIX
RESOURCE_MANIFEST_FILE_PREFIX_LENGTH
TryDeserResourceManifestError_None
TryDeserResourceManifestError_EmptyFile
TryDeserResourceManifestError_InvalidOrMissingHeader
TryDeserResourceManifestError_NumErrors
@Types
ResourceManifestEntry_t
ResourceManifest_t
TryDeserResourceManifestError_t
@Functions
const char* GetTryDeserResourceManifestErrorStr(TryDeserResourceManifestError_t enumValue);
void FreeResourceManifestEntry(ResourceManifest_t* manifest, ResourceManifestEntry_t* entry)
void FreeResourceManifest(ResourceManifest_t* manifest)
ResourceManifestEntry_t* ResourceManifestFind(ResourceManifest_t* manifest, MyStr_t path, bool caseSensitive = false)
ResourceManifestEntry_t* ResourceManifestAdd(ResourceManifest_t* manifest, MyStr_t path, bool allowDuplicates = false, bool caseSensitive = false, bool* addEntryOut = nullptr)
bool TryDeserResourceManifest(MyStr_t fileContents, ProcessLog_t* log, ResourceManifest_t* manifestOut, MemArena_t* memArena)
*/
