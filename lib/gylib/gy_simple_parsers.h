/*
File:   gy_simple_parsers.h
Author: Taylor Robbins
Date:   05\09\2023
LineParser Description:
	** This is a simple utility that helps a deserialization process break a string into lines
	** Handles stuff line /n vs /r/n and keeping track of the lineIndex
TextParser Description:
	** TODO:
XmlParser Description:
	** TODO:
*/

#ifndef _GY_SIMPLE_PARSERS_H
#define _GY_SIMPLE_PARSERS_H

// +--------------------------------------------------------------+
// |                            Types                             |
// +--------------------------------------------------------------+
struct LineParser_t
{
	u64 byteIndex;
	u64 lineBeginByteIndex;
	u64 lineIndex; //This is not zero based! It's more like a line number you'd see in the gutter of a text editor!
	bool isStreamBased;
	MyStr_t fileContents;
	Stream_t* stream;
	u64 chunkReadSize;
};

enum ParsingTokenType_t
{
	ParsingTokenType_Unknown = 0,
	ParsingTokenType_FilePrefix, //lines starting with #
	ParsingTokenType_Directive, //lines starting with @
	ParsingTokenType_KeyValuePair, //lines with key: value (where leading/trailing whitespace is stripped from key and value)
	ParsingTokenType_Comment, //anything after a // on any line
	ParsingTokenType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetParsingTokenTypeStr(ParsingTokenType_t enumValue);
#else
const char* GetParsingTokenTypeStr(ParsingTokenType_t enumValue)
{
	switch (enumValue)
	{
		case ParsingTokenType_Unknown:      return "Unknown";
		case ParsingTokenType_FilePrefix:   return "FilePrefix";
		case ParsingTokenType_Directive:    return "Directive";
		case ParsingTokenType_KeyValuePair: return "KeyValuePair";
		case ParsingTokenType_Comment:      return "Comment";
		default: return "Unknown";
	}
}
#endif

struct ParsingToken_t
{
	ParsingTokenType_t type;
	MyStr_t str;
	MyStr_t key;
	MyStr_t value;
};
struct TextParser_t
{
	LineParser_t lineParser;
	MyStr_t currentLine;
	u64 byteIndex;
};

//NOTE: XmlParsingError_t is pre-declared in gy_process_log.h

enum XmlParseResultType_t
{
	XmlParseResultType_None,
	XmlParseResultType_Token,
	XmlParseResultType_EndToken,
	XmlParseResultType_Comment,
	XmlParseResultType_Contents,
	XmlParseResultType_Directive,
	XmlParseResultType_Error,
	XmlParseResultType_NumTypes,
};
#ifdef GYLIB_HEADER_ONLY
const char* GetXmlParseResultTypeStr(XmlParseResultType_t enumValue);
#else
const char* GetXmlParseResultTypeStr(XmlParseResultType_t enumValue)
{
	switch (enumValue)
	{
		case XmlParseResultType_None:      return "None";
		case XmlParseResultType_Token:     return "Token";
		case XmlParseResultType_EndToken:  return "EndToken";
		case XmlParseResultType_Comment:   return "Comment";
		case XmlParseResultType_Contents:  return "Contents";
		case XmlParseResultType_Directive: return "Directive";
		case XmlParseResultType_Error:     return "Error";
		default: return "Unknown";
	}
}
#endif

struct XmlProperty_t
{
	MyStr_t key;
	MyStr_t value; //often encoded with escape characters
	bool valueWasQuoted;
};
struct XmlToken_t
{
	u64 tokenParseIndex;
	MyStr_t type;
	VarArray_t properties;
};
struct XmlParser_t
{
	MemArena_t* allocArena;
	
	u64 nextTokenParseIndex;
	
	LineParser_t lineParser;
	MyStr_t currentLine;
	u64 byteIndex;
	
