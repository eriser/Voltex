//
//  Voice.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "Voice.h"

double Voice::nextSample() {
    //Returns the sum of the synth with the volume adjusted by the notes envelope and velocity, if the synth is not active returns 0 without doing any calulations. The velocity is scaled from a seven bit value to a double between 0 and 1 to be used as a gain value.
    if (!isActive) return 0.0;
    return mSynth.nextSample() * mEnvelope.nextSample() * mVelocity / 127.0;
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

Synth* Voice::getSynth() {
    return &mSynth;
}
