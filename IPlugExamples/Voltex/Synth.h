//
//  Synth.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Inspired by the Oscillator class from Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//

#ifndef __Voltex__Synth__
#define __Voltex__Synth__

#include <math.h>
#include <tr1/array>

#include "WaveTable.h"

class Synth {
public:
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void setWavetables(std::tr1::array<WaveTable*, 8> *tables);
        
    double nextSample();
    
    Synth() :
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    mFrequency(440.0),
    mPhase(0.0),
    mPhaseIncrement(1.0)
    {};
    
    void reset();
private:
    const double mPI;
    const double twoPI;
    double mFrequency;
    double mPhase;
    double mPhaseIncrement;
    static double mSampleRate;
    
    void calculatePhaseIncrement();
    
    
    std::tr1::array<WaveTable*, 8> *tables;
};

#endif /* defined(__Voltex__Synth__) */
