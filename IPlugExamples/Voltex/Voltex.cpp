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

#ifdef WIN32
#include <array>
#include <functional>
#else
#include <tr1/array>
#include <tr1/functional>
#endif


const int kNumPrograms = 5;
const double parameterStep = 0.001;

// Global paramaters
enum EParams {
    // Volume Envelope:
    mVolumeEnvAttack = 0,
    mVolumeEnvDecay,
    mVolumeEnvSustain,
    mVolumeEnvRelease,
    
    // Master
    mGain,
    
	//Tabs
	mTabOne,
	mTabTwo,
	mTabThree,
	mTabFour, 
	mTabFive,
	mTabSix,
	mTabSeven,
	mTabEight,

    //Switches
    mSwitchOne,
    mSwitchTwo,
    mSwitchThree,
    mSwitchFour,
    mSwitchFive,
    mSwitchSix,
    mSwitchSeven,
    mSwitchEight,
    
    // Table
    mAttackOne,
    mAttackTwo,
    mAttackThree,
    mAttackFour,
    mAttackFive,
    mAttackSix,
    mAttackSeven,
    mAttackEight,

    mDecayOne,
    mDecayTwo,
    mDecayThree,
    mDecayFour,
    mDecayFive,
    mDecaySix,
    mDecaySeven,
    mDecayEight,
    
    mSustainOne,
    mSustainTwo,
    mSustainThree,
    mSustainFour,
    mSustainFive,
    mSustainSix,
    mSustainSeven,
    mSustainEight,
    
    mReleaseOne,
    mReleaseTwo,
    mReleaseThree,
    mReleaseFour,
    mReleaseFive,
    mReleaseSix,
    mReleaseSeven,
    mReleaseEight,
    
    mGainOne,
    mGainTwo,
    mRGainThree,
    mGainFour,
    mGainFive,
    mGainSix,
    mGainSeven,
    mGainEight,
    
    kNumParams
};

//Layout paramaters
enum ELayout {
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	kKeybX = 166,
	kKeybY = 614,

	//Master Section:
	kMasterX = 793,
	kMasterY = 53,

	kMasterAttackX = 693,
	kMasterDecayX = kMasterAttackX + 65,
	kMasterSustainX = kMasterDecayX + 66,
	kMasterReleaseX = kMasterSustainX + 66,

	kMasterEnvelopeY = 231,


	//Table:
	kTableX = 232,
	kTableY = 98,
	kTableSpaceX = 43,
	kTableSpaceY = 40,
	kTableSpaceGainY = 50,

	//On/off Buttons
	kSwitchX = 325,
	kSwitchY = 358,
	kSwitchSpaceX = 86,

	//tabs
	kTabX = 272,
	kTabY = 344,
	kTabNum = 8,
	kTabXDifference = kSwitchX - kTabX

};

enum ETab {
	
};

Voltex::Voltex(IPlugInstanceInfo instanceInfo) : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), lastVirtualKeyboardNoteNumber(virtualKeyboardMinimumNoteNumber - 1) {
    TRACE;
    
    //initialize wavetables
    for (int i = 0; i < 8; i++) {
        waveTables[i] = new WaveTable();
    }
    
    
    //test tables
    /*    WaveTable* squareTable;
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
     waveTables[0] = squareTable; */
    
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
    
    //Connect noteOn and noteOff signals to slots in the voice manager, this way they voice manager does not need to know anything about the midi reciever and vice versa.
    mMIDIReceiver.noteOn.Connect(&voiceManager, &VoiceManager::onNoteOn);
    mMIDIReceiver.noteOff.Connect(&voiceManager, &VoiceManager::onNoteOff);
}

