//
//  VectorSpace.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-18.
//
//

#ifndef __Voltex__VectorSpace__
#define __Voltex__VectorSpace__

#include "IControl.h"
#include <vector>
#include "GallantSignal.h"
using Gallant::Signal1;

#ifdef WIN32
#include <array>
#else
#include <tr1/array>
#endif

class VectorPoint {
private:
    static unsigned long counter;
public:
    VectorPoint() :
    uid(++counter)
    {
        
    }
    unsigned int uid; //used to identify points even if they have the same x and y
    double x;
    double y;
    //another reason why c variants are better than Java, operator overloads
    bool operator < (const VectorPoint& point) const { return (this->x < point.x); };
    bool operator >(const VectorPoint& point) const {  return (this->x > point.x); };
    bool operator == (const VectorPoint& point) const { return (this->uid == point.uid); };
    bool operator != (const VectorPoint& point) const { return (this->uid != point.uid); };
};

class VectorSpace : public IControl {
protected:
    std::vector<VectorPoint> points;
    VectorPoint selected;
    bool isDragging;
    
    double convertToGraphicX(double value) {
        double min = (double) this->mRECT.L;
        double distance = (double) this->mRECT.W();
        return value * distance + min;
    };
    double convertToPercentX(double value) {
        double min = (double) this->mRECT.L;
        double position = value - min;
        double distance = (double) this->mRECT.W();
        return fmax(fmin((position / distance),  1), 0);
    };
    double convertToGraphicY(double value) {
        double min = (double) this->mRECT.T + 2;
        double distance = (double) this->mRECT.H() - 2;
        return ((1 - value) * distance + min); //1 - value since we want 1 to be at the top of the space
    };
    double convertToPercentY(double value) {
        double min = (double) this->mRECT.T;
        double position = value - min;
        double distance = (double) this->mRECT.H();
        return fmax(fmin((1 - position / distance),  1), 0);
        
    };
public:
    VectorSpace(IPlugBase *pPlug, IRECT pR) : IControl(pPlug, pR), sendSignals(false) {
        clear();
    };
    ~VectorSpace() {};
    
    Signal1<int> tableChanged;
    
    int index;
    bool sendSignals;
    
    bool Draw(IGraphics *pGraphics);
    VectorPoint getPoint(double x, double y, double epsilon);
    void OnMouseDblClick(int x, int y, IMouseMod* pMouseMod);
    void OnMouseUp(int x, int y, IMouseMod* pMouseMod);
    void OnMouseDown(int x, int y, IMouseMod* pMouseMod);
    void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod);
    
    void clear();
    
    std::tr1::array<double, 2048> getValues();
    void setValues(std::tr1::array<double, 2048> table, int precision);
};

#endif /* defined(__Voltex__VectorSpace__) */
