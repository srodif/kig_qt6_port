// object_hierarchy.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_MISC_OBJECT_HIERARCHY_H
#define KIG_MISC_OBJECT_HIERARCHY_H

#include "../objects/common.h"

#include <map>

class ArgParser;

class ObjectHierarchy
{
public:
  class Node;
private:
  std::vector<Node*> mnodes;
  uint mnumberofargs;
  uint mnumberofresults;
  std::vector<int> margrequirements;

  int visit( const Object* o, std::map<const Object*, int>& );
public:
  ObjectHierarchy( const Objects& from, const Object* to );
  ObjectHierarchy( const Objects& from, const Objects& to );
  ObjectHierarchy( const ObjectHierarchy& h );
  ~ObjectHierarchy();

  // this creates a new ObjectHierarchy, that takes a.size() less
  // arguments, but uses copies of the ObjectImp's in a instead..
  ObjectHierarchy withFixedArgs( const Args& a ) const;

  std::vector<ObjectImp*> calc( const Args& a, const KigDocument& doc ) const;

  // saves the ObjectHierarchy data in children xml tags of parent..
  void serialize( QDomElement& parent, QDomDocument& doc ) const;
  // deserialize the ObjectHierarchy data from the xml element
  // parent..
  ObjectHierarchy( const QDomElement& parent );

  // build a set of objects that interdepend according to this
  // ObjectHierarchy..
  Objects buildObjects( const Objects& os, const KigDocument& ) const;

  ArgParser argParser() const;

  uint numberOfArgs() const { return mnumberofargs; };
  uint numberOfResults() const { return mnumberofresults; };

  int idOfLastResult() const;

  ObjectHierarchy transformFinalObject( const Transformation& t ) const;
};

#endif
