/*
File:   game_board.cpp
Author: Taylor Robbins
Date:   12\19\2023
Description: 
	** Holds logic pertaining to interactions with the Sudoku board and it's cells
*/

Cell_t* GetCell(Board_t* board, v2i gridPos)
{
	NotNull(board);
	if (gridPos.x < 0 || gridPos.x >= BOARD_WIDTH ||
		gridPos.y < 0 || gridPos.y >= BOARD_HEIGHT)
	{
		return nullptr;
	}
	
	return &board->cells[gridPos.x][gridPos.y];
}

void InitBoard(Board_t* board, MyStr_t initialState)
{
	NotNull(board);
	ClearPointer(board);
	
	u64 cIndex = 0;
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			while (cIndex < initialState.length && (initialState.chars[cIndex] == '\r' || initialState.chars[cIndex] == '\n')) { cIndex++; }
			char c = (cIndex < initialState.length ? initialState.chars[cIndex] : ' ');
			
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(board, gridPos);
			cell->gridPos = gridPos;
			if (c >= '0' && c <= '9')
			{
				cell->value = (u8)(c - '0');
				FlagSet(cell->flags, CellFlag_IsGiven);
			}
			cIndex++;
		}
	}
}

void LoadSaveInfo(Board_t* board, MyStr_t saveInfo)
{
	u64 cIndex = 0;
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(board, gridPos);
			char c = (cIndex < saveInfo.length ? saveInfo.chars[cIndex] : ' ');
			
			if (c >= '1' && c <= '9' && !IsFlagSet(cell->flags, CellFlag_IsGiven))
			{
				cell->value = (u8)(c - '0');
			}
			
			if (c != '\n' && c != '\r' && cIndex < saveInfo.length) { cIndex++; }
		}
	}
	
	if (cIndex < saveInfo.length && saveInfo.chars[cIndex] == '\r') { cIndex++; }
	if (cIndex < saveInfo.length && saveInfo.chars[cIndex] == '\n') { cIndex++; }
	
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(board, gridPos);
			cell->notes = 0x0000;
			
			for (i32 noteIndex = 0; noteIndex < 9; noteIndex++)
			{
				char c = (cIndex < saveInfo.length ? saveInfo.chars[cIndex] : ' ');
				if (c == '1' + noteIndex)
				{
					FlagSet(cell->notes, (1 << noteIndex));
				}
				if (c != '\n' && c != '\r' && cIndex < saveInfo.length) { cIndex++; }
			}
			
			if (cIndex < saveInfo.length && saveInfo.chars[cIndex] == '\r') { cIndex++; }
			if (cIndex < saveInfo.length && saveInfo.chars[cIndex] == '\n') { cIndex++; }
		}
	}
}

void BoardLayoutUi(Board_t* board, reci availableRec)
{
	NotNull(board);
	board->mainRec.width = MinI32(availableRec.width, availableRec.height);
	if (board->mainRec.width >= 9 * MIN_CELL_SIZE + BOARD_MARGIN*2) { board->mainRec.width -= BOARD_MARGIN*2; }
	board->mainRec.height = board->mainRec.width;
	board->mainRec.x = availableRec.x + availableRec.width/2 - board->mainRec.width/2;
	board->mainRec.y = availableRec.y + availableRec.height/2 - board->mainRec.height/2;
	//TODO: Should we make sure the mainRec width/height is divisible by 9?
	
	board->cellSize = NewVec2i(board->mainRec.width / BOARD_WIDTH, board->mainRec.height / BOARD_HEIGHT);
	
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(board, gridPos);
			cell->mainRec = NewReci(
				board->mainRec.x + (board->cellSize.width * xPos),
				board->mainRec.y + (board->cellSize.height * yPos),
				board->cellSize
			);
			cell->innerRec = NewReci(
				cell->mainRec.x      + ((xPos > 0) ? CELL_DIVIDER_THICKNESS : 0),
				cell->mainRec.y      + ((yPos > 0) ? CELL_DIVIDER_THICKNESS : 0),
				cell->mainRec.width  - ((xPos > 0) ? CELL_DIVIDER_THICKNESS : 0),
				cell->mainRec.height - ((yPos > 0) ? CELL_DIVIDER_THICKNESS : 0)
			);
			if ((xPos % 3) == 2 && xPos < BOARD_WIDTH-1)  { cell->innerRec.width  -= ((GROUP_DIVIDER_THICKNESS+1) / 2); }
			if ((yPos % 3) == 2 && yPos < BOARD_HEIGHT-1) { cell->innerRec.height -= ((GROUP_DIVIDER_THICKNESS+1) / 2); }
			if (xPos == BOARD_WIDTH-1)  { cell->innerRec.width  += 1; }
			if (yPos == BOARD_HEIGHT-1) { cell->innerRec.height += 1; }
		}
	}
}