void Voltex::CreateParams() {
    //GetParam(int param)->InitDouble(defaultVal, minVal, maxVal, step, label);
    
    //Envelope
    GetParam(mVolumeEnvAttack)->InitDouble("Volume Env Attack", 0.01, 0.01, 5.0, parameterStep);
    GetParam(mVolumeEnvDecay)->InitDouble("Volume Env Decay", 0.5, 0.01, 15.0, parameterStep);
    GetParam(mVolumeEnvSustain)->InitDouble("Volume Env Sustain", 0.1, 0.001, 1.0, parameterStep);
    GetParam(mVolumeEnvRelease)->InitDouble("Volume Env Release", 1.0, 0.001, 15.0, parameterStep);
    
    GetParam(mVolumeEnvAttack)->SetShape(3);
    GetParam(mVolumeEnvDecay)->SetShape(3);
    GetParam(mVolumeEnvSustain)->SetShape(2);
    GetParam(mVolumeEnvRelease)->SetShape(3);
    
    //Master
    GetParam(mGain)->InitDouble("Gain", 80.0, 0.0, 100.0, parameterStep);
    GetParam(mGain)->SetShape(2);
    
	//Tabs
	for (int i = mTabOne; i <= mTabEight; i++) {
		GetParam(i)->InitEnum("Wavetable Tab", 0, 8);
		GetParam(i)->SetDisplayText(0, "Wavetable Tab");
	}

    //Switches
    for (int i = mSwitchOne; i <= mSwitchEight; i++) {
        if (i == mSwitchOne) {
             GetParam(i)->InitEnum("Wavetable Switch", 1, 2);
        } else {
             GetParam(i)->InitEnum("Wavetable Switch", 0, 2);
        }
        GetParam(i)->SetDisplayText(0, "Wavetable Switch");
        
    }
    
    //Table:
    //Attack
    for (int i = mAttackOne; i <= mAttackEight; i++) {
        GetParam(i)->InitDouble("Volume Env Attack", 1.0, 0.0, 1.0, parameterStep);
        GetParam(i)->SetShape(3);
    }
    //Decay
    for (int i = mDecayOne; i <= mDecayEight; i++) {
        GetParam(i)->InitDouble("Volume Env Decay", 1.0, 0.0, 1.0, parameterStep);
        GetParam(i)->SetShape(3);
    }
    //Sustain
    for (int i = mSustainOne; i <= mSustainEight; i++) {
        GetParam(i)->InitDouble("Volume Env Sustain", 1.0, 0.0, 1.0, parameterStep);
        GetParam(i)->SetShape(2);
    }
    //Release
    for (int i = mReleaseOne; i <= mReleaseEight; i++) {
        GetParam(i)->InitDouble("Volume Env Release", 1.0, 0.0, 1.0, parameterStep);
        GetParam(i)->SetShape(3);
    }
    //Gain
    for (int i = mGainOne; i <= mGainEight; i++) {
        GetParam(i)->InitDouble("Gain", 75.0, 0.0, 100.0, parameterStep);
        GetParam(i)->SetShape(2);
    }
    
    //Initial param update
    for (int i = 0; i < kNumParams; i++) {
        OnParamChange(i);
    }
}

void Voltex::CreateGraphics() {
    //Get this plugins graphics instance and attach the background image
    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachBackground(BG_ID, BG_FN);
    
    //Initialize keyboard
    IBitmap whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
    IBitmap blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);
    
    //                            C#      D#          F#      G#        A#
    int keyCoordinates[12] = { 0, 15, 20, 37, 46, 68, 81, 96, 103, 118, 125, 140 };
    mVirtualKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, virtualKeyboardMinimumNoteNumber, /* octaves: */ 5, &whiteKeyImage, &blackKeyImage, keyCoordinates);
    pGraphics->AttachControl(mVirtualKeyboard);
    
    //Create on/off buttons and tabs
	
	IBitmap switches = pGraphics->LoadIBitmap(SWITCHES_ID, SWITCHES_FN, 2);
	IBitmap tab[kTabNum];
    for (int i = 0; i < kTabNum; i++) {
        char c[28];
        sprintf(c, TAB_FN, i + 1);
        tab[i] = pGraphics->LoadIBitmap(TAB_ONE_ID + i, c, 2);
    }

	int x = 0, y = 0;
	x = kSwitchX;
	for (int v = mSwitchOne; v <= mSwitchEight; v++) {
        if (v == mSwitchThree || v == mSwitchTwo) {
			pGraphics->AttachControl(new ISwitchControl(this, x - kTabXDifference - 1, kTabY, v, &tab[v - mSwitchOne]));
            pGraphics->AttachControl(new ISwitchControl(this, x - 1, kSwitchY, v, &switches));
        } else {
			pGraphics->AttachControl(new ISwitchControl(this, x - kTabXDifference, kTabY, v, &tab[v - mSwitchOne]));
			pGraphics->AttachControl(new ISwitchControl(this, x, kSwitchY, v, &switches));
		}
		x = kSwitchSpaceX + x;
	}

    //Create knobs
    IBitmap knobBitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, 64);
    
    //Envelope
    pGraphics->AttachControl(new IKnobMultiControl(this, kMasterAttackX, kMasterEnvelopeY, mVolumeEnvAttack, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, kMasterDecayX, kMasterEnvelopeY, mVolumeEnvDecay, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, kMasterSustainX, kMasterEnvelopeY, mVolumeEnvSustain, &knobBitmap));
    pGraphics->AttachControl(new IKnobMultiControl(this, kMasterReleaseX, kMasterEnvelopeY, mVolumeEnvRelease, &knobBitmap));
    
    //Master
    pGraphics->AttachControl(new IKnobMultiControl(this, kMasterX, kMasterY, mGain, &knobBitmap));
    
    
    //Table:
    x = kTableX, y = kTableY;
    //Attack
    for (int i = mAttackOne; i <= mAttackEight; i++) {
        pGraphics->AttachControl(new IKnobMultiControl(this, x, y, i, &knobBitmap));
        x += kTableSpaceX;
    }
    x = kTableX;
    y += kTableSpaceY;
    //Decay
    for (int i = mDecayOne; i <= mDecayEight; i++) {
        pGraphics->AttachControl(new IKnobMultiControl(this, x, y, i, &knobBitmap));
        x += kTableSpaceX;
    }
    x = kTableX;
    y += kTableSpaceY;
    //Sustain
    for (int i = mSustainOne; i <= mSustainEight; i++) {
        pGraphics->AttachControl(new IKnobMultiControl(this, x, y, i, &knobBitmap));
        x += kTableSpaceX;
    }
    x = kTableX;
    y += kTableSpaceY;
    //Release
    for (int i = mReleaseOne; i <= mReleaseEight; i++) {
        pGraphics->AttachControl(new IKnobMultiControl(this, x, y, i, &knobBitmap));
        x += kTableSpaceX;
    }
    x = kTableX;
    y += kTableSpaceGainY;
    //Gain
    for (int i = mGainOne; i <= mGainEight; i++) {
        pGraphics->AttachControl(new IKnobMultiControl(this, x, y, i, &knobBitmap));
        x += kTableSpaceX;
    }
    
    AttachGraphics(pGraphics);
}

