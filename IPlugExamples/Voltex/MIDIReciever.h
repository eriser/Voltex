//
//  MIDIReciever.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//  Based on MIDIReceiver from Martin Finke's "Making Audio Plugins"
//      http://martin-finke.de/blog/tags/making_audio_plugins.html
//

// MIDIReciever wraps a midi cue and handles midi events. It passes note on and off messages to the rest of the system via signals and slots

#ifndef __SpaceBass__MIDIReceiver__
#define __SpaceBass__MIDIReceiver__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "IMidiQueue.h"
#include "GallantSignal.h"

using Gallant::Signal2;

class MIDIReceiver {
private:
    IMidiQueue mMidiQueue;
    static const int keyCount = 128;
    int mNumKeys; // how many keys are being played
    bool mKeyStatus[keyCount]; // status for each note
    int mOffset;
    
public:
    MIDIReceiver() :
    mNumKeys(0),
    mOffset(0) {
        for (int i = 0; i < keyCount; i++) {
            mKeyStatus[i] = false;
        }
    };
    
    Signal2<int, int> noteOn;
    Signal2<int, int> noteOff;
    
    // Returns true if the key with a given index is currently pressed
    inline bool getKeyStatus(int keyIndex) const { return mKeyStatus[keyIndex]; }
    // Returns the number of keys currently pressed
    inline int getNumKeys() const { return mNumKeys; }
    void advance();
    void onMessageReceived(IMidiMsg* midiMessage);
    inline void Flush(int nFrames) { mMidiQueue.Flush(nFrames); mOffset = 0; }
    inline void Resize(int blockSize) { mMidiQueue.Resize(blockSize); }
};

#endif