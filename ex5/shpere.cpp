#include "shpere.h"

#include "MyMesh.h"

Sphere::Sphere()
: _C (0.0, 0.0, 0.0)
, _r (1.0) {
}

Sphere::Sphere(Point3d C, double r)
: _C (C)
, _r (r) {
}

int Sphere::intersect(Ray& ray, double tMax, double& t, Point3d& P,
					  Vector3d& N, Color3d& texColor) {
	Vector3d tmp = ray.O() - this->_C;
	double b = 2 * dot(ray._D(), tmp);
	double a = dot(ray._D(), ray._D());
	double c = dot(tmp, tmp) - (this->_r * this->_r);
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

	t = minT;
	P = ray._D()(t);
	N = P - this->_C();
	N.normalize();
	return 1;
}

Color3d Sphere::textureDiffuse(const Point3d& P) const {
	return Color3d(0.5,0.5,0.5);
}
