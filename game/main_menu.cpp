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
	switch (subMenu)
	{
		case MMenuSubMenu_None:
		{
			AddButtonMainMenu(MMenuAction_Play,     NewStr("PLAY"));
			AddButtonMainMenu(MMenuAction_Settings, NewStr("SETTINGS"));
			AddButtonMainMenu(MMenuAction_Exit,     NewStr("EXIT"));
		} break;
		
		case MMenuSubMenu_LevelSelect:
		{
			MemArena_t* scratch = GetScratchArena();
			
			FilesInDir_t levelFiles = GetFilesInDirectory(NewStr("Resources/Text"), scratch, false);
			if (levelFiles.success)
			{
				PrintLine_D("There are %llu levels! (allocation %llu %p)", levelFiles.count, levelFiles.mainAllocation.length, levelFiles.mainAllocation.pntr);
				#if 0
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PrintLine_D("[%llu]: \"%.*s\"", fIndex, StrPrint(levelFiles.paths[fIndex]));
				}
				#endif
				
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PushMemMark(scratch);
					MyStr_t levelPath = levelFiles.paths[fIndex];
					MyStr_t levelName = GetFileNamePart(levelPath, false);
					MyStr_t completedPath = GetLevelSaveFilePath(scratch, levelPath, true);
					MyStr_t savePath = GetLevelSaveFilePath(scratch, levelPath, false);
					bool isCompleted = DoesFileExist(true, completedPath);
					bool hasSaveFile = (isCompleted || DoesFileExist(true, savePath));
					MyStr_t displayText = PrintInArenaStr(scratch, "%.*s", StrPrint(levelName));
					for (u64 cIndex = 0; cIndex < displayText.length; cIndex++) { displayText.chars[cIndex] = GetUppercaseAnsiiChar(displayText.chars[cIndex]); }
					MMenuBtn_t* newBtn = AddButtonMainMenu(MMenuAction_Level, displayText, levelPath);
					newBtn->isCompleted = isCompleted;
					newBtn->hasSaveFile = hasSaveFile;
					PopMemMark(scratch);
				}
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
	
	PrintLine_D("The %s sub-menu contains %llu buttons", GetMMenuSubMenuStr(subMenu), mmenu->buttons.length); //TODO: Remove me!
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

// +--------------------------------------------------------------+
// |                            Start                             |
// +--------------------------------------------------------------+
void StartAppState_MainMenu(bool initialize, AppState_t prevState, MyStr_t transitionStr)
{
	if (initialize)
	{
		mmenu->handSheet = LoadSpriteSheet(NewStr("Resources/Sheets/hand"), 1);
		Assert(mmenu->handSheet.isValid);
		
		mmenu->titleFont = LoadFont(NewStr(MMENU_TITLE_FONT_PATH));
		Assert(mmenu->titleFont.isValid);
		
		mmenu->buttonFont = LoadFont(NewStr(MMENU_BTN_FONT_PATH));
		Assert(mmenu->buttonFont.isValid);
		
		mmenu->levelBtnFont = LoadFont(NewStr(MMENU_LEVEL_BTN_FONT_PATH));
		Assert(mmenu->levelBtnFont.isValid);
		
		mmenu->titleRec.size = MeasureText(mmenu->titleFont.font, NewStr(PROJECT_NAME));
		mmenu->titleRec.topLeft = NewVec2i(ScreenSize.width/2 - mmenu->titleRec.size.width/2, ScreenSize.height/4 - mmenu->titleRec.size.height/2);
		
		mmenu->subMenu = MMenuSubMenu_None;
		
		CreateVarArray(&mmenu->buttons, mainHeap, sizeof(MMenuBtn_t));
		
		mmenu->selectionIndex = 0;
		
		mmenu->initialized = true;
	}
	
	MainMenuGenerateButtons(mmenu->subMenu);
	PlaceButtonsListMainMenu();
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
		if (mmenu->buttons.length > 0)
		{
			mmenu->scrollToSelection = true;
			if (mmenu->selectionIndex < 0)
			{
				mmenu->selectionIndex = 0;
			}
			else
			{
				mmenu->selectionIndex--;
				if (mmenu->selectionIndex < 0) { mmenu->selectionIndex = mmenu->buttons.length-1; }
			}
			//TODO: Play a sound effect
		}
		else { mmenu->selectionIndex = -1; }
	}
	if (BtnPressed(Btn_Down))
	{
		HandleBtnExtended(Btn_Down);
		if (mmenu->buttons.length > 0)
		{
			mmenu->scrollToSelection = true;
			if (mmenu->selectionIndex < 0)
			{
				mmenu->selectionIndex = 0;
			}
			else
			{
				mmenu->selectionIndex++;
				if ((u64)mmenu->selectionIndex >= mmenu->buttons.length) { mmenu->selectionIndex = 0; }
			}
			//TODO: Play a sound effect
		}
		else { mmenu->selectionIndex = -1; }
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
		
		if (mmenu->buttons.length > 0)
		{
			mmenu->scrollToSelection = true;
			if (mmenu->selectionIndex < 0)
			{
				mmenu->selectionIndex = 0;
			}
			else
			{
				for (i32 mIndex = 0; mIndex < AbsI32(numMoves); mIndex++)
				{
					if (numMoves >= 0)
					{
						mmenu->selectionIndex++;
						if ((u64)mmenu->selectionIndex >= mmenu->buttons.length) { mmenu->selectionIndex = 0; }
					}
					else
					{
						mmenu->selectionIndex--;
						if (mmenu->selectionIndex < 0) { mmenu->selectionIndex = mmenu->buttons.length-1; }
					}
				}
			}
			//TODO: Play a sound effect
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
	
	if (mmenu->subMenu == MMenuSubMenu_None)
	{
		PdBindFont(&mmenu->titleFont);
		PdDrawText(NewStr(PROJECT_NAME), mmenu->titleRec.topLeft);
	}
	
	// +==============================+
	// |        Render Buttons        |
	// +==============================+
	VarArrayLoop(&mmenu->buttons, bIndex)
	{
		VarArrayLoopGet(MMenuBtn_t, button, &mmenu->buttons, bIndex);
		reci mainRec = button->mainRec + mmenu->buttonListRec.topLeft + NewVec2i(0, RoundR32i(-mmenu->scroll));
		Font_t* font = ((button->action == MMenuAction_Level) ? &mmenu->levelBtnFont : &mmenu->buttonFont);
		MyStr_t displayText = button->displayText;
		
		if (button->isCompleted)
		{
			PdDrawRec(mainRec, kColorBlack);
			PdDrawRecOutline(mainRec, 2, false, kColorBlack);
		}
		else if (button->hasSaveFile)
		{
			displayText = PrintInArenaStr(scratch, "%.*s*", StrPrint(displayText));
			PdDrawTexturedRecPart(gl->ditherTexture, mainRec, NewReci(0, 0, mainRec.size));
			PdDrawRecOutline(mainRec, 2, false, kColorBlack);
		}
		else
		{
			PdDrawRec(mainRec, kColorWhite);
			PdDrawRecOutline(mainRec, 2, false, kColorBlack);
		}
		
		v2i displayTextPos = mainRec.topLeft + button->displayTextPos;
		PdBindFont(font);
		LCDBitmapDrawMode oldDrawMode = kDrawModeCopy;
		if (button->isCompleted) { oldDrawMode = PdSetDrawMode(kDrawModeNXOR); }
		PdDrawText(displayText, displayTextPos);
		if (button->isCompleted) { PdSetDrawMode(oldDrawMode); }
		
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
