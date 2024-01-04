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

#define MAX_NUM_UNDOS       320 //moves (about 9x9 * 4)
#define UNDO_COMBINE_TIME   1000 //ms

#define UNDO_BTN_HOLD_TIME          1000 //ms
#define UNDO_BTN_FIRST_REPEAT_TIME  500 //ms
#define UNDO_BTN_REPEAT_TIME        200 //ms

#define BTN_PROMPT_SPACING       15 //px
#define BTN_PROMPT_TEXT_SPACING  2 //px

#define CURSOR_OUTLINE_THICKNESS     1
#define CURSOR_INNER_MARGIN          1
#define CURSOR_MAX_PREV_POSITIONS    4
#define CURSOR_PREV_POS_ANIM_TIME    180 //ms
#define CURSOR_NOTE_TAKING_ANIM_TIME  60 //ms
#define CURSOR_MAX_UNDO_ANIMS        16
#define CURSOR_UNDO_ANIM_TIME        300 //ms

#endif //  _GAME_DEFINES_H