	VarArray_t parentTokens; //XmlToken_t
	XmlToken_t newToken;
	bool newTokenIsOpening;
};
struct XmlParseResult_t
{
	XmlParseResultType_t type;
	XmlToken_t token;
	MyStr_t string;
	XmlParsingError_t error;
};

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	LineParser_t NewLineParser(MyStr_t fileContents);
	LineParser_t NewLineParser(Stream_t* streamPntr);
	bool LineParserGetLine(LineParser_t* parser, MyStr_t* lineOut, MemArena_t* chunkArena = nullptr);
	bool LineParserIsFinished(const LineParser_t* parser);
	TextParser_t NewTextParser(MyStr_t fileContents);
	TextParser_t NewTextParser(Stream_t* streamPntr);
	bool TextParserGetToken(TextParser_t* parser, ParsingToken_t* tokenOut, MemArena_t* chunkArena = nullptr);
	XmlParser_t NewXmlParser(MemArena_t* arenaForLists, MyStr_t fileContents);
	void FreeXmlParser(XmlParser_t* parser);
	#if GYLIB_SCRATCH_ARENA_AVAILABLE
	bool XmlParserCheckIdentifierHasValidChars(XmlParser_t* parser, MyStr_t identifierStr, XmlParseResult_t* result, ProcessLog_t* log);
	bool XmlParserGetToken(XmlParser_t* parser, XmlParseResult_t* result, ProcessLog_t* log);
	#endif
	XmlProperty_t* GetXmlProperty(XmlToken_t* token, MyStr_t propertyKey, bool ignoreCase = false);
#else

// +--------------------------------------------------------------+
// |                     LineParser Functions                     |
// +--------------------------------------------------------------+
LineParser_t NewLineParser(MyStr_t fileContents)
{
	NotNullStr(&fileContents);
	LineParser_t result = {};
	result.byteIndex = 0;
	result.lineIndex = 0;
	result.isStreamBased = false;
	result.fileContents = fileContents;
	return result;
}
LineParser_t NewLineParser(Stream_t* streamPntr)
{
	NotNull(streamPntr);
	Assert(StreamIsValid(streamPntr));
	LineParser_t result = {};
	result.byteIndex = 0;
	result.lineIndex = 0;
	result.isStreamBased = true;
	result.stream = streamPntr;
	result.chunkReadSize = 1024;
	return result;
}

//chunkArena is only required if the parser isStreamBased, and that stream doesn't support StaticRead
bool LineParserGetLine(LineParser_t* parser, MyStr_t* lineOut, MemArena_t* chunkArena = nullptr)
{
	NotNull(parser);
	
	if (parser->isStreamBased)
	{
		NotNull(parser->stream);
		if (StreamIsOver(parser->stream)) { return false; }
		MyStr_t nextLine = StreamReadUntil(parser->stream, NewStr("\n"), false, chunkArena, parser->chunkReadSize);
		if (nextLine.length > 0 && nextLine.chars[nextLine.length-1] == '\r') { nextLine.length--; }
		SetOptionalOutPntr(lineOut, nextLine);
		return true;
	}
	else
	{
		NotNullStr(&parser->fileContents);
		if (parser->byteIndex >= parser->fileContents.length) { return false; }
		parser->lineIndex++;
		parser->lineBeginByteIndex = parser->byteIndex;
		
		u64 endOfLineByteSize = 0;
		u64 startIndex = parser->byteIndex;
		while (parser->byteIndex < parser->fileContents.length)
		{
			char nextChar = parser->fileContents.pntr[parser->byteIndex];
			char nextNextChar = parser->fileContents.pntr[parser->byteIndex+1];
			if (nextChar != nextNextChar &&
				(nextChar     == '\n' || nextChar     == '\r') &&
				(nextNextChar == '\n' || nextNextChar == '\r'))
			{
				endOfLineByteSize = 2;
				break;
			}
			else if (nextChar == '\n' || nextChar == '\r')
			{
				endOfLineByteSize = 1;
				break;
			}
			else
			{
				parser->byteIndex++;
			}
		}
		
		MyStr_t line = NewStr(parser->byteIndex - startIndex, &parser->fileContents.pntr[startIndex]);
		parser->byteIndex += endOfLineByteSize;
		SetOptionalOutPntr(lineOut, line);
		return true;
	}
}

bool LineParserIsFinished(const LineParser_t* parser)
{
	if (parser->isStreamBased)
	{
		return StreamIsOver(parser->stream);
	}
	else
	{
		return (parser->byteIndex >= parser->fileContents.length);
	}
}

// +--------------------------------------------------------------+
// |                     TextParser Functions                     |
// +--------------------------------------------------------------+
TextParser_t NewTextParser(MyStr_t fileContents)
{
	TextParser_t result = {};
	result.lineParser = NewLineParser(fileContents);
	result.byteIndex = 0;
	result.currentLine = MyStr_Empty;
	return result;
}
TextParser_t NewTextParser(Stream_t* streamPntr)
{
	TextParser_t result = {};
	result.lineParser = NewLineParser(streamPntr);
	result.byteIndex = 0;
	result.currentLine = MyStr_Empty;
	return result;
}

//chunkArena is only required if the parser isStreamBased, and that stream doesn't support StaticRead
bool TextParserGetToken(TextParser_t* parser, ParsingToken_t* tokenOut, MemArena_t* chunkArena = nullptr)
{
	NotNull(parser);
	NotNull(tokenOut);
	
	while (!LineParserIsFinished(&parser->lineParser))
	{
		if (parser->byteIndex >= parser->currentLine.length)
		{
			bool gotLine = LineParserGetLine(&parser->lineParser, &parser->currentLine, chunkArena);
			if (!gotLine)
			{
				// GyLibWriteLine_W("Got no more lines from lineParser");
				return false;
			}
			parser->byteIndex = 0;
		}
		
		MyStr_t line = StrSubstring(&parser->currentLine, parser->byteIndex);
		u64 numTrimmedWhitespaceChars = TrimLeadingWhitespace(&line);
		if (line.length == 0)
		{
			parser->byteIndex = parser->currentLine.length;
			continue;
		}
		
		//TODO: This doesn't handle if a // shows up inside something like a string where it shouldn't be treated as a comment
		u64 commentStartIndex = 0;
		bool lineContainsComment = FindSubstring(line, "//", &commentStartIndex);
		
		if (lineContainsComment && commentStartIndex == 0)
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_Comment;
			tokenOut->str = line;
			tokenOut->value = StrSubstring(&tokenOut->str, 2, tokenOut->str.length);
			parser->byteIndex = parser->currentLine.length;
			return true;
		}
		else if (lineContainsComment)
		{
			line = StrSubstring(&line, 0, commentStartIndex);
		}
		
		u64 colonIndex = 0;
		bool lineContainsColon = FindSubstring(line, ":", &colonIndex);
		
		if (lineContainsColon)
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_KeyValuePair;
			tokenOut->str = line;
			tokenOut->key = StrSubstring(&line, 0, colonIndex);
			tokenOut->value = StrSubstring(&line, colonIndex+1, line.length);
			TrimWhitespace(&tokenOut->key);
			TrimWhitespace(&tokenOut->value);
			parser->byteIndex += numTrimmedWhitespaceChars + line.length;
			return true;
		}
		
