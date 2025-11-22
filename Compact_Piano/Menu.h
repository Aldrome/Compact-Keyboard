#ifndef MENU_H
#define MENU_H

#include <ILI9341_t3n.h>
#include <ILI9341_fonts.h>
#include <ili9341_t3n_font_Arial.h>

#include "Metronome.h"

enum MenuAction {
     MENU_ACTION_NONE,
     MENU_ACTION_GOTO_MUSIC_PLAYER,
     MENU_ACTION_NEXT_SONG,
     MENU_ACTION_PREV_SONG,
     MENU_ACTION_PLAY_PAUSE,
     MENU_ACTION_GOTO_METRONOME,
     MENU_ACTION_CHANGE_BPM,
     MENU_ACTION_TOGGLE_METRONOME,
     MENU_ACTION_GOTO_SETTINGS,
     MENU_ACTION_GO_BACK,
     MENU_ACTION_GO_HOME
};

enum MenuScreen {
     SCREEN_MAIN,
     SCREEN_MUSIC_PLAYER,
     SCREEN_METRONOME,
     SCREEN_SETTINGS
};

enum PlayerState { PLAYING, PAUSED };

class Menu {
public:
     Menu(ILI9341_t3n& tftDisplay);

     void drawMenu();

     void selectNext();
     void selectPrev();
     MenuAction pressButton();
     
     void drawPeakMeterFrame();
     void updatePeakBars(float leftPeak, float rightPeak);

     void goHome();
     void goBack();

     void setPlayerState(PlayerState state);
     void setMetronomeInfo(MetronomeState state, int bpm);
     void setBPMEditing(bool editing);

private:
     ILI9341_t3n* tft;
     
     MenuScreen currentScreen = SCREEN_MAIN;
     
     // Menu definitions
     static const char* const mainMenuLabels[];
     static const int mainMenuCount;

     static const char* const musicPlayerMenuLabels[];
     static const int musicPlayerMenuCount;

     static const char* const metronomeMenuLabels[];
     static const int metronomeMenuCount;

     static const char* const settingsMenuLabels[];
     static const int settingsMenuCount;

     const char* const* getCurrentMenuItems(int& count);

     int menuIndex = 0;

     int prevLeftHeight = 0;
     int prevRightHeight = 0;

     static constexpr int BAR_WIDTH      = 30;
     static constexpr int BAR_Y_TOP      = 40;
     static constexpr int BAR_Y_LEFT     = 240;
     static constexpr int BAR_Y_RIGHT    = 280;
     static constexpr int BAR_HEIGHT_MAX = 170;

     PlayerState playerState = PAUSED;

     MetronomeState metronomeState = METRONOME_STOPPED;
     int currentBPM = 120;
     bool isBPMEditing = false;
};

#endif
