//
//  Synth.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Header addapted from the Oscillator class of Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//
//
//  The synth is responsable for pitch shifting the wavetable as well as applying it's gain and the
//  envelope.
//
//
//  A note about "Adapted from" and "Based on":
//  The structure of the signal/midi chain is an extention of Martin Finke's "Making Audio Plugins".
//  Many of the headers are mdified version of the one used in the audio plugins tutorial. The
//  the implementations are either completly original or versions that have been heavily modified for
//  Voltex.
//

#ifndef __Voltex__Synth__
#define __Voltex__Synth__

#include <math.h>

#ifdef WIN32
#include <array>
#else
#include <tr1/array>
#endif

#include "WaveTable.h"
#include "EnvelopeGenerator.h"

class Synth {
public:
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void setWavetables(std::tr1::array<WaveTable*, 8> *tables);
    
    void setEnvelopeStage(EnvelopeGenerator::EnvelopeStage stage);
        
    double nextSample();
    
    Synth() :
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    mFrequency(440.0),
    mPhase(0.0),
    mPhaseIncrement(1.0),
    mEnvelopeStage(EnvelopeGenerator::ENVELOPE_STAGE_OFF)
    {
        
    }
    
    
    void reset();
private:
    const double mPI;
    const double twoPI;
    double mFrequency;
    double mPhase;
    double mPhaseIncrement;
    EnvelopeGenerator::EnvelopeStage mEnvelopeStage;
    static double mSampleRate;
    
    void calculatePhaseIncrement();
  
    std::tr1::array<WaveTable*, 8> *tables;
};

#endif /* defined(__Voltex__Synth__) */
