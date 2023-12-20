/*
File:   game_helpers.cpp
Author: Taylor Robbins
Date:   12\20\2023
Description: 
	** Some functions that are helpful for all AppStates
*/

void SetCurrentLevel(MyStr_t levelPath)
{
	FreeString(mainHeap, &gl->currentLevel);
	gl->currentLevel = AllocString(mainHeap, &levelPath);
}
