// direction.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef ABSTRACTLINE_H
#define ABSTRACTLINE_H

#include "curve.h"

struct LineData;

/**
 * This class is an abstract interface to an object that is somewhat
 * of a line.. Examples are Line, Segment, Ray etc.
 */
class AbstractLine
  : public Curve
{
  typedef Curve Parent;
  int msize;
public:
  AbstractLine();
  AbstractLine( const AbstractLine& l );
  virtual const Coordinate p1() const = 0;
  virtual const Coordinate p2() const = 0;
  const LineData lineData() const;

  const Coordinate direction() const;

  virtual bool isa( int type ) const;

  double slope() const;

  const uint numberOfProperties() const;
  const Property property( uint which, const KigWidget& w ) const;
  const QCStringList properties() const;
  const QString equationString( const KigWidget& w ) const;
};

#endif
