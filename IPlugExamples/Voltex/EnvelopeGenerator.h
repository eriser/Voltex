//
//  EnvelopeGenerator.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Based on EnvelopeGenerator from Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//

#ifndef __Voltex__EnvelopeGenerator__
#define __Voltex__EnvelopeGenerator__

#include <cmath>

#include "GallantSignal.h"
using Gallant::Signal0;
using Gallant::Signal1;

class EnvelopeGenerator {
public:
    enum EnvelopeStage {
        ENVELOPE_STAGE_OFF = 0,
        ENVELOPE_STAGE_ATTACK,
        ENVELOPE_STAGE_DECAY,
        ENVELOPE_STAGE_SUSTAIN,
        ENVELOPE_STAGE_RELEASE,
        kNumEnvelopeStages
    };
    void enterStage(EnvelopeStage newStage);
    double nextSample();
    static void setSampleRate(double newSampleRate);
    inline EnvelopeStage getCurrentStage() const { return currentStage; };
    const double minimumLevel;
    
    EnvelopeGenerator() :
    minimumLevel(0.0001),
    currentStage(ENVELOPE_STAGE_OFF),
    currentLevel(minimumLevel),
    multiplier(1.0),
    currentSampleIndex(0),
    nextStageSampleIndex(0)
    {
        stageValue[ENVELOPE_STAGE_OFF] = 0.0;
        stageValue[ENVELOPE_STAGE_ATTACK] = 0.01;
        stageValue[ENVELOPE_STAGE_DECAY] = 0.5;
        stageValue[ENVELOPE_STAGE_SUSTAIN] = 0.1;
        stageValue[ENVELOPE_STAGE_RELEASE] = 1.0;
        //default values are piano like
    };
    
    void setStageValue(EnvelopeStage stage, double value);
    
    Signal0<> beganEnvelopeCycle;
    Signal0<> finishedEnvelopeCycle;
    Signal1<int> changedEnvelopeStage;
    
    void reset() {
        currentStage = ENVELOPE_STAGE_OFF;
        currentLevel = minimumLevel;
        multiplier = 1.0;
        currentSampleIndex = 0;
        nextStageSampleIndex = 0;
    }
private:
    EnvelopeStage currentStage;
    double currentLevel;
    double multiplier;
    static double sampleRate;
    double stageValue[kNumEnvelopeStages];
    void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
    unsigned long long currentSampleIndex;
    unsigned long long nextStageSampleIndex;
};

#endif /* defined(__Voltex__EnvelopeGenerator__) */
