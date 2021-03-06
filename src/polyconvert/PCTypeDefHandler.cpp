/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    PCTypeDefHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading polygon type maps
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>
#include "PCTypeMap.h"
#include "PCTypeDefHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================
PCTypeDefHandler::PCTypeDefHandler(OptionsCont& oc, PCTypeMap& con)
    : SUMOSAXHandler("Detector-Defintion"),
      myOptions(oc),  myContainer(con) {}


PCTypeDefHandler::~PCTypeDefHandler() {}


void
PCTypeDefHandler::myStartElement(int element,
                                 const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_POLYTYPE) {
        bool ok = true;
        // get the id, report an error if not given or empty...
        std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
        const double layer = attrs.getOpt<double>(SUMO_ATTR_LAYER, id.c_str(), ok, myOptions.getFloat("layer"));
        const bool discard = attrs.getOpt<bool>(SUMO_ATTR_DISCARD, id.c_str(), ok, false);
        const bool allowFill = attrs.getOpt<bool>(SUMO_ATTR_FILL, id.c_str(), ok, myOptions.getBool("fill"));
        const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, myOptions.getString("type"));
        const std::string prefix = attrs.getOpt<std::string>(SUMO_ATTR_PREFIX, id.c_str(), ok, myOptions.getString("prefix"));
        const std::string color = attrs.getOpt<std::string>(SUMO_ATTR_COLOR, id.c_str(), ok, myOptions.getString("color"));
        // !!! what about error handling?
        if (!myContainer.add(id, type, color, prefix, layer, discard, allowFill)) {
            WRITE_ERROR("Could not add polygon type '" + id + "' (probably the id is already used).");
        }
    }
}


/****************************************************************************/

