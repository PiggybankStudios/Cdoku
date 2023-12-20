/*
File:   game_cursor.cpp
Author: Taylor Robbins
Date:   12\19\2023
Description: 
	** The cursor is how the player moves around the board and interacts with cells
*/

void InitCursor(Cursor_t* cursor, v2i startingPos, Board_t* board)
{
	NotNull(cursor);
	ClearPointer(cursor);
	cursor->gridPos = startingPos;
	Cell_t* cell = GetCell(board, cursor->gridPos);
	NotNull(cell);
	cursor->cellValue = cell->value;
	cursor->notePos = NewVec2i(1, 1);
}

Cell_t* GetSelectedCell(Board_t* board, Cursor_t* cursor)
{
	NotNull2(board, cursor);
	Cell_t* result = GetCell(board, cursor->gridPos);
	NotNull(result);
	return result;
}

void UpdateCursor(Cursor_t* cursor, Board_t* board)
{
	// +==============================+
	// |       Cursor Movement        |
	// +==============================+
	if (!cursor->makingNotes)
	{
		if (BtnPressed(Btn_Left))
		{
			HandleBtnExtended(Btn_Left);
			cursor->gridPos.x--;
			if (cursor->gridPos.x < 0) { cursor->gridPos.x = BOARD_WIDTH-1; }
		}
		if (BtnPressed(Btn_Right))
		{
			HandleBtnExtended(Btn_Right);
			cursor->gridPos.x++;
			if (cursor->gridPos.x >= BOARD_WIDTH) { cursor->gridPos.x = 0; }
		}
		if (BtnPressed(Btn_Up))
		{
			HandleBtnExtended(Btn_Up);
			cursor->gridPos.y--;
			if (cursor->gridPos.y < 0) { cursor->gridPos.y = BOARD_HEIGHT-1; }
		}
		if (BtnPressed(Btn_Down))
		{
			HandleBtnExtended(Btn_Down);
			cursor->gridPos.y++;
			if (cursor->gridPos.y >= BOARD_HEIGHT) { cursor->gridPos.y = 0; }
		}
	}
	
	// +==============================+
	// |    Cursor Notes Movement     |
	// +==============================+
	if (cursor->makingNotes)
	{
		if (BtnPressed(Btn_Left))
		{
			HandleBtnExtended(Btn_Left);
			cursor->notePos.x--;
			if (cursor->notePos.x < 0) { cursor->notePos.x = 3-1; }
		}
		if (BtnPressed(Btn_Right))
		{
			HandleBtnExtended(Btn_Right);
			cursor->notePos.x++;
			if (cursor->notePos.x >= 3) { cursor->notePos.x = 0; }
		}
		if (BtnPressed(Btn_Up))
		{
			HandleBtnExtended(Btn_Up);
			cursor->notePos.y--;
			if (cursor->notePos.y < 0) { cursor->notePos.y = 3-1; }
		}
		if (BtnPressed(Btn_Down))
		{
			HandleBtnExtended(Btn_Down);
			cursor->notePos.y++;
			if (cursor->notePos.y >= 3) { cursor->notePos.y = 0; }
		}
	}
	
	Cell_t* selectedCell = GetSelectedCell(board, cursor);
	
	// +======================================+
	// | Btn_A Places Numbers or Makes Notes  |
	// +======================================+
	//TODO: Add support for holding Btn_A to move quickly
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		if (cursor->makingNotes)
		{
			u16 noteFlag = (1 << (cursor->notePos.x + (cursor->notePos.y * 3)));
			FlagToggle(selectedCell->notes, noteFlag);
		}
		else
		{
			if (!IsFlagSet(selectedCell->flags, CellFlag_IsGiven))
			{
				selectedCell->value++;
				if (selectedCell->value > 9) { selectedCell->value = 0; }
			}
		}
	}
	
	// +==========================================+
	// | Btn_B Toggles Notes Mode or Clears Cell  |
	// +==========================================+
	//TODO: Add support for holding Btn_A to move quickly
	if (BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		if (cursor->makingNotes)
		{
			cursor->makingNotes = false;
		}
		else
		{
			if (!IsFlagSet(selectedCell->flags, CellFlag_IsGiven))
			{
				if (selectedCell->value == 0)
				{
					cursor->makingNotes = true;
				}
				else
				{
					selectedCell->value = 0;
				}
			}
		}
	}
}

void RenderCursor(Cursor_t* cursor, Board_t* board)
{
	Cell_t* selectedCell = GetSelectedCell(board, cursor);
	
	if (cursor->makingNotes)
	{
		v2i noteSize = NewVec2i(selectedCell->mainRec.width / 3, selectedCell->mainRec.height / 3);
		reci noteRec = NewReci(
			selectedCell->innerRec.x + noteSize.width * cursor->notePos.x,
			selectedCell->innerRec.y + noteSize.height * cursor->notePos.y,
			noteSize
		);
		
		u16 noteFlag = (1 << (cursor->notePos.x + (cursor->notePos.y * 3)));
		
		if (IsFlagSet(selectedCell->notes, noteFlag))
		{
			LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
			PdDrawRec(noteRec, kColorBlack);
			PdSetDrawMode(oldDrawMode);
		}
		else
		{
			PdDrawRecOutline(noteRec, 1, false, kColorBlack);
		}
	}
	else
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdDrawRec(selectedCell->innerRec, kColorBlack);
		PdSetDrawMode(oldDrawMode);
	}
}
