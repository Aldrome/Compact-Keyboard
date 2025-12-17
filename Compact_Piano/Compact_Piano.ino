#include <ILI9341_t3n.h>
#include <ILI9341_fonts.h>
#include <ili9341_t3n_font_Arial.h>
#include <Encoder.h>

#include "Menu.h"
#include "Metronome.h"
//#include "Synth.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputUSB            usbIn;           //xy=55,182.33334732055664
AudioPlaySdWav           playWav;        //xy=57.33332824707031,61.5
AudioSynthSimpleDrum     drum1;          //xy=75.33332824707031,301.3333282470703
AudioAmplifier           ampL1;          //xy=229.3333282470703,36.5
AudioAmplifier           ampR1;          //xy=229.3333282470703,66.5
AudioFilterStateVariable filterL1;       //xy=387.3333282470703,27.5
AudioFilterStateVariable filterR1;       //xy=387.3333282470703,73.5
AudioFilterStateVariable filterL2;        //xy=388.3333282470703,184.3333282470703
AudioFilterStateVariable filterR2;        //xy=389.3333435058594,230.33334922790527
AudioAmplifier           amp3;           //xy=389.3333435058594,299.33330726623535
AudioAmplifier           ampL2;          //xy=656.3333282470703,28.5
AudioAmplifier           ampR2;          //xy=656.3333282470703,58.5
AudioMixer4              mixerL;         //xy=658.3333282470703,154.5
AudioMixer4              mixerR;         //xy=658.3333282470703,214.5
AudioAnalyzePeak         peakL;          //xy=910.9999847412109,228.3333282470703
AudioAnalyzePeak         peakR;          //xy=911.0000305175781,258.3333511352539
AudioOutputUSB           usbOut;         //xy=916.3333282470703,31.5
AudioOutputI2S           audioOutput;    //xy=927.3333282470703,178.5
AudioConnection          patchCord1(usbIn, 0, filterL2, 0);
AudioConnection          patchCord2(usbIn, 1, filterR2, 0);
AudioConnection          patchCord3(playWav, 0, ampL1, 0);
AudioConnection          patchCord4(playWav, 1, ampR1, 0);
AudioConnection          patchCord5(drum1, amp3);
AudioConnection          patchCord6(ampL1, 0, filterL1, 0);
AudioConnection          patchCord7(ampR1, 0, filterR1, 0);
AudioConnection          patchCord8(filterL1, 0, ampL2, 0);
AudioConnection          patchCord9(filterL1, 0, mixerL, 0);
AudioConnection          patchCord10(filterR1, 0, ampR2, 0);
AudioConnection          patchCord11(filterR1, 0, mixerR, 0);
AudioConnection          patchCord12(filterL2, 0, mixerL, 1);
AudioConnection          patchCord13(filterR2, 0, mixerR, 1);
AudioConnection          patchCord14(amp3, 0, mixerR, 2);
AudioConnection          patchCord15(amp3, 0, mixerL, 2);
AudioConnection          patchCord16(ampL2, 0, usbOut, 0);
AudioConnection          patchCord17(ampR2, 0, usbOut, 1);
AudioConnection          patchCord18(mixerL, 0, audioOutput, 0);
AudioConnection          patchCord19(mixerL, peakL);
AudioConnection          patchCord20(mixerR, 0, audioOutput, 1);
AudioConnection          patchCord21(mixerR, peakR);
AudioControlSGTL5000     sgtl5000;       //xy=56.5,439.49999618530273
// GUItool: end automatically generated code



// Green L
// WHite R

// --- Pin Usage ---
#define TFT_CS   28
#define TFT_RST  25
#define TFT_DC   24
#define TFT_MOSI 26
#define TFT_SCK  27
#define TFT_MISO 1
#define TFT_SD_CS   9
#define AUDIO_SD_CS 10

#define ENC_CLK       29
#define ENC_DT        30
#define ENC_SW        31

