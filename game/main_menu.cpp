/*
File:   main_menu.cpp
Author: Taylor Robbins
Date:   12\19\2023
Description: 
	** Holds the AppState that runs the Main Menu (where you can choose to play the game, change settings, or exit )
*/

MainMenuState_t* mmenu = nullptr;

// +--------------------------------------------------------------+
// |                           Helpers                            |
// +--------------------------------------------------------------+
void ClearButtonsMainMenu()
{
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		FreeString(mainHeap, &button->displayText);
		FreeString(mainHeap, &button->referencePath);
	}
	VarArrayClear(&mmenu->buttons);
}
MMenuBtn_t* AddButtonMainMenu(MMenuAction_t action, MyStr_t displayText, MyStr_t referencePath = MyStr_Empty_Const)
{
	MMenuBtn_t* result = VarArrayAdd(&mmenu->buttons, MMenuBtn_t);
	NotNull(result);
	ClearPointer(result);
	result->action = action;
	result->displayText = AllocString(mainHeap, &displayText);
	result->referencePath = AllocString(mainHeap, &referencePath);
	NotNullStr(&result->displayText);
	Font_t* font = ((action == MMenuAction_Level) ? &mmenu->levelBtnFont : &mmenu->buttonFont);
	result->displayTextSize = MeasureText(font->font, result->displayText);
	result->mainRec.size = result->displayTextSize;
	if (action == MMenuAction_Level)
	{
		result->mainRec.size += NewVec2i(2*MMENU_LEVEL_BTN_HORIZONTAL_PADDING, 2*MMENU_LEVEL_BTN_VERTICAL_PADDING);
	}
	else
	{
		result->mainRec.size += NewVec2i(2*MMENU_BTN_HORIZONTAL_PADDING, 2*MMENU_BTN_VERTICAL_PADDING);
	}
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		if (button->mainRec.width > result->mainRec.width) { result->mainRec.width = button->mainRec.width; }
		else if (button->mainRec.width < result->mainRec.width) { button->mainRec.width = result->mainRec.width; }
		button->displayTextPos = NewVec2i(button->mainRec.width/2 - button->displayTextSize.width/2, button->mainRec.height/2 - button->displayTextSize.height/2);
	}
	return result;
}
void MainMenuGenerateButtons(MMenuSubMenu_t subMenu)
{
	ClearButtonsMainMenu();
	mmenu->numSaveFilesUnchecked = 0;
	u8 totalTimer = StartPerfTime();
	
	switch (subMenu)
	{
		case MMenuSubMenu_None:
		{
			AddButtonMainMenu(MMenuAction_Play,     NewStr("PUZZLES"));
			AddButtonMainMenu(MMenuAction_Settings, NewStr("SETTINGS"));
			// AddButtonMainMenu(MMenuAction_Exit,     NewStr("EXIT"));
		} break;
		
		case MMenuSubMenu_LevelSelect:
		{
			MemArena_t* scratch = GetScratchArena();
			
			u8 getFilesTimer = StartPerfTime();
			FilesInDir_t levelFiles = GetFilesInDirectory(NewStr("Resources/Text"), scratch, false);
			u64 getFilesTime = EndPerfTime(getFilesTimer);
			if (levelFiles.success)
			{
				PrintLine_D("There are %llu levels! (time=" PERF_FORMAT_STR " allocation=%llub)", levelFiles.count, PERF_FORMAT(getFilesTime), levelFiles.mainAllocation.length);
				#if 0
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PrintLine_D("[%llu]: \"%.*s\"", fIndex, StrPrint(levelFiles.paths[fIndex]));
				}
				#endif
				
				u8 genFileBtnsTimer = StartPerfTime();
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PushMemMark(scratch);
					MyStr_t levelPath = levelFiles.paths[fIndex];
					MyStr_t levelName = GetFileNamePart(levelPath, false);
					MyStr_t displayText = PrintInArenaStr(scratch, "%.*s", StrPrint(levelName));
					for (u64 cIndex = 0; cIndex < displayText.length; cIndex++) { displayText.chars[cIndex] = GetUppercaseAnsiiChar(displayText.chars[cIndex]); }
					MMenuBtn_t* newBtn = AddButtonMainMenu(MMenuAction_Level, displayText, levelPath);
					newBtn->checkedForSaveFiles = false;
					mmenu->numSaveFilesUnchecked++;
					PopMemMark(scratch);
				}
				u64 genFileBtnsTime = EndPerfTime(genFileBtnsTimer);
				PrintLine_D("File Btns generated (time=" PERF_FORMAT_STR ")", PERF_FORMAT(genFileBtnsTime));
			}
			else
			{
				WriteLine_E("Failed to enumerate level files!");
			}
			
			// AddButtonMainMenu(MMenuAction_Back, NewStr("BACK"));
			
			FreeScratchArena(scratch);
		} break;
		
		default: AddButtonMainMenu(MMenuAction_Exit, NewStr("EXIT")); break;
	}
	
	u64 totalTime = EndPerfTime(totalTimer);
	PrintLine_D("The %s sub-menu contains %llu buttons (" PERF_FORMAT_STR ")", GetMMenuSubMenuStr(subMenu), mmenu->buttons.length, PERF_FORMAT(totalTime));
}
void PlaceButtonsListMainMenu()
{
	v2i totalListSize = Vec2i_Zero;
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		if (bIndex > 0) { totalListSize.height += ((button->action == MMenuAction_Level) ? MMENU_LEVEL_BTN_SPACING : MMENU_BTN_SPACING); }
		button->mainRec.y = totalListSize.y;
		totalListSize.width = MaxI32(totalListSize.width, button->mainRec.width);
		totalListSize.height += button->mainRec.height;
	}
	mmenu->buttonListRec.size = totalListSize;
	mmenu->buttonListRec.x = ScreenSize.width/2 - mmenu->buttonListRec.width/2;
	if (mmenu->subMenu == MMenuSubMenu_None)
	{
		mmenu->buttonListRec.y = (ScreenSize.height + (mmenu->titleRec.y + mmenu->titleRec.height)) / 2;
	}
	else
	{
		mmenu->buttonListRec.y = ScreenSize.height / 2;
	}
	mmenu->buttonListRec.y = mmenu->buttonListRec.y - (mmenu->buttonListRec.height/2);
	if (mmenu->buttonListRec.y < MMENU_BTN_TOP_BOTTOM_MARGIN)
	{
		mmenu->buttonListRec.y = MMENU_BTN_TOP_BOTTOM_MARGIN;
		mmenu->scrollMax = (r32)((mmenu->buttonListRec.y + mmenu->buttonListRec.height + MMENU_BTN_TOP_BOTTOM_MARGIN) - ScreenSize.height);
		if (mmenu->scrollMax < 0) { mmenu->scrollMax = 0; }
	}
	else
	{
		mmenu->scrollMax = 0;
	}
}
void MainMenuGotoSubMenu(MMenuSubMenu_t newSubMenu)
{
	mmenu->oldSelectionIndices[mmenu->subMenu] = mmenu->selectionIndex;
	mmenu->subMenu = newSubMenu;
	MainMenuGenerateButtons(mmenu->subMenu);
	PlaceButtonsListMainMenu();
	mmenu->selectionIndex = mmenu->oldSelectionIndices[newSubMenu];
	if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex >= mmenu->buttons.length)
	{
		mmenu->selectionIndex = (mmenu->buttons.length > 0) ? ((i64)mmenu->buttons.length-1) : -1;
	}
	mmenu->jumpToSelection = true;
}

