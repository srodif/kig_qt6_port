// calcPaths.h
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

#ifndef CALCPATHS_H
#define CALCPATHS_H

#include "objects.h"

/**
 * This function sorts os such that they're in the right order for
 * calc()-ing.  This means that child objects must appear after their
 * parents.  We assume here that none of the objects in os have
 * children or parents that aren't also in os.
 */
Objects calcPath( const Objects& os );

/**
 * This is a different function for more or less the same purpose.  It
 * takes a few Objects, which are considered to have been calced
 * already.  Then, it puts the necessary part of their children in the
 * right order, so that calc()-ing correctly updates all of their data
 * ( they're calc'ed in the right order, i mean... ).  The objects in
 * from are normally not included in the output, unless they appear
 * somewhere in the middle of the calc-path towards to...
 */
Objects calcPath( const Objects& from, const Object* to );

#endif