#define MUX_SIG_PIN    41
#define MUX_S2_PIN     40
#define MUX_S1_PIN     39
#define MUX_S0_PIN     38

#define BACK_BUTTON_PIN 0
#define HOME_BUTTON_PIN 2

// --- MUX Definition ---
#define VOLUME_MUX_CHANNEL     0
#define FILTER_MUX_CHANNEL     1

// --- TFT ---
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

// --- Peak Meter ---
elapsedMillis msecs;
#define MAX_VOLUME     0.6

// --- Playlist ---
struct SongEntry {       
     const char* filename;
     int bpm;
     int offset;
};

const SongEntry playlist[] = {
     {"MYSTIC.WAV",   160,  0},
     {"BITEME.WAV",   174,  184}, 
     {"GLITCH.WAV",   150,  30},
     {"BAMBOO.WAV",   154,   194}
};

const int totalSongs = sizeof(playlist) / sizeof(playlist[0]);
unsigned int currentSong = 0;
PlayerState state = PAUSED;

// --- Encoder ---
Encoder knob(ENC_DT, ENC_CLK);
long lastPosition = 0;
bool lastEncoderButtonState = HIGH;
bool lastBackButtonState = HIGH;
bool lastHomeButtonState = HIGH;
unsigned long lastButtonPress = 0;
const unsigned long debounceTime = 200;

elapsedMillis serialTimer;

// --- Menu System ---
Menu menu(tft);

// --- Metronome System ---
Metronome metronome{&drum1}; 
bool isBPMEditing = false;

// --- Functions ---
void playCurrentSong() {
     SongEntry activeSong = playlist[currentSong];

     Serial.print("Playing: ");
     Serial.println(activeSong.filename);
     playWav.play(activeSong.filename);

     SongMetronomeData metaData;
     metaData.bpm = activeSong.bpm;
     metaData.offsetMillis = activeSong.offset;
     metronome.loadSongInfo(metaData);
     metronome.syncStart();
     menu.setMetronomeInfo(metronome.getState(), metronome.getBPM());

     state = PLAYING;
     menu.setPlayerState(state);
     delay(25);
}

void stopPlayback() {
     playWav.stop();
     metronome.syncStop();
     state = PAUSED;
     menu.setPlayerState(state);
}

void nextSong() {
     playWav.stop();
     metronome.syncStop();

     currentSong++;
     if (currentSong >= totalSongs) currentSong = 0;
     Serial.println(playlist[currentSong].filename);
     playCurrentSong();
}

int readMuxChannel(int channel) {
    // Set S0, S1, S2 pins based on the channel number
    // S0 is the LSB (bit 0), S2 is the MSB (bit 2)
    digitalWrite(MUX_S0_PIN, (channel & 0b001) ? HIGH : LOW);
    digitalWrite(MUX_S1_PIN, (channel & 0b010) ? HIGH : LOW);
    digitalWrite(MUX_S2_PIN, (channel & 0b100) ? HIGH : LOW);

    delayMicroseconds(5); 

    analogRead(MUX_SIG_PIN); 

    delayMicroseconds(5);

    return analogRead(MUX_SIG_PIN);
}