		if (StrStartsWith(line, "#"))
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_FilePrefix;
			tokenOut->str = line;
			tokenOut->value = StrSubstring(&line, 1);
			parser->byteIndex += numTrimmedWhitespaceChars + line.length;
			return true;
		}
		else if (StrStartsWith(line, "@"))
		{
			ClearPointer(tokenOut);
			tokenOut->type = ParsingTokenType_Directive;
			tokenOut->str = line;
			tokenOut->value = StrSubstring(&line, 1);
			parser->byteIndex += numTrimmedWhitespaceChars + line.length;
			return true;
		}
		
		ClearPointer(tokenOut);
		tokenOut->type = ParsingTokenType_Unknown;
		tokenOut->str = line;
		parser->byteIndex += numTrimmedWhitespaceChars + line.length;
		return true;
	}
	
	// GyLibWriteLine_W("Line Parser is finished");
	return false;
}

// +--------------------------------------------------------------+
// |                     XmlParser Functions                      |
// +--------------------------------------------------------------+
//TODO: Add support for Stream_t based XmlParser
XmlParser_t NewXmlParser(MemArena_t* arenaForLists, MyStr_t fileContents)
{
	XmlParser_t result = {};
	result.allocArena = arenaForLists;
	result.lineParser = NewLineParser(fileContents);
	result.currentLine = MyStr_Empty;
	result.byteIndex = 0;
	CreateVarArray(&result.parentTokens, result.allocArena, sizeof(XmlToken_t));
	return result;
}

void FreeXmlParser(XmlParser_t* parser)
{
	NotNull(parser);
	VarArrayLoop(&parser->parentTokens, tIndex)
	{
		VarArrayLoopGet(XmlToken_t, token, &parser->parentTokens, tIndex);
		FreeVarArray(&token->properties);
	}
	FreeVarArray(&parser->parentTokens);
	ClearPointer(parser);
}

