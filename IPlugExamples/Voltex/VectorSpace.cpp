//
//  VectorSpace.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-18.
//
//

#include "VectorSpace.h"

#include <algorithm>

const int epsilon = 3;

unsigned long VectorPoint::counter = 1;
VectorSpace::Tools VectorSpace::currentTool = VectorSpace::kToolCursor;

bool VectorSpace::Draw(IGraphics *pGraphics) {
    //No point doing any work if hidden...
    if (IControl::IsHidden()) {
        return true;
    }
    
    IColor colour(255, 50, 200, 20); //a nice green
    IColor selection(255, 200, 500, 20); //not green
    VectorPoint previous;
    previous.uid = 0;
    
    //Iterate through all points
    for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        VectorPoint current = *it;
        //draw the point, if it is not one of the end points, which have uids of -1
        if (current.uid > 1) {
            if (!(std::find(selected.begin(), selected.end(), current) != selected.end())) {
                //If the point is not selected draw it in the normal colour
                pGraphics->DrawCircle(&colour, convertToGraphicX(current.x), convertToGraphicY(current.y), 2, 0, true);
            } else {
                //If the point is selected draw it in the selection colour
                pGraphics->DrawCircle(&selection, convertToGraphicX(current.x), convertToGraphicY(current.y), 2, 0, true);
            }
        }
        
        if (previous.uid != 0) {
            //Draw lines between the points
            pGraphics->DrawLine(&colour, convertToGraphicX(previous.x), convertToGraphicY(previous.y),convertToGraphicX(current.x), convertToGraphicY(current.y), 0, true);
        }
        
        //update the previous point
        previous = current;
    }
    return true;
};

VectorPoint VectorSpace::getPoint(double x, double y = -1.0) {
    for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
        VectorPoint point = *it;
        double xGraphic = convertToGraphicX(point.x);
        double yGraphic = convertToGraphicY(point.y);
        
        if (
            // X check
            (xGraphic - epsilon < x && xGraphic + epsilon > x) &&
            // Y check, if the y is -1.0 no check is performed
            (y == -1.0 || (yGraphic - epsilon < y && yGraphic + epsilon > y))
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
        // No point exist, add a new one
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
        // A point exists, we delete it
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
        // if the control key is held, clear the vecrtor space
        clear();
        return;
    }
    
    std::sort(points.begin(), points.end());
    VectorPoint imHere = getPoint(x, y);
    
    if ((currentTool == kToolPencil && imHere.uid == 0 && !pMouseMod->R) || ((currentTool == kToolDelete && pMouseMod->R) && imHere.uid == 0)) {
        // If we are using the pencil tool and left clicking add a new point
        VectorPoint newPoint;
        newPoint.x = convertToPercentX(x);
        newPoint.y = convertToPercentY(y);
        points.push_back(newPoint);
        // make sure that the list is still in order
        std::sort(points.begin(), points.end());
        SetDirty();
        if (sendSignals) {
            tableChanged(index);
        }
    } else if ((currentTool == kToolDelete && !pMouseMod->R) || (currentTool == kToolPencil && pMouseMod->R)) {
        // We delete the point if the tool is delete and it is a left click, or if it is the pencil tool and a right click
        if (imHere.uid > 1) {
            for (int i = 0; i < selected.size(); i++) {
                points.erase(std::remove(points.begin(), points.end(), selected[i]), points.end());
            }
            for (std::vector<VectorPoint>::iterator i = selected.begin(); i != selected.end();) {
                i = selected.erase(i);
            }
            SetDirty();
            if (sendSignals) {
                tableChanged(index);
            }
        }
    } else {
        SetDirty();
        if (sendSignals) {
            tableChanged(index);
        }
    }
};

void VectorSpace::OnMouseDown(int x, int y, IMouseMod* pMouseMod) {
    if (pMouseMod->S || currentTool == kToolPencil) {
        //Drawing, no point running the mouse down code
        return;
    }
    
    VectorPoint imHere = getPoint(x, y);
    
    if (imHere.uid == 0) {
        // Clear the selection
        for (std::vector<VectorPoint>::iterator i = selected.begin(); i != selected.end();) {
            i = selected.erase(i);
        }
        SetDirty();
    } else {
        if (!(std::find(selected.begin(), selected.end(), imHere) != selected.end())) {
            // add point to selection
            selected.push_back(imHere);
        } else {
            // if point is already in the selection, remove it
            std::vector<VectorPoint>::iterator i = std::find(selected.begin(), selected.end(), imHere);
            if (i != selected.end()) {
                selected.erase(i);
            }
        }
        isDragging = true;
        SetDirty();
    }
};

