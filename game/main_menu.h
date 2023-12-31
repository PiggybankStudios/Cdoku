/*
File:   main_menu.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#define MMENU_TITLE_FONT_PATH        "Resources/Fonts/SunnySpellsReduced"
#define MMENU_BTN_FONT_PATH          "Resources/Fonts/whacky_joe"      
#define MMENU_LEVEL_BTN_FONT_PATH    "Resources/Fonts/whacky_joe"      

// NOTE: Every frame we will check for 3 level btn's save files (3-6ms of work each)
// which should keep us running at a solid 30fps
#define MMENU_NUM_SAVE_FILE_CHECKS_PER_FRAME  3 //DoesFileExist calls

#define MMENU_SCROLL_LAG          4 //divisor
#define MMENU_SCROLL_PAST         (PLAYDATE_SCREEN_HEIGHT/2)
#define MMENU_CRANK_MOVE_DELTA    ToRadians32(30) //radians

#define MMENU_BTN_SPACING            10 //px
#define MMENU_BTN_HORIZONTAL_PADDING 10 //px
#define MMENU_BTN_VERTICAL_PADDING   4 //px
#define MMENU_BTN_TOP_BOTTOM_MARGIN  10 //px

#define MMENU_LEVEL_BTN_SPACING            3 //px
#define MMENU_LEVEL_BTN_HORIZONTAL_PADDING 60 //px
#define MMENU_LEVEL_BTN_VERTICAL_PADDING   8 //px

#define MMENU_BTN_SELECT_ANIM_TIME   200 //ms

enum MMenuAction_t
{
	MMenuAction_None = 0,
	MMenuAction_Play,
	MMenuAction_Continue,
	MMenuAction_Level,
	MMenuAction_Settings,
	MMenuAction_Back,
	MMenuAction_Exit,
	MMenuAction_NumActions,
};
const char* GetMMenuActionStr(MMenuAction_t enumValue)
{
	switch (enumValue)
	{
		case MMenuAction_None:     return "None";
		case MMenuAction_Play:     return "Play";
		case MMenuAction_Continue: return "Continue";
		case MMenuAction_Level:    return "Level";
		case MMenuAction_Settings: return "Settings";
		case MMenuAction_Back:     return "Back";
		case MMenuAction_Exit:     return "Exit";
		default: return "Unknown";
	}
}

enum MMenuSubMenu_t
{
	MMenuSubMenu_None = 0,
	MMenuSubMenu_LevelSelect,
	MMenuSubMenu_Options,
	MMenuSubMenu_NumSubMenus,
};
const char* GetMMenuSubMenuStr(MMenuSubMenu_t enumValue)
{
	switch (enumValue)
	{
		case MMenuSubMenu_None:        return "None";
		case MMenuSubMenu_LevelSelect: return "LevelSelect";
		case MMenuSubMenu_Options:     return "Options";
		default: return "Unknown";
	}
}

struct MMenuBtn_t
{
	MMenuAction_t action;
	MyStr_t displayText;
	MyStr_t referencePath;
	bool checkedForSaveFiles;
	bool isCompleted;
	bool hasSaveFile;
	r32 selectedAnim;
	
	reci mainRec;
	v2i displayTextSize;
	v2i displayTextPos; //relative
};

struct MainMenuState_t
{
	bool initialized;
	
	SpriteSheet_t handSheet;
	SpriteSheet_t btnCornersSheet;
	Font_t titleFont;
	Font_t buttonFont;
	Font_t levelBtnFont;
	Texture_t titleSprite;
	Texture_t bottomDitherSprite;
	
	bool lastPlayedExists;
	bool lastPlayedIsUnfinished;
	MyStr_t lastPlayedPath;
	
	MMenuSubMenu_t subMenu;
	i64 oldSelectionIndices[MMenuSubMenu_NumSubMenus];
	i64 selectionIndex;
	u64 numSaveFilesUnchecked;
	VarArray_t buttons; //MMenuBtn_t
	
	bool scrollToSelection;
	bool jumpToSelection;
	r32 scroll;
	r32 scrollGoto;
	r32 scrollMax;
	r32 crankAngleRef;
	
	reci titleRec;
	reci buttonListRec;
};

#endif //  _MAIN_MENU_H
