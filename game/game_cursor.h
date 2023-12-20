/*
File:   game_cursor.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_CURSOR_H
#define _GAME_CURSOR_H

#define CURSOR_OUTLINE_THICKNESS 1
#define CURSOR_INNER_MARGIN      1

struct Cursor_t
{
	v2i gridPos;
	u8 cellValue;
	bool makingNotes;
	v2i notePos;
};

#endif //  _GAME_CURSOR_H
