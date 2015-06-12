//
//  WaveTable.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#ifndef __Voltex__WaveTable__
#define __Voltex__WaveTable__

#include "EnvelopeGenerator.h"


#ifdef WIN32
#include <array>
#else
#include <tr1/array>
#endif

#include <iostream>

#define TWO_PI (4*acos(0.0))
#define TABLE_LENGTH 2048
#define INTERP_MODE 2 //The number of points to interpolate decimal phase values with. Valid values are 2 and 4. 2 should be less cpu intensive, but it will give a lower quality result.

class WaveTable {
public:
    void setValues (std::tr1::array<double, TABLE_LENGTH> newValues);
    std::tr1::array<double, TABLE_LENGTH> getValues ();
    void setMixValue (EnvelopeGenerator::EnvelopeStage stage, double value);
    double getMixValue (EnvelopeGenerator::EnvelopeStage stage);
    void setGain(double newGain);
    double getGain();
    void setEnabled(bool enabled);
    bool isEnabled();
    double getValueAt (double index);
    
    int size() {return TABLE_LENGTH;}
    
    WaveTable(): gain(0.8) //initialization list
	{
		//Constructor Body
        values = *new std::tr1::array<double, TABLE_LENGTH>();
		for (int i = 0; i < TABLE_LENGTH; i++) {
			values[i] = 0;
		}
        //Default envelope values
        mix[EnvelopeGenerator::ENVELOPE_STAGE_OFF] = 0.0;
        mix[EnvelopeGenerator::ENVELOPE_STAGE_ATTACK] = 1.0;
        mix[EnvelopeGenerator::ENVELOPE_STAGE_DECAY] = 1.0;
        mix[EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN] = 1.0;
        mix[EnvelopeGenerator::ENVELOPE_STAGE_RELEASE] = 1.0;
    };
    
private:
    std::tr1::array<double, TABLE_LENGTH> values;
    double gain;
    bool enabled;
    double mix[EnvelopeGenerator::kNumEnvelopeStages];
    
    //interpolates between L0 and H0 taking the previous (L1) and next (H1) points into account
    inline float ThirdInterp(const float x,const float L1,const float L0,const
                             float H0,const float H1)
    {
        return
        L0 +
        .5f*
        x*(H0-L1 +
           x*(H0 + L0*(-2) + L1 +
              x*( (H0 - L0)*9 + (L1 - H1)*3 +
                 x*((L0 - H0)*15 + (H1 -  L1)*5 +
                    x*((H0 - L0)*6 + (L1 - H1)*2 )))));
    }

};

#endif /* defined(__Voltex__WaveTable__) */
