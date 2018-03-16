/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEParkingSpace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// A lane area vehicles can halt at (GNE version)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <foreign/fontstash/fontstash.h>
#include <iostream>
#include <string>
#include <utility>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/xml/SUMOSAXHandler.h>

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include "GNENet.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEParkingSpace::GNEParkingSpace(GNEViewNet* viewNet, GNEParkingArea* parkingAreaParent, double x, double y, double z, double width, double length, double angle, bool blockMovement) :
    GNEAdditional(parkingAreaParent->generateParkingSpaceID(), viewNet, SUMO_TAG_PARKING_SPACE, ICON_PARKINGSPACE, true, blockMovement, parkingAreaParent),
    myX(x),
    myY(y),
    myZ(z),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
}


GNEParkingSpace::~GNEParkingSpace() {}


void
GNEParkingSpace::writeAdditional(OutputDevice& device) const {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_X, myX);
    device.writeAttr(SUMO_ATTR_Y, myY);
    device.writeAttr(SUMO_ATTR_Z, myZ);
    device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    device.writeAttr(SUMO_ATTR_ANGLE, myAngle);
    // write block movement attribute only if it's enabled
    if (myBlockMovement) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlockMovement);
    }
    // Close tag
    device.closeTag();
}


void 
GNEParkingSpace::moveGeometry(const Position & oldPos, const Position & offset) {
    // restore old position, apply offset and update Geometry
    Position pos = oldPos;
    pos.add(offset);
    myX = pos.x();
    myY = pos.y();
    updateGeometry();
}


void 
GNEParkingSpace::commitGeometryMoving(const Position & oldPos, GNEUndoList * undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_X, toString(myX), true, toString(oldPos.x())));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_Y, toString(myY), true, toString(oldPos.y())));
    undoList->p_end();
}


void
GNEParkingSpace::updateGeometry() {
    myShape.clear();
    myShape.push_back(Position(myX, myY));
    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshElement(this);
}


Position 
GNEParkingSpace::getPositionInView() const {
    return Position(myX, myY);
}


const std::string&
GNEParkingSpace::getParentName() const {
    return myAdditionalParent->getMicrosimID();
}


void
GNEParkingSpace::drawGL(const GUIVisualizationSettings& ) const {
    // push name and matrix
    glPushName(getGlID());
    glPushMatrix();
    // Traslate matrix and draw green contour
    glTranslated(myX, myY, getType() + 0.1);
    glRotated(myAngle, 0, 0, 1);
    // Set Color depending of selection
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedConnectionColor);
    } else {
        GLHelper::setColor(RGBColor(0, 255, 0, 255));
    }
    GLHelper::drawBoxLine(Position(0, myLength + 0.05), 0, myLength + 0.1, (myWidth / 2) + 0.05);
    // Traslate matrix and draw blue innen
    glTranslated(0, 0, 0.1);
    // Set Color depending of selection
    if (isAdditionalSelected()) {
        GLHelper::setColor(myViewNet->getNet()->selectedAdditionalColor);
    } else {
        GLHelper::setColor(RGBColor(255,200,200, 255));
    }
    GLHelper::drawBoxLine(Position(0, myLength), 0, myLength, myWidth / 2);
    // Traslate matrix and draw lock icon
    glTranslated(0, myLength/2, 0.1);
    drawLockIcon();
    // pop matrix and name
    glPopMatrix();
    glPopName();
}


std::string
GNEParkingSpace::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_X:
            return toString(myX);
        case SUMO_ATTR_Y:
            return toString(myY);
        case SUMO_ATTR_Z:
            return toString(myZ);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_ANGLE:
            return toString(myAngle);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_PARENT:
            return myAdditionalParent->getID();
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_X:
        case SUMO_ATTR_Y:
        case SUMO_ATTR_Z:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_ANGLE:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_PARENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEParkingSpace::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_X:
            return canParse<double>(value);
        case SUMO_ATTR_Y:
            return canParse<double>(value);
        case SUMO_ATTR_Z:
            return canParse<double>(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_PARENT:
            return (myViewNet->getNet()->getAdditional(SUMO_TAG_PARKING_AREA, value) != NULL);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEParkingSpace::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_X:
            myX = parse<double>(value);
            break;
        case SUMO_ATTR_Y:
            myY = parse<double>(value);
            break;
        case SUMO_ATTR_Z:
            myZ = parse<double>(value);
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_ANGLE:
            myAngle = parse<double>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_PARENT:
            changeAdditionalParent(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // After setting attribute always update Geometry
    updateGeometry();
}


/****************************************************************************/
