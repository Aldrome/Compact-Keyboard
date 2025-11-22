#include "Menu.h"

// Macro to safely calculate the number of elements in an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// --- Menu Data Definitions ---

// Main Menu
const char* const Menu::mainMenuLabels[] = {
     "Music Player",
     "Metronome",
     "Settings"
};
const int Menu::mainMenuCount = ARRAY_SIZE(mainMenuLabels);

// Music Player Submenu
const char* const Menu::musicPlayerMenuLabels[] = {
     "Prev Song", 
     "Next Song", 
     "", 
     "<< Back"
};
const int Menu::musicPlayerMenuCount = ARRAY_SIZE(musicPlayerMenuLabels);

// Metronome Submenu
const char* const Menu::metronomeMenuLabels[] = {
     "BPM:", 
     "",
     "<< Back"
};
const int Menu::metronomeMenuCount = ARRAY_SIZE(metronomeMenuLabels);

// Settings Submenu
const char* const Menu::settingsMenuLabels[] = {
     "Settings (Empty)", 
     "<< Back"
};
const int Menu::settingsMenuCount = ARRAY_SIZE(settingsMenuLabels);

// ---------------------------

// Constant for the menu drawing area width (leaving space for meters)
static constexpr int MENU_WIDTH = 220;

Menu::Menu(ILI9341_t3n& tftDisplay) : tft(&tftDisplay) {

}

void Menu::setPlayerState(PlayerState state) {
     playerState = state;
}

void Menu::setMetronomeInfo(MetronomeState state, int bpm) {
     metronomeState = state;
     currentBPM = bpm;
}

void Menu::setBPMEditing(bool editing) {
     isBPMEditing = editing;
}

// Helper to get the current menu items and count based on the screen state
const char* const* Menu::getCurrentMenuItems(int& count) {
     switch (currentScreen) {
          case SCREEN_MUSIC_PLAYER:
               count = musicPlayerMenuCount;
               return musicPlayerMenuLabels;
          case SCREEN_METRONOME:
               count = metronomeMenuCount;
               return metronomeMenuLabels;
          case SCREEN_SETTINGS:
               count = settingsMenuCount;
               return settingsMenuLabels;
          case SCREEN_MAIN:
          default:
               count = mainMenuCount;
               return mainMenuLabels;
     }
}

// Clears only the menu area (left side) and draws the menu items
void Menu::drawMenu() {
     // Clear only the menu display area (0, 0) to (MENU_WIDTH, 240)
     tft->fillRect(0, 0, MENU_WIDTH, 240, ILI9341_BLACK); 
     tft->setFont(Arial_24);

     int count;
     const char* const* items = getCurrentMenuItems(count);

     const char* tempLabels[musicPlayerMenuCount]; 
     if (currentScreen == SCREEN_MUSIC_PLAYER) {
          for (int i = 0; i < musicPlayerMenuCount; ++i) {
               tempLabels[i] = musicPlayerMenuLabels[i];
          }
          // Update the "Play/Pause" label (at index 2)
          if (playerState == PLAYING) {
               tempLabels[2] = "Pause";
          } else {
               tempLabels[2] = "Play";
          }
          items = tempLabels; // Use the modified array for drawing
     }
     
     else if (currentScreen == SCREEN_METRONOME) {
          for (int i = 0; i < metronomeMenuCount; ++i) {
               tempLabels[i] = metronomeMenuLabels[i];
          }

          char bpmLabel[20];
          snprintf(bpmLabel, sizeof(bpmLabel), "BPM: %d", currentBPM);
          tempLabels[0] = strdup(bpmLabel);
          
          // Update the "Start/Stop" label (Index 1)
          if (metronomeState == METRONOME_RUNNING) {
               tempLabels[1] = "Stop"; 
          } else {
               tempLabels[1] = "Start"; 
          }
          
          items = tempLabels;
     }
     
     // Draw the current menu screen title
     tft->setTextColor(ILI9341_YELLOW);
     tft->setCursor(10, 10);
     switch (currentScreen) {
          case SCREEN_MUSIC_PLAYER: tft->println("Music Player"); break;
          case SCREEN_METRONOME:    tft->println("Metronome"); break;
          case SCREEN_SETTINGS:     tft->println("Settings"); break;
          case SCREEN_MAIN:
          default:                  tft->println("Main Menu"); break;
     }

     tft->setFont(Arial_16);

     // Draw menu items
     for (int i = 0; i < count; i++) {
          if (i == menuIndex) tft->setTextColor(ILI9341_RED);
          else tft->setTextColor(ILI9341_WHITE);
          
          tft->setCursor(10, 45 + i * 24); // Start drawing items below the title
          tft->println(items[i]);

          // Add BPM editing cursor if in edit mode
          if (currentScreen == SCREEN_METRONOME && i == 0 && isBPMEditing && (millis() % 1000 < 500)) {
               // This is a simple visual cursor to show editing mode
               tft->drawRect(tft->getCursorX() + 1, 45 + i * 24 + 1, 10, 1, ILI9341_RED);
          }
          
          tft->println();
     }

     if (currentScreen == SCREEN_METRONOME && tempLabels[0] != metronomeMenuLabels[0]) {
         free((void*)tempLabels[0]); 
     }

     tft->updateScreen();
}

void Menu::selectPrev() {
     int count;
     getCurrentMenuItems(count);
     menuIndex--;
     if (menuIndex < 0) menuIndex = count - 1;
     this->drawMenu();
}

void Menu::selectNext() {
     int count;
     getCurrentMenuItems(count);
     menuIndex++;
     if (menuIndex >= count) menuIndex = 0;
     this->drawMenu();
}

