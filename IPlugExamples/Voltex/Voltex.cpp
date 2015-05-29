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

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

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
    mTab,
    
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
    
    mToolCursor,
    mToolPencil,
    mToolSelection,
    mToolDelete,
    
	mPreset,
	mLoad,
	mVuMeter,

    kNumParams
};

enum WaveForms {
    wBlank = 0,
    wSine,
    wTriangle,
    wSquare,
    wNoise,
    numWaveForms
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
    
    kMasterEnvelopeY = 137,
    
    //Table:
    kTableX = 292,//232,
    kTableY = 101,
    kTableSpaceX = 44,
    kTableSpaceY = 40,
    kTableSpaceGainY = 50,
    
    //On/off Buttons
    kSwitchX = 295,
    kSwitchY = 358,
    kSwitchSpaceX = 90,
    
    //tabs
    kTabX = 240,
    kTabY = 344,
    kTabMaxX = 957,
    kTabMaxY = 484,
    
    //Numbers
    kNumX = 314,
    kNumY = 369,
    kNumXSpacing = 90,
    
    //vector space
    kVectorSpaceX = 240,
    kVectorSpaceY = 392,
    kVectorSpaceMaxX = 955,
    kVectorSpaceMaxY = 606,
    
    //Buttons
    kButtonX = 181,
    kButtonY = 387,
    kButtonYSpacing = 56,
    
    //Load
    kLoadX = 685,
    kLoadY = 212,
    
    //Presets
    kPresetsX = 748,
    kPresetsY = 213
};

