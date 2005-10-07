#ifndef ROAbstractRouteDefLoader_h
#define ROAbstractRouteDefLoader_h
//---------------------------------------------------------------------------//
//                        ROAbstractRouteDefLoader.h -
//  The basic class for loading routes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:51  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ---------------------------------------------
// Revision 1.5  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.4  2003/03/20 16:45:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:37  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROLoader;
class RONet;
class OptionsCont;
class ROVehicleBuilder;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROAbstractRouteDefLoader
 * Base class for loaders of route which do have a certain format. XML-reading
 * loaders are not derived directly, but use the derived ROTypedXMLRoutesLoader
 * class as their upper class.
 */
class ROAbstractRouteDefLoader {
public:
    /// Constructor
    ROAbstractRouteDefLoader(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end, const std::string &file="");

    /// Destructor
    virtual ~ROAbstractRouteDefLoader();

    /** @brief Skips routes which begin before the given time
        This method uses the method myReadRoutesAtLeastUntil(time) to overread
        the first routes, so the loaders must determine by themselves whether
        to build a route or not (the departure time has to be between myBegin
        and the given timestep */
    void skipUntilBegin();

    /// Adds routes from the file until the given time is reached
    void readRoutesAtLeastUntil(SUMOTime time);

    /// Closes the reading of the routes
    virtual void closeReading() = 0;

    /// Returns the name of the route type
    virtual std::string getDataName() const = 0;

    /// Initialises the reader
    virtual bool init(OptionsCont &options) = 0;

    /// Returns the time the current (last read) route starts at
    virtual SUMOTime getCurrentTimeStep() const = 0;

    /// Returns the information whether no routes are available from this loader anymore
    virtual bool ended() const = 0;

	friend class ROLoader;

protected:
    /** @brief Builds routes
        All routes between the loader's current time step and the one given shall
        be processed. If the route's departure time is lower than the value of
        "myBegin", the route should not be added into the container. */ // !!! not very good
    virtual bool myReadRoutesAtLeastUntil(SUMOTime time) = 0;

protected:
    /// The network to add routes to
    RONet &_net;

    /// The time for which the first route shall be compute
    SUMOTime myBegin;

    /// The time for which the first route shall be compute
    SUMOTime myEnd;

    /// The vehicle builder to use
    ROVehicleBuilder &myVehicleBuilder;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

