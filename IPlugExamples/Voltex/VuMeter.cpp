//
//  VuMeter.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-22.
//
//

#include "VuMeter.h"

bool VuMeter::Draw(IGraphics* pGraphics) {
    printf("Drawing Vu Meter\n");
    
    //IRECT(mRECT.L, mRECT.T, mRECT.W , mRECT.T + (mValue * mRECT.H));
    pGraphics->FillIRect(&COLOR_RED, &mRECT);
    
    //pGraphics->FillIRect(&COLOR_BLUE, &mRECT);
    
    IRECT filledBit = IRECT(mRECT.L, mRECT.T, mRECT.R, mRECT.B - (mValue * mRECT.H()));
    pGraphics->FillIRect(&mColor, &filledBit);
    return true;
}