#if GYLIB_SCRATCH_ARENA_AVAILABLE
bool XmlParserCheckIdentifierHasValidChars(XmlParser_t* parser, MyStr_t identifierStr, XmlParseResult_t* result, ProcessLog_t* log)
{
	NotNull(parser);
	NotNullStr(&identifierStr);
	NotNull(result);
	NotNull(log);
	for (u64 cIndex = 0; cIndex < identifierStr.length; )
	{
		u32 codepoint = 0;
		u8 codepointByteSize = GetCodepointForUtf8Str(identifierStr, cIndex, &codepoint);
		if (codepointByteSize == 0)
		{
			LogPrintLine_E(log, "Invalid UTF-8 encoding in identifierStr on line %llu", parser->lineParser.lineIndex);
			result->error = XmlParsingError_InvalidUtf8;
			return false;
		}
		if (!(codepoint >= '0' && codepoint <= '9') &&
			!(codepoint >= 'A' && codepoint <= 'Z') &&
			!(codepoint >= 'a' && codepoint <= 'z') &&
			codepoint != '_' && codepoint != ':' && codepoint != '-')
		{
			LogPrintLine_E(log, "Invalid character in identifierStr on line %llu: 0x%08X \'%c\'", parser->lineParser.lineIndex, codepoint, (char)codepoint);
			result->error = XmlParsingError_InvalidCharInIdentifier;
			return false;
		}
		cIndex += codepointByteSize;
	}
	return true;
}

