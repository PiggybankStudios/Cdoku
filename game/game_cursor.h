/*
File:   game_cursor.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_CURSOR_H
#define _GAME_CURSOR_H

#define CURSOR_OUTLINE_THICKNESS  1
#define CURSOR_INNER_MARGIN       1
#define CURSOR_MAX_PREV_POSITIONS 4
#define CURSOR_PREV_POS_ANIM_TIME    133.333333f //ms
#define CURSOR_NOTE_TAKING_ANIM_TIME  66.666666f //ms

struct CursorPrevPos_t
{
	v2i gridPos;
	Dir2_t moveDir;
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
};

#endif //  _GAME_CURSOR_H
