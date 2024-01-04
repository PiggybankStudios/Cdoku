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

void CursorAddPrevPosition(Cursor_t* cursor, v2i gridPos, Dir2_t moveDir)
{
	for (u64 pIndex = 0; pIndex < CURSOR_MAX_PREV_POSITIONS; pIndex++)
	{
		CursorPrevPos_t* prevPos = &cursor->prevPositions[pIndex];
		if (prevPos->animProgress == 0.0f)
		{
			prevPos->gridPos = gridPos;
			prevPos->moveDir = moveDir;
			prevPos->animProgress = 1.0f;
			break;
		}
	}
}

void CursorAddUndoAnim(Cursor_t* cursor, v2i gridPos)
{
	for (u64 uIndex = 0; uIndex < CURSOR_MAX_UNDO_ANIMS; uIndex++)
	{
		UndoAnim_t* undoAnim = &cursor->undoAnims[uIndex];
		if (undoAnim->animProgress == 0.0f)
		{
			undoAnim->gridPos = gridPos;
			undoAnim->animProgress = 1.0f;
			break;
		}
	}
}

bool UpdateCursor(Cursor_t* cursor, Board_t* board, MoveList_t* history)
{
	bool boardChanged = false;
	
	// +==================================+
	// | Update Prev Position Animations  |
	// +==================================+
	for (u64 pIndex = 0; pIndex < CURSOR_MAX_PREV_POSITIONS; pIndex++)
	{
		CursorPrevPos_t* prevPos = &cursor->prevPositions[pIndex];
		if (prevPos->animProgress > 0.0f)
		{
			UpdateAnimationDown(&prevPos->animProgress, CURSOR_PREV_POS_ANIM_TIME);
			game->screenIsDirty = true;
			if (prevPos->animProgress > 0.0f) { game->nextUpdateIsDirty = true; }
		}
	}
	if (cursor->moveAnimProgress > 0.0f)
	{
		UpdateAnimationDown(&cursor->moveAnimProgress, CURSOR_PREV_POS_ANIM_TIME);
		game->screenIsDirty = true;
		if (cursor->moveAnimProgress > 0.0f) { game->nextUpdateIsDirty = true; }
	}
	if (cursor->makingNotesAnimProgress > 0.0f)
	{
		UpdateAnimationDown(&cursor->makingNotesAnimProgress, CURSOR_NOTE_TAKING_ANIM_TIME);
		game->screenIsDirty = true;
		if (cursor->makingNotesAnimProgress > 0.0f) { game->nextUpdateIsDirty = true; }
	}
	
	// +==============================+
	// |    Update Undo Animations    |
	// +==============================+
	for (u32 uIndex = 0; uIndex < CURSOR_MAX_UNDO_ANIMS; uIndex++)
	{
		UndoAnim_t* undoAnim = &cursor->undoAnims[uIndex];
		if (undoAnim->animProgress > 0.0f)
		{
			UpdateAnimationDown(&undoAnim->animProgress, CURSOR_UNDO_ANIM_TIME);
			game->screenIsDirty = true;
			if (undoAnim->animProgress > 0.0f) { game->nextUpdateIsDirty = true; }
		}
	}
	
	// +==============================+
	// |       Cursor Movement        |
	// +==============================+
	if (!cursor->makingNotes)
	{
		if (BtnPressed(Btn_Left))
		{
			HandleBtnExtended(Btn_Left);
			CursorAddPrevPosition(cursor, cursor->gridPos, Dir2_Left);
			cursor->moveAnimProgress = 1.0f;
			cursor->gridPos.x--;
			if (cursor->gridPos.x < 0) { cursor->gridPos.x = BOARD_WIDTH-1; }
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Right))
		{
			HandleBtnExtended(Btn_Right);
			CursorAddPrevPosition(cursor, cursor->gridPos, Dir2_Right);
			cursor->moveAnimProgress = 1.0f;
			cursor->gridPos.x++;
			if (cursor->gridPos.x >= BOARD_WIDTH) { cursor->gridPos.x = 0; }
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Up))
		{
			HandleBtnExtended(Btn_Up);
			CursorAddPrevPosition(cursor, cursor->gridPos, Dir2_Up);
			cursor->moveAnimProgress = 1.0f;
			cursor->gridPos.y--;
			if (cursor->gridPos.y < 0) { cursor->gridPos.y = BOARD_HEIGHT-1; }
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Down))
		{
			HandleBtnExtended(Btn_Down);
			CursorAddPrevPosition(cursor, cursor->gridPos, Dir2_Down);
			cursor->moveAnimProgress = 1.0f;
			cursor->gridPos.y++;
			if (cursor->gridPos.y >= BOARD_HEIGHT) { cursor->gridPos.y = 0; }
			game->screenIsDirty = true;
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
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Right))
		{
			HandleBtnExtended(Btn_Right);
			cursor->notePos.x++;
			if (cursor->notePos.x >= 3) { cursor->notePos.x = 0; }
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Up))
		{
			HandleBtnExtended(Btn_Up);
			cursor->notePos.y--;
			if (cursor->notePos.y < 0) { cursor->notePos.y = 3-1; }
			game->screenIsDirty = true;
		}
		if (BtnPressed(Btn_Down))
		{
			HandleBtnExtended(Btn_Down);
			cursor->notePos.y++;
			if (cursor->notePos.y >= 3) { cursor->notePos.y = 0; }
			game->screenIsDirty = true;
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
		game->screenIsDirty = true;
		if (cursor->makingNotes)
		{
			u16 noteFlag = (1 << (cursor->notePos.x + (cursor->notePos.y * 3)));
			FlagToggle(selectedCell->notes, noteFlag);
			if (game->undoIndex > 0) { PopMoves(history, game->undoIndex); game->undoIndex = 0; }
			PushMove(history, 0, NewMove(
				(u8)cursor->gridPos.x,
				(u8)cursor->gridPos.y,
				(u8)(1 + cursor->notePos.x + cursor->notePos.y*3),
				true,
				false,
				false
			));
			boardChanged = true;
		}
		else
		{
			if (!IsFlagSet(selectedCell->flags, CellFlag_IsGiven))
			{
				u8 oldValue = selectedCell->value;
				selectedCell->value++;
				if (selectedCell->value > 9) { selectedCell->value = 0; }
				if (game->undoIndex > 0) { PopMoves(history, game->undoIndex); game->undoIndex = 0; }
				PushMove(history, oldValue, NewMove(
					(u8)cursor->gridPos.x,
					(u8)cursor->gridPos.y,
					((selectedCell->value == 0) ? oldValue : selectedCell->value),
					false,
					(selectedCell->value == 0),
					false
				));
				boardChanged = true;
			}
		}
	}
	
	// +==================================================+
	// | Tapping Btn_B Toggles Notes Mode or Clears Cell  |
	// +==================================================+
	//TODO: Add support for holding Btn_A to move quickly
	if (BtnTapped(Btn_B, UNDO_BTN_HOLD_TIME))
	{
		HandleBtn(Btn_B);
		game->screenIsDirty = true;
		if (cursor->makingNotes)
		{
			cursor->makingNotes = false;
			cursor->makingNotesAnimProgress = 1.0f;
		}
		else
		{
			if (!IsFlagSet(selectedCell->flags, CellFlag_IsGiven))
			{
				if (selectedCell->value == 0)
				{
					cursor->makingNotes = true;
					cursor->makingNotesAnimProgress = 1.0f;
				}
				else
				{
					u8 oldValue = selectedCell->value;
					selectedCell->value = 0;
					if (game->undoIndex > 0) { PopMoves(history, game->undoIndex); game->undoIndex = 0; }
					PushMove(history, oldValue, NewMove(
						(u8)cursor->gridPos.x,
						(u8)cursor->gridPos.y,
						oldValue,
						false,
						true,
						false
					));
					boardChanged = true;
				}
			}
		}
	}
	
	// +==============================+
	// |     Holding Btn_B Undoes     |
	// +==============================+
	u32 holdTime = UNDO_BTN_HOLD_TIME;
	if (TimeSince(input->btnStates[Btn_B].lastTransitionTime) >= UNDO_BTN_HOLD_TIME + UNDO_BTN_FIRST_REPEAT_TIME) { holdTime = UNDO_BTN_REPEAT_TIME; }
	else if (TimeSince(input->btnStates[Btn_B].lastTransitionTime) >= UNDO_BTN_HOLD_TIME) { holdTime = UNDO_BTN_FIRST_REPEAT_TIME; }
	if (BtnHeld(Btn_B, holdTime))
	{
		HandleBtn(Btn_B);
		if (game->history.numMoves > game->undoIndex)
		{
			Move_t prevMove = game->history.moves[game->undoIndex];
			PrintLine_D("Undoing[%u] %s %u at (%u, %u)",
				(u32)game->undoIndex,
				(prevMove.isNote ? "Note" : (prevMove.isClear ? "Clear" : "Place")),
				(u32)prevMove.value,
				(u32)prevMove.x, (u32)prevMove.y
			);
			UndoMove(&game->board, prevMove);
			CursorAddUndoAnim(cursor, NewVec2i(prevMove.x, prevMove.y));
			game->undoIndex++;
			// Silently undo any "autoInserted" moves that the player doesn't know about, that happened before the prevMove
			while (game->history.numMoves > game->undoIndex && game->history.moves[game->undoIndex].autoInserted)
			{
				Move_t autoMove = game->history.moves[game->undoIndex];
				UndoMove(&game->board, autoMove);
				CursorAddUndoAnim(cursor, NewVec2i(autoMove.x, autoMove.y));
				game->undoIndex++;
			}
			if (cursor->makingNotes)
			{
				cursor->makingNotes = false;
				cursor->makingNotesAnimProgress = 1.0f;
			}
			game->screenIsDirty = true;
			boardChanged = true;
		}
	}
	
	return boardChanged;
}

