// object_type.cc
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

#include "object_type.h"

#include "../misc/i18n.h"

const char* ObjectType::fullName() const
{
  return mfulltypename;
}

ObjectType::~ObjectType()
{
}

ObjectType::ObjectType( const char fulltypename[],
                        const struct ArgParser::spec argsspec[],
                        int n, int any )
  : mfulltypename( fulltypename ),
    margsparser( argsspec, n, any )
{
}

bool ObjectType::canMove() const
{
  return false;
}

void ObjectType::move( RealObject*, const Coordinate&,
                       const Coordinate& ) const
{
}

const ArgParser& ObjectType::argsParser() const
{
  return margsparser;
}

bool ObjectType::inherits( int ) const
{
  return false;
}