void VectorSpace::OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMouseMod) {
    if (currentTool == kToolSelection) {
        // Find the leftmost and rightmost points of the mouses movement
        int prev = x - dX, lower = prev, higher = x;
        if (prev > x) {
            lower = x;
            higher = prev;
        }
        //Iterate though the mouses movement
        for (int i = lower; i < higher; i++) {
            VectorPoint point = getPoint(i);
            //If the point exist, add it to the selection
            if (point.x != 0 && point.x != 1) {
                if (!(std::find(selected.begin(), selected.end(), point) != selected.end())) {
                    selected.push_back(point);
                }
                SetDirty();
            }
        }
    } else if (selected.empty() || isDragging == false) {
        if (currentTool == kToolPencil ||  pMouseMod->S) {
            //Draw freehand
            
            // Find the mouses leftmost and rightmost points.
            int prev = x - dX, lower = prev, higher = x;
            if (prev > x) {
                lower = x;
                higher = prev;
            }
            // Iterate through the X coordinates that the mouse passed
            for (int i = lower; i < higher; i++) {
                if (i == 0 || i == 1) {
                    // Don't touch the end points
                    break;
                }
                bool pointMoved = false;
                // Iterate over all points
                for (std::vector<VectorPoint>::iterator it = points.begin(); it != points.end(); ++it) {
                    VectorPoint* point = &(*it);
                    double xGraphic = convertToGraphicX(point->x);
                    if ((((xGraphic - epsilon) < i && (xGraphic + epsilon) > i)) && (point->x > 0 && point->x < 1)) {
                        // If a point exists at the x location move it to the mouses y location
                        point->y = convertToPercentY(y);
                        pointMoved = true;
                        break;
                    }
                }
                if (!pointMoved && (convertToPercentX(i) > 0 && convertToPercentX(i) < 1)) {
                    // If no point exists at the x location, add an new point
                    VectorPoint newPoint;
                    newPoint.x = convertToPercentX(i);
                    newPoint.y = convertToPercentY(y);
                    points.push_back(newPoint);
                    std::sort(points.begin(), points.end());
                }
                SetDirty();
            }
        }
    } else {
        // nothing to do
        return;
    }
    
    // Drag selected points:
    
    if (selected.size() == 1) {
        //If only one point is selected, move it to the mouse location
        std::vector<VectorPoint>::iterator it = std::find(points.begin(), points.end(), selected[0]);
        if (it != points.end()) {
            (&(*it))->x = convertToPercentX(x);
            (&(*it))->y = convertToPercentY(y);
        }
    } else if (selected.size() > 1) {
        //If mulitple points are selected move them all by the amount that the mouse moved
        for (int i = 0; i < selected.size(); i++) {
            std::vector<VectorPoint>::iterator it = std::find(points.begin(), points.end(), selected[i]);
            if (it != points.end()) {
                (&(*it))->x = convertToPercentX(convertToGraphicX((&(*it))->x) + dX);
                (&(*it))->y = convertToPercentY(convertToGraphicY((&(*it))->y) + dY);
            }
        }
    } else {
        return;
    }
    SetDirty();
}

void VectorSpace::clear() {
    //remove all elements
    for (std::vector<VectorPoint>::iterator i = points.begin(); i != points.end();) {
         i = points.erase(i);
    }
    //Points at the very ends of the space need to be recreated as they must always be there
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
    // Iterate through the array we are filling
    for (int i = 0; i < 2048; i++) {
        //Find the points to the left and right of the desired x value
        VectorPoint a, b;
        double targetX = i / 2048.0;
        for(int j=0; j < points.size(); j++) {
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
            //if the values are the same, that means that the desired x lines up with the x of an existing point, so we can just take that points y
            values[i] = (2 * a.y) - 1;
        } else {
            //interpolate the y value from the y
            //                Base  Fraction             Difference
            values[i] = (2 * (a.y + ((i / 2048) - a.x) * (b.y - a.y))) - 1;
        }
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
