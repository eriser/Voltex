//
//  VectorSpace.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-18.
//
//

#include "VectorSpace.h"

#include <algorithm>

const int epsilon = 4;

unsigned long VectorPoint::counter = 1;

bool VectorSpace::Draw(IGraphics *pGraphics) {
    if (IControl::IsHidden()) {
        return true;
    }
    
    IColor color(255, 50, 200, 20); //a nice green
    IColor selection(255, 200, 500, 20); //not green
    VectorPoint previous;
    previous.uid = 0;
    
    for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        VectorPoint current = *it;
        //draw the point
        if (current.uid > 1) {
            if (current != selected) {
                pGraphics->DrawCircle(&color, convertToGraphicX(current.x), convertToGraphicY(current.y), 2, 0, true);
            } else {
                pGraphics->DrawCircle(&selection, convertToGraphicX(current.x), convertToGraphicY(current.y), 2, 0, true);
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

VectorPoint VectorSpace::getPoint(double x, double y) {
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
    VectorPoint imHere = getPoint(x, y);
    
    //the uid = 0 means no point
    if (imHere.uid == 0) {
        VectorPoint newPoint;
        newPoint.x = convertToPercentX(x);
        newPoint.y = convertToPercentY(y);
        points.push_back(newPoint);
        // And we sort it!
        std::sort(points.begin(), points.end());
        SetDirty();
        if (sendSignals) {
            tableChanged(index);
        }
    } else {
        // We delete the point
        if (imHere.uid > 1) {
            points.erase(std::remove(points.begin(), points.end(), imHere), points.end());
            SetDirty();
            if (sendSignals) {
                tableChanged(index);
            }
        }
    }
};

void VectorSpace::OnMouseUp(int x, int y, IMouseMod* pMouseMod) {
    isDragging = false;
    if (pMouseMod->C) {
        clear();
        return;
    } else {
        std::sort(points.begin(), points.end());
    }
    SetDirty();
    if (sendSignals) {
        tableChanged(index);
    }
};

void VectorSpace::OnMouseDown(int x, int y, IMouseMod* pMouseMod) {
    if (pMouseMod->S) {
        //Drawing
        return;
    }
    
    VectorPoint imHere = getPoint(x, y);
    
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
        if (pMouseMod->S) {
            //Draw freehand
            int prev = x - dX, lower = prev, higher = x;
            if (prev > x) {
                lower = x;
                higher = prev;
            }
            for (int i = lower; i < higher; i++) {
                if (i == 0 || i == 1) {
                    break;
                }
                bool pointMoved = false;
                for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
                    VectorPoint* point = &(*it);
                    double xGraphic = convertToGraphicX(point->x);
                    if ((xGraphic - epsilon) < i && (xGraphic + epsilon) > i) {
                        point->y = convertToPercentY(y);
                        pointMoved = true;
                        break;
                    }
                }
                if (!pointMoved) {
                    //Add an new point
                    VectorPoint newPoint;
                    newPoint.x = convertToPercentX(i);
                    newPoint.y = convertToPercentY(y);
                    points.push_back(newPoint);
                    // And we sort it!
                    std::sort(points.begin(), points.end());
                }
                SetDirty();
            }
        } else {
            //nothing to do
            return;
        }
    }
    
    std::vector<VectorPoint>::iterator it = std::find(points.begin(), points.end(), selected);
    
    if (it != points.end()) {
        (&(*it))->x = convertToPercentX(x);
        (&(*it))->y = convertToPercentY(y);
    }
    SetDirty();
};

void VectorSpace::clear() {
    //remove all elements (points.clear() doesn't work for some reason)
    for (std::vector<VectorPoint>::iterator i = points.begin(); i != points.end();) {
         i = points.erase(i);
    }
    //Points at the very ends of the space
    VectorPoint start;
    start.x = 0;
    start.y = .5;
    start.uid = 1;
    VectorPoint end;
    end.x = 1;
    end.y = .5;
    end.uid = 1;
    points.push_back(start);
    points.push_back(end);
    SetDirty();
    if (sendSignals) {
        tableChanged(index);
    }
}

std::tr1::array<double, 2048> VectorSpace::getValues() {
    std::tr1::array<double, 2048> values;
    for (int i = 0; i < 2048; i++) {
        VectorPoint a, b;
        double targetX = i / 2048.0;
        for(int j=0; j < points.size(); j++){
            if (points[j].x <= targetX && points[j+1].x >= targetX) {
                a = points[j];
                if ((j + 1) < points.size()) {
                    b = points[j++];
                } else {
                    b = points[0]; // loop around
                }
                break;
            }
        }
        if (a == b) {
            values[i] = (2 * a.y) - 1;
        } else {
            //interpolate
            //          Base  Fraction             Difference
            values[i] = a.y + ((i / 2048) - a.x) * (b.y - a.y);
        }
//        printf("%d: i = %d between %f and %f. Value = %f between %f and %f, a=b: %d\n", index, i, a.x, b.x, values[i], a.y, b.y, a == b);
    }
    return values;
}

void VectorSpace::setValues(std::tr1::array<double, 2048> table, int precision) {
    clear();
    double increment = 2048.0 / precision;
    for (double i = 0; i < 2048; i += increment) {
        VectorPoint point;
        point.x = i / 2048.0;
        point.y = (table[i] + 1) / 2;
        points.push_back(point);
    }
    std::sort(points.begin(), points.end());
    SetDirty();
    if (sendSignals) {
        tableChanged(index);
    }
}
