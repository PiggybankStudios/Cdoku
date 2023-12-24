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

MyStr_t GetLevelSaveFilePath(MemArena_t* memArena, MyStr_t levelPath, bool completed)
{
	MyStr_t levelName = GetFileNamePart(levelPath, false);
	return PrintInArenaStr(memArena, "%.*s_%s.txt", StrPrint(levelName), (completed ? "completed" : "save"));
}
