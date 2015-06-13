//
//  VoiceManager.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Adapted from the VoiceManager class of Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//
//
//  The VoiceManager is the top of the singal chain, it begins to abstract MIDI data.
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
    
    Voice getVoice(int index);
    
    void setSampleRate(double sampleRate) {
        //Update all of the members sample rates.
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
    
    //When changing paramaters in all of the voices we use the functional library. This would have been more worth it if there where more paramters to change. Soemthing bery similar was done in some of the tutorials I use to learn about the WDL-OL library and it seemed like a good idea.
    //Basicaly we create a VoiceChangerFunction by filling in all of the paramaters in the setVolumeEnvelopeStageValue() function except for the voice paramater. The function with filled in the values that we want to update then the changeAllVoices() function just has to iterate through the voices and pass the voice argument. This means that the code changing the paramaters does nto have to be aware of the voices.
    typedef std::tr1::function<void (Voice&)> VoiceChangerFunction;
    
    inline void changeAllVoices(VoiceChangerFunction changer) {
        for (int i = 0; i < NumberOfVoices; i++) {
            changer(voices[i]);
        }
    }
    
    static void setVolumeEnvelopeStageValue(Voice& voice, EnvelopeGenerator::EnvelopeStage stage, double value) {
        voice.mEnvelope.setStageValue(stage, value);
    }
    
    static const int NumberOfVoices = 64;
private:
    Voice voices[NumberOfVoices];
    Voice* findFreeVoice();
};

#endif /* defined(__SpaceBass__VoiceManager__) */
