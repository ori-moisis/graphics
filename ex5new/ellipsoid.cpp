#include "ellipsoid.h"
#define GLM_PRECISION_HIGHP_FLOAT

Ellipsoid::Ellipsoid() : Sphere(), _T(1.0f), _invT(1.0f), _transT(1.0f) {
}

Ellipsoid::Ellipsoid(Point3d C, double r, glm::mat4 T)
: Sphere(C, r),
  _T(T),
  _invT(glm::inverse(T)),
  _transT(glm::transpose(_T)) {
}

Ellipsoid::~Ellipsoid()
{
}

Vector3d applyTrans(const glm::mat4& T, const Vector3d& p) {
	glm::vec4 origP(p[0], p[1], p[2], 1);
	glm::vec4 TP(T * origP);
	return Vector3d(TP.x/TP.w, TP.y/TP.w, TP.z/TP.w);
}

int Ellipsoid::intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P,
					 	 OUT Vector3d& N, OUT Color3d& texColor) const {

	// Transform ray
	Point3d pTO = applyTrans(_invT, ray.O());
	Vector3d pTD = applyTrans(_invT, ray.D());
	pTD.normalize();
	Ray TRay(pTO, pTD);

	// Intersect as sphere
	int result =  Sphere::intersect(TRay, tMax, t, P, N, texColor);

	// Transform normal
	N = applyTrans(_transT, N);
	N.normalize();

	// Transform point
	P = applyTrans(_T, P);

	return result;
}

Color3d Ellipsoid::textureDiffuse(const Point3d& P) const {
	return Color3d(0.0f);
}
