/*
File:   game_main.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_MAIN_H
#define _GAME_MAIN_H

#include "game_version.h"
#include "game_defines.h"

#include "main_menu.h"
#include "game_state.h"

struct GameGlobals_t
{
	bool initialized;
	
	MyStr_t currentLevel;
	Texture_t highlightDither;
	Texture_t errorDither;
	Texture_t noteDither;
	SpriteSheet_t btnPromptsSheet;
};

#endif //  _GAME_MAIN_H
