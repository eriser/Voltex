#define PLUG_MFR "Voltex"
#define PLUG_NAME "Voltex"

#define PLUG_CLASS_NAME Voltex

#define BUNDLE_MFR "SamuelDewan" //TODO: find out how to change this without breaking all fo the graphics
#define BUNDLE_NAME "Voltex"

#define PLUG_ENTRY Voltex_Entry
#define PLUG_VIEW_ENTRY Voltex_ViewEntry

#define PLUG_ENTRY_STR "Voltex_Entry"
#define PLUG_VIEW_ENTRY_STR "Voltex_ViewEntry"

#define VIEW_CLASS Voltex_View
#define VIEW_CLASS_STR "Voltex_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID '3tri'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'Acme' //TODO: find out how to change this without breaking all fo the graphics

// ProTools stuff
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
#define _PIDS_
const int PLUG_TYPE_IDS[2] = {'MSN1', 'MSN2'};
#endif
#define PLUG_MFR_PT "SamuelDewan\nSamuelDewan\nAcme\n"
#define PLUG_NAME_PT "Voltex\nIPMS"
#define PLUG_TYPE_PT "Effect"

#if (defined(AAX_API) || defined(RTAS_API))
#define PLUG_CHANNEL_IO "1-1 2-2"
#else
#define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 1

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1

#define PLUG_DOES_STATE_CHUNKS 0

// Unique IDs for each image resource.
#define BG_ID         101
#define WHITE_KEY_ID  102
#define BLACK_KEY_ID  103
#define KNOB_ID       104
#define SWITCHES_ID   105
#define TAB_ID		  106

// Image resource locations for this plug.
#define BG_FN         "resources/img/bg.png"
#define WHITE_KEY_FN  "resources/img/whitekey.png"
#define BLACK_KEY_FN  "resources/img/blackkey.png"
#define KNOB_FN       "resources/img/knob.png"
#define SWITCHES_FN   "resources/img/switch.png"
#define TAB_FN	      "resources/img/tabs.png"

// GUI default dimensions
#define GUI_WIDTH   960
#define GUI_HEIGHT  680
// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ oness
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL ""
#define MFR_EMAIL ""
#define EFFECT_TYPE_VST3 "Instrument|Synth"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
 "Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
 "Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
 "Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
 "Instrument|SpaceBass", "Instrument|SpaceBass|Sampler", "Instrument|External", "Spatial",
 "Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
 "Surround"
 */
