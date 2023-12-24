/*
File:   game_state.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "game_board.h"
#include "game_cursor.h"

struct GameState_t
{
	bool initialized;
	bool completed;
	u32 completedAnimStartTime;
	
	PDMenuItem* mainMenuItem;
	bool mainMenuRequested;
	
	SpriteSheet_t numbersSheet;
	SpriteSheet_t notesSheet;
	Texture_t backgroundTexture;
	
	Board_t board;
	Cursor_t cursor;
	
	bool screenIsDirty;
};

#endif //  _GAME_STATE_H