//NOTE: This XML parser is making a lot of assumptions, it probably doesn't parse the full XML format
//      <!-- ... --> and <? ... ?> tokens get thrown out without the calling code getting a chance to look at them
//      Token property key-value pairs have to be on a single line (the key can't be on a separate line from the value)
bool XmlParserGetToken(XmlParser_t* parser, XmlParseResult_t* result, ProcessLog_t* log)
{
	NotNull(parser);
	NotNull(parser->allocArena);
	NotNull(result);
	
	if (parser->newTokenIsOpening)
	{
		XmlToken_t* newTokenSpace = VarArrayAdd(&parser->parentTokens, XmlToken_t);
		NotNull(newTokenSpace);
		MyMemCopy(newTokenSpace, &parser->newToken, sizeof(XmlToken_t));
	}
	else
	{
		FreeVarArray(&parser->newToken.properties);
	}
	ClearStruct(parser->newToken);
	parser->newTokenIsOpening = false;
	
	bool inComment = false;
	bool inToken = false;
	bool isEndingToken = false;
	bool foundTokenType = false;
	bool inDirectiveToken = false;
	while (true)
	{
		if (parser->byteIndex >= parser->currentLine.length)
		{
			bool gotNextLine = LineParserGetLine(&parser->lineParser, &parser->currentLine);
			if (!gotNextLine) { break; }
			parser->byteIndex = 0;
		}
		Assert(parser->byteIndex <= parser->currentLine.length);
		
		u64 workingStartIndex = parser->byteIndex;
		MyStr_t workingLine = StrSubstring(&parser->currentLine, parser->byteIndex);
		workingStartIndex += TrimLeadingWhitespace(&workingLine);
		TrimTrailingWhitespace(&workingLine);
		
		if (workingLine.length == 0)
		{
			parser->byteIndex = parser->currentLine.length;
			continue;
		}
		
		if (inComment)
		{
			u64 endCommentIndex = 0;
			if (FindSubstring(workingLine, "-->", &endCommentIndex))
			{
				parser->byteIndex = workingStartIndex + endCommentIndex + 3;
				inComment = false;
				continue;
			}
			else
			{
				parser->byteIndex = parser->currentLine.length;
				continue;
			}
		}
		else if (inToken)
		{
			if (!foundTokenType)
			{
				//find the first whitespace or other name ending character (like " or >)
				u64 nameEndingCharIndex = 0;
				bool nameGoesTillEndOfLine = !FindNextCharInStr(workingLine, 0, " \t\"<>", &nameEndingCharIndex);
				if (nameGoesTillEndOfLine) { nameEndingCharIndex = workingLine.length; }
				if (nameEndingCharIndex == 0)
				{
					if (log != nullptr) { LogPrintLine_E(log, "No name given for token on line %llu: \"%.*s\"", parser->lineParser.lineIndex, StrPrint(workingLine)); }
					result->type = XmlParseResultType_Error;
					result->error = XmlParsingError_NoTypeFoundForToken;
					return true;
				}
				
				
				if (isEndingToken)
				{
					if (nameGoesTillEndOfLine)
					{
						if (log != nullptr ) { LogPrintLine_E(log, "Unexpected end of line when parsing closing token on line %llu byte %llu: \"%.*s\"", parser->lineParser.lineIndex, workingStartIndex, StrPrint(workingLine)); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_UnexpectedEol;
						return true;
					}
					if (workingLine.pntr[nameEndingCharIndex] != '>')
					{
						if (log != nullptr ) { LogPrintLine_E(log, "Expected > instead of 0x%08X \'%c\' when parsing closing token on line %llu byte %llu: \"%.*s\"", CharToU32(workingLine.pntr[nameEndingCharIndex]), workingLine.pntr[nameEndingCharIndex], parser->lineParser.lineIndex, workingStartIndex, StrPrint(workingLine)); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_ExpectedClosingAngleBracket;
						return true;
					}
					
					MyStr_t endingTokenType = NewStr(nameEndingCharIndex, workingLine.pntr);
					
					if (parser->parentTokens.length == 0)
					{
						if (log != nullptr ) { LogPrintLine_E(log, "Unexpected closing token. No tokens have been started on line %llu: \"%.*s\"", parser->lineParser.lineIndex, StrPrint(workingLine)); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_UnexpectedClosingToken;
						return true;
					}
					XmlToken_t* parentToken = VarArrayGetHard(&parser->parentTokens, parser->parentTokens.length-1, XmlToken_t);
					if (!StrEquals(parentToken->type, endingTokenType))
					{
						if (log != nullptr ) { LogPrintLine_E(log, "Ending token mismatch. Expect token \"%.*s\" to end, not \"%.*s\" on line %llu: \"%.*s\"", StrPrint(parentToken->type), StrPrint(endingTokenType), parser->lineParser.lineIndex, StrPrint(workingLine)); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_ClosingTokenMismatch;
						return true;
					}
					
					MyMemCopy(&parser->newToken, parentToken, sizeof(XmlToken_t));
					parser->newTokenIsOpening = false;
					VarArrayPop(&parser->parentTokens, XmlToken_t);
					
					isEndingToken = false;
					inToken = false;
					parser->byteIndex = workingStartIndex + nameEndingCharIndex + 1;
					
					result->type = XmlParseResultType_EndToken;
					MyMemCopy(&result->token, &parser->newToken, sizeof(XmlToken_t));
					return true;
				}
				else
				{
					ClearStruct(parser->newToken);
					parser->newToken.type = NewStr(nameEndingCharIndex, workingLine.pntr);
					
					if (!XmlParserCheckIdentifierHasValidChars(parser, parser->newToken.type, result, log))
					{
						//error and log were already done by XmlParserCheckIdentifierHasValidChars
						return true;
					}
					
					CreateVarArray(&parser->newToken.properties, parser->allocArena, sizeof(XmlProperty_t));
					
					foundTokenType = true;
					if (nameGoesTillEndOfLine) { parser->byteIndex = parser->currentLine.length; }
					else { parser->byteIndex = workingStartIndex + nameEndingCharIndex; }
					continue;
				}
			}
			else
			{
				if (StrStartsWith(workingLine, "/>"))
				{
					parser->newToken.tokenParseIndex = parser->nextTokenParseIndex;
					parser->nextTokenParseIndex++;
					parser->newTokenIsOpening = false;
					parser->byteIndex = workingStartIndex + 2;
					result->type = XmlParseResultType_Token;
					MyMemCopy(&result->token, &parser->newToken, sizeof(XmlToken_t));
					return true;
				}
				else if (StrStartsWith(workingLine, ">"))
				{
					parser->newToken.tokenParseIndex = parser->nextTokenParseIndex;
					parser->nextTokenParseIndex++;
					parser->newTokenIsOpening = true;
					parser->byteIndex = workingStartIndex + 1;
					result->type = XmlParseResultType_Token;
					MyMemCopy(&result->token, &parser->newToken, sizeof(XmlToken_t));
					return true;
				}
				else
				{
					u32 nextCodepoint = 0;
					u8 nextCodepointByteSize = GetCodepointForUtf8Str(workingLine, 0, &nextCodepoint);
					UNUSED(nextCodepointByteSize);
					if (!(nextCodepoint >= 'A' && nextCodepoint <= 'Z') &&
						!(nextCodepoint >= 'a' && nextCodepoint <= 'z') &&
						nextCodepoint != '_')
					{
						if (log != nullptr) { LogPrintLine_E(log, "Invalid first character for property of token on line %llu: 0x%08X \'%c\'", parser->lineParser.lineIndex, nextCodepoint, (char)nextCodepoint); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_InvalidPropertyFirstChar;
						return true;
					}
					
					u64 equalsIndex = 0;
					bool foundEquals = FindNextCharInStr(workingLine, 0, "=", &equalsIndex);
					if (!foundEquals)
					{
						if (log != nullptr) { LogPrintLine_E(log, "No equals found for property of token on line %llu: \"%.*s\"", parser->lineParser.lineIndex, StrPrint(workingLine)); }
						result->type = XmlParseResultType_Error;
						result->error = XmlParsingError_NoEqualsForProperty;
						return true;
					}
					
					MyStr_t propertyKey = StrSubstring(&workingLine, 0, equalsIndex);
					TrimTrailingWhitespace(&propertyKey);
					if (!XmlParserCheckIdentifierHasValidChars(parser, propertyKey, result, log))
					{
						//error and log were already done by XmlParserCheckIdentifierHasValidChars
						return false;
					}
					
					//TODO: This is not super safe, per-se. If we see a quoted string for the value
					u64 valueEndIndex = 0;
					bool foundValueEndChar = FindNextCharInStr(workingLine, equalsIndex+1, " \t<>", &valueEndIndex, true);
					if (!foundValueEndChar)
					{
						valueEndIndex = workingLine.length;
					}
					
					bool propertyValueWasQuoted = false;
					MyStr_t propertyValue = StrSubstring(&workingLine, equalsIndex+1, valueEndIndex);
					TrimLeadingWhitespace(&propertyValue);
					if (StrStartsWith(propertyValue, "\"") && StrEndsWith(propertyValue, "\""))
					{
						propertyValueWasQuoted = true;
						propertyValue = StrSubstring(&propertyValue, 1, propertyValue.length-1);
					}
					else
					{
						if (propertyValue.length == 0)
						{
							if (log != nullptr) { LogPrintLine_E(log, "Missing property value on line %llu: \"%.*s\"", parser->lineParser.lineIndex, StrPrint(workingLine)); }
							result->type = XmlParseResultType_Error;
							result->error = XmlParsingError_MissingPropertyValue;
							return true;
						}
						//TODO: We should somehow check that the value is "valid" for implicit types
						//      Stuff like: true, false, 100, -1.01, etc.
					}
					
					XmlProperty_t* newProperty = VarArrayAdd(&parser->newToken.properties, XmlProperty_t);
					NotNull(newProperty);
					ClearPointer(newProperty);
					newProperty->key = propertyKey;
					newProperty->value = propertyValue;
					newProperty->valueWasQuoted = propertyValueWasQuoted;
					
					parser->byteIndex = workingStartIndex + valueEndIndex;
				}
			}
		}
		else if (inDirectiveToken)
		{
			//TODO: Technically this doesn't handle if the directive token has a string with ?> in it,
			//      but that's not a use case we have right now so I'm not going to solve it
			u64 endDirectiveIndex = 0;
			if (FindSubstring(workingLine, "?>", &endDirectiveIndex))
			{
				parser->byteIndex = workingStartIndex + endDirectiveIndex + 2;
				inDirectiveToken = false;
				continue;
			}
			else
			{
				parser->byteIndex = parser->currentLine.length;
				continue;
			}
		}
		else
		{
			if (StrStartsWith(workingLine, "<!--"))
			{
				inComment = true;
				parser->byteIndex = workingStartIndex + 3;
				continue;
			}
			else if (StrStartsWith(workingLine, "<?"))
			{
				inDirectiveToken = true;
				parser->byteIndex = workingStartIndex + 2;
				continue;
			}
			else if (StrStartsWith(workingLine, "</"))
			{
				inToken = true;
				isEndingToken = true;
				foundTokenType = false;
				parser->byteIndex = workingStartIndex + 2;
				continue;
			}
			else if (StrStartsWith(workingLine, "<"))
			{
				inToken = true;
				isEndingToken = false;
				foundTokenType = false;
				parser->byteIndex = workingStartIndex + 1;
				continue;
			}
			else
			{
				u64 nextOpenBracketIndex = 0;
				bool foundOpenBracket = FindNextCharInStr(workingLine, 0, "<", &nextOpenBracketIndex, true);
				if (!foundOpenBracket) { nextOpenBracketIndex = workingLine.length; }
				Assert(nextOpenBracketIndex > 0);
				
				MyStr_t contentStr = NewStr(nextOpenBracketIndex, workingLine.pntr);
				if (parser->parentTokens.length == 0)
				{
					if (log != nullptr ) { LogPrintLine_E(log, "Invalid character(s) found outside token on line %llu byte %llu: \"%.*s\"", parser->lineParser.lineIndex, workingStartIndex, StrPrint(workingLine)); }
					result->type = XmlParseResultType_Error;
					result->error = XmlParsingError_InvalidCharacterOutsideToken;
					return true;
				}
				
				parser->byteIndex = (foundOpenBracket ? (workingStartIndex + nextOpenBracketIndex) : parser->currentLine.length);
				result->type = XmlParseResultType_Contents;
				result->string = contentStr;
				return true;
			}
		}
	}
	
	if (parser->parentTokens.length > 0)
	{
		if (log != nullptr )
		{
			LogPrintLine_E(log, "Missing closing token for %llu token%s:", parser->parentTokens.length, (parser->parentTokens.length == 1) ? "" : "s");
			VarArrayLoop(&parser->parentTokens, tIndex)
			{
				VarArrayLoopGet(XmlToken_t, token, &parser->parentTokens, tIndex);
				LogPrintLine_E(log, "  [%llu]: %llu \"%.*s\"", tIndex, token->tokenParseIndex, StrPrint(token->type));
			}
		}
		result->type = XmlParseResultType_Error;
		result->error = XmlParsingError_MissingClosingTokens;
		return true;
	}
	
	return false;
}
#endif

XmlProperty_t* GetXmlProperty(XmlToken_t* token, MyStr_t propertyKey, bool ignoreCase = false)
{
	NotNull(token);
	VarArrayLoop(&token->properties, pIndex)
	{
		VarArrayLoopGet(XmlProperty_t, property, &token->properties, pIndex);
		if ((!ignoreCase && StrEquals(property->key, propertyKey)) ||
			(ignoreCase && StrEqualsIgnoreCase(property->key, propertyKey)))
		{
			return property;
		}
	}
	return nullptr;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_SIMPLE_PARSERS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
ParsingTokenType_Unknown
ParsingTokenType_FilePrefix
ParsingTokenType_KeyValuePair
ParsingTokenType_Comment
ParsingTokenType_NumTypes
XmlParseResultType_None
XmlParseResultType_Token
XmlParseResultType_EndToken
XmlParseResultType_Comment
XmlParseResultType_Contents
XmlParseResultType_Directive
XmlParseResultType_Error
XmlParseResultType_NumTypes
@Types
LineParser_t
ParsingTokenType_t
ParsingToken_t
TextParser_t
XmlParseResultType_t
XmlProperty_t
XmlToken_t
XmlParser_t
XmlParseResult_t
@Functions
const char* GetParsingTokenTypeStr(ParsingTokenType_t enumValue)
const char* GetXmlParsingErrorStr(XmlParsingError_t error)
const char* GetXmlParseResultTypeStr(XmlParseResultType_t enumValue)
LineParser_t NewLineParser(Stream_t* streamPntr)
bool LineParserGetLine(LineParser_t* parser, MyStr_t* lineOut, MemArena_t* chunkArena = nullptr)
bool LineParserIsFinished(const LineParser_t* parser);
TextParser_t NewTextParser(Stream_t* streamPntr)
bool TextParserGetToken(TextParser_t* parser, ParsingToken_t* tokenOut, MemArena_t* chunkArena = nullptr)
XmlParser_t NewXmlParser(MemArena_t* arenaForLists, MyStr_t fileContents)
void FreeXmlParser(XmlParser_t* parser)
bool XmlParserCheckIdentifierHasValidChars(XmlParser_t* parser, MyStr_t identifierStr, XmlParseResult_t* result, ProcessLog_t* log)
bool XmlParserGetToken(XmlParser_t* parser, XmlParseResult_t* result, ProcessLog_t* log)
XmlProperty_t* GetXmlProperty(XmlToken_t* token, MyStr_t propertyKey, bool ignoreCase = false)
*/
