//
//  Voice.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Addapted from the Voice class of Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//

#ifndef __SpaceBass__Voice__
#define __SpaceBass__Voice__

#include "EnvelopeGenerator.h"
#include "Synth.h"

class Voice {
public:
    friend class VoiceManager;
    
    Voice():
    mNoteNumber(-1),
    mVelocity(0),
    isActive(false)
    {
        // Set free everytime volume envelope has fully faded out of RELEASE stage: (connect the setFree method to the finishedEnvelopeCycle signal)
        mEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
        mEnvelope.changedEnvelopeStage.Connect(&mSynth, &Synth::setEnvelopeStage);
    };
    
    Synth* getSynth();
    
    inline void setNoteNumber(int noteNumber) {
        mNoteNumber = noteNumber;
        //Calulate frequnecy of note with base note of A4 (440.0 hz)
        double frequency = 440.0 * pow(2.0, (mNoteNumber - 69.0) / 12.0);
        mSynth.setFrequency(frequency);
    }
    
    double nextSample();
    void setFree();
    
    void reset();
    
private:
    Synth mSynth;
    EnvelopeGenerator mEnvelope;
    
    int mNoteNumber;
    int mVelocity;
    bool isActive;
};


#endif /* defined(__SpaceBass__Voice__) */