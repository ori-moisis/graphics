#include "triangle.h"

Triangle::Triangle(const std::vector<Point3d>& points)
: _p(points)
, _textured(false) {
    this->calcCacheables();
}

Triangle::Triangle(const std::vector<Point3d>& points, const std::vector<Point2d>& texCoord)
: _p(points)
, _t(texCoord)
, _textured(true) {
    this->calcCacheables();
}

Triangle::~Triangle() {
}

bool Triangle::isInside(IN const Point3d& p, OUT Point2d& texCoord) const {
    Vector3d ap = p - this->_p[0];
    double apDotAb = OpenMesh::dot(ap, this->_ab);
    double apDotAc = OpenMesh::dot(ap, this->_ac);

    double alpha = this->_acDotAc * apDotAb - this->_abDotAc * apDotAc;
    if (alpha < 0) {
        return false;
    }
    double beta = this->_abDotAb * apDotAc - this->_abDotAc * apDotAb;
    if (beta < 0) {
        return false;
    }
    if (this->_denom < (alpha + beta)) {
        // Gamme will be negative
        return false;
    }
    alpha /= this->_denom;
    beta /= this->_denom;
    double gamma = 1 - alpha - beta;
    
    if (this->_textured) {
        texCoord = this->_t[0] * gamma + this->_t[1] * alpha + this->_t[2] * beta;
    }

    return true;
}

void Triangle::calcCacheables() {
    this->_ab = this->_p[1] - this->_p[0];
    this->_ac = this->_p[2] - this->_p[0];
    this->_abDotAb = OpenMesh::dot(this->_ab, this->_ab);
    this->_abDotAc = OpenMesh::dot(this->_ab, this->_ac);
    this->_acDotAc = OpenMesh::dot(this->_ac, this->_ac);
    this->_denom = this->_abDotAb * this->_acDotAc - this->_abDotAc * this->_abDotAc;
}
