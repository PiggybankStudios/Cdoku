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
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PrintLine_D("[%llu]: \"%.*s\"", fIndex, StrPrint(levelFiles.paths[fIndex]));
				}
				
				for (u64 fIndex = 0; fIndex < levelFiles.count; fIndex++)
				{
					PushMemMark(scratch);
					MyStr_t levelPath = levelFiles.paths[fIndex];
					MyStr_t levelName = GetFileNamePart(levelPath, false);
					MyStr_t savePath = GetLevelSaveFilePath(scratch, levelPath);
					bool hasSaveFile = DoesFileExist(true, savePath);
					MyStr_t displayText = PrintInArenaStr(scratch, "%.*s%s", StrPrint(levelName), (hasSaveFile ? "*" : ""));
					for (u64 cIndex = 0; cIndex < displayText.length; cIndex++) { displayText.chars[cIndex] = GetUppercaseAnsiiChar(displayText.chars[cIndex]); }
					MMenuBtn_t* newBtn = AddButtonMainMenu(MMenuAction_Level, displayText, levelPath);
					PopMemMark(scratch);
				}
			}
			else
			{
				WriteLine_E("Failed to enumerate level files!");
			}
			
			AddButtonMainMenu(MMenuAction_Back, NewStr("BACK"));
			
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
	mmenu->buttonListRec.y = (ScreenSize.height + (mmenu->titleRec.y + mmenu->titleRec.height)) / 2 - (mmenu->buttonListRec.height/2);
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
	// |        Handle Up/Down        |
	// +==============================+
	if (BtnPressed(Btn_Up))
	{
		HandleBtnExtended(Btn_Up);
		if (mmenu->buttons.length > 0)
		{
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
		reci mainRec = button->mainRec + mmenu->buttonListRec.topLeft;
		Font_t* font = ((button->action == MMenuAction_Level) ? &mmenu->levelBtnFont : &mmenu->buttonFont);
		
		PdDrawRec(mainRec, kColorWhite);
		PdDrawRecOutline(mainRec, 2, false, kColorBlack);
		
		v2i displayTextPos = mainRec.topLeft + button->displayTextPos;
		PdBindFont(font);
		PdDrawText(button->displayText, displayTextPos);
		
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
