//
//  WaveTable.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-04-11.
//
//

#include "WaveTable.h"

void WaveTable::setEnvelopeValue(EnvelopeGenerator::EnvelopeStage stage, double value) {
    envelopeValues[stage] = value;
}

double WaveTable::getEnvelopeValue(EnvelopeGenerator::EnvelopeStage stage) {
    return envelopeValues[stage];
}

void WaveTable::setValues(std::tr1::array<double, TABLE_LENGTH> newValues) {
    values = newValues;
}

std::tr1::array<double, TABLE_LENGTH> WaveTable::getValues() {
    return values;
}

double WaveTable::getValueAt(double location) {
    double index = (location / TWO_PI) * TABLE_LENGTH;
    
    int a = (int)index;
    if (a == index) {
        return values[a];
    }
#if INTERP_MODE == 2
    //Two point linear interpolation works as follows: (f(x) = f(a) + (f(a + 1) - f(a)) * (x - a))) where x is the desired index and a is the closest index to the left of x (|x|)
    int b = a + 1;
    if (b > (TABLE_LENGTH - 1)) {b -= TABLE_LENGTH;} // if we are looking for a sample that is above the end of the array (a decimal between (length) and (length + 1)), then the next value is actually the first value in the array, since the system is cyclical.
    
    //     Base               Fraction      Difference
    double out = values[a] + (index - a) * (values[b] - values[a]);
    return out;
#elif INTERP_MODE == 4
    //Four point (cubic) interpolation.
    int b = a, c = a + 1, d = a + 2;
    a--;
    if (a < 0) {a += TABLE_LENGTH;}
    if (c > (TABLE_LENGTH - 1)) {c -= TABLE_LENGTH;}
    if (d > (TABLE_LENGTH - 1)) {d -= TABLE_LENGTH;}
    //Again, the lines above wrap the values around the edge of the array if we are working at the very edge, there is more to wrap as we need two values on either side, and in theory both top values could be too high and the bottom most value could be too low.
    
    double aValue = values[a], bValue = values[b], cValue = values[c], dValue = values[d];
    
//    double out = ThirdInterp(index, aValue, bValue, cValue, dValue);
    double out = bValue + 0.5 * index*(cValue - aValue + index*(2.0*aValue - 5.0*bValue + 4.0*cValue - dValue + index*(3.0*(bValue - cValue) + dValue - aValue)));
    return out;
#endif
}

void WaveTable::setGain(double newGain) {
    gain = newGain;
}

double WaveTable::getGain() {
    return gain;
}