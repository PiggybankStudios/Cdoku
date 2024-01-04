/*
File:   game_undo.h
Author: Taylor Robbins
Date:   01\02\2024
*/

#ifndef _GAME_UNDO_H
#define _GAME_UNDO_H

union Move_t
{
	u16 wordValue;
	struct
	{
		u8 value:4;
		u8 x:4;
		u8 y:4;
		u8 isNote:1;
		u8 isClear:1;
		u8 autoInserted:1;
		u8 unused1:1;
	};
	struct
	{
		u16 mask:15;
		u8 unused2:1;
	};
};

struct MoveList_t
{
	u32 prevMoveTime;
	u8 numMoves;
	Move_t moves[MAX_NUM_UNDOS];
};

inline Move_t NewMove(u8 x, u8 y, u8 value, bool isNote, bool isClear, bool autoInserted)
{
	Move_t result = {};
	result.value = value;
	result.x = x;
	result.y = y;
	result.isNote = isNote;
	result.isClear = isClear;
	result.autoInserted = autoInserted;
	return result;
}
inline Move_t NewMove(u16 wordValue)
{
	Move_t result = {};
	result.wordValue = wordValue;
	return result;
}

#endif //  _GAME_UNDO_H