Voltex::~Voltex() {}

void Voltex::CreatePresets() {
    //TODO make code be here
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
        leftOutput[i] = rightOutput[i] = (voiceManager.nextSample() * gain);
    }
    
    mMIDIReceiver.Flush(nFrames);
}

void Voltex::Reset() {
    TRACE;
    //Reset all
    IMutexLock lock(this);
    double sampleRate = GetSampleRate();
    voiceManager.setSampleRate(sampleRate);
}

void Voltex::OnParamChange(int paramIdx) {
    //Yay! It's threadsafe (I hope)
    IMutexLock lock(this);
    //We will need this later
    IParam* param = GetParam(paramIdx);
    //just some imports...
    using std::tr1::placeholders::_1;
    using std::tr1::bind;
    //This has to be defined here as the switch is in protected scope
    VoiceManager::VoiceChangerFunction changer; //TODO find a way to not need to define this here
    switch(paramIdx) {
        case mGain:
            gain = param->Value() / 100.0;
            break;
        // Volume Envelope:
        //In each case we need to create a VoiceChangerFunction wich defines the stage and value fields but leaves the voice field empty so that it can be filled later as it is being iterated through
        case mVolumeEnvAttack:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
            voiceManager.changeAllVoices(changer);
            break;
        case mVolumeEnvDecay:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
            voiceManager.changeAllVoices(changer);
            break;
        case mVolumeEnvSustain:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
            voiceManager.changeAllVoices(changer);
            break;
        case mVolumeEnvRelease:
            changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
            voiceManager.changeAllVoices(changer);
            break;
        default:
            if (paramIdx >= mAttackOne && paramIdx <= mAttackEight) {
                //Attack
                waveTables[paramIdx - (mAttackOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Value());
            } else if (paramIdx >= mDecayOne && paramIdx <= mDecayEight) {
                //Decay
                waveTables[paramIdx - (mDecayOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Value());
            } else if (paramIdx >= mSustainOne && paramIdx <= mSustainEight) {
                //Sustain
                waveTables[paramIdx - (mSustainOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Value());
            } else if (paramIdx >= mReleaseOne && paramIdx <= mReleaseEight) {
                //Release
                waveTables[paramIdx - (mReleaseOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Value());
            } else if (paramIdx >= mGainOne && paramIdx <= mGainEight) {
                //Gain
                waveTables[paramIdx - (mGainOne)]->setGain(param->Value());
            } else if (paramIdx >= mSwitchOne && paramIdx <= mSwitchEight) {
                //Gain
                waveTables[paramIdx - (mSwitchOne)]->setEnabled(param->Value());
            } else {
                //oops
            }
            break;
    }
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