void Menu::goHome() {
     currentScreen = SCREEN_MAIN;
     menuIndex = 0;
     this->drawMenu();
     // No need to redraw drawPeakMeterFrame() here, as it is permanent
     // and the menu area clear does not affect it.
}

void Menu::goBack() {
     switch (currentScreen) {
          case SCREEN_MUSIC_PLAYER:
          case SCREEN_METRONOME:
          case SCREEN_SETTINGS:
               goHome();
               break;
          case SCREEN_MAIN:
          default:
               // Do nothing if already on the main screen
               break;
     }
}

MenuAction Menu::pressButton() {
     MenuAction action = MENU_ACTION_NONE;

     if (currentScreen == SCREEN_MAIN) {
          switch (menuIndex) {
               case 0: // Music Player
                    currentScreen = SCREEN_MUSIC_PLAYER;
                    menuIndex = 0; // Reset index for the new screen
                    this->drawMenu();
                    action = MENU_ACTION_GOTO_MUSIC_PLAYER;
                    break;
               case 1:
                    currentScreen = SCREEN_METRONOME;
                    menuIndex = 0;
                    this->drawMenu();
                    action = MENU_ACTION_GOTO_METRONOME;
                    break;
               case 2: // Settings
                    currentScreen = SCREEN_SETTINGS;
                    menuIndex = 0; // Reset index for the new screen
                    this->drawMenu();
                    action = MENU_ACTION_GOTO_SETTINGS;
                    break;
          }
     } else if (currentScreen == SCREEN_MUSIC_PLAYER) {
          switch (menuIndex) {
               case 0: action = MENU_ACTION_PREV_SONG; break;
               case 1: action = MENU_ACTION_NEXT_SONG; break;
               case 2: action = MENU_ACTION_PLAY_PAUSE; break;
               case 3: action = MENU_ACTION_GO_BACK; goBack(); break; // Back option
          }
     } else if (currentScreen == SCREEN_METRONOME) { // <-- NEW
          switch (menuIndex) {
               case 0: // BPM:
                    action = MENU_ACTION_CHANGE_BPM; 
                    // Note: Actual BPM editing state is handled in Testing.ino
                    break;
               case 1: // Start/Stop
                    action = MENU_ACTION_TOGGLE_METRONOME;
                    break;
               case 2: // << Back
                    action = MENU_ACTION_GO_BACK; 
                    goBack(); 
                    break;
          }
     } else if (currentScreen == SCREEN_SETTINGS) {
          switch (menuIndex) {
               case 0: // Settings (empty) - future functionality
                    // action = MENU_ACTION_SETTINGS_OPTION_1;
                    break; 
               case 1: action = MENU_ACTION_GO_BACK; goBack(); break; // Back option
          }
     }

     return action;
}

void Menu::drawPeakMeterFrame() {
     // Draw a frame for the peak meters on the right side of the screen
     tft->setTextColor(ILI9341_YELLOW);
     // Draw grey background for the meter bars
     tft->fillRect(BAR_Y_LEFT, BAR_Y_TOP, BAR_WIDTH, BAR_HEIGHT_MAX, tft->color565(54, 54, 54));
     tft->fillRect(BAR_Y_RIGHT, BAR_Y_TOP, BAR_WIDTH, BAR_HEIGHT_MAX, tft->color565(54, 54, 54));
     tft->setFont(Arial_14);
     // Draw channel labels
     tft->setCursor(BAR_Y_LEFT + 9, BAR_Y_TOP + BAR_HEIGHT_MAX + 5); tft->println("L");
     tft->setCursor(BAR_Y_RIGHT + 9, BAR_Y_TOP + BAR_HEIGHT_MAX + 5); tft->println("R");
}

void Menu::updatePeakBars(float leftPeak, float rightPeak) {
     int leftHeight  = leftPeak * BAR_HEIGHT_MAX;
     int rightHeight = rightPeak * BAR_HEIGHT_MAX;

     // Ensure heights are within bounds
     if (leftHeight > BAR_HEIGHT_MAX) leftHeight = BAR_HEIGHT_MAX;
     if (rightHeight > BAR_HEIGHT_MAX) rightHeight = BAR_HEIGHT_MAX;

     if (leftHeight != prevLeftHeight) {
          if (leftHeight > prevLeftHeight) {
               // Draw the increase in height (green)
               tft->fillRect(BAR_Y_LEFT, BAR_Y_TOP + BAR_HEIGHT_MAX - leftHeight, BAR_WIDTH, leftHeight - prevLeftHeight, ILI9341_GREEN);
          } else {
               // Clear the decrease in height (dark gray background)
               tft->fillRect(BAR_Y_LEFT, BAR_Y_TOP + BAR_HEIGHT_MAX - prevLeftHeight, BAR_WIDTH, prevLeftHeight - leftHeight, tft->color565(54, 54, 54));
          }
          prevLeftHeight = leftHeight;
     }

     if (rightHeight != prevRightHeight) {
          if (rightHeight > prevRightHeight) {
               // Draw the increase in height (green)
               tft->fillRect(BAR_Y_RIGHT, BAR_Y_TOP + BAR_HEIGHT_MAX - rightHeight, BAR_WIDTH, rightHeight - prevRightHeight, ILI9341_GREEN);
          } else {
               // Clear the decrease in height (dark gray background)
               tft->fillRect(BAR_Y_RIGHT, BAR_Y_TOP + BAR_HEIGHT_MAX - prevRightHeight, BAR_WIDTH, prevRightHeight - rightHeight, tft->color565(54, 54, 54));
          }
          prevRightHeight = rightHeight;
     }
}
