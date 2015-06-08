#ifndef __VOLTEX__
#define __VOLTEX__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "MIDIReceiver.h"
#include "VoiceManager.h"
#include "VectorSpace.h"

#define NUM_TABLES  8
#define NUM_PRESETS 5

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
    
    int getNumParams();
    int getVectorSpacePrecision();
    
    std::tr1::array<WaveTable*, NUM_TABLES> waveTables;
    std::tr1::array<VectorSpace*, NUM_TABLES> vectorSpaces;
    
private:
    void CreatePresets();
    MIDIReceiver mMIDIReceiver;
    
    IControl* mVirtualKeyboard;
    void processVirtualKeyboard();
    
    IControl* toolCursor;
    IControl* toolPencil;
    IControl* toolSelection;
    IControl* toolDelete;
    
	IControl* load;
    IControl* presets;
    
    VoiceManager voiceManager;
    

    IGraphics* pGraphics;
    
    void CreateParams();
    void CreateGraphics();
	void PresetOsc(int c);
    void updateWaveTable(int table);
    
    double gain;
    
    void processMLoad(IParam* param);
    void saveToFile();
    void loadFromFile();
    
    bool firstUpdate;
	int OscLengthVal[7];

};

#endif
