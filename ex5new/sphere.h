//
//  sphere.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : A class that represents a sphere on the 3d space.
//            Inherits from Object class, implementing the method to test
//            intersection of a given ray with the sphere.
//

#ifndef _SPHERE_HH
#define _SPHERE_HH


//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "object.h"


//////////////////////////////
// Class Decleration        //
//////////////////////////////
class Sphere : public Object 
{
public:
  // Constructor - create a default sphere  //
  Sphere();

  // Constructor - create a sphere with the given parameters  //
  Sphere(Point3d C, double r);

  virtual ~Sphere();

  // Ray Sphere intersection //
  virtual int intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor) const;

 //private:

  Point3d   _C; // The sphere's center  //
  double    _r; // The sphere's radius  //
} ;

#endif /* _SPHERE_HH */
