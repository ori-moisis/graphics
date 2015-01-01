//
//  polygon.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : A class that represents a convex polygon on the 3d space.
//            Inherits from Object class, implementing the method to
//            test intersection of a given ray with the polygon.
//

#ifndef _POLYGON_HH
#define _POLYGON_HH

//////////////////////////////
// Library Includes         //
//////////////////////////////

#include <vector>

//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "object.h"
#include "triangle.h"


//////////////////////////////
// Class Decleration        //
//////////////////////////////

using namespace std;

class Polygon : public Object 
{
public:

  // Constructor - create a default polygon //
  Polygon();

  // Destructor - get rid of a polygon  //
  virtual ~Polygon();

  // Constructor - Create a polygon from the given vertices //
  Polygon(vector<Point3d> & vertices);

  // Constructor - Create a polygon from the given vertices, using the given normal //
  Polygon(vector<Point3d> & vertices, Vector3d & normal);

  // Constructor - Create a polygon from the given vertices, using the given texture map coordinates //
  Polygon(vector<Point3d> & vertices, vector<Point2d> textices);

  // Constructor - Create a polygon from the given vertices, using the given normal and texture map coordintes //
  Polygon(vector<Point3d> & vertices, vector<Point2d> textices, Vector3d & normal);

  // Ray intersection with the convex polygon //
  virtual int intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor);

private:
  // triangulate - split the polygon to triangles (saved on the _triangles memeber) //
  void triangulate();


private:
  bool              _textured;  // Does the polygon have a texture map coordinates  //
  vector<Point2d>   _textices;  // The polygon's texture map coordinates            //
  vector<Point3d>   _vertices;  // The polygon's vertices                           //
  vector<Triangle*> _triangles; // The polygon's triangles                          //
  Vector3d          _normal;    // The polygon's normal                             //

} ;

#endif /* _POLYGON_HH */