void RenderBoard(Board_t* board, Cursor_t* cursor)
{
	PdDrawRec(board->mainRec, kColorWhite);
	PdDrawRecOutline(board->mainRec, BOARD_OUTLINE_THICKNESS, true, kColorBlack);
	Cell_t* selectedCell = GetCell(board, cursor->gridPos);
	NotNull(selectedCell);
	
	// +==============================+
	// | Render Cell Divider Columns  |
	// +==============================+
	for (i32 column = 1; column < BOARD_WIDTH; column++)
	{
		bool isGroupDivider = ((column % GROUP_WIDTH) == 0);
		i32 thickness = (isGroupDivider ? GROUP_DIVIDER_THICKNESS : CELL_DIVIDER_THICKNESS);
		PdDrawRec(NewReci(
			board->mainRec.x + (board->cellSize.width * column) - (thickness/2),
			board->mainRec.y,
			thickness,
			board->mainRec.height
			),
			kColorBlack
		);
	}
	
	// +==============================+
	// |   Render Cell Divider Rows   |
	// +==============================+
	for (i32 row = 1; row < BOARD_HEIGHT; row++)
	{
		bool isGroupDivider = ((row % GROUP_WIDTH) == 0);
		i32 thickness = (isGroupDivider ? GROUP_DIVIDER_THICKNESS : CELL_DIVIDER_THICKNESS);
		PdDrawRec(NewReci(
			board->mainRec.x,
			board->mainRec.y + (board->cellSize.height * row) - (thickness/2),
			board->mainRec.width,
			thickness
			),
			kColorBlack
		);
	}
	
	
	// +==============================+
	// |        Render Numbers        |
	// +==============================+
	for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
	{
		for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
		{
			v2i gridPos = NewVec2i(xPos, yPos);
			Cell_t* cell = GetCell(board, gridPos);
			
			if (cell->value > 0)
			{
				Assert(cell->value <= 9);
				
				bool foundConflict = false;
				if (!foundConflict)
				{
					for (i32 xOffset = 0; xOffset < BOARD_WIDTH; xOffset++)
					{
						if (xOffset != xPos)
						{
							Cell_t* otherCell = GetCell(board, NewVec2i(xOffset, yPos));
							if (otherCell->value == cell->value)
							{
								foundConflict = true;
								break;
							}
						}
					}
				}
				if (!foundConflict)
				{
					for (i32 yOffset = 0; yOffset < BOARD_HEIGHT; yOffset++)
					{
						if (yOffset != yPos)
						{
							Cell_t* otherCell = GetCell(board, NewVec2i(xPos, yOffset));
							if (otherCell->value == cell->value)
							{
								foundConflict = true;
								break;
							}
						}
					}
				}
				if (!foundConflict)
				{
					v2i groupBase = NewVec2i(xPos - (xPos % GROUP_WIDTH), yPos - (yPos % GROUP_HEIGHT));
					for (i32 yOffset = 0; yOffset < GROUP_HEIGHT; yOffset++)
					{
						for (i32 xOffset = 0; xOffset < GROUP_WIDTH; xOffset++)
						{
							if (gridPos != groupBase + NewVec2i(xOffset, yOffset))
							{
								Cell_t* otherCell = GetCell(board, groupBase + NewVec2i(xOffset, yOffset));
								if (otherCell->value == cell->value)
								{
									foundConflict = true;
									break;
								}
							}
						}
						if (foundConflict) { break; }
					}
				}
				
				if (foundConflict)
				{
					PdDrawTexturedRecPart(game->errorTexture, cell->innerRec, NewReci(0, 0, cell->innerRec.size));
				}
				else if (cell->value == selectedCell->value && cell->gridPos != selectedCell->gridPos)
				{
					PdDrawTexturedRecPart(game->ditherTexture, cell->innerRec, NewReci(0, 0, cell->innerRec.size));
				}
				
				reci numberRec = NewReci(
					cell->mainRec.x + cell->mainRec.width/2 - game->numbersSheet.frameSize.width/2,
					cell->mainRec.y + cell->mainRec.height/2 - game->numbersSheet.frameSize.height/2,
					game->numbersSheet.frameSize
				);
				v2i numberFrame = NewVec2i((i32)((cell->value-1) % 3), (i32)((cell->value-1) / 3));
				if (IsFlagSet(cell->flags, CellFlag_IsGiven)) { numberFrame.y += 3; }
				PdDrawSheetFrame(game->numbersSheet, numberFrame, numberRec);
			}
			else
			{
				for (i32 noteY = 0; noteY < 3; noteY++)
				{
					for (i32 noteX = 0; noteX < 3; noteX++)
					{
						u16 noteFlag = (1 << (noteX + (noteY * 3)));
						if (IsFlagSet(cell->notes, noteFlag))
						{
							v2i noteSize = NewVec2i(cell->mainRec.width / 3, cell->mainRec.height / 3);
							reci noteRec = NewReci(
								cell->innerRec.x + noteSize.width * noteX + noteSize.width/2 - game->notesSheet.frameSize.width/2,
								cell->innerRec.y + noteSize.height * noteY + noteSize.height/2 - game->notesSheet.frameSize.height/2,
								game->notesSheet.frameSize
							);
							v2i noteFrame = NewVec2i(noteX, noteY);
							PdDrawSheetFrame(game->notesSheet, noteFrame, noteRec);
						}
					}
				}
			}
		}
	}
}
