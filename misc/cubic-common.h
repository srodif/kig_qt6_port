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

#ifndef KIG_MISC_CUBIC_COMMON_H
#define KIG_MISC_CUBIC_COMMON_H

#include "common.h"

class Transformation;

/**
 * This class represents an equation of a cubic in the form
 * "a_{ijk} x_i x_j x_k = 0" (in homogeneous coordinates, i,j,k = 0,1,2),
 * i <= j <= k.
 * The coefficients are stored in lessicografic order.
 */
class CubicCartesianData
{
public:
  double coeffs[10];
  explicit CubicCartesianData();
  CubicCartesianData( double a000, double a001, double a002,
                      double a011, double a012, double a022,
                      double a111, double a112, double a122,
                      double a222 )
    {
      coeffs[0] = a000;
      coeffs[1] = a001;
      coeffs[2] = a002;
      coeffs[3] = a011;
      coeffs[4] = a012;
      coeffs[5] = a022;
      coeffs[6] = a111;
      coeffs[7] = a112;
      coeffs[8] = a122;
      coeffs[9] = a222;
    };
  CubicCartesianData( const double incoeffs[10] );

  static CubicCartesianData invalidData();
  bool valid() const;
};

bool operator==( const CubicCartesianData& lhs, const CubicCartesianData& rhs );

/**
 * This function calcs a cartesian cubic equation such that the
 * given points are on the cubic.  There can be at most 9 and at
 * least 2 point.  If there are less than 9, than the coefficients
 * will be chosen to 1.0 if possible
 */

const CubicCartesianData calcCubicThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianData calcCubicCuspThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianData calcCubicNodeThroughPoints (
    const std::vector<Coordinate>& points );

double calcCubicYvalue ( double x, double ymin, double ymax,
                         int root, CubicCartesianData data,
                         bool& valid, int& numroots );

const Coordinate calcCubicLineIntersect( const CubicCartesianData& c,
                                         const LineData& l,
                                         int root, bool& valid );

void calcCubicLineRestriction ( CubicCartesianData data,
         Coordinate p1, Coordinate dir,
         double& a, double& b, double& c, double& d );

const CubicCartesianData calcCubicTransformation (
  const CubicCartesianData& data,
  const Transformation& t, bool& valid );

#endif
