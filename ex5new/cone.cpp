#include "cone.h"

glm::vec3 toGlm(const Vector3d& v) {
	return glm::vec3(v[0], v[1], v[2]);
}

Vector3d toOpenMesh(const glm::vec3& v) {
	return Vector3d(v.x, v.y, v.z);
}

Cone::Cone(Point3d V, double theta, Vector3d D, double(h)) : _theta((theta*M_PI)/180), _h(h) {
	_V = toGlm(V);
	D.normalize();
	_D = toGlm(D);
	double cosTheta = cos(_theta);
	glm::mat3 tmpMat(cosTheta*cosTheta);
	_M = glm::outerProduct(_D, _D) - tmpMat;
	_r = _h * tan(_theta);
	_maxDist = _r / sin(_theta);
	_planeD = glm::dot(_D, _V);
	_center = _V - glm::vec3(_D.x*_h, _D.y*_h, _D.z*_h);
}

Cone::~Cone() {
}


int Cone::intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P,
					OUT Vector3d& N, OUT Color3d& texColor) const {

	glm::vec3 rayO(toGlm(ray.O()));
	glm::vec3 rayD(toGlm(ray.D()));

	// Check point on cap
	double tmpT = glm::dot(-_D, (rayO - _center)) / glm::dot(_D, rayD);
	if (tmpT > tMax) {
		return NO_INTERSECTION;
	}
	glm::vec3 tmpP(toGlm(ray(tmpT)));
	if (glm::distance(tmpP, _center) < _r) {
		t = tmpT;
		P = toOpenMesh(tmpP);
		N = toOpenMesh(-_D);
		return INTERSECTION;
	}

	glm::vec3 delta = rayO - this->_V;

	double c2 = glm::dot(rayD, _M * rayD);
	double c1 = glm::dot(rayD, _M * delta);
	double c0 = glm::dot(delta, _M * delta);

	double disc = c1*c1 - c0*c2;

	if (disc < 0) {
		return NO_INTERSECTION;
	}

	disc = sqrt(disc);
	double sol1 = (-c1 + disc) / (c2);
	double sol2 = (-c1 - disc) / (c2);

	double minT = std::min(sol1, sol2);

	if (minT > tMax) {
		return NO_INTERSECTION;
	}
	if (minT < 0) {
		minT = std::max(sol1, sol2);
	}
	if (minT < 0) {
		return NO_INTERSECTION;
	}

	t = minT;
	P = ray(t);


//	std::cout << "P is (" << P[0] << ", " << P[1] << ", " << P[2] << ")"<< std::endl;
//	std::cout << "_V is (" << _V[0] << ", " << _V[1] << ", " << _V[2] << ")"<< std::endl;
//	std::cout << "_D is (" << _D[0] << ", " << _D[1] << ", " << _D[2] << ")"<< std::endl;
//	std::cout << "planeD is: " << planeD << std::endl;

	glm::vec3 PG(toGlm(P));

	// Take only lower side of the cone
	if (glm::dot(_D, PG) > _planeD) {
		return NO_INTERSECTION;
	}

	// Limit height
	double dist = glm::length(_V - PG);
	if (dist > _maxDist)
	{
		return NO_INTERSECTION;
	}

	// Calculate normal
	double tant = tan(_theta);
	Vector3d U(PG.x - _V.x, 0, PG.z - _V.z);
	U.normalize();
	N = Vector3d(U[0]/tant, tant, U[2]/tant);
	N.normalize();

//	glm::vec3 NG(2*P[0], 2*P[1], -2*P[2]);
//	N = toOpenMesh(glm::inverse(_M) * NG);
//	std::cout << "Normal is (" << N[0] << ", " << N[1] << ", " << N[2] << ")"<< std::endl;
	return INTERSECTION;
}

