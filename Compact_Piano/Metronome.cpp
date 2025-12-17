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

}

void Metronome::metronomeTickHandler() {
     if (instance != nullptr) {
          instance->tick();
     }
}

void Metronome::setBPM(int bpm) {
     currentBPM = constrain(bpm, 30, 280); 

     float frequencyHz = (float)currentBPM / 60.0f;
     usInterval = (unsigned long)(1000000.0f / frequencyHz);
}

void Metronome::setOffset(int offset) {
     offsetMillis = constrain(offset, -500, 500); 
}

void Metronome::start() {
     if (state != METRONOME_RUNNING) {
          state = METRONOME_RUNNING;

          if (!musicEngineRunning) {
               float frequencyHz = (float)currentBPM / 60.0f;
               usInterval = (unsigned long)(1000000.0f / frequencyHz);
               
               if (!engineRunning) {
                    metronomeTimer.begin(metronomeTickHandler, 1000);
                    engineRunning = true;
                    isFirstBeat = true; // First beat uses offset (even in standalone)
                    Serial.println("Metronome Engine STARTED (Standalone).");
               }
          }
          
          Serial.println("Metronome Sound ENABLED.");
     }
}

void Metronome::stop() {
     if (state != METRONOME_STOPPED) {
          state = METRONOME_STOPPED;

          if (!musicEngineRunning) {
               metronomeTimer.end();
               engineRunning = false;
               Serial.println("Metronome Engine STOPPED (Standalone).");
          }
          
          Serial.println("Metronome Sound DISABLED.");
     }
}

void Metronome::syncStart() {
     musicEngineRunning = true;
     
     if (!engineRunning) {
          metronomeTimer.begin(metronomeTickHandler, 1000); // 1ms tick
          engineRunning = true;
     }
     
     tickTimer = 0;
     isFirstBeat = true;

     Serial.print("Metronome Engine Synced. BPM: ");
     Serial.print(currentBPM);
     Serial.print(", Offset: ");
     Serial.print(offsetMillis);
     Serial.println(" ms. Sound state unchanged.");
}

void Metronome::syncStop() {
     musicEngineRunning = false;
     Serial.println("Metronome Engine Stopped (Music Ended).");

     if (state == METRONOME_STOPPED) {
          if (engineRunning) {
               metronomeTimer.end();
               engineRunning = false;
               Serial.println("Metronome Engine Halted.");
          }
     }
}

void Metronome::loadSongInfo(const SongMetronomeData& data) {
     setBPM(data.bpm);
     setOffset(data.offsetMillis);
     Serial.print("Loaded new song info: ");
     Serial.print(currentBPM);
     Serial.print(" BPM, ");
     Serial.print(offsetMillis);
     Serial.println(" ms offset.");
}

void Metronome::tick() {
     if (!engineRunning) return;

     // Logic:
     // 1. We check if enough time has passed for a beat.
     // 2. Ideally, beat time = usInterval.
     // 3. HOWEVER, for the FIRST beat, we add/subtract the offset.
     
     // Determine the required time for THIS specific beat
     unsigned long targetInterval = usInterval;

     if (isFirstBeat) {
          // Apply offset (convert ms to us)
          // If offset is positive (+50ms), first beat takes LONGER to arrive (delayed start)
          // If offset is negative (-50ms), first beat arrives SOONER
          long offsetUs = (long)offsetMillis * 1000;
          
          if ((long)usInterval + offsetUs > 1000) {
               targetInterval = usInterval + offsetUs;
          } else {
               targetInterval = 1000; // Minimum safety delay (1ms)
          }
     }

     // Check if time has passed (convert tickTimer millis to micros for precision)
     if (tickTimer * 1000 >= targetInterval) {
          
          // --- AUDIO OUTPUT ---
          // Only play if the user has ENABLED the metronome in the menu
          if (state == METRONOME_RUNNING) {
               tickSynth->noteOn();
          }
          // --------------------

          // Reset for next beat
          tickTimer = 0;
          isFirstBeat = false; // Subsequent beats use standard usInterval
     }
}
