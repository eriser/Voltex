#ifndef __VOLTEX__
#define __VOLTEX__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "MIDIReceiver.h"
#include "VoiceManager.h"

class Voltex : public IPlug
{
public:
    Voltex(IPlugInstanceInfo instanceInfo);
    ~Voltex();
    
    void Reset();
    void OnParamChange(int paramIdx);
    void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
    // to receive MIDI messages:
    void ProcessMidiMsg(IMidiMsg* pMsg);
    
    // Needed for the GUI keyboard:
    // Should return non-zero if one or more keys are playing.
    inline int GetNumKeys() const { return mMIDIReceiver.getNumKeys(); };
    // Should return true if the specified key is playing.
    inline bool GetKeyStatus(int key) const { return mMIDIReceiver.getKeyStatus(key); };
    static const int virtualKeyboardMinimumNoteNumber = 48;
    int lastVirtualKeyboardNoteNumber;
    
	

private:
    void CreatePresets();
    MIDIReceiver mMIDIReceiver;
    
    IControl* mVirtualKeyboard;
    void processVirtualKeyboard();
    
    VoiceManager voiceManager;
    
    std::tr1::array<WaveTable*, 8> waveTables;
    
    void CreateParams();
    void CreateGraphics();
    
    double gain;
};

#endif
