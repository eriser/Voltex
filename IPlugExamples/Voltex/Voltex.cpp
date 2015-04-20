#include "Voltex.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain"
#include "IPlug_include_in_plug_src.h"
#pragma clang diagnostic pop
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"

#include <math.h>
#include <algorithm>

#include <tr1/array>

#include <functional>

const int kNumPrograms = 5;
const double parameterStep = 0.001;

enum EParams {
    // Volume Envelope:
    mVolumeEnvAttack = 0,
    mVolumeEnvDecay,
    mVolumeEnvSustain,
    mVolumeEnvRelease,
    kNumParams
};

enum ELayout {
    kWidth = GUI_WIDTH,
    kHeight = GUI_HEIGHT,
    kKeybX = 1,
    kKeybY = 234
};

Voltex::Voltex(IPlugInstanceInfo instanceInfo) : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), lastVirtualKeyboardNoteNumber(virtualKeyboardMinimumNoteNumber - 1) {
    TRACE;
    
    WaveTable* squareTable;
    squareTable = new WaveTable();
    
    std::tr1::array<double, 2048> values;
    for (int i = 0; i < 2048; i++) {
        if (i < 1024) {
            values[i] = -1.0;
        } else {
            values[i] = 1.0;
        }
    }
    squareTable->setValues(values);
    waveTables[0] = squareTable;

    WaveTable* sineTable;
    sineTable = new WaveTable();
    
    std::tr1::array<double, 2048> sinValues;
    for (int i = 0; i < 2048; i++) {
        sinValues[i] = sin((i / 2048.0) * (4 * acos(0.0)));
    }
    sineTable->setValues(sinValues);
    waveTables[1] = sineTable;
    
    voiceManager.setWavetables(&waveTables);
    
    CreateParams();
    CreateGraphics();
    CreatePresets();
    
    mMIDIReceiver.noteOn.Connect(&voiceManager, &VoiceManager::onNoteOn);
    mMIDIReceiver.noteOff.Connect(&voiceManager, &VoiceManager::onNoteOff);
}

void Voltex::CreateParams() {
    GetParam(mVolumeEnvAttack)->InitDouble("Volume Env Attack", 0.01, 0.01, 10.0, parameterStep);
    GetParam(mVolumeEnvDecay)->InitDouble("Volume Env Decay", 0.5, 0.01, 15.0, parameterStep);
    GetParam(mVolumeEnvSustain)->InitDouble("Volume Env Sustain", 0.1, 0.001, 1.0, parameterStep);
    GetParam(mVolumeEnvRelease)->InitDouble("Volume Env Release", 1.0, 0.001, 15.0, parameterStep);
    
    GetParam(mVolumeEnvAttack)->SetShape(3);
    GetParam(mVolumeEnvDecay)->SetShape(3);
    GetParam(mVolumeEnvSustain)->SetShape(2);
    GetParam(mVolumeEnvRelease)->SetShape(3);
    
    for (int i = 0; i < kNumParams; i++) {
        OnParamChange(i);
    }
}

void Voltex::CreateGraphics() {
    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachBackground(BG_ID, BG_FN);
    
    IBitmap whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
    IBitmap blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);
    
    //                            C#     D#          F#      G#      A#
    int keyCoordinates[12] = { 0, 7, 12, 20, 24, 36, 43, 48, 56, 60, 69, 72 };
    mVirtualKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, virtualKeyboardMinimumNoteNumber, /* octaves: */ 5, &whiteKeyImage, &blackKeyImage, keyCoordinates);
    
    pGraphics->AttachControl(mVirtualKeyboard);
    
    
    IBitmap knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 64);
    
    pGraphics->AttachControl(new IKnobMultiControl(this, 27, 144, mVolumeEnvAttack, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, 131, 144, mVolumeEnvDecay, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, 236, 144, mVolumeEnvSustain, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, 343, 144, mVolumeEnvRelease, &knobBitmap));
    
    
    AttachGraphics(pGraphics);
}



Voltex::~Voltex() {}

void Voltex::CreatePresets() {
}

void Voltex::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
    // Mutex is already locked for us.
    
    //The goal here is to overwrite whatever comes in with the compined output of all the voices. If the plugin is loaded corectly there will be nothing coming in, so we will be replacing an empty array with the values that we want.
    double *leftOutput = outputs[0];
    double *rightOutput = outputs[1];
    processVirtualKeyboard();
    for (int i = 0; i < nFrames; ++i) {
        mMIDIReceiver.advance();
        //The left and right channels are equal as our synth only works in mono
        leftOutput[i] = rightOutput[i] = voiceManager.nextSample();
    }
    
    mMIDIReceiver.Flush(nFrames);
}

void Voltex::Reset() {
    TRACE;
    IMutexLock lock(this);
    double sampleRate = GetSampleRate();
    voiceManager.setSampleRate(sampleRate);
}

void Voltex::OnParamChange(int paramIdx) {
    IMutexLock lock(this);
    IParam* param = GetParam(paramIdx);
    using std::tr1::placeholders::_1;
    using std::tr1::bind;
    VoiceManager::VoiceChangerFunction changer;
    switch(paramIdx) {
        // Volume Envelope:
        case mVolumeEnvAttack:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
            break;
        case mVolumeEnvDecay:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
            break;
        case mVolumeEnvSustain:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
            break;
        case mVolumeEnvRelease:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
            break;
        }
        voiceManager.changeAllVoices(changer);
}

void Voltex::ProcessMidiMsg(IMidiMsg* pMsg) {
    mMIDIReceiver.onMessageReceived(pMsg);
    mVirtualKeyboard->SetDirty();
}

void Voltex::processVirtualKeyboard() {
    IKeyboardControl* virtualKeyboard = (IKeyboardControl*) mVirtualKeyboard;
    int virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + virtualKeyboardMinimumNoteNumber;
    
    if(lastVirtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
        // The note number has changed from a valid key to something else (valid key or nothing). Release the valid key:
        IMidiMsg midiMessage;
        midiMessage.MakeNoteOffMsg(lastVirtualKeyboardNoteNumber, 0);
        mMIDIReceiver.onMessageReceived(&midiMessage);
    }
    
    if (virtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
        // A valid key is pressed that wasn't pressed the previous call. Send a "note on" message to the MIDI receiver:
        IMidiMsg midiMessage;
        midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
        mMIDIReceiver.onMessageReceived(&midiMessage);
    }
    lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}