//
//  VoiceManager.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "VoiceManager.h"

Voice VoiceManager::getVoice(int index) {
    return voices[index];
}

Voice* VoiceManager::findFreeVoice() {
    Voice* freeVoice = NULL;
    for (int i = 0; i < NumberOfVoices; i++) {
        if (!voices[i].isActive) {
            freeVoice = &(voices[i]);
            break;
        }
    }
    return freeVoice;
}

void VoiceManager::onNoteOn(int noteNumber, int velocity) {
    printf("VoiceManager Note On\n");
    //Find a free voice, if there are no free voices it means that someone is either playing a black MIDI file of leaning on a MIDI keyboard, either way they won't notice that the 65th note didn't play so implementing vocie "stealing" (forcing a voice to become free) is not a priority. If it becomes a problem we could go up to 96 voices.
    Voice* voice = findFreeVoice();
    if (!voice) {
        return;
    }
    //Make sure that our voice is shiny and new (we do this when we are about to use it since we don't want to waste time reseting a vocie that might not every be used again)
    voice->reset();
    voice->setNoteNumber(noteNumber);
    //Velocity is essentialy a respresentation of how hard the key is being pressed, we use it to directly effect the volume of the note.
    voice->mVelocity = velocity;
    voice->isActive = true;
    //Start the envelope
    voice->mEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK);
    voice->mSynth.onNoteOn(noteNumber, velocity);
}

void VoiceManager::onNoteOff(int noteNumber, int velocity) {
    printf("VoiceManager Note Off\n");
    // Find the voice(s) with the given noteNumber:
    for (int i = 0; i < NumberOfVoices; i++) {
        Voice& voice = voices[i];
        if (voice.isActive && voice.mNoteNumber == noteNumber) {
            //The voice does not turn off when the key is not being pressed, it goes into the release stage so that the sound can be faded out. If the note didn't fade their would be a sharp click whenever a note stoped playing since the waveform would be cut. The release phase of the envelope is also used for effect, it is alot like the right pedal of a piano.
            voice.mEnvelope.enterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE);
            voice.mSynth.onNoteOff(noteNumber, velocity);
        }
    }
}

double VoiceManager::nextSample() {
    //Returns the sum of all voices.
    double output = 0.0;
    for (int i = 0; i < NumberOfVoices; i++) {
        Voice& voice = voices[i];
        output += voice.nextSample();
    }
    //Unlike inside of each synth the voices are not averaged, they are summed. This recreates the effect of multiple notes being louder then a single note. There is a small chance that this could become too loud if all of the notes happen to be in phase but this is not likely (the same chance exists with a piano or other physical insturment that playes multiple notes at the same time).
    return output;
}