//
//  VuMeter.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-22.
//
//

#ifndef __Voltex__VuMeter__
#define __Voltex__VuMeter__

#include "IControl.h"

class VuMeter : public IControl {
public:
    VuMeter(IPlugBase* pPlug, IRECT pR, int param) : IControl(pPlug, pR, param) {
        mColor = COLOR_BLUE;
    }
    
    ~VuMeter() {}
    
    bool Draw(IGraphics* pGraphics);
    
    bool isDirty() { return true; } //This component will change so much we might as well just always draw it
protected:
    IColor mColor;
};


#endif /* defined(__Voltex__VuMeter__) */