void MainMenuCheckForSaveFileForBtn(MMenuBtn_t* button)
{
	if (!button->checkedForSaveFiles)
	{
		MemArena_t* scratch = GetScratchArena();
		MyStr_t completedPath = GetLevelSaveFilePath(scratch, button->referencePath, true);
		MyStr_t savePath = GetLevelSaveFilePath(scratch, button->referencePath, false);
		button->isCompleted = DoesFileExist(true, completedPath);
		button->hasSaveFile = (button->isCompleted || DoesFileExist(true, savePath));
		button->checkedForSaveFiles = true;
		Decrement(mmenu->numSaveFilesUnchecked);
		FreeScratchArena(scratch);
	}
}

void MainMenuMoveSelectionUp()
{
	if (mmenu->buttons.length > 0)
	{
		if (mmenu->selectionIndex < 0)
		{
			mmenu->selectionIndex = 0;
		}
		else
		{
			mmenu->selectionIndex--;
			if (mmenu->selectionIndex < 0)
			{
				if (mmenu->buttons.length <= 15) { mmenu->selectionIndex = (i64)(mmenu->buttons.length-1); }
				else { mmenu->selectionIndex = 0; }
			}
		}
		//TODO: Play a sound effect
	}
	else { mmenu->selectionIndex = -1; }
}
void MainMenuMoveSelectionDown()
{
	if (mmenu->buttons.length > 0)
	{
		if (mmenu->selectionIndex < 0)
		{
			mmenu->selectionIndex = 0;
		}
		else
		{
			mmenu->selectionIndex++;
			if ((u64)mmenu->selectionIndex >= mmenu->buttons.length)
			{
				if (mmenu->buttons.length <= 15) { mmenu->selectionIndex = 0; }
				else { mmenu->selectionIndex = (i64)(mmenu->buttons.length-1); }
			}
		}
		//TODO: Play a sound effect
	}
	else { mmenu->selectionIndex = -1; }
}

