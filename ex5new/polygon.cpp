#include "polygon.h"


Polygon::Polygon()
: _textured (false)
, _normal (0,0,1)
{
}

Polygon::~Polygon() {
    for (std::vector<Triangle*>::const_iterator tri_iter = this->_triangles.begin();
         tri_iter != this->_triangles.end ();
         ++tri_iter) {
        delete *tri_iter;
    }
    this->_triangles.clear();
}

Polygon::Polygon(vector<Point3d>& vertices)
: _textured (false)
, _vertices (vertices)
, _normal (calcNormal(vertices))
{
    this->triangulate();
}

Polygon::Polygon(vector<Point3d>& vertices, Vector3d& normal)
: _textured (false)
, _vertices (vertices)
, _normal (normal)
{
    this->triangulate();
}

Polygon::Polygon(vector<Point3d>& vertices, vector<Point2d> textices)
: _textured (true)
, _textices (textices)
, _vertices (vertices)
, _normal (calcNormal(vertices))
{
    this->triangulate();
}

Polygon::Polygon(vector<Point3d>& vertices, vector<Point2d> textices,
        Vector3d& normal)
: _textured (true)
, _textices (textices)
, _vertices (vertices)
, _normal (normal)
{
    this->triangulate();
}

int Polygon::intersect(Ray& ray, double tMax, double& t, Point3d& P,
                       Vector3d& N, Color3d& texColor) const {
    // Find the intersection point with the polygon's plane
    double nDotD = OpenMesh::dot(this->_normal, ray.D());

    if (nDotD < EPS && nDotD > -EPS) {
        return NO_INTERSECTION;
    }

    double tmpT = OpenMesh::dot(this->_normal, ray.O() - this->_vertices[0]) / -nDotD;
    if (tmpT > tMax || tmpT < EPS) {
        return NO_INTERSECTION;
    }

    Point3d tmpP = ray(tmpT);
    Point2d texCoord(0,0);
    for (std::vector<Triangle*>::const_iterator tri_iter = this->_triangles.begin();
         tri_iter != this->_triangles.end ();
         ++tri_iter) {
        if ((*tri_iter)->isInside(tmpP, texCoord)) {
            t = tmpT;
            P = tmpP;
            N = this->_normal;
            texColor = COLOR_WHITE;
            if (this->_textured && this->_diffuseTexture) {
                Bpixel color = (*this->_diffuseTexture)(texCoord[0] * this->_diffuseTexture->width(), 
                                                        texCoord[1] * this->_diffuseTexture->height());
                texColor[0] = color.r;
                texColor[1] = color.g;
                texColor[2] = color.b;
                texColor = texColor * COLOR_NORMALIZE;
            }
            return INTERSECTION;
        }
    }

    return NO_INTERSECTION;
}

void Polygon::triangulate() {
    if (! this->_triangles.empty ()) {
        // Already triangulated
        return;
    }

    for (size_t i = 2; i < this->_vertices.size(); ++i) {
        std::vector<Point3d> points;
        points.push_back(this->_vertices[0]);
        points.push_back(this->_vertices[i-1]);
        points.push_back(this->_vertices[i]);
        if (this->_textured) {
            std::vector<Point2d> texCoord;
            texCoord.push_back(this->_textices[0]);
            texCoord.push_back(this->_textices[i-1]);
            texCoord.push_back(this->_textices[i]);
            this->_triangles.push_back(new Triangle(points, texCoord));
        } else {
            this->_triangles.push_back(new Triangle(points));
        }
    }
}

Vector3d Polygon::calcNormal(const vector<Point3d>& vertices) {
    if (vertices.size() < 3) {
        return Vector3d(0,0,1);
    }

    // Return the cross product of two vectors
    Vector3d normal = (vertices[1] - vertices[0]) % (vertices[2] - vertices[1]);
    normal.normalize();
    return normal;
}
