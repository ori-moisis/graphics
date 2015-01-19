
#ifndef _ELLIPSOID_HH
#define _ELLIPSOID_HH

#include "glm/glm.hpp"
#include "sphere.h"

class Ellipsoid : public Sphere
{
public:
  // Constructor - create a default ellipsoid  //
  Ellipsoid();

  // Constructor - create an ellipsoid with the given parameters  //
  Ellipsoid(Point3d C, double r, glm::mat4 T);

  virtual ~Ellipsoid();

  virtual int intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor) const;

  Color3d textureDiffuse(const Point3d& P) const;

private:
  glm::mat4 _T;
  glm::mat4 _invT;
  glm::mat4 _transT;

};

#endif /* _ELLIPSOID_HH */