void MainMenuRenderFancyBtn(reci drawRec)
{
	v2i cornerSize = mmenu->btnCornersSheet.frameSize;
	PdDrawSheetFrame(mmenu->btnCornersSheet, NewVec2i(0, 0), NewReci(drawRec.topLeft, cornerSize));
	PdDrawSheetFrame(mmenu->btnCornersSheet, NewVec2i(0, 1), NewReci(drawRec.x, drawRec.y + drawRec.height - cornerSize.height, cornerSize));
	PdDrawSheetFrame(mmenu->btnCornersSheet, NewVec2i(1, 0), NewReci(drawRec.x + drawRec.width - cornerSize.width, drawRec.y, cornerSize));
	PdDrawSheetFrame(mmenu->btnCornersSheet, NewVec2i(1, 1), NewReci(drawRec.x + drawRec.width - cornerSize.width, drawRec.y + drawRec.height - cornerSize.height, cornerSize));
	PdDrawRec(NewReci(drawRec.x + cornerSize.width, drawRec.y, drawRec.width - 2*cornerSize.width, drawRec.height), kColorBlack);
	PdDrawRec(NewReci(drawRec.x, drawRec.y + cornerSize.height, drawRec.width, drawRec.height - 2*cornerSize.height), kColorBlack);
}

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_MainMenu(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		mmenu->handSheet = LoadSpriteSheet(NewStr("Resources/Sheets/hand"), 1);
		Assert(mmenu->handSheet.isValid);
		
		mmenu->btnCornersSheet = LoadSpriteSheet(NewStr("Resources/Sheets/button_corners"), 2);
		Assert(mmenu->btnCornersSheet.isValid);
		
		mmenu->titleFont = LoadFont(NewStr(MMENU_TITLE_FONT_PATH));
		Assert(mmenu->titleFont.isValid);
		
		mmenu->buttonFont = LoadFont(NewStr(MMENU_BTN_FONT_PATH));
		Assert(mmenu->buttonFont.isValid);
		
		mmenu->levelBtnFont = LoadFont(NewStr(MMENU_LEVEL_BTN_FONT_PATH));
		Assert(mmenu->levelBtnFont.isValid);
		
		mmenu->titleSprite = LoadTexture(NewStr("Resources/Sprites/title_text"));
		Assert(mmenu->titleSprite.isValid);
		
		mmenu->bottomDitherSprite = LoadTexture(NewStr("Resources/Textures/title_bottom_dither"));
		Assert(mmenu->bottomDitherSprite.isValid);
		
		mmenu->titleRec.size = MeasureText(mmenu->titleFont.font, NewStr("C-DOKU"));
		// mmenu->titleRec.size = mmenu->titleSprite.size;
		mmenu->titleRec.topLeft = NewVec2i(ScreenSize.width/2 - mmenu->titleRec.size.width/2, ScreenSize.height/4 - mmenu->titleRec.size.height/2);
		
		mmenu->subMenu = MMenuSubMenu_None;
		
		CreateVarArray(&mmenu->buttons, mainHeap, sizeof(MMenuBtn_t));
		
		#if 0
		MemArena_t* scratch = GetScratchArena();
		FilesInDir_t baseFiles = GetFilesInDirectory(NewStr(""), scratch, true);
		PrintLine_D("There are %llu files in the base folder:", baseFiles.count);
		for (u64 fIndex = 0; fIndex < baseFiles.count; fIndex++)
		{
			u64 numFiles = 0;
			if (StrEndsWith(baseFiles.paths[fIndex], "/"))
			{
				FilesInDir_t innerFiles = GetFilesInDirectory(baseFiles.paths[fIndex], scratch, true);
				numFiles = innerFiles.count;
			}
			PrintLine_D("[%llu]: %.*s (%llu file%s)", fIndex, StrPrint(baseFiles.paths[fIndex]), numFiles, (numFiles == 0) ? "" : "s");
		}
		FreeScratchArena(scratch);
		#endif
		
		mmenu->selectionIndex = 0;
		MainMenuGenerateButtons(mmenu->subMenu);
		PlaceButtonsListMainMenu();
		
		mmenu->initialized = true;
	}
	else
	{
		// Upon returning to the level select screen from playing a level, we need to recheck which save files exist
		VarArrayLoop(&mmenu->buttons, bIndex)
		{
			VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
			if (button->action == MMenuAction_Level && button->checkedForSaveFiles)
			{
				button->checkedForSaveFiles = false;
				mmenu->numSaveFilesUnchecked++;
			}
		}
	}
	
	mmenu->jumpToSelection = true;
	
	mmenu->crankAngleRef = ToRadians32(input->crankAngle);
}

