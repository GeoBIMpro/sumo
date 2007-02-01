/****************************************************************************/
/// @file    NamedObjectCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A map over named objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NamedObjectCont_h
#define NamedObjectCont_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <vector>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NamedObjectCont
 * An associative storage (map) for objects (pointers to them to be exact),
 * which do have a name.
 */
template<class T>
class NamedObjectCont
{
public:
    /// Constructor
    NamedObjectCont() : myHaveChanged(false)
    { }

    /// Destructor
    virtual ~NamedObjectCont()
    {
        for (typename IDMap::iterator i=myMap.begin();
                i!=myMap.end();
                i++) {

            delete(*i).second;
        }
    }


    /** @brief Adds an item
        If another item with the same name is already known, false is reported
        and the item is not added. */
    virtual bool add(const std::string &id, T item)
    {
        if (myMap.find(id)!=myMap.end()) {
            return false;
        }
        myMap.insert(std::make_pair(id, item));
        myHaveChanged = true;
//        myVector.push_back(item);
        return true;
    }


    /** @brief Retrieves an item
        Returns 0 when no such item is stored within the container */
    T get(const std::string &id) const
        {
            typename std::map<std::string, T>::const_iterator i = myMap.find(id);
            if (i==myMap.end()) {
                return 0;
            }
            return (*i).second;
        }


    /// Removes all items from the container (deletes them, too)
    void clear()
    {
        for (typename IDMap::iterator i=myMap.begin(); i!=myMap.end(); i++) {
            delete(*i).second;
        }
        myMap.clear();
        myVector.clear();
        myHaveChanged = true;
    }


    /// Returns the number of items within the container
    size_t size() const
    {
//        assert(myMap.size()==myVector.size());
        return myMap.size();
    }


    /// Removes the named item from the container
    bool erase(const std::string &id)
    {
        typename IDMap::iterator i=myMap.find(id);
        if (i==myMap.end()) {
            return false;
        }
        T o = (*i).second;
        myMap.erase(i);
        // and from the vector
        typename ObjectVector::iterator i2 =
            find(myVector.begin(), myVector.end(), o);
        myHaveChanged = true;
        if (i2!=myVector.end()) {
            myVector.erase(i2);
        }
        delete o;
        return true;
    }


    /* @brief Returns a vector that contains all objects. */
    const std::vector<T> &buildAndGetStaticVector() const
    {
        if (myHaveChanged) {
            myVector.clear();
            typename IDMap::const_iterator i;
            for (i=myMap.begin(); i!=myMap.end(); ++i) {
                myVector.push_back((*i).second);
            }
            myHaveChanged = false;
        }
        return myVector;
    }

    /* @brief Returns a vector that contains all objects. */
    std::vector<T> getTempVector() const
    {
        std::vector<T> ret;
        typename IDMap::const_iterator i;
        for (i=myMap.begin(); i!=myMap.end(); ++i) {
            ret.push_back((*i).second);
        }
        return ret;
    }

    const std::map<std::string, T> &getMyMap() const
    {
        return myMap;
    }


private:
    /// Definition of the key to pointer map type
    typedef std::map< std::string, T > IDMap;

    /// Definition of the container type iterator
    typedef typename IDMap::iterator myContIt;

    /// The map from key to object
    IDMap myMap;

    /// Definition of the key to pointer map type
    typedef std::vector<T> ObjectVector;

    /// The vector of all known items
    mutable ObjectVector myVector;

    /// Information whether the vector is out of sync with the map
    mutable bool myHaveChanged;

};


#endif

/****************************************************************************/

