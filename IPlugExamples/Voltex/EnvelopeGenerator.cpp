//
//  EnvelopeGenerator.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "EnvelopeGenerator.h"

double EnvelopeGenerator::sampleRate = 44100.0;

double EnvelopeGenerator::nextSample() {
    if (currentStage != ENVELOPE_STAGE_OFF && currentStage != ENVELOPE_STAGE_SUSTAIN) { //Changing the off or sustain stages requires midi input, not counting samples
        if (currentSampleIndex == nextStageSampleIndex) {
            //We decide when to move to the next stage by number of samples
            EnvelopeStage newStage = static_cast<EnvelopeStage>((currentStage + 1) % kNumEnvelopeStages);
            enterStage(newStage);
        }
        //level is updated with multipliers to make beutifuly smooth fades
        currentLevel *= multiplier;
        currentSampleIndex++;
    }
    return currentLevel;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples) {
    //Calculate a multiplier for a nice smooth exponential fade
    multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
    //Maxim - This is what Mr. Kesner was hearing when he said that our fades where so nice and smooth. Many plugins just use a linear fade but this sounds much better.
}

void EnvelopeGenerator::enterStage(EnvelopeStage newStage) {
    if (currentStage == newStage) return; //nothing to do here
    if (currentStage == ENVELOPE_STAGE_OFF) {
        if (emitSignals)
            beganEnvelopeCycle(); //Send signal
    }
    if (newStage == ENVELOPE_STAGE_OFF) {
        if (emitSignals)
            finishedEnvelopeCycle(); //Send signal
    }
    
    currentStage = newStage;
    currentSampleIndex = 0;
    if (currentStage == ENVELOPE_STAGE_OFF ||
        currentStage == ENVELOPE_STAGE_SUSTAIN) {
        nextStageSampleIndex = 0; //theses stages keep going until a MIDI key is pressed or released
    } else {
        nextStageSampleIndex = stageValue[currentStage] * sampleRate; //convert user friendly seconds to alogrithm friendly samples
    }
    switch (newStage) {
        case ENVELOPE_STAGE_OFF: //No sound is produced, the level is zero and it always stays the same
            currentLevel = 0.0;
            multiplier = 1.0;
            break;
        case ENVELOPE_STAGE_ATTACK:
            currentLevel = minimumLevel; //we need to start at the bottom (not a Drake refrence)
            calculateMultiplier(currentLevel, 1.0, nextStageSampleIndex); //calculate a nice fade to the top in however many sample we have
            break;
        case ENVELOPE_STAGE_DECAY:
            currentLevel = 1.0; //this time we are starting at the top
            calculateMultiplier(currentLevel, fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel), nextStageSampleIndex); //calculate a fade down to either the sustaion level or the minimumLevel (whichever is higher) in the time alocated by the user for decay.
            break;
        case ENVELOPE_STAGE_SUSTAIN:
            currentLevel = stageValue[ENVELOPE_STAGE_SUSTAIN]; //no fades here, we just keep it at the preset volume for sustain
            multiplier = 1.0;
            break;
        case ENVELOPE_STAGE_RELEASE:
            // We could go from ATTACK/DECAY to RELEASE, so we're not changing currentLevel here.
            calculateMultiplier(currentLevel, minimumLevel, nextStageSampleIndex);
            break;
        default:
            break; //something went wrong if we end up here
    }
}

void EnvelopeGenerator::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate; //high tech
    
    //the envelope timeing will be incorrect if the sample rate changes during an attack decay or release stage. Fortunatly that will probably never happen, the user would have to be activly playing notes and adjusting their settings at the same time, while possible the glitchyness of our prefrences window makes it diffcult.
}

void EnvelopeGenerator::setStageValue(EnvelopeStage stage, double value) {
    stageValue[stage] = value;
    if (stage == currentStage) {
        // Re-calculate the multiplier and nextStageSampleIndex
        if(currentStage == ENVELOPE_STAGE_ATTACK ||
           currentStage == ENVELOPE_STAGE_DECAY ||
           currentStage == ENVELOPE_STAGE_RELEASE) {
            double nextLevelValue;
            switch (currentStage) {
                case ENVELOPE_STAGE_ATTACK:
                    nextLevelValue = 1.0;
                    break;
                case ENVELOPE_STAGE_DECAY:
                    nextLevelValue = fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel);
                    break;
                case ENVELOPE_STAGE_RELEASE:
                    nextLevelValue = minimumLevel;
                    break;
                default:
                    break;
            }
            // How far the generator is into the current stage:
            double currentStageProcess = (currentSampleIndex + 0.0) / nextStageSampleIndex;
            // How much of the current stage is left:
            double remainingStageProcess = 1.0 - currentStageProcess;
            unsigned long long samplesUntilNextStage = remainingStageProcess * value * sampleRate;
            nextStageSampleIndex = currentSampleIndex + samplesUntilNextStage;
            calculateMultiplier(currentLevel, nextLevelValue, samplesUntilNextStage);
        } else if(currentStage == ENVELOPE_STAGE_SUSTAIN) {
            currentLevel = value;
        }
    }
    if (currentStage == ENVELOPE_STAGE_DECAY &&
        stage == ENVELOPE_STAGE_SUSTAIN) {
        // We have to decay to a different sustain value than before.
        // Re-calculate multiplier:
        unsigned long long samplesUntilNextStage = nextStageSampleIndex - currentSampleIndex;
        calculateMultiplier(currentLevel, fmax(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel), samplesUntilNextStage);
    }
}
