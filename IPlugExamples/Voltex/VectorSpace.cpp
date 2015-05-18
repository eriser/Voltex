//
//  VectorSpace.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-18.
//
//

#include "VectorSpace.h"

unsigned int VectorPoint::counter = 1;

bool VectorSpace::Draw(IGraphics *pGraphics) {
    if (IControl::IsHidden()) {
        return true;
    }
    
    IColor color(255, 50, 200, 20); //a nice green
    IColor selection(255, 200, 500, 20); //not green
    VectorPoint previous;
    previous.uid = 0;
//    VectorPoint previous;
//    previous.x = 0;
//    previous.y = .5;
    
    for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        VectorPoint current = *it;
        //draw the point
        if (current.uid > 1) {
            if (current != selected) {
                pGraphics->DrawCircle(&color, convertToGraphicX(current.x), convertToGraphicY(current.y), 3, 0, true);
            } else {
                pGraphics->DrawCircle(&selection, convertToGraphicX(current.x), convertToGraphicY(current.y), 3, 0, true);
            }
        }
        
        if (previous.uid != 0) {
            pGraphics->DrawLine(&color, convertToGraphicX(previous.x), convertToGraphicY(previous.y),convertToGraphicX(current.x), convertToGraphicY(current.y), 0, true);
        }
        
        //update the previous point
        previous = current;
    }
    return true;
};

VectorPoint VectorSpace::getPoint(double x, double y, double epsilon /* symbol for precision*/) {
    for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        VectorPoint point = *it;
        double xGraphic = convertToGraphicX(point.x);
        double yGraphic = convertToGraphicY(point.y);
        
        if (
            // X check
            (xGraphic - epsilon < x && xGraphic + epsilon > x) &&
            // Y check
            (yGraphic - epsilon < y && yGraphic + epsilon > y)
            ) {
            return point;
        }
    }
    
    // Nothing found, we return a "blank" point
    VectorPoint none;
    none.uid = 0;
    return none;
};

void VectorSpace::OnMouseDblClick(int x, int y, IMouseMod* pMouseMod) {
    VectorPoint imHere = getPoint(x, y, 6);
    
    //the uid = 0 means no point
    if (imHere.uid == 0) {
        VectorPoint newPoint;
        newPoint.x = convertToPercentX(x);
        newPoint.y = convertToPercentY(y);
        points.push_back(newPoint);
        // And we sort it!
        std::sort(points.begin(), points.end());
        SetDirty();
    } else {
        // We delete the point
        if (imHere.uid > 1) {
            points.erase(std::remove(points.begin(), points.end(), imHere), points.end());
            SetDirty();
        }
    }
};

void VectorSpace::OnMouseUp(int x, int y, IMouseMod* pMouseMod) {
    isDragging = false;
    std::sort(points.begin(), points.end());
    SetDirty();
};

void VectorSpace::OnMouseDown(int x, int y, IMouseMod* pMouseMod) {
    VectorPoint imHere = getPoint(x, y, 6);
    
    if (imHere.uid < 2) {
        // We erase selected
        VectorPoint none;
        none.uid = 0;
        selected = none;
        // Not needed, but who knows.
        isDragging = false;
        SetDirty();
    } else {
        selected = imHere;
        isDragging = true;
        SetDirty();
    }
};

void VectorSpace::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod) {
    if (selected.uid == 0 || isDragging == false) {
        // Nothing to do
        return;
    }
    
    std::vector<VectorPoint>::iterator it = std::find(points.begin(), points.end(), selected);
    
    if (it != points.end()) {
        (&(*it))->x = convertToPercentX(x);
        (&(*it))->y = convertToPercentY(y);
    }
    SetDirty();
};

void VectorSpace::SetDirty() {
    IControl::SetDirty();
    //update values here
}