void handlePotentiometers() {
     const float MIN_CUTOFF = 80.0f;
     const float MAX_CUTOFF = 14700.0f;

     const float disableThreshold = 0.95f;

     int volumePotValue = readMuxChannel(VOLUME_MUX_CHANNEL);
     float volume = (volumePotValue / 1023.0) * MAX_VOLUME;

     if (volume < 0.01) {
          sgtl5000.volume(0.0);
          sgtl5000.muteHeadphone();
     } else {
          sgtl5000.volume(volume);
          sgtl5000.unmuteHeadphone();
     }
     ampL2.gain(volume);
     ampR2.gain(volume);

     int filterPotValue = readMuxChannel(FILTER_MUX_CHANNEL);
     float norm = filterPotValue / 1023.0f;

     const float Q_RAMP_START = 0.85f;
     const float MAX_RESONANCE = 5.0f;
     const float MIN_RESONANCE = 1.0f;
     
     float currentResonance = MAX_RESONANCE;
     
     if (norm >= disableThreshold) {
         filterL1.frequency(20000.0f); filterR1.frequency(20000.0f);
         filterL2.frequency(20000.0f); filterR2.frequency(20000.0f);
         filterL1.resonance(MIN_RESONANCE); filterR1.resonance(MIN_RESONANCE);
         filterL2.resonance(MIN_RESONANCE); filterR2.resonance(MIN_RESONANCE);
         return;
     }

     if (norm >= Q_RAMP_START) {
         float rampRange = disableThreshold - Q_RAMP_START;
         float rampPos = norm - Q_RAMP_START;

         float inverseRampFactor = 1.0f - (rampPos / rampRange); 

         currentResonance = MIN_RESONANCE + (MAX_RESONANCE - MIN_RESONANCE) * inverseRampFactor;
         currentResonance = constrain(currentResonance, MIN_RESONANCE, MAX_RESONANCE);
     }

     float cutoff = MIN_CUTOFF + (MAX_CUTOFF - MIN_CUTOFF) * norm / disableThreshold;

     filterL1.frequency(cutoff); filterR1.frequency(cutoff);
     filterL2.frequency(cutoff); filterR2.frequency(cutoff);
     
     filterL1.resonance(currentResonance); filterR1.resonance(currentResonance);
     filterL2.resonance(currentResonance); filterR2.resonance(currentResonance);

     if (serialTimer >= 200) { // Print every 200 milliseconds
          serialTimer = 0;
          Serial.print("Vol(0): Raw=");
          Serial.print(volumePotValue);
          Serial.print(" | Filter(1): Raw=");
          Serial.println(filterPotValue);
     }
}

void executeMenuAction(MenuAction action) {
     switch (action) {
          case MENU_ACTION_PREV_SONG:
               metronome.syncStop();
               if (currentSong == 0) {
                    currentSong = totalSongs - 1;
               } else {
                    currentSong--;
               }
               playCurrentSong();
               break;

          case MENU_ACTION_NEXT_SONG:
               nextSong();
               break;

          case MENU_ACTION_PLAY_PAUSE:
               if (state == PLAYING) stopPlayback();
               else playCurrentSong();
               menu.drawMenu();
               break;

          case MENU_ACTION_GOTO_METRONOME:
               isBPMEditing = false; // Reset editing state on screen entry
               menu.setBPMEditing(isBPMEditing); // Tell the menu the state
               break;
               
          case MENU_ACTION_CHANGE_BPM:
               // Toggle BPM editing mode when BPM option is selected
               isBPMEditing = !isBPMEditing;
               menu.setBPMEditing(isBPMEditing); // Tell the menu the state
               menu.drawMenu();
               break;
               
          case MENU_ACTION_TOGGLE_METRONOME:
               if (metronome.getState() == METRONOME_RUNNING) metronome.stop();
               else metronome.start();
               
               // Update menu display with the new state
               menu.setMetronomeInfo(metronome.getState(), metronome.getBPM());
               menu.drawMenu();
               break;

          case MENU_ACTION_GO_BACK:
               if (isBPMEditing) {
                    isBPMEditing = false;
                    menu.setBPMEditing(isBPMEditing);
                    menu.drawMenu(); // Redraw to exit editing focus
                    return; // Consumes the back action to exit edit mode
               }
               // Menu class handles drawing for non-editing mode back, just log
               break;

          case MENU_ACTION_GOTO_MUSIC_PLAYER:
               break;
          case MENU_ACTION_GOTO_SETTINGS:
               break;
          case MENU_ACTION_GO_HOME:
               isBPMEditing = false;
               break;
          default:
               break;
     }
}

