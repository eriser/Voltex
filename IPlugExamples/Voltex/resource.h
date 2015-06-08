
#define PRESETS_ID      113
#define LOAD_ID         114
#define ONE_ID          115
#define TWO_ID          116
#define THREE_ID        117
#define FOUR_ID         118
#define FIVE_ID         119
#define SIX_ID          120
#define SEVEN_ID        121
#define EIGHT_ID        122
#define SAVE_MENU_ID    123
#define LOAD_MENU_ID    124
#define POSC_ID         125

// Image resource locations for this plug.
#define BG_FN             "resources/img/bg.png"
#define WHITE_KEY_FN      "resources/img/whitekey.png"
#define BLACK_KEY_FN      "resources/img/blackkey.png"
#define KNOB_FN           "resources/img/knob.png"
#define SWITCHES_FN       "resources/img/switch.png"
#define TAB_FN            "resources/img/tab.png"
#define DBCOVERBG_FN      "resources/img/dBCoverBg.png"
#define DBCOVERRECT_FN    "resources/img/dBCoverRect.png"
#define CURSORCLK_FN      "resources/img/ButtonPressed/cursorClicked.png"
#define PENCILCLK_FN      "resources/img/ButtonPressed/pencilClicked.png"
#define SELECTCLK_FN      "resources/img/ButtonPressed/selectClicked.png"
#define TRASHCLK_FN       "resources/img/ButtonPressed/trashClicked.png"
#define PRESETS_FN		    "resources/img/presets.png"
#define LOAD_FN           "resources/img/load.png"
#define	ONE_FN			      "resources/img/Numbers/1.png"
#define TWO_FN            "resources/img/Numbers/2.png"
#define THREE_FN          "resources/img/Numbers/3.png"
#define FOUR_FN           "resources/img/Numbers/4.png"
#define FIVE_FN           "resources/img/Numbers/5.png"
#define SIX_FN            "resources/img/Numbers/6.png"
#define SEVEN_FN          "resources/img/Numbers/7.png"
#define EIGHT_FN          "resources/img/Numbers/8.png"
#define SAVE_MENU_FN      "resources/img/savemenu.png"
#define LOAD_MENU_FN      "resources/img/loadmenu.png"
#define POSC_FN	          "resources/img/periodNum.png"

// GUI default dimensions
#define GUI_WIDTH   795
#define GUI_HEIGHT  680
// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
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
