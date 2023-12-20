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

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_Game(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		game->backgroundTexture = LoadTexture(NewStr("Resources/Textures/background"));
		Assert(game->backgroundTexture.isValid);
		
		game->numbersSheet = LoadSpriteSheet(NewStr("Resources/Sheets/numbers_large"), 3);
		Assert(game->numbersSheet.isValid);
		
		game->notesSheet = LoadSpriteSheet(NewStr("Resources/Sheets/numbers_small"), 3);
		Assert(game->notesSheet.isValid);
		
		InitBoard(&game->board, NewStr(
			"1        "
			" 2     6 "
			"  3      "
			"         "
			"    5    "
			"         "
			"      7  "
			" 4     8 "
			"        9"
		));
		InitCursor(&game->cursor, NewVec2i(4, 4), &game->board);
		game->initialized = true;
	}
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_Game(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		
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
	
	PdDrawTexturedRec(game->backgroundTexture.bitmap, game->backgroundTexture.size, NewReci(0, 0, PLAYDATE_SCREEN_WIDTH, PLAYDATE_SCREEN_HEIGHT));
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