void handleMenuInput() {
     long newPosition = knob.read();

     // Handle Encoder Rotation 
     if (isBPMEditing) {
          if (abs(newPosition - lastPosition) >= 2) {
               // Calculate the full change (delta) that occurred
               int delta = (newPosition - lastPosition) / 2; 
               int newBPM = metronome.getBPM() + delta;
               
               metronome.setBPM(newBPM); // Update the Metronome object
               
               // Inform the menu of the new BPM and redraw
               menu.setMetronomeInfo(metronome.getState(), metronome.getBPM()); 
               menu.drawMenu();
               
               lastPosition = newPosition; // Use the full new position
          }
     } else { // Normal Menu Navigation Mode
          if (newPosition - lastPosition >= 2) {
               menu.selectNext();
               lastPosition = newPosition;
          } else if (newPosition - lastPosition <= -2) {
               menu.selectPrev();
               lastPosition = newPosition;
          }
     }

     // Handle Encoder Button Press (Select/Execute)
     bool encoderButtonState = digitalRead(ENC_SW);
     unsigned long now = millis();

     if (encoderButtonState == LOW && lastEncoderButtonState == HIGH && (now - lastButtonPress > debounceTime)) {
          MenuAction action = menu.pressButton();
          executeMenuAction(action);
          lastButtonPress = now;
     }
     lastEncoderButtonState = encoderButtonState;
     
     // Handle Dedicated Home Button Press
     bool homeButtonState = digitalRead(HOME_BUTTON_PIN);

     if (homeButtonState == LOW && lastHomeButtonState == HIGH && (now - lastButtonPress > debounceTime)) {
          menu.goHome();
          executeMenuAction(MENU_ACTION_GO_HOME);
          lastButtonPress = now;
     }
     lastHomeButtonState = homeButtonState;

     bool backButtonState = digitalRead(BACK_BUTTON_PIN);

     if (backButtonState == LOW && lastBackButtonState == HIGH && (now - lastButtonPress > debounceTime)) {
          menu.goBack();
          executeMenuAction(MENU_ACTION_GO_BACK); 
          lastButtonPress = now;
     }
     lastBackButtonState = backButtonState;
}

// --- Setup ---
void setup() {
     Serial.begin(9600);
     AudioMemory(20);

     drum1.frequency(2000.0f);
     drum1.length(10);
     drum1.pitchMod(0.3f);
     drum1.secondMix(0.0f);

     sgtl5000.enable();
     sgtl5000.volume(0.4);

     ampL1.gain(0.5f); ampR1.gain(0.5f);

     if (!SD.begin(AUDIO_SD_CS)) {
          Serial.println("SD Card not found!");
     }

     SPI1.begin();
     tft.begin();
     tft.setRotation(2);
     tft.useFrameBuffer(true);
     
     // Initialize the TFT
     tft.fillScreen(ILI9341_BLACK);
     menu.drawPeakMeterFrame();
     menu.drawMenu();
     menu.setPlayerState(state);
     menu.setMetronomeInfo(metronome.getState(), metronome.getBPM());

     mixerL.gain(2, 1.0f); 
     mixerR.gain(2, 1.0f);
     
     amp3.gain(1.5f); 

     metronome.begin();

     pinMode(ENC_SW, INPUT_PULLUP);
     pinMode(BACK_BUTTON_PIN, INPUT_PULLUP); 
     pinMode(HOME_BUTTON_PIN, INPUT_PULLUP);

     pinMode(MUX_S0_PIN, OUTPUT);
     pinMode(MUX_S1_PIN, OUTPUT);
     pinMode(MUX_S2_PIN, OUTPUT);

     pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH);
     pinMode(AUDIO_SD_CS, OUTPUT); digitalWrite(AUDIO_SD_CS, HIGH);
}

// --- Loop ---
void loop() {
     handleMenuInput();
     handlePotentiometers();

     if (msecs > 66) {
          msecs = 0;
          if (peakL.available() && peakR.available()) {
               menu.updatePeakBars(peakL.read(), peakR.read());
               tft.updateScreen();
          }
     }

     if (state == PLAYING && !playWav.isPlaying()) {
          nextSong();
     }
}