Voltex::Voltex(IPlugInstanceInfo instanceInfo) : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), lastVirtualKeyboardNoteNumber(virtualKeyboardMinimumNoteNumber - 1) {
    TRACE;
    
    //initialize wavetables
    for (int i = 0; i < NUM_TABLES; i++) {
        waveTables[i] = new WaveTable();
    }
    
    //initialize vector spaces
    for (int i = 0; i < NUM_TABLES; i++) {
        vectorSpaces[i] = new VectorSpace(this, IRECT(kVectorSpaceX, kVectorSpaceY, kVectorSpaceMaxX, kVectorSpaceMaxY));
        vectorSpaces[i]->index = i;
        vectorSpaces[i]->tableChanged.Connect(this, &Voltex::updateWaveTable);
    }
	voiceManager.setWavetables(&waveTables);

    firstUpdate = true;
    
    CreateParams();
    CreateGraphics();
    CreatePresets();
    
    firstUpdate = false;
    
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
    GetParam(mPreset)->InitEnum("Wavetable Tab", wBlank, NUM_TABLES);
    GetParam(mPreset)->SetDisplayText(0, "Wavetable Tab");
    
    //Switches
    for (int i = mSwitchOne; i <= mSwitchEight; i++) {
        GetParam(i)->InitEnum("Wavetable Switch", 0, 2);
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
    
    //Tools
    GetParam(mToolCursor)->InitBool("Cursor", true);
    GetParam(mToolPencil)->InitBool("Pencil", false);
    GetParam(mToolSelection)->InitBool("Selection", false);
    GetParam(mToolDelete)->InitBool("Delete", false);
    
    GetParam(mVuMeter)->InitDouble("Vu Meter", 0, 0.0, 1.0, parameterStep);
    
    //Presets
    GetParam(mPreset)->InitEnum("Preset", wBlank, numWaveForms);
    
    //Load
    GetParam(mLoad)->InitBool("Load", false);
    
    //Initial param update
    for (int i = 0; i < kNumParams; i++) {
        OnParamChange(i);
    }
    
    for (int i = 0; i < NUM_TABLES; i++) {
        vectorSpaces[i]->sendSignals = true;
    }
    
}

void Voltex::CreateGraphics() {
    //Get this plugins graphics instance and attach the background image
    pGraphics = MakeGraphics(this, kWidth, kHeight);
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
    IBitmap tab = pGraphics->LoadIBitmap(TAB_ID, TAB_FN, 2);
    
    
    //Tabs
    pGraphics->AttachControl(new IRadioButtonsControl(this, *new IRECT(kTabX, kTabY, kTabMaxX, kTabMaxY), mTab, NUM_TABLES, &tab, kHorizontal, false));
    
    //Tab Numbers
    
    IBitmap one = pGraphics->LoadIBitmap(ONE_ID, ONE_FN, 1);
    IBitmap two = pGraphics->LoadIBitmap(TWO_ID, TWO_FN, 1);
    IBitmap three = pGraphics->LoadIBitmap(THREE_ID, THREE_FN, 1);
    IBitmap four = pGraphics->LoadIBitmap(FOUR_ID, FOUR_FN, 1);
    IBitmap five = pGraphics->LoadIBitmap(FIVE_ID, FIVE_FN, 1);
    IBitmap six = pGraphics->LoadIBitmap(SIX_ID, SIX_FN, 1);
    IBitmap seven = pGraphics->LoadIBitmap(SEVEN_ID, SEVEN_FN, 1);
    IBitmap eight = pGraphics->LoadIBitmap(EIGHT_ID, EIGHT_FN, 1);
    
    pGraphics->AttachControl(new IBitmapControl(this, kNumX, kNumY, &one));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + kNumXSpacing, kNumY, &two));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (2 * kNumXSpacing), kNumY, &three));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (3 * kNumXSpacing), kNumY, &four));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (4 * kNumXSpacing), kNumY, &five));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (5 * kNumXSpacing), kNumY, &six));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (6 * kNumXSpacing), kNumY, &seven));
    pGraphics->AttachControl(new IBitmapControl(this, kNumX + (7 * kNumXSpacing), kNumY, &eight));
    
    //Switches
    int x = kSwitchX, y = 0;
    for (int v = mSwitchOne; v <= mSwitchEight; v++) {
        if (v == mSwitchThree || v == mSwitchTwo) {
            pGraphics->AttachControl(new ISwitchControl(this, x - 1, kSwitchY, v, &switches));
        } else {
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
    
    //Vector spaces
    for (int i = 0; i < NUM_TABLES; i++) {
        pGraphics->AttachControl(vectorSpaces[i]);
    }
    
    //Buttons
    IBitmap cursorClick = pGraphics->LoadIBitmap(CURSORCLK_ID, CURSORCLK_FN, 2);
    IBitmap pencilClick = pGraphics->LoadIBitmap(PENCILCLK_ID, PENCILCLK_FN, 2);
    IBitmap selectClick = pGraphics->LoadIBitmap(SELECTCLK_ID, SELECTCLK_FN, 2);
    IBitmap trashClick = pGraphics->LoadIBitmap(TRASHCLK_ID, TRASHCLK_FN, 2);
    
    toolCursor = new ISwitchControl(this, kButtonX, kButtonY, mToolCursor, &cursorClick);
    pGraphics->AttachControl(toolCursor);
    toolPencil = new ISwitchControl(this, kButtonX, kButtonY + kButtonYSpacing, mToolPencil, &pencilClick);
    pGraphics->AttachControl(toolPencil);
    toolSelection = new ISwitchControl(this, kButtonX, kButtonY + (2 * kButtonYSpacing), mToolSelection, &selectClick);
    pGraphics->AttachControl(toolSelection);
    toolDelete = new ISwitchControl(this, kButtonX, kButtonY + (3 * kButtonYSpacing), mToolDelete, &trashClick);
    pGraphics->AttachControl(toolDelete);
    
    //load and waveform selection
    IBitmap loadIcon = pGraphics->LoadIBitmap(LOAD_ID, LOAD_FN, 2);
    IBitmap presetIcon = pGraphics->LoadIBitmap(PRESETS_ID, PRESETS_FN, NUM_PRESETS);
    
    load = new IContactControl(this, kLoadX, kLoadY, mLoad, &loadIcon);
    pGraphics->AttachControl(load);
    presets = new ISwitchControl(this, kPresetsX, kPresetsY, mPreset, &presetIcon);
    pGraphics->AttachControl(presets);
    
    //dB meter
    //moves vertically 246 (from 363 to 609) pixels from  0 dB to -inf. dB
    
    //pGraphics->AttachControl(new VuMeter(this, *new IRECT(64, 388, 320, 80), mVuMeter));
    //IBitmap dBCoverBg = pGraphics->LoadIBitmap(DBCOVERBG_ID, DBCOVERBG_FN, 1);
    //IBitmap dBCoverRect = pGraphics->LoadIBitmap(DBCOVERRECT_ID, DBCOVERRECT_FN, 1);
    
    //pGraphics->AttachControl(new IBitmapControl(this, 65, 7, &dBCoverRect)); //replace 3rd param. with a changing value based on total. amplitude.
    //pGraphics->AttachControl(new IBitmapControl(this, 61, 92, &dBCoverBg));
    
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
		GetGUI()->SetParameterFromPlug(mVuMeter, leftOutput[i], true);
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
                waveTables[paramIdx - (mAttackOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, param->Int());
            } else if (paramIdx >= mDecayOne && paramIdx <= mDecayEight) {
                //Decay
                waveTables[paramIdx - (mDecayOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_DECAY, param->Int());
            } else if (paramIdx >= mSustainOne && paramIdx <= mSustainEight) {
                //Sustain
                waveTables[paramIdx - (mSustainOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, param->Int());
            } else if (paramIdx >= mReleaseOne && paramIdx <= mReleaseEight) {
                //Release
                waveTables[paramIdx - (mReleaseOne)]->setMixValue(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, param->Int());
            } else if (paramIdx >= mGainOne && paramIdx <= mGainEight) {
                //Gain
                waveTables[paramIdx - (mGainOne)]->setGain(param->Value());
            } else if (paramIdx == mTab) {
                //Tabs
                vectorSpaces[(int)param->Value()]->Hide(false);
                for (int i = 0; i < NUM_TABLES; i++) {
                    if (i != (int)param->Value()) {
                        vectorSpaces[(int)i]->Hide(true);
                    }
                }
            } else if (paramIdx >= mSwitchOne && paramIdx <= mSwitchEight) {
                //Switches
                waveTables[paramIdx - (mSwitchOne)]->setEnabled(param->Bool());
            } else if (paramIdx == mToolCursor) {
                //Tool: Cursor
                if (param->Bool() == true) {
                    if (!firstUpdate) {
                        toolPencil->SetValueFromPlug(false);
                        toolSelection->SetValueFromPlug(false);
                        toolDelete->SetValueFromPlug(false);
                    }
                    VectorSpace::currentTool = VectorSpace::kToolCursor;
                } else {
                    if (!firstUpdate) {
                        toolCursor->SetValueFromPlug(true);
                    }
                }
            } else if (paramIdx == mToolPencil) {
                //Tool: Pencil
                if (param->Bool() == true) {
                    if (!firstUpdate) {
                        toolCursor->SetValueFromPlug(false);
                        toolSelection->SetValueFromPlug(false);
                        toolDelete->SetValueFromPlug(false);
                    }
                    VectorSpace::currentTool = VectorSpace::kToolPencil;
                } else {
                    if (!firstUpdate) {
                        toolPencil->SetValueFromPlug(true);
                    }
                }
            } else if (paramIdx == mToolSelection) {
                //Tool: Selection
                if (param->Bool() == true) {
                    if (!firstUpdate) {
                        toolPencil->SetValueFromPlug(false);
                        toolCursor->SetValueFromPlug(false);
                        toolDelete->SetValueFromPlug(false);
                    }
                    VectorSpace::currentTool = VectorSpace::kToolSelection;
                } else {
                    if (!firstUpdate) {
                        toolSelection->SetValueFromPlug(true);
                    }
                }
            } else if (paramIdx == mToolDelete) {
                //Tool: Delete
                if (param->Bool() == true) {
                    if (!firstUpdate) {
                        toolPencil->SetValueFromPlug(false);
                        toolSelection->SetValueFromPlug(false);
                        toolCursor->SetValueFromPlug(false);
                    }
                    VectorSpace::currentTool = VectorSpace::kToolDelete;
                } else {
                    if (!firstUpdate) {
                        toolDelete->SetValueFromPlug(true);
                    }
                }
            } else if (paramIdx == mLoad){
                if (param->Bool() == true) {
                    printf("%d\n", GetParam(mPreset)->Int());
                    for (int i = 0; i < vectorSpaces.size(); i++) {
                        if (!vectorSpaces[i]->IsHidden()) {
                            if (static_cast<WaveForms>(GetParam(mPreset)->Int()) == wBlank) {
                                WaveTable* emptyTable;
                                emptyTable = new WaveTable();
                                
                                std::tr1::array<double, 2048> emptyValues;
                                for (int j = 0; j < 2048; j++) {
                                    emptyValues[j] = 0;
                                }
                                
                                emptyTable->setValues(emptyValues);
                                waveTables[i] = emptyTable;
                                vectorSpaces[i]->clear();
                                vectorSpaces[i]->Redraw();
                            } else if (static_cast<WaveForms>(GetParam(mPreset)->Int()) == wSine) {
                                WaveTable* sineTable;
                                sineTable = new WaveTable();
                                        
                                std::tr1::array<double, 2048> sinValues;
                                for (int j = 0; j < 2048; j++) {
                                    sinValues[j] = sin((j / 2048.0) * (4 * acos(0.0)));
                                }
                                sineTable->setValues(sinValues);
                                waveTables[i] = sineTable;
                                vectorSpaces[i]->setValues(sinValues, kVectorSpaceMaxY - kVectorSpaceY);
                                vectorSpaces[i]->Redraw();
                            } else if (static_cast<WaveForms>(GetParam(mPreset)->Int()) == wTriangle) {
                                WaveTable* triangleTable;
                                triangleTable = new WaveTable();
                                        
                                std::tr1::array<double, 2048> triangleValues;
                                for (int j = 0; j < 2048; j++) {
                                    int phase = j - 512;
                                    while (phase < 0) {
                                        phase += 2048;
                                    }
                                    double value = -1.0 + ((phase / 2048.0) * 2);
                                    triangleValues[j] = 2.0 * (fabs(value) - 0.5);
                                }
                                triangleTable->setValues(triangleValues);
                                waveTables[i] = triangleTable;
                                vectorSpaces[i]->setValues(triangleValues, kVectorSpaceMaxY - kVectorSpaceY);
                                vectorSpaces[i]->Redraw();
                            } else if (static_cast<WaveForms>(GetParam(mPreset)->Int()) == wSquare) {
                                WaveTable* squareTable;
                                squareTable = new WaveTable();
                                std::tr1::array<double, 2048> squareValues;
                                for (int j = 0; j < 2048; j++) {
                                    if (j < 1024) {
                                        squareValues[j] = -1;
                                    } else {
                                        squareValues[j] = 1;
                                    }
                                }
                                squareTable->setValues(squareValues);
                                waveTables[i] = squareTable;
                                vectorSpaces[i]->setValues(squareValues, kVectorSpaceMaxY - kVectorSpaceY);
                                vectorSpaces[i]->Redraw();
                            } else if (static_cast<WaveForms>(GetParam(mPreset)->Int()) == wNoise) {
                                WaveTable* noiseTable;
                                noiseTable = new WaveTable();
                                
                                std::tr1::array<double, 2048> noiseValues;
                                
                                srand(time(NULL));
                                for (int j = 0; j < 2048; j++) {
                                    noiseValues[j] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2)) - 1;
                                }
                                
                                noiseTable->setValues(noiseValues);
                                waveTables[i] = noiseTable;
                                vectorSpaces[i]->setValues(noiseValues, kVectorSpaceMaxY - kVectorSpaceY);
                            }
                        }}
                }
            } else {
                //oops
            }
            break;
    }
}

void Voltex::updateWaveTable(int table) {
    waveTables[table]->setValues(vectorSpaces[table]->getValues());
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