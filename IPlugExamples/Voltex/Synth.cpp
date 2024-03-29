//
//  Synth.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "Synth.h"

double Synth::mSampleRate = 44100.0;

void Synth::reset() {
    mPhase = 0.0;
    mPhaseIncrement = 0.0;
}

void Synth::setFrequency(double frequency) {
    mFrequency = frequency;
    calculatePhaseIncrement();
}

void Synth::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    calculatePhaseIncrement();
}

void Synth::calculatePhaseIncrement() {
    // Phase increment for fundemental frequency of f is geven by: (p = N * f * T) where n is the number of samples in the wavetable and T is the sample period (wavelength, (T = 1/f))
    // At this level we asume that their are two pi samples, this is scaled in the wavetable class. This way the synth does not need to know how many samples thier actually are.
    // p            = N          * f      * T
    mPhaseIncrement = mFrequency * TWO_PI * (1 / mSampleRate);
}

double Synth::nextSample() {
    double value = 0.0;
    
    int numTables = 0;
    
    //mix all of the wavetables together
    for (int i = 0 ; i < 8; i++) {
        if ((*tables)[i]->isEnabled()) {
            value +=(*tables)[i]->getValueAt(mPhase) * (*tables)[i]->getGain() * (*tables)[i]->getMixValue(mEnvelopeStage);
            numTables++;
        }
    }
    
    value = value / numTables; // If we didn't do this the synth would clip if there was more than one wavtable (in phase). This keeps the volume the same no matter how many tables are in use.
    
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI; // loop back to the begining of the wavetable. We have to subtract twoPi instead of just going back to zero in order to preserve the phase increment beetween samples. This is done in a loop incase the phase increment is really huge.
    }
    return value;
}

void Synth::setEnvelopeStage(EnvelopeGenerator::EnvelopeStage stage) {
    mEnvelopeStage = stage;
}

void Synth::setWavetables(std::tr1::array<WaveTable*, 8> *newTables) {
    tables = newTables;
}