/*
File:   game_board.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_BOARD_H
#define _GAME_BOARD_H

#define BOARD_WIDTH  9 //cells
#define BOARD_HEIGHT 9 //cells
#define GROUP_WIDTH  3 //cells
#define GROUP_HEIGHT 3 //cells

#define BOARD_MARGIN 16 //px
#define MIN_CELL_SIZE 10 //px
#define BOARD_OUTLINE_THICKNESS 3 //px
#define GROUP_DIVIDER_THICKNESS 2 //px
#define CELL_DIVIDER_THICKNESS  1 //px

enum CellFlag_t
{
	CellFlag_None = 0x00,
	
	CellFlag_IsGiven = 0x01,
	// CellFlag_Unused  = 0x02,
	// CellFlag_Unused  = 0x04,
	// CellFlag_Unused  = 0x08,
	// CellFlag_Unused  = 0x10,
	// CellFlag_Unused  = 0x20,
	// CellFlag_Unused  = 0x40,
	// CellFlag_Unused  = 0x80,
	
	CellFlag_NumFlags = 1,
};
const char* GetCellFlagStr(CellFlag_t enumValue)
{
	switch (enumValue)
	{
		case CellFlag_None:        return "None";
		case CellFlag_IsGiven:     return "IsGiven";
		default: return "Unknown";
	}
}

struct Cell_t
{
	v2i gridPos;
	u8 flags;
	u8 value;
	u16 notes;
	reci mainRec;
	reci innerRec;
};

struct Board_t
{
	reci mainRec;
	v2i cellSize;
	Cell_t cells[BOARD_WIDTH][BOARD_HEIGHT];
};

#endif //  _GAME_BOARD_H
