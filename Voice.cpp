//
//  Voice.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "Voice.h"

double Voice::nextSample() {
    //Returns the sum of the synth with the volume adjusted by the notes envelope and velocity, if the synth is not active returns 0 without doing any calulations
    if (!isActive) return 0.0;
    double out = mSynth.nextSample() * mEnvelope.nextSample() * mVelocity / 127.0;
//    printf("%f\n", out);
    return out;
}



void Voice::setFree() {
    isActive = false;
}

void Voice::reset() {
    mNoteNumber = -1;
    mVelocity = 0;
    mSynth.reset();
    mEnvelope.reset();
}
