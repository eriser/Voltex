//
//  MIDIReceiver.cpp
//  Voltex
//
//

#include "MIDIReceiver.h"

void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {
    IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
    // We're only interested in Note On/Off messages (not CC, pitch, etc.)
    if(status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
        mMidiQueue.Add(midiMessage);
    }
}

void MIDIReceiver::advance() {
    while (!mMidiQueue.Empty()) {
        IMidiMsg* midiMessage = mMidiQueue.Peek(); //get the next message
        if (midiMessage->mOffset > mOffset) break;
        
        IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
        int noteNumber = midiMessage->NoteNumber();
        int velocity = midiMessage->Velocity();
        // There are only note on/off messages in the queue, see ::OnMessageReceived
        // Update key staus and num keys and send signal.
        if (status == IMidiMsg::kNoteOn && velocity) {
            if(mKeyStatus[noteNumber] == false) {
                mKeyStatus[noteNumber] = true;
                mNumKeys += 1;
                noteOn(noteNumber, velocity);
            }
        } else {
            if(mKeyStatus[noteNumber] == true) {
                mKeyStatus[noteNumber] = false;
                mNumKeys -= 1;
                noteOff(noteNumber, velocity);
            }
        }
        mMidiQueue.Remove(); //Remove processed mesage from queue
    }
    mOffset++;
}