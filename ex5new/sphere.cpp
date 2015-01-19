#include "sphere.h"

#include "MyMesh.h"

Sphere::Sphere()
: _C (0.0, 0.0, 0.0)
, _r (1.0) {
}

Sphere::Sphere(Point3d C, double r)
: _C (C)
, _r (r) {
}

Sphere::~Sphere()
{
}

int Sphere::intersect(Ray& ray, double tMax, double& t, Point3d& P,
                      Vector3d& N, Color3d& texColor) const {
    Vector3d tmp = ray.O() - this->_C;
    double b = 2 * OpenMesh::dot(ray.D(), tmp);
    double a = 1.0; //OpenMesh::dot(ray.D(), ray.D());
    double c = OpenMesh::dot(tmp, tmp) - (this->_r * this->_r);
    double disc = b*b - 4*a*c;

    if (disc < 0) {
        return 0;
    }

    disc = sqrt(disc);
    double sol1 = (-b + disc) / (2*a);
    double sol2 = (-b - disc) / (2*a);
    double minT = std::min(sol1, sol2);
    if (minT > tMax) {
        return 0;
    }
    if (minT < EPS) {
        minT = std::max(sol1, sol2);
    }
    if (minT < EPS) {
        return 0;
    }

    t = minT;
    P = ray(t);
    N = P - this->_C;
    N.normalize();

    // Make sure the normal is pointing in the right direction
    if (OpenMesh::dot(N, ray.D()) > 0) {
        N = -N;
    }

    texColor = COLOR_WHITE;
    if (this->_diffuseTexture) {
        double theta = atan2(N[0], N[2]);
        double phi = atan2(N[1], sqrt(N[0]*N[0] + N[2]*N[2]));

        double texU = CLAMP((theta + M_PI) / (2 * M_PI)) * this->_diffuseTexture->width();
        double texV = CLAMP((phi + M_PI_2) / M_PI) * this->_diffuseTexture->height();

        Bpixel& tex = (*this->_diffuseTexture)(texU, texV);
        texColor[0] = tex.r;
        texColor[1] = tex.g;
        texColor[2] = tex.b;
        texColor = texColor * COLOR_NORMALIZE;
    }

    return 1;
}

Color3d Sphere::textureDiffuse(const Point3d& P) const {
    return Color3d(0.5,0.5,0.5);
}

