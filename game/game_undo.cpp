/*
File:   game_undo.cpp
Author: Taylor Robbins
Date:   01\02\2024
Description: 
	** Holds functions that help us manage a list of moves that constitute our
	** undoable actions performed by the player
*/

void InitMoveList(MoveList_t* list)
{
	NotNull(list);
	ClearPointer(list);
	//TODO: Anything to initialize here that's non-zero?
}

void PushMove(MoveList_t* list, Move_t move)
{
	NotNull(list);
	
	if (!move.isNote && !move.isClear && list->numMoves > 0 && list->prevMoveTime != 0 && TimeSince(list->prevMoveTime) <= UNDO_COMBINE_TIME)
	{
		Move_t prevMove = list->moves[0];
		if (!prevMove.isClear && prevMove.x == move.x && prevMove.y == move.y)
		{
			list->moves[0].wordValue = move.wordValue;
			list->prevMoveTime = ProgramTime;
			return;
		}
	}
	
	for (u8 mIndex = MAX_NUM_UNDOS-1; mIndex > 0; mIndex--)
	{
		list->moves[mIndex].wordValue = list->moves[mIndex-1].wordValue;
	}
	list->moves[0].wordValue = move.wordValue;
	if (list->numMoves < MAX_NUM_UNDOS) { list->numMoves++; }
	list->prevMoveTime = ProgramTime;
}

void PopMove(MoveList_t* list)
{
	Assert(list->numMoves > 0);
	for (u8 mIndex = 0; mIndex+1 < MAX_NUM_UNDOS; mIndex++)
	{
		list->moves[mIndex].wordValue = list->moves[mIndex + 1].wordValue;
	}
	Decrement(list->numMoves);
}
void PopMoves(MoveList_t* list, u8 numMoves)
{
	Assert(list->numMoves >= numMoves);
	for (u8 mIndex = 0; mIndex+numMoves < MAX_NUM_UNDOS; mIndex++)
	{
		list->moves[mIndex].wordValue = list->moves[mIndex + numMoves].wordValue;
	}
	DecrementBy(list->numMoves, numMoves);
}

Move_t GetLastMove(MoveList_t* list)
{
	Assert(list->numMoves > 0);
	return list->moves[0];
}

void DoMove(Board_t* board, Move_t move)
{
	Assert(move.x < BOARD_WIDTH);
	Assert(move.y < BOARD_HEIGHT);
	Cell_t* cell = GetCell(board, NewVec2i(move.x, move.y));
	if (move.isNote)
	{
		Assert(move.value > 0);
		u16 noteBitValue = (1 << (move.value-1));
		FlagToggle(cell->notes, noteBitValue);
	}
	else if (move.isClear)
	{
		cell->value = 0;
	}
	else
	{
		cell->value = move.value;
	}
}

void UndoMove(Board_t* board, Move_t move)
{
	Assert(move.x < BOARD_WIDTH);
	Assert(move.y < BOARD_HEIGHT);
	Cell_t* cell = GetCell(board, NewVec2i(move.x, move.y));
	if (move.isNote)
	{
		Assert(move.value > 0);
		u16 noteBitValue = (1 << (move.value-1));
		FlagToggle(cell->notes, noteBitValue);
	}
	else if (move.isClear)
	{
		cell->value = move.value;
	}
	else
	{
		cell->value = 0;
	}
}
