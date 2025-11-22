#ifndef METRONOME_H
#define METRONOME_H

#include <IntervalTimer.h>

enum MetronomeState { METRONOME_RUNNING, METRONOME_STOPPED };

struct SongMetronomeData {
     int bpm;
     int offsetMillis;
};

class AudioSynthSimpleDrum;

class Metronome {
public:
     // Constructor might take audio components or timer references
     Metronome(AudioSynthSimpleDrum* drumSynth);

     void begin();
     
     void setBPM(int bpm);
     int getBPM() const { return currentBPM; }

     void setOffset(int offsetMillis); 
     int getOffset() const { return offsetMillis; }

     void start();
     void stop();
     
     MetronomeState getState() const { return state; }

     void loadSongInfo(const SongMetronomeData& data);

     static void metronomeTickHandler();

private:
     AudioSynthSimpleDrum* tickSynth;

     MetronomeState state = METRONOME_STOPPED;
     
     int currentBPM = 120;
     int offsetMillis = 0;

     IntervalTimer metronomeTimer;
     
     void tick();

     static Metronome* instance;
};

#endif