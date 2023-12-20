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
	
	if (initialState.length == BOARD_WIDTH * BOARD_HEIGHT)
	{
		u64 cIndex = 0;
		for (i32 yPos = 0; yPos < BOARD_HEIGHT; yPos++)
		{
			for (i32 xPos = 0; xPos < BOARD_WIDTH; xPos++)
			{
				v2i gridPos = NewVec2i(xPos, yPos);
				Cell_t* cell = GetCell(board, gridPos);
				char c = initialState.chars[cIndex];
				if (c >= '0' && c <= '9')
				{
					cell->value = (u8)(c - '0');
					FlagSet(cell->flags, CellFlag_IsGiven);
				}
				cIndex++;
			}
		}
	}
	else { Assert(initialState.length == 0); }
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
		}
	}
}

void RenderBoard(Board_t* board, Cursor_t* cursor)
{
	PdDrawRec(board->mainRec, kColorWhite);
	PdDrawRecOutline(board->mainRec, BOARD_OUTLINE_THICKNESS, true, kColorBlack);
	
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
