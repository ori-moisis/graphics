//
//  ray.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : A class representing a ray in the 3D space.
//

#ifndef _RAY_HH
#define _RAY_HH

//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"


//////////////////////////////
// Class Decleration        //
//////////////////////////////

class Ray 
{
public:
  // Constructor - create a default ray //
  Ray()
  {
    _O      = POINT_ORIGIN;
    _D      = POINT_ORIGIN;
  }

  // Constructor - Create a ray in the given direction from the given origin point  //
  Ray(const Point3d& O, const Vector3d& D) 
  {
    _O = O ;
    _D = D ;
    _D.normalize() ;
  }

  // O - get the ray's origin point //
  const Point3d& O() const
  {
    return _O ;
  } 
  
  // D - get the ray's direction vector //
  const Vector3d& D() const
  {
    return _D ;
  }
  
  // operator() - Evaluate ray on the given parameter t //
  Point3d operator()(double t) { return _O + _D * t ; }

private:
  Point3d       _O;       // Ray origion                                      //
  Vector3d      _D;       // Ray direction - should be a unit vector          //
} ;

#endif /* _RAY_HH */