void RenderCursor(Cursor_t* cursor, Board_t* board)
{
	Cell_t* selectedCell = GetSelectedCell(board, cursor);
	
	// +==============================+
	// |  Render Previous Positions   |
	// +==============================+
	for (u64 pIndex = 0; pIndex < CURSOR_MAX_PREV_POSITIONS; pIndex++)
	{
		CursorPrevPos_t* prevPos = &cursor->prevPositions[pIndex];
		if (prevPos->animProgress > 0.0f)
		{
			Cell_t* prevCell = GetCell(board, prevPos->gridPos);
			reci drawRec = prevCell->innerRec;
			switch (prevPos->moveDir)
			{
				case Dir2_Left:  drawRec = ReciExpandRight(drawRec, -RoundR32i((r32)drawRec.width  * EaseCubicIn(1-prevPos->animProgress))); drawRec = ReciDeflateY(drawRec, RoundR32i(((r32)drawRec.height/2) * (1-prevPos->animProgress))); break;
				case Dir2_Right: drawRec = ReciExpandLeft(drawRec,  -RoundR32i((r32)drawRec.width  * EaseCubicIn(1-prevPos->animProgress))); drawRec = ReciDeflateY(drawRec, RoundR32i(((r32)drawRec.height/2) * (1-prevPos->animProgress))); break;
				case Dir2_Up:    drawRec = ReciExpandDown(drawRec,  -RoundR32i((r32)drawRec.height * EaseCubicIn(1-prevPos->animProgress))); drawRec = ReciDeflateX(drawRec, RoundR32i(((r32)drawRec.width/2) * (1-prevPos->animProgress))); break;
				case Dir2_Down:  drawRec = ReciExpandUp(drawRec,    -RoundR32i((r32)drawRec.height * EaseCubicIn(1-prevPos->animProgress))); drawRec = ReciDeflateX(drawRec, RoundR32i(((r32)drawRec.width/2) * (1-prevPos->animProgress))); break;
			}
			LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
			PdDrawRec(drawRec, kColorBlack);
			PdSetDrawMode(oldDrawMode);
		}
	}
	
	// +==============================+
	// |     Render Notes Cursor      |
	// +==============================+
	if (cursor->makingNotes)
	{
		v2i noteSize = NewVec2i(selectedCell->mainRec.width / 3, selectedCell->mainRec.height / 3);
		reci noteRec = NewReci(
			selectedCell->innerRec.x + noteSize.width * cursor->notePos.x,
			selectedCell->innerRec.y + noteSize.height * cursor->notePos.y,
			noteSize
		);
		if (cursor->makingNotesAnimProgress > 0.0f)
		{
			noteRec.x      = RoundR32i(LerpR32((r32)noteRec.x,      (r32)selectedCell->innerRec.x,      EaseQuadraticIn(cursor->makingNotesAnimProgress)));
			noteRec.y      = RoundR32i(LerpR32((r32)noteRec.y,      (r32)selectedCell->innerRec.y,      EaseQuadraticIn(cursor->makingNotesAnimProgress)));
			noteRec.width  = RoundR32i(LerpR32((r32)noteRec.width,  (r32)selectedCell->innerRec.width,  EaseQuadraticIn(cursor->makingNotesAnimProgress)));
			noteRec.height = RoundR32i(LerpR32((r32)noteRec.height, (r32)selectedCell->innerRec.height, EaseQuadraticIn(cursor->makingNotesAnimProgress)));
		}
		
		u16 noteFlag = (1 << (cursor->notePos.x + (cursor->notePos.y * 3)));
		
		if (IsFlagSet(selectedCell->notes, noteFlag) || cursor->makingNotesAnimProgress > 0.0f)
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
	// +==============================+
	// |      Render Main Cursor      |
	// +==============================+
	else
	{
		reci cursorRec = selectedCell->innerRec;
		if (cursor->moveAnimProgress > 0.0f)
		{
			cursorRec = ReciDeflate(
				cursorRec,
				RoundR32i(((r32)cursorRec.width/2) * EaseCubicIn(cursor->moveAnimProgress)),
				RoundR32i(((r32)cursorRec.height/2) * EaseCubicIn(cursor->moveAnimProgress))
			);
		}
		else if (cursor->makingNotesAnimProgress > 0.0f)
		{
			v2i noteSize = NewVec2i(selectedCell->mainRec.width / 3, selectedCell->mainRec.height / 3);
			reci noteRec = NewReci(
				selectedCell->innerRec.x + noteSize.width * cursor->notePos.x,
				selectedCell->innerRec.y + noteSize.height * cursor->notePos.y,
				noteSize
			);
			if (cursor->makingNotesAnimProgress > 0.0f)
			{
				cursorRec.x      = RoundR32i(LerpR32((r32)cursorRec.x,      (r32)noteRec.x,      EaseQuadraticIn(cursor->makingNotesAnimProgress)));
				cursorRec.y      = RoundR32i(LerpR32((r32)cursorRec.y,      (r32)noteRec.y,      EaseQuadraticIn(cursor->makingNotesAnimProgress)));
				cursorRec.width  = RoundR32i(LerpR32((r32)cursorRec.width,  (r32)noteRec.width,  EaseQuadraticIn(cursor->makingNotesAnimProgress)));
				cursorRec.height = RoundR32i(LerpR32((r32)cursorRec.height, (r32)noteRec.height, EaseQuadraticIn(cursor->makingNotesAnimProgress)));
			}
		}
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		PdDrawRec(cursorRec, kColorBlack);
		PdSetDrawMode(oldDrawMode);
	}
	
	// +==============================+
	// |      Render Undo Anims       |
	// +==============================+
	for (u32 uIndex = 0; uIndex < CURSOR_MAX_UNDO_ANIMS; uIndex++)
	{
		UndoAnim_t* undoAnim = &cursor->undoAnims[uIndex];
		if (undoAnim->animProgress > 0.0f)
		{
			Cell_t* cell = GetCell(board, undoAnim->gridPos);
			NotNull(cell);
			reci drawRec = NewReci(
				cell->innerRec.x + cell->innerRec.width/2 - game->undoAnimSheet.frameSize.width/2,
				cell->innerRec.y + cell->innerRec.height/2 - game->undoAnimSheet.frameSize.height/2,
				game->undoAnimSheet.frameSize
			);
			i32 frameIndex = (i32)LerpR32(0, (r32)game->undoAnimSheet.numFramesX, 1 - undoAnim->animProgress);
			if (frameIndex >= game->undoAnimSheet.numFramesX) { frameIndex = game->undoAnimSheet.numFramesX-1; }
			v2i animFrame = NewVec2i(frameIndex, 0);
			reci oldClipRec = PdAddClipRec(cell->innerRec);
			// LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeXOR);
			PdDrawSheetFrame(game->undoAnimSheet, animFrame, drawRec);
			// PdDrawRec(drawRec, kColorBlack);
			// PdSetDrawMode(oldDrawMode);
			PdSetClipRec(oldClipRec);
		}
	}
}
