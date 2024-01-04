/*
File:   game_cursor.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_CURSOR_H
#define _GAME_CURSOR_H

struct CursorPrevPos_t
{
	v2i gridPos;
	Dir2_t moveDir;
	r32 animProgress;
};

struct UndoAnim_t
{
	v2i gridPos;
	r32 animProgress;
};

struct Cursor_t
{
	v2i gridPos;
	u8 cellValue;
	bool makingNotes;
	v2i notePos;
	CursorPrevPos_t prevPositions[CURSOR_MAX_PREV_POSITIONS];
	r32 moveAnimProgress;
	r32 makingNotesAnimProgress;
	UndoAnim_t undoAnims[CURSOR_MAX_UNDO_ANIMS];
};

#endif //  _GAME_CURSOR_H
