#include "polygon.h"


Polygon::Polygon()
: _textured (false)
, _normal (0,0,0)
{
}

Polygon::~Polygon() {
}

Polygon::Polygon(vector<Point3d>& vertices)
: _textured (false)
, _vertices (vertices)
, _normal (0,0,0)
{
}

Polygon::Polygon(vector<Point3d>& vertices, Vector3d& normal)
: _textured (false)
, _vertices (vertices)
, _normal (normal)
{
}

Polygon::Polygon(vector<Point3d>& vertices, vector<Point2d> textices)
: _textured (true)
, _textices (textices)
, _vertices (vertices)
, _normal (0,0,0)
{
}

Polygon::Polygon(vector<Point3d>& vertices, vector<Point2d> textices,
        Vector3d& normal)
: _textured (true)
, _textices (textices)
, _vertices (vertices)
, _normal (normal)
{
}

int Polygon::intersect(Ray& ray, double tMax, double& t, Point3d& P,
                       Vector3d& N, Color3d& texColor) const {
    return 0;
}

void Polygon::triangulate() {

}
