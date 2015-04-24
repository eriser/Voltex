//
//  VoiceManager.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Addapted from the VoiceManager class of Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//

#ifndef __SpaceBass__VoiceManager__
#define __SpaceBass__VoiceManager__

#include "Voice.h"
#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

class VoiceManager {
public:
    void onNoteOn(int noteNumber, int velocity);
    void onNoteOff(int noteNumber, int velocity);
    double nextSample();
    
    void setSampleRate(double sampleRate) {
        EnvelopeGenerator::setSampleRate(sampleRate);
        for (int i = 0; i < NumberOfVoices; i++) {
            Voice& voice = voices[i];
            voice.mSynth.setSampleRate(sampleRate);
        }
    }
    
    void setWavetables (std::tr1::array<WaveTable*, 8> *waveTables) {
        for (int i = 0; i < NumberOfVoices; i++) {
            Voice& voice = voices[i];
            voice.mSynth.setWavetables(waveTables);
        }
    }
    
    typedef std::tr1::function<void (Voice&)> VoiceChangerFunction;
    
    inline void changeAllVoices(VoiceChangerFunction changer) {
        for (int i = 0; i < NumberOfVoices; i++) {
            changer(voices[i]);
        }
    }
    
    static void setVolumeEnvelopeStageValue(Voice& voice, EnvelopeGenerator::EnvelopeStage stage, double value) {
        voice.mEnvelope.setStageValue(stage, value);
    }
private:
    static const int NumberOfVoices = 64;
    Voice voices[NumberOfVoices];
    Voice* findFreeVoice();
};

#endif /* defined(__SpaceBass__VoiceManager__) */
