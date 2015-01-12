#include "triangle.h"


//  Point3d _p0;  // One of the triangle vertices   //
//  Vector3d _u;  // a vector from p0 to p1         //
//  Vector3d _v;  // a vector from p0 to p2         //
//
//  Point2d _t0;  // The texture map coordiate of p0  //
//  Point2d _tu;  // The texture map coordinate of p1 //
//  Point2d _tv;  // The texture map coordinate of p2 //


Triangle::Triangle(const Vector3d& normal, Point3d p0, Point3d p1, Point3d p2)
: _normal(normal)
, _p0(p0)
, _p1(p1)
, _p2(p2)
, _textured(false)
{
}

Triangle::Triangle(const Vector3d& normal, Point3d p0, Point3d p1, Point3d p2, Point2d t0, Point2d t1,
				   Point2d t2)
: _normal(normal)
, _p0(p0)
, _p1(p1)
, _p2(p2)
, _textured(false)
{
}

Triangle::~Triangle() {
}


bool Triangle::isInside(const Point3d& p, Point2d& texCoord) const {

	bool b1 = OpenMesh::dot((_p1-_p0) % (p-_p0), _normal) < 0;
    if ((OpenMesh::dot((_p2-_p1) % (p-_p1), _normal) < 0) != b1) return false;
    if ((OpenMesh::dot((_p0-_p2) % (p-_p2), _normal) < 0) != b1) return false;

    return true;
}


