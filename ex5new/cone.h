#ifndef _CONE_HH
#define _CONE_HH

#include "object.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

class Cone : public Object
{
public:

  // Constructor - create a sphere with the given parameters  //
  Cone(Point3d V, double theta, Vector3d D, double h);

  virtual ~Cone();

  // Ray Cone intersection //
  virtual int intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor) const;

private:
  glm::vec3  _V;     // The cone's apex (meeting point)//
  double     _theta; // The cone's opening angle       //
  glm::vec3  _D;     // The cone's axis (orientation)  //
  glm::mat3  _M;
  double 	 _h;
  double     _r;
  double 	 _maxDist;
  double     _planeD;
  glm::vec3  _center;
  double     _tant;
};

#endif /* _CONE_HH */
