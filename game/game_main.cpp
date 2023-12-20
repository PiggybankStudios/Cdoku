/*
File:   game_main.cpp
Author: Taylor Robbins
Date:   12\19\2023
Description: 
	** None 
*/

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
GameGlobals_t* gl = nullptr;

// +--------------------------------------------------------------+
// |                         Source Files                         |
// +--------------------------------------------------------------+
#include "main_menu.cpp"
#include "game_state.cpp"

// +--------------------------------------------------------------+
// |                        Main Functions                        |
// +--------------------------------------------------------------+
AppState_t InitGame()
{
	WriteLine_O("+==============================+");
	PrintLine_O("|     %s v%u.%u(%0u)     |", PROJECT_NAME, GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_BUILD);
	WriteLine_O("+==============================+");
	
	RegisterAppState_MainMenu();
	RegisterAppState_Game();
	
	gl = AllocStruct(fixedHeap, GameGlobals_t);
	ClearPointer(gl);
	gl->initialized = true;
	
	return FIRST_APP_STATE;
}

void PreUpdateGame()
{
	
}
void PostUpdateGame()
{
	
}

void PreRenderGame()
{
	
}
void PostRenderGame()
{
	
}