// +--------------------------------------------------------------+
// |                             Stop                             |
// +--------------------------------------------------------------+
void StopAppState_MainMenu(bool deinitialize, AppState_t nextState)
{
	if (deinitialize)
	{
		//TODO: Free mmenu->handSheet
		ClearButtonsMainMenu();
		FreeVarArray(&mmenu->buttons);
		ClearPointer(mmenu);
	}
}

// +--------------------------------------------------------------+
// |                            Update                            |
// +--------------------------------------------------------------+
void UpdateAppState_MainMenu()
{
	MemArena_t* scratch = GetScratchArena();
	
	// +==============================+
	// |     Btn_A Selects Button     |
	// +==============================+
	if (BtnPressed(Btn_A))
	{
		HandleBtnExtended(Btn_A);
		if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex < mmenu->buttons.length)
		{
			MMenuBtn_t* selectedBtn = VarArrayGetHard(&mmenu->buttons, (u64)mmenu->selectionIndex, MMenuBtn_t);
			switch (selectedBtn->action)
			{
				// +==============================+
				// |      Play Btn Selected       |
				// +==============================+
				case MMenuAction_Play:
				{
					MainMenuGotoSubMenu(MMenuSubMenu_LevelSelect);
				} break;
				
				// +==============================+
				// |        Level Selected        |
				// +==============================+
				case MMenuAction_Level:
				{
					SetCurrentLevel(selectedBtn->referencePath);
					PushAppState(AppState_Game);
				} break;
				
				// +==============================+
				// |        Back Selected         |
				// +==============================+
				case MMenuAction_Back:
				{
					MainMenuGotoSubMenu(MMenuSubMenu_None);
				} break;
				
				// +==============================+
				// |     Unknown Btn Selected     |
				// +==============================+
				default:
				{
					PrintLine_E("The %s button doesn't work yet!", GetMMenuActionStr(selectedBtn->action));
				} break;
			}
		}
		
	}
	
	// +==============================+
	// |    Btn_B Leaves Sub-Menu     |
	// +==============================+
	if (BtnPressed(Btn_B))
	{
		HandleBtnExtended(Btn_B);
		if (mmenu->subMenu != MMenuSubMenu_None)
		{
			MainMenuGotoSubMenu(MMenuSubMenu_None);
		}
	}
	
	// +==============================+
	// |        Handle Up/Down        |
	// +==============================+
	if (BtnPressed(Btn_Up))
	{
		HandleBtnExtended(Btn_Up);
		MainMenuMoveSelectionUp();
		mmenu->scrollToSelection = true;
	}
	if (BtnPressed(Btn_Down))
	{
		HandleBtnExtended(Btn_Down);
		MainMenuMoveSelectionDown();
		mmenu->scrollToSelection = true;
	}
	
	// +==============================+
	// |         Handle Crank         |
	// +==============================+
	r32 crankDiff = AngleDiffR32(ToRadians32(input->crankAngle), mmenu->crankAngleRef);
	if (AbsR32(crankDiff) >= MMENU_CRANK_MOVE_DELTA)
	{
		i32 numMoves = (i32)(crankDiff / MMENU_CRANK_MOVE_DELTA);
		mmenu->crankAngleRef += (r32)numMoves * MMENU_CRANK_MOVE_DELTA;
		mmenu->crankAngleRef = AngleFixR32(mmenu->crankAngleRef);
		
		mmenu->scrollToSelection = true;
		for (i32 mIndex = 0; mIndex < AbsI32(numMoves); mIndex++)
		{
			if (numMoves >= 0) { MainMenuMoveSelectionDown(); }
			else { MainMenuMoveSelectionUp(); }
		}
	}
	
	// +==============================+
	// |    Scroll to Selected Btn    |
	// +==============================+
	if (mmenu->selectionIndex >= 0 && (mmenu->scrollToSelection || mmenu->jumpToSelection))
	{
		MMenuBtn_t* selectedBtn = VarArrayGetHard(&mmenu->buttons, (u64)mmenu->selectionIndex, MMenuBtn_t);
		mmenu->scrollGoto = (r32)selectedBtn->mainRec.y + (r32)selectedBtn->mainRec.height/2 - (r32)ScreenSize.height/2;
		// r32 selectedBtnScrollTop = (r32)selectedBtn->mainRec.y;
		// r32 selectedBtnScrollBottom = (r32)((selectedBtn->mainRec.y + selectedBtn->mainRec.height) - ScreenSize.height);
		// if (mmenu->scrollGoto < selectedBtnScrollBottom)
		// {
		// 	mmenu->scrollGoto = selectedBtnScrollBottom + MMENU_SCROLL_PAST;
		// }
		// else if (mmenu->scrollGoto > selectedBtnScrollTop)
		// {
		// 	mmenu->scrollGoto = selectedBtnScrollTop - MMENU_SCROLL_PAST;
		// }
	}
	
	// +==============================+
	// |       Update Scrolling       |
	// +==============================+
	mmenu->scroll = ClampR32(mmenu->scroll, 0, mmenu->scrollMax);
	mmenu->scrollGoto = ClampR32(mmenu->scrollGoto, 0, mmenu->scrollMax);
	r32 scrollDelta = mmenu->scrollGoto - mmenu->scroll;
	if (AbsR32(scrollDelta) > 1.0f && !mmenu->jumpToSelection)
	{
		mmenu->scroll += scrollDelta / MMENU_SCROLL_LAG;
	}
	else
	{
		mmenu->scroll = mmenu->scrollGoto;
	}
	
	// +==============================+
	// |     Check for Save Files     |
	// +==============================+
	if (mmenu->numSaveFilesUnchecked > 0)
	{
		u64 numChecks = 0;
		VarArrayLoop(&mmenu->buttons, bIndex)
		{
			VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
			if (button->action == MMenuAction_Level && !button->checkedForSaveFiles)
			{
				MainMenuCheckForSaveFileForBtn(button);
				numChecks++;
				if (numChecks >= MMENU_NUM_SAVE_FILE_CHECKS_PER_FRAME) { break; }
			}
		}
	}
	
	// +==============================+
	// |   Update Button Animations   |
	// +==============================+
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex == bIndex)
		{
			UpdateAnimationUp(&button->selectedAnim, MMENU_BTN_SELECT_ANIM_TIME);
		}
		else
		{
			UpdateAnimationDown(&button->selectedAnim, MMENU_BTN_SELECT_ANIM_TIME);
		}
	}
	
	mmenu->scrollToSelection = false;
	mmenu->jumpToSelection = false;
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                            Render                            |
// +--------------------------------------------------------------+
void RenderAppState_MainMenu(bool isOnTop)
{
	MemArena_t* scratch = GetScratchArena();
	
	pd->graphics->clear(kColorWhite);
	PdSetDrawMode(kDrawModeCopy);
	
	// +==============================+
	// |         Render Title         |
	// +==============================+
	if (mmenu->subMenu == MMenuSubMenu_None)
	{
		PdDrawTexturedRec(mmenu->bottomDitherSprite, NewReci(0, ScreenSize.height - mmenu->bottomDitherSprite.height, ScreenSize.width, mmenu->bottomDitherSprite.height));
		PdBindFont(&mmenu->titleFont);
		PdDrawText(NewStr("C-DOKU"), mmenu->titleRec.topLeft);
		// PdDrawTexturedRec(mmenu->titleSprite, mmenu->titleRec);
	}
	
	// +==============================+
	// |        Render Buttons        |
	// +==============================+
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		reci mainRec = button->mainRec + mmenu->buttonListRec.topLeft + NewVec2i(0, RoundR32i(-mmenu->scroll));
		if (RecisIntersect(mainRec, ScreenRec))
		{
			bool isLevelBtn = (button->action == MMenuAction_Level);
			Font_t* font = (isLevelBtn ? &mmenu->levelBtnFont : &mmenu->buttonFont);
			MyStr_t displayText = button->displayText;
			bool drawInvertedText = false;
			v2i displayTextPos = mainRec.topLeft + button->displayTextPos;
			
			if (!button->checkedForSaveFiles)
			{
				MainMenuCheckForSaveFileForBtn(button);
			}
			
			if (button->isCompleted)
			{
				PdDrawRec(mainRec, kColorBlack);
				PdDrawRecOutline(mainRec, 2, false, kColorBlack);
				drawInvertedText = true;
			}
			else if (button->hasSaveFile)
			{
				displayText = PrintInArenaStr(scratch, "%.*s*", StrPrint(displayText));
				PdDrawTexturedRecPart(gl->ditherTexture, mainRec, NewReci(0, 0, mainRec.size));
				PdDrawRecOutline(mainRec, 2, false, kColorBlack);
			}
			else if (isLevelBtn)
			{
				PdDrawRec(mainRec, kColorWhite);
				PdDrawRecOutline(mainRec, 2, false, kColorBlack);
			}
			else
			{
				mainRec = ReciInflate(mainRec,
					RoundR32i(LerpR32(0, mainRec.width*0.125f, EaseCubicOut(button->selectedAnim))),
					RoundR32i(LerpR32(0, mainRec.height*0.125f, EaseCubicOut(button->selectedAnim)))
				);
				displayTextPos = mainRec.topLeft + NewVec2i(mainRec.width/2 - button->displayTextSize.width/2, mainRec.height/2 - button->displayTextSize.height/2);
				MainMenuRenderFancyBtn(mainRec);
				drawInvertedText = true;
			}
			
			PdBindFont(font);
			LCDBitmapDrawMode oldDrawMode = kDrawModeCopy;
			if (drawInvertedText) { oldDrawMode = PdSetDrawMode(kDrawModeNXOR); }
			PdDrawText(displayText, displayTextPos);
			if (drawInvertedText) { PdSetDrawMode(oldDrawMode); }
			
			if (mmenu->selectionIndex >= 0 && (u64)mmenu->selectionIndex == bIndex)
			{
				reci handRec;
				handRec.size = mmenu->handSheet.frameSize;
				handRec.x = mainRec.x - handRec.width + 2;
				handRec.y = mainRec.y + mainRec.height/2 - handRec.height/2;
				handRec.x += RoundR32i(Oscillate(-3, 1, 1000));
				PdDrawSheetFrame(mmenu->handSheet, NewVec2i(0, 0), handRec);
			}
		}
	}
	
	// +==============================+
	// |    Render Completed Text     |
	// +==============================+
	{
		PdBindFont(&mmenu->levelBtnFont); //TODO: Change this font?
		MyStr_t loadingText = MyStr_Empty;
		#if 0
		loadingText = PrintInArenaStr(scratch, "Loading %llu/%llu", mmenu->buttons.length - mmenu->numSaveFilesUnchecked, mmenu->buttons.length);
		#else
		u64 numCompletedLevels = 0;
		u64 numLevelsTotal = 0;
		VarArrayLoop(&mmenu->buttons, bIndex)
		{
			VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
			if (button->action == MMenuAction_Level)
			{
				numLevelsTotal++;
				if (button->isCompleted) { numCompletedLevels++; }
			}
		}
		if (numLevelsTotal > 0)
		{
			loadingText = PrintInArenaStr(scratch, "Completed: %llu%s/%llu", numCompletedLevels, (mmenu->numSaveFilesUnchecked > 0) ? "*" : "", numLevelsTotal);
		}
		#endif
		
		if (!IsEmptyStr(loadingText))
		{
			v2i loadingTextSize = MeasureText(mmenu->levelBtnFont.font, loadingText);
			
			OffscreenTarget_t target = GetOffscreenTarget(loadingTextSize);
			NotNull(target.pntr);
			PdPushOffscreenTarget(target);
			
			PdDrawRec(NewReci(0, 0, loadingTextSize), kColorWhite);
			PdDrawText(loadingText, Vec2i_Zero);
			
			PdPopOffscreenTarget(target);
			
			reci loadingTextDrawRec = NewReci(
				ScreenSize.width - loadingTextSize.height,
				ScreenSize.height/2 - loadingTextSize.width/2,
				loadingTextSize.height,
				loadingTextSize.width
			);
			PdDrawOffscreenResults(target, loadingTextDrawRec, Dir2_Up);
		}
	}
	
	// +==============================+
	// |      Render Debug Info       |
	// +==============================+
	if (pig->debugEnabled)
	{
		LCDBitmapDrawMode oldDrawMode = PdSetDrawMode(kDrawModeNXOR);
		
		v2i textPos = NewVec2i(1, 1);
		if (pig->perfGraph.enabled) { textPos.y += pig->perfGraph.mainRec.y + pig->perfGraph.mainRec.height + 1; }
		PdBindFont(&pig->debugFont);
		i32 stepY = pig->debugFont.lineHeight + 1;
		
		PdDrawTextPrint(textPos, "Game: v%u.%u(%03u)", GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_BUILD);
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "Engine: v%u.%u(%03u)", PIG_VERSION_MAJOR, PIG_VERSION_MINOR, PIG_VERSION_BUILD);
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "Scroll: %g (%g, max %g)", (r64)mmenu->scroll, (r64)mmenu->scrollGoto, (r64)mmenu->scrollMax);
		textPos.y += stepY;
		PdDrawTextPrint(textPos, "Crank: %g %g", (r64)input->crankAngle, (r64)ToDegrees32(mmenu->crankAngleRef));
		textPos.y += stepY;
		
		PdSetDrawMode(oldDrawMode);
	}
	
	FreeScratchArena(scratch);
}

// +--------------------------------------------------------------+
// |                           Register                           |
// +--------------------------------------------------------------+
void RegisterAppState_MainMenu()
{
	mmenu = (MainMenuState_t*)RegisterAppState(
		AppState_MainMenu,
		sizeof(MainMenuState_t),
		StartAppState_MainMenu,
		StopAppState_MainMenu,
		UpdateAppState_MainMenu,
		RenderAppState_MainMenu
	);
}
