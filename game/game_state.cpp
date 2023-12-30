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
	
	MyStr_t saveFilePath = GetLevelSaveFilePath(scratch, gl->currentLevel, game->completed);
	bool writeSuccess = WriteEntireFile(saveFilePath, ToMyStr(&output));
	PrintLine_D("%s to \"%.*s\"", (writeSuccess ? "Saved successfully" : "Failed to save"), StrPrint(saveFilePath));
	
	if (game->completed)
	{
		// Delete the in-progress save file if we have completed this level
		MyStr_t oldSaveFilePath = GetLevelSaveFilePath(scratch, gl->currentLevel, false);
		if (DoesFileExist(true, oldSaveFilePath))
		{
			DeleteFile(oldSaveFilePath);
		}
	}
	
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
		
		MyStr_t completedFilePath = GetLevelSaveFilePath(scratch, gl->currentLevel, true);
		MyStr_t saveFilePath = GetLevelSaveFilePath(scratch, gl->currentLevel, false);
		if (DoesFileExist(true, completedFilePath))
		{
			PrintLine_I("Reading completed save data from \"%.*s\"", StrPrint(saveFilePath));
			game->completed = true;
			game->completedAnimStartTime = ProgramTime;
			game->completedAnimRand = GetRandU32(&pig->random);
			MyStr_t completedFileContents;
			if (ReadEntireFile(true, completedFilePath, &completedFileContents, scratch))
			{
				LoadSaveInfo(&game->board, completedFileContents);
			}
		}
		else if (DoesFileExist(true, saveFilePath))
		{
			PrintLine_I("Reading save data from \"%.*s\"", StrPrint(saveFilePath));
			MyStr_t saveFileContents;
			if (ReadEntireFile(true, saveFilePath, &saveFileContents, scratch))
			{
				LoadSaveInfo(&game->board, saveFileContents);
			}
		}
		
		game->initialized = true;
		FreeScratchArena(scratch);
	}
	
	game->mainMenuItem = pd->system->addMenuItem("Main Menu", GameMainMenuSelectedCallback, nullptr);
	
	game->screenIsDirty = true;
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
	
	if (game->nextUpdateIsDirty)
	{
		game->screenIsDirty = true;
		game->nextUpdateIsDirty = false;
	}
	
	if (game->mainMenuRequested)
	{
		game->mainMenuRequested = false;
		PopAppState();
	}
	
	// +==============================+
	// |        Update Cursor         |
	// +==============================+
	bool boardChanged = false;
	if (!game->completed)
	{
		boardChanged = UpdateCursor(&game->cursor, &game->board);
	}
	
	// +==============================+
	// |     Check for Conflicts      |
	// +==============================+
	if (boardChanged)
	{
		BoardUpdateConflicts(&game->board);
	}
	
	// +==============================+
	// |     Check for Completion     |
	// +==============================+
	if (boardChanged && !game->completed)
	{
		if (BoardCheckCompletion(&game->board))
		{
			WriteLine_I("Board completed!");
			game->completed = true;
			game->completedAnimStartTime = ProgramTime;
			game->completedAnimRand = GetRandU32(&pig->random);
			GameSaveState();
		}
	}
	if (game->completed && game->completedAnimStartTime != 0 && TimeSince(game->completedAnimStartTime) <= GAME_COMPLETION_ANIM_TIME)
	{
		game->screenIsDirty = true;
		game->nextUpdateIsDirty = true;
	}
	
	// +===============================+
	// | Debug Restart Completion Anim |
	// +===============================+
	#if DEBUG_BUILD
	if (game->completed && BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		game->completedAnimStartTime = ProgramTime;
		game->completedAnimRand = GetRandU32(&pig->random);
	}
	#endif
	
	// +====================================================+
	// | Btn_B Returns to Main Menu When Level is Complete  |
	// +====================================================+
	if (game->completed && BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		PopAppState();
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_Game(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	GameUiLayout();
	
	if (game->screenIsDirty || pig->debugEnabled || pig->perfGraph.enabled)
	{
		
		pd->graphics->clear(kColorWhite);
		PdSetDrawMode(kDrawModeCopy);
		
		// +==============================+
		// |      Render Background       |
		// +==============================+
		if (!pig->debugEnabled)
		{
			PdDrawTexturedRec(game->backgroundTexture, NewReci(0, 0, PLAYDATE_SCREEN_WIDTH, PLAYDATE_SCREEN_HEIGHT));
		}
		
		// +==============================+
		// |      Render Status Bar       |
		// +==============================+
		{
			reci statusRec;
			statusRec.x = 0;
			statusRec.y = game->board.mainRec.y + game->board.mainRec.height;
			statusRec.width = ScreenSize.width;
			statusRec.height = ScreenSize.height - statusRec.y;
			MyStr_t currentLevelName = GetFileNamePart(gl->currentLevel, false);
			v2i currentLevelNameSize = MeasureText(pig->debugFont.font, currentLevelName);
			v2i currentLevelNameDrawPos = NewVec2i(
				statusRec.x + statusRec.width - 5 - currentLevelNameSize.width,
				statusRec.y + statusRec.height/2 - currentLevelNameSize.height/2
			);
			
			PdDrawRec(statusRec, kColorBlack);
			PdSetDrawMode(kDrawModeNXOR);
			
			PdBindFont(&pig->debugFont);
			PdDrawText(currentLevelName, currentLevelNameDrawPos);
			
			PdSetDrawMode(kDrawModeCopy);
		}
		
		// +==============================+
		// |         Render Board         |
		// +==============================+
		RenderBoard(&game->board, &game->cursor, game->completed);
		
		// +==============================+
		// |        Render Cursor         |
		// +==============================+
		if (!game->completed)
		{
			RenderCursor(&game->cursor, &game->board);
		}
		
		// +==============================+
		// | Render Completion Animation  |
		// +==============================+
		if (game->completed && game->completedAnimStartTime != 0)
		{
			u32 animRunTime = TimeSince(game->completedAnimStartTime);
			r32 animProgress = (r32)animRunTime / (r32)GAME_COMPLETION_ANIM_TIME;
			RandomSeries_t completionRandom;
			CreateRandomSeries(&completionRandom);
			SeedRandomSeriesU32(&completionRandom, game->completedAnimRand);
			
			LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
			
			const u32 numColumns = 40;
			for (u32 column = 0; column < numColumns; column++)
			{
				reci columnRec = NewReci(column * (ScreenSize.width / numColumns), 0, (ScreenSize.width / numColumns), ScreenSize.height);
				r32 speed = GetRandR32(&completionRandom, 0.4f, 1.0f);
				r32 offset = GetRandR32(&completionRandom, 0, 1.0f - speed);
				EasingStyle_t easingStyle = EasingStyle_QuadraticInOut;
				switch (GetRandU32(&completionRandom, 0, 7))
				{
					case 0: easingStyle = EasingStyle_Linear; break;
					case 1: easingStyle = EasingStyle_QuadraticOut; break;
					case 2: easingStyle = EasingStyle_QuadraticInOut; break;
					case 3: easingStyle = EasingStyle_QuadraticIn; break;
					case 4: easingStyle = EasingStyle_CubicOut; break;
					case 5: easingStyle = EasingStyle_CubicInOut; break;
					case 6: easingStyle = EasingStyle_CubicIn; break;
				}
				columnRec.height = RoundR32i((r32)columnRec.height * Ease(easingStyle, SubAnimAmountR32(animProgress, offset, offset + speed)));
				PdDrawRec(columnRec, kColorBlack);
			}
			
			// reci invertRec = NewReci(0, 0, RoundR32i(ScreenSize.width * animProgress), ScreenSize.height);
			// PdDrawRec(invertRec, kColorBlack);
			
			PdSetDrawMode(oldDrawMode);
		}
		
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
			
			PdDrawTextPrint(textPos, "Memory: %.2lf%%", ((r64)mainHeap->used / (r64)MAIN_HEAP_MAX_SIZE) * 100.0);
			textPos.y += stepY;
			PdDrawTextPrint(textPos, "ProgramTime: %u (%u)", ProgramTime, input->realProgramTime);
			textPos.y += stepY;
			PdDrawTextPrint(textPos, "Screen: %s%s", game->screenIsDirty ? "Yes" : "No", game->nextUpdateIsDirty ? " (Double)" : "");
			textPos.y += stepY;
			
			PdSetDrawMode(oldDrawMode);
		}
		
		game->screenIsDirty = false;
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
