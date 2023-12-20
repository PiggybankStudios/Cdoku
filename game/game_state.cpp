/*
File:   game_state.cpp
Author: Taylor Robbins
Date:   12\19\2023
Description: 
	** Holds the AppState that runs the main game (where you actually play Sudoku)
*/

GameState_t* game = nullptr;

#include "game_board.cpp"
#include "game_cursor.cpp"

void GameMainMenuSelectedCallback(void* userPntr)
{
	UNUSED(userPntr);
	game->mainMenuRequested = true;
}

MyStr_t GetGameSaveFilePath(MemArena_t* memArena)
{
	MyStr_t levelName = GetFileNamePart(gl->currentLevel, false);
	return PrintInArenaStr(memArena, "%.*s_save.txt", StrPrint(levelName));
}

void GameSaveState()
{
	MemArena_t* scratch = GetScratchArena();
	
	StringBuilder_t output;
	NewStringBuilder(&output, scratch, (BOARD_WIDTH * BOARD_HEIGHT) + (BOARD_WIDTH * BOARD_HEIGHT * (9+1)));
	
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(&game->board, gridPos);
			if (cell->value > 0)
			{
				Assert(cell->value <= 9);
				StringBuilderAppendPrint(&output, "%c", '0' + cell->value);
			}
			else
			{
				StringBuilderAppend(&output, " ");
			}
		}
	}
	
	StringBuilderAppendLine(&output);
	
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(&game->board, gridPos);
			if (cell->notes != 0x0000)
			{
				for (i32 noteIndex = 0; noteIndex < 9; noteIndex++)
				{
					if (IsFlagSet(cell->notes, (1 << noteIndex)))
					{
						StringBuilderAppendPrint(&output, "%c", '0' + noteIndex+1);
					}
					else
					{
						StringBuilderAppend(&output, "-");
					}
				}
			}
			StringBuilderAppendLine(&output);
		}
	}
	
	MyStr_t saveFilePath = GetGameSaveFilePath(scratch);
	bool writeSuccess = WriteEntireFile(saveFilePath, ToMyStr(&output));
	PrintLine_D("%s to \"%.*s\"", (writeSuccess ? "Saved successfully" : "Failed to save"), StrPrint(saveFilePath));
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_Game(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		MemArena_t* scratch = GetScratchArena();
		
		game->numbersSheet = LoadSpriteSheet(NewStr("Resources/Sheets/numbers_large"), 3);
		Assert(game->numbersSheet.isValid);
		
		game->notesSheet = LoadSpriteSheet(NewStr("Resources/Sheets/numbers_small"), 3);
		Assert(game->notesSheet.isValid);
		
		game->backgroundTexture = LoadTexture(NewStr("Resources/Textures/background"));
		Assert(game->backgroundTexture.isValid);
		
		game->ditherTexture = LoadTexture(NewStr("Resources/Textures/dither1"));
		Assert(game->ditherTexture.isValid);
		
		MyStr_t levelFileContents = MyStr_Empty;
		
		NotNullStr(&gl->currentLevel);
		if (!IsEmptyStr(gl->currentLevel))
		{
			if (!ReadEntireFile(false, gl->currentLevel, &levelFileContents, scratch))
			{
				PrintLine_E("Failed to load level from \"%.*s\"", StrPrint(gl->currentLevel));
			}
		}
		
		if (IsEmptyStr(levelFileContents))
		{
			//This is just a placeholder level to use as a backup
			levelFileContents = NewStringInArenaNt(scratch,
				" 87 3  4 \n"
				"    89 3 \n"
				"  9  2  1\n"
				"1  8     \n"
				"   6  7  \n"
				" 7  9 1 6\n"
				"4   6    \n"
				"     39  \n"
				"83  4    \n"
			);
		}
		
		InitBoard(&game->board, levelFileContents);
		InitCursor(&game->cursor, NewVec2i(4, 4), &game->board);
		
		MyStr_t saveFilePath = GetGameSaveFilePath(scratch);
		if (DoesFileExist(true, saveFilePath))
		{
			MyStr_t saveFileContents;
			if (ReadEntireFile(true, saveFilePath, &saveFileContents, scratch))
			{
				PrintLine_I("Reading save data from \"%.*s\"", StrPrint(saveFilePath));
				LoadSaveInfo(&game->board, saveFileContents);
			}
		}
		
		game->initialized = true;
		FreeScratchArena(scratch);
	}
	
	game->mainMenuItem = pd->system->addMenuItem("Main Menu", GameMainMenuSelectedCallback, nullptr);
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_Game(bool deinitialize, AppState_t nextState)
{
	pd->system->removeMenuItem(game->mainMenuItem);
	game->mainMenuItem = nullptr;
	
	if (deinitialize)
	{
		GameSaveState();
		
		ClearPointer(game);
	}
}

// +--------------------------------------------------------------+
// |                            Layout                            |
// +--------------------------------------------------------------+
void GameUiLayout()
{
	BoardLayoutUi(&game->board, NewReci(0, 0, PLAYDATE_SCREEN_WIDTH, PLAYDATE_SCREEN_HEIGHT));
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_Game()
{
	MemArena_t* scratch = GetScratchArena();
	
	if (game->mainMenuRequested)
	{
		game->mainMenuRequested = false;
		PopAppState();
	}
	
	UpdateCursor(&game->cursor, &game->board);
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_Game(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	GameUiLayout();
	
	pd->graphics->clear(kColorWhite);
	PdSetDrawMode(kDrawModeCopy);
	
	PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, PLAYDATE_SCREEN_WIDTH, PLAYDATE_SCREEN_HEIGHT));
	RenderBoard(&game->board, &game->cursor);
	RenderCursor(&game->cursor, &game->board);
	
	// +==============================+
	// |         Debug Render         |
	// +==============================+
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (pig->perfGraph.enabled) { textPos.y += pig->perfGraph.mainRec.y + pig->perfGraph.mainRec.height + 1; }
		PdBindFont(&pig->debugFont);
		i32 stepY = pig->debugFont.lineHeight + 1;
		
		PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
		textPos.y += stepY;
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_Game()
{
	game = (GameState_t*)RegisterAppState(
		AppState_Game,
		sizeof(GameState_t),
		StartAppState_Game,
		StopAppState_Game,
		UpdateAppState_Game,
		RenderAppState_Game
	);
}
