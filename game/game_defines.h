/*
File:   game_defines.h
Author: Taylor Robbins
Date:   12\19\2023
*/

#ifndef _GAME_DEFINES_H
#define _GAME_DEFINES_H

#define MAIN_FONT_PATH   "/System/Fonts/Asheville-Sans-14-Bold.pft"

#define REFRESH_RATE     50 //Hz
#define FIRST_APP_STATE  AppState_MainMenu

#define GAME_COMPLETION_ANIM_TIME   3000 //ms

#define LAST_PLAYED_LEVEL_PATH  "last_played.txt"      

#define MAX_NUM_UNDOS       16 //moves
#define UNDO_COMBINE_TIME   1000 //ms

#define UNDO_BTN_HOLD_TIME          1000 //ms
#define UNDO_BTN_FIRST_REPEAT_TIME  500 //ms
#define UNDO_BTN_REPEAT_TIME        200 //ms

#endif //  _GAME_DEFINES_H
