// Metronome.cpp

#include "Metronome.h"
#include <Arduino.h>
#include <Audio.h>
#include "synth_simple_drum.h"

Metronome* Metronome::instance = nullptr;

Metronome::Metronome(AudioSynthSimpleDrum* drumSynth) : tickSynth(drumSynth) {
     instance = this;
}

void Metronome::begin() {
     // Any setup code for audio/timers goes here
     Serial.println("Metronome initialized.");
}

void Metronome::metronomeTickHandler() {
     if (instance != nullptr) {
          instance->tick();
     }
}

void Metronome::setBPM(int bpm) {
     int oldBPM = currentBPM;
     currentBPM = constrain(bpm, 30, 280); 
     
     if (currentBPM != oldBPM && state == METRONOME_RUNNING) {
          float frequencyHz = (float)currentBPM / 60.0f;
          metronomeTimer.begin(metronomeTickHandler, (1000000.0f / frequencyHz)); // Recalculate microseconds
     }
}

void Metronome::setOffset(int offset) {
     offsetMillis = constrain(offset, -500, 500); 
}

void Metronome::start() {
     if (state != METRONOME_RUNNING) {
          state = METRONOME_RUNNING;

          float frequencyHz = (float)currentBPM / 60.0f;
          // Time interval in microseconds = 1,000,000 / frequencyHz
          metronomeTimer.begin(metronomeTickHandler, (1000000.0f / frequencyHz)); 
          
          Serial.print("Metronome started at ");
          Serial.print(currentBPM);
          Serial.println(" BPM.");
     }
}

void Metronome::stop() {
     if (state != METRONOME_STOPPED) {
          state = METRONOME_STOPPED;
          metronomeTimer.end();
          Serial.println("Metronome stopped.");
     }
}

void Metronome::loadSongInfo(const SongMetronomeData& data) {
     setBPM(data.bpm);
     setOffset(data.offsetMillis);
     stop(); // CRITICAL: Ensure metronome is stopped
     Serial.print("Loaded new song info: ");
     Serial.print(currentBPM);
     Serial.print(" BPM, ");
     Serial.print(offsetMillis);
     Serial.println(" ms offset.");
}

void Metronome::tick() {
     tickSynth->noteOn();
}
