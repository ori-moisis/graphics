//
//  triangle.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : A class that represents a triangle on the 3d space.
//            Implements a method checking if a given point is inside
//            the triangle.
//

#ifndef _TRIANGLE_HH
#define _TRIANGLE_HH

#include <vector>

//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "MyVecs.h"


//////////////////////////////
// Class Decleration        //
//////////////////////////////

class Triangle
{
public:
    // Constructor - create a triangle from the given points  //
    Triangle(const std::vector<Point3d>& points);
    Triangle(const std::vector<Point3d>& points, const std::vector<Point2d>& texCoord);

    // Destructor - get rid of a triangle //
    ~Triangle();

    // isInside - return true if the given point is inside the triangle //
    bool isInside(IN const Point3d& p, OUT Point2d& texCoord) const;

private:

    void calcCacheables();

    std::vector<Point3d> _p;
    std::vector<Point2d> _t;

    bool  _textured;

    // Cached calculations
    Vector3d _ab;
    Vector3d _ac;
    double _abDotAb;
    double _abDotAc;
    double _acDotAc;
    double _denom;

};

#endif /* _TRIANGLE_HH */
