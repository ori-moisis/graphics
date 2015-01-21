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


static const double myEps = 1e-4;

int Cone::intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P,
					OUT Vector3d& N, OUT Color3d& texColor) const {

	texColor = COLOR_WHITE;
	glm::vec3 rayO(toGlm(ray.O()));
	glm::vec3 rayD(toGlm(ray.D()));

	// Check point on cap
	//double capT;
	Vector3d capP;
	Vector3d capN;
	bool hitCap = false;
	bool hitCone = true;

	double capT = -glm::dot(-_D, (rayO - _center)) / glm::dot(-_D, rayD);

	glm::vec3 tmpP(toGlm(ray(capT)));
	if (glm::distance(tmpP, _center) <= _r && capT < tMax && capT > myEps) {
		capP = toOpenMesh(tmpP);
		capN = toOpenMesh(-_D);
		//std::cout << "HIT CAP" << std::endl;
		//std::cout << "Normal is (" << capN[0] << ", " << capN[1] << ", " << capN[2] << ")"<< std::endl;
		//std::cout << "AT POINT " << capP << std::endl;

		hitCap = true;
	}

	glm::vec3 delta = rayO - this->_V;

	double c2 = glm::dot(rayD, _M * rayD);
	double c1 = glm::dot(rayD, _M * delta);
	double c0 = glm::dot(delta, _M * delta);

	double disc = c1*c1 - c0*c2;

	if (disc < 0) {
		hitCone = false;
	}

	disc = sqrt(disc);
	double sol1 = (-c1 + disc) / (c2);
	double sol2 = (-c1 - disc) / (c2);

	double coneT = std::min(sol1, sol2);

	if (coneT > tMax) {
		hitCone = false;
	}
	if (coneT < myEps) {
		coneT = std::max(sol1, sol2);
	}
	if (coneT < myEps) {
		hitCone = false;
	}

	tmpP = toGlm(ray(coneT));

	//std::cout << "Cone P is " << toOpenMesh(tmpP) << std::endl;

	// Take only lower side of the cone
	if (glm::dot(_D, tmpP) > _planeD) {
		hitCone = false;
	}

	// Limit height
	double dist = glm::length(_V - tmpP);
	if (dist > _maxDist)
	{
		hitCone = false;
	}


	if ((hitCone && hitCap && capT < coneT) || (hitCap && !hitCone)) {
		N = capN; P = capP; t = capT;
		return INTERSECTION;
	}

	if (!hitCone)
	{
		return NO_INTERSECTION;
	}

	t = coneT;
	P = toOpenMesh(tmpP);

	// Calculate normal
	double tant = tan(_theta);
	Vector3d U(tmpP.x - _V.x, 0, tmpP.z - _V.z);
	U.normalize();
	N = Vector3d(U[0]/tant, tant, U[2]/tant);
//
//	glm::vec3 NG(2*P[0], 2*P[1], -2*P[2]);
//	glm::rota
//	NG = NG - _V;
//	N = toOpenMesh(NG);

	N.normalize();

	std::cout << "t is " << t << std::endl;
	std::cout << "Normal is (" << N[0] << ", " << N[1] << ", " << N[2] << ")"<< std::endl;
	return INTERSECTION;
